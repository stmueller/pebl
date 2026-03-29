// ScaleDefinition.cpp - PEBL Scale data model implementation (ScaleRunner format)
// Copyright (c) 2026 Shane T. Mueller
// Licensed under GPL

#include "ScaleDefinition.h"
#include <json.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;
using json = nlohmann::json;

// Parse a visible_when JSON value into flat conditions
// Returns true if parsed successfully (even if complex)
static void ParseVisibleWhen(const json& vw,
                             bool& has_visible_when,
                             std::string& logic,
                             std::vector<VisibleWhenCondition>& conditions,
                             bool& is_complex)
{
    has_visible_when = true;
    logic = "all";
    conditions.clear();
    is_complex = false;

    if (vw.is_null()) {
        has_visible_when = false;
        return;
    }

    // Helper lambda to parse a single simple condition object
    auto parseSimple = [](const json& obj, VisibleWhenCondition& c) -> bool {
        if (obj.contains("parameter") && obj["parameter"].is_string()) {
            c.source_type = "parameter";
            c.source_name = obj["parameter"].get<std::string>();
        } else if ((obj.contains("item") && obj["item"].is_string()) ||
                   (obj.contains("question") && obj["question"].is_string())) {
            c.source_type = "item";
            c.source_name = obj.contains("item") ? obj["item"].get<std::string>() : obj["question"].get<std::string>();
        } else {
            return false;
        }
        if (obj.contains("operator") && obj["operator"].is_string()) {
            c.op = obj["operator"].get<std::string>();
        }
        if (obj.contains("value")) {
            if (obj["value"].is_string()) {
                c.value = obj["value"].get<std::string>();
                c.is_list = false;
            } else if (obj["value"].is_array()) {
                c.is_list = true;
                for (const auto& v : obj["value"]) {
                    if (v.is_string()) {
                        c.values.push_back(v.get<std::string>());
                    } else {
                        c.values.push_back(v.dump());
                    }
                }
            } else {
                // Scalar number/bool — store as string
                c.value = obj["value"].dump();
                c.is_list = false;
            }
        }
        return true;
    };

    // Case 1: Simple single condition {parameter/item, operator, value}
    if (vw.contains("parameter") || vw.contains("item") || vw.contains("question")) {
        VisibleWhenCondition c;
        if (parseSimple(vw, c)) {
            conditions.push_back(c);
        }
        return;
    }

    // Case 2: Compound all/any
    std::string key;
    if (vw.contains("all")) {
        key = "all";
        logic = "all";
    } else if (vw.contains("any")) {
        key = "any";
        logic = "any";
    } else {
        // Unknown structure
        is_complex = true;
        return;
    }

    if (!vw[key].is_array()) {
        is_complex = true;
        return;
    }

    // Check each child — if any child is itself an all/any group, mark complex
    for (const auto& child : vw[key]) {
        if (child.contains("all") || child.contains("any")) {
            is_complex = true;
            conditions.clear();
            return;
        }
        VisibleWhenCondition c;
        if (parseSimple(child, c)) {
            conditions.push_back(c);
        } else {
            is_complex = true;
            conditions.clear();
            return;
        }
    }
}

// Serialize conditions back to JSON
static json SerializeVisibleWhen(const std::string& logic,
                                 const std::vector<VisibleWhenCondition>& conditions)
{
    auto condToJson = [](const VisibleWhenCondition& c) -> json {
        json obj;
        if (c.source_type == "item") {
            obj["item"] = c.source_name;
        } else {
            obj["parameter"] = c.source_name;
        }
        obj["operator"] = c.op;
        if (c.is_list) {
            json arr = json::array();
            for (const auto& v : c.values) {
                arr.push_back(v);
            }
            obj["value"] = arr;
        } else {
            obj["value"] = c.value;
        }
        return obj;
    };

    if (conditions.size() == 1) {
        return condToJson(conditions[0]);
    }

    json arr = json::array();
    for (const auto& c : conditions) {
        arr.push_back(condToJson(c));
    }
    return json({{logic, arr}});
}

ScaleDefinition::ScaleDefinition()
    : mDefaultRequired(-1)
    , mDirty(false)
    , mSourceIsOSD(false)
{
    mDataOutput.individual_file = "{code}-{subnum}.csv";
    mDataOutput.pooled_file = "{code}-pooled.csv";
    mDataOutput.report_file = "{code}-report-{subnum}.html";
}

ScaleDefinition::~ScaleDefinition()
{
}

std::shared_ptr<ScaleDefinition> ScaleDefinition::CreateNew(const std::string& code)
{
    auto scale = std::make_shared<ScaleDefinition>();
    scale->mScaleInfo.code = code;
    scale->mScaleInfo.name = code;  // Default to code, user will change

    // Add default English translations for required keys
    scale->mTranslations["en"]["question_head"] = "Please answer the following questions:";
    scale->mTranslations["en"]["debrief"] = "Thank you for completing this questionnaire.";

    scale->mDirty = true;
    return scale;
}

std::shared_ptr<ScaleDefinition> ScaleDefinition::LoadFromFile(const std::string& jsonPath)
{
    auto scale = std::make_shared<ScaleDefinition>();
    if (!scale->LoadDefinitionJSON(jsonPath)) {
        return nullptr;
    }
    scale->mDirty = false;
    return scale;
}

std::shared_ptr<ScaleDefinition> ScaleDefinition::LoadFromOSDFile(const std::string& osdPath)
{
    auto scale = std::make_shared<ScaleDefinition>();
    if (!scale->LoadFromOSDBundlePath(osdPath)) {
        return nullptr;
    }
    scale->mDirty = false;
    return scale;
}

bool ScaleDefinition::LoadFromScalesDir(const std::string& basePath, const std::string& scaleCode)
{
    // Load from <basePath>/<code>/<code>.json  (fall back to <code>.osd bundle)
    std::string scaleDir = basePath + "/" + scaleCode;
    std::string defPath = scaleDir + "/" + scaleCode + ".json";
    std::string osdPath = scaleDir + "/" + scaleCode + ".osd";

    if (fs::exists(defPath)) {
        if (!LoadDefinitionJSON(defPath)) {
            return false;
        }
        // If a .osd bundle also exists, supplement mParameters from it.
        // This handles the case where the user edited the .osd directly to add
        // parameters but the .json was saved before those parameters were added.
        // The .osd parameters block is the authoritative source for parameter metadata.
        if (fs::exists(osdPath)) {
            try {
                std::ifstream osdFile(osdPath);
                if (osdFile.is_open()) {
                    json bundle;
                    osdFile >> bundle;
                    if (bundle.contains("definition") && bundle["definition"].contains("parameters")) {
                        for (auto& [key, value] : bundle["definition"]["parameters"].items()) {
                            ScaleParameter param;
                            if (value.contains("type"))        param.type = value["type"];
                            if (value.contains("default"))     param.defaultValue = value["default"].is_string()
                                                                    ? value["default"].get<std::string>()
                                                                    : value["default"].dump();
                            if (value.contains("description")) param.description = value["description"];
                            if (value.contains("options")) {
                                for (const auto& opt : value["options"]) {
                                    param.options.push_back(opt.is_string() ? opt.get<std::string>() : opt.dump());
                                }
                            }
                            // Only add — do not overwrite params already set from .json
                            if (mParameters.find(key) == mParameters.end()) {
                                mParameters[key] = param;
                            }
                        }
                    }
                }
            } catch (...) {
                // OSD supplement is best-effort; don't fail the load
            }
        }
    } else if (fs::exists(osdPath)) {
        // .osd bundle includes both definition and translations — load all at once
        if (!LoadFromOSDBundlePath(osdPath)) {
            return false;
        }
        mDirty = false;
        return true;
    } else {
        return false;
    }

    // Load translations from the same directory
    // Support both PEBL naming (.pbl-<lang>.json) and OSD format (.<lang>.json)
    std::string pblPrefix = scaleCode + ".pbl-";
    std::string defFilename = scaleCode + ".json";  // Skip the definition file itself

    try {
        if (fs::exists(scaleDir)) {
            for (const auto& entry : fs::directory_iterator(scaleDir)) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();

                    // Skip the definition file itself
                    if (filename == defFilename) continue;

                    // Try PEBL format: {code}.pbl-{lang}.json
                    if (filename.find(pblPrefix) == 0 &&
                        filename.size() >= 5 &&
                        filename.substr(filename.size() - 5) == ".json") {
                        std::string lang = filename.substr(pblPrefix.length());
                        lang = lang.substr(0, lang.length() - 5);
                        LoadTranslationJSON(entry.path().string(), lang);
                        continue;
                    }

                    // Try OSD format: {code}.{lang}.json
                    std::string openPrefix = scaleCode + ".";
                    if (filename.find(openPrefix) == 0 &&
                        filename.size() >= 5 &&
                        filename.substr(filename.size() - 5) == ".json") {
                        std::string lang = filename.substr(openPrefix.length());
                        lang = lang.substr(0, lang.length() - 5);
                        if (!lang.empty() && lang.find('.') == std::string::npos) {
                            LoadTranslationJSON(entry.path().string(), lang);
                        }
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        // Continue even if translation loading fails
    }

    mDirty = false;
    return true;
}

bool ScaleDefinition::SaveToFile(const std::string& jsonPath)
{
    if (SaveDefinitionJSON(jsonPath)) {
        mDirty = false;
        return true;
    }
    return false;
}

bool ScaleDefinition::ExportToJSON(const std::string& definitionsPath, const std::string& translationsPath)
{
    // Save definition
    std::string defFile = definitionsPath + "/" + mScaleInfo.code + ".json";
    if (!SaveDefinitionJSON(defFile)) {
        return false;
    }

    // Save translations in new format only (.{lang}.json)
    for (const auto& [lang, translations] : mTranslations) {
        std::string transFile = translationsPath + "/" + mScaleInfo.code + "." + lang + ".json";
        if (!SaveTranslationJSON(transFile, lang)) {
            return false;
        }
    }

    mDirty = false;
    return true;
}

bool ScaleDefinition::ExportToOSD(const std::string& outputDir)
{
    // Build an OSD bundle: {"osd_version":"1.0","definition":{...},"translations":{...}}
    // Builds definition JSON from in-memory model (no dependency on .json file on disk).
    try {
        json definition;
        if (!BuildDefinitionJSONObject(definition)) {
            printf("ExportToOSD: failed to build definition JSON\n");
            return false;
        }

        json translations;
        for (const auto& [lang, transMap] : mTranslations) {
            json transJson;
            for (const auto& [key, value] : transMap) {
                transJson[key] = value;
            }
            translations[lang] = transJson;
        }

        json bundle;
        bundle["osd_version"] = "1.0";
        bundle["definition"] = definition;
        bundle["translations"] = translations;

        std::string osdFile = outputDir + "/" + mScaleInfo.code + ".osd";
        std::ofstream f(osdFile);
        if (!f.is_open()) return false;
        f << bundle.dump(2);
        return true;
    } catch (const std::exception& e) {
        printf("ExportToOSD error: %s\n", e.what());
        return false;
    }
}

bool ScaleDefinition::LoadDefinitionJSON(const std::string& jsonPath)
{
    try {
        std::ifstream file(jsonPath);
        if (!file.is_open()) {
            return false;
        }
        json j;
        file >> j;
        return ParseDefinitionFromJSON(j);
    } catch (const std::exception& e) {
        std::cerr << "Error loading scale definition: " << e.what() << std::endl;
        return false;
    }
}

bool ScaleDefinition::ParseDefinitionFromJSON(const json& j)
{
    try {
        // Preserve raw JSON so unknown fields (pages, response_footer, etc.)
        // survive round-trip through load/save
        mRawDefinition = j;

        // Load scale_info
        if (j.contains("scale_info")) {
            auto& info = j["scale_info"];
            if (info.contains("name")) mScaleInfo.name = info["name"];
            if (info.contains("code")) mScaleInfo.code = info["code"];
            if (info.contains("abbreviation")) mScaleInfo.abbreviation = info["abbreviation"];
            if (info.contains("description")) mScaleInfo.description = info["description"];
            if (info.contains("citation")) mScaleInfo.citation = info["citation"];
            if (info.contains("license")) mScaleInfo.license = info["license"];
            if (info.contains("license_explanation")) mScaleInfo.license_explanation = info["license_explanation"];
            if (info.contains("license_url")) mScaleInfo.license_url = info["license_url"];
            if (info.contains("version")) mScaleInfo.version = info["version"];
            if (info.contains("url")) mScaleInfo.url = info["url"];
            if (info.contains("domain")) mScaleInfo.domain = info["domain"];
        }

        // Load parameters
        if (j.contains("parameters")) {
            mParameters.clear();
            for (auto& [key, value] : j["parameters"].items()) {
                ScaleParameter param;
                if (value.contains("type")) param.type = value["type"];
                if (value.contains("default")) {
                    if (value["default"].is_string()) {
                        param.defaultValue = value["default"];
                    } else {
                        param.defaultValue = value["default"].dump();
                    }
                }
                if (value.contains("description")) param.description = value["description"];
                if (value.contains("options") && value["options"].is_array()) {
                    for (const auto& opt : value["options"]) {
                        if (opt.is_string()) {
                            param.options.push_back(opt.get<std::string>());
                        } else {
                            param.options.push_back(opt.dump());
                        }
                    }
                }
                mParameters[key] = param;
            }
        }

        // Load likert_options
        if (j.contains("likert_options")) {
            auto& lo = j["likert_options"];
            if (lo.contains("points")) mLikertOptions.points = lo["points"];
            if (lo.contains("min")) mLikertOptions.min = lo["min"];
            if (lo.contains("max")) mLikertOptions.max = lo["max"];
            if (lo.contains("question_head")) mLikertOptions.question_head = lo["question_head"];
            if (lo.contains("labels")) {
                mLikertOptions.labels = lo["labels"].get<std::vector<std::string>>();
            }
        }

        // Load default_required
        if (j.contains("default_required")) {
            if (j["default_required"].is_boolean()) {
                mDefaultRequired = j["default_required"].get<bool>() ? 1 : 0;
            }
        }

        // Load dimensions
        if (j.contains("dimensions")) {
            mDimensions.clear();
            for (const auto& dj : j["dimensions"]) {
                ScaleDimension d;
                if (dj.contains("id")) d.id = dj["id"];
                if (dj.contains("name")) d.name = dj["name"];
                if (dj.contains("abbreviation")) d.abbreviation = dj["abbreviation"];
                if (dj.contains("description")) d.description = dj["description"];
                if (dj.contains("enabled_param") && !dj["enabled_param"].is_null()) {
                    d.enabled_param = dj["enabled_param"];
                }
                if (dj.contains("selectable") && dj["selectable"].is_boolean()) {
                    d.selectable = dj["selectable"].get<bool>();
                }
                if (dj.contains("default_enabled") && dj["default_enabled"].is_boolean()) {
                    d.default_enabled = dj["default_enabled"].get<bool>();
                }
                if (dj.contains("visible_when") && !dj["visible_when"].is_null()) {
                    bool is_complex = false;
                    ParseVisibleWhen(dj["visible_when"],
                                     d.has_visible_when,
                                     d.visible_when_logic,
                                     d.visible_when,
                                     is_complex);
                    // Dimensions don't support complex mode — just keep conditions
                }
                mDimensions.push_back(d);
            }
        }

        // Load questions — "items" is the OSD/OpenScales key; "questions" is the legacy battery key
        const std::string itemsKey = j.contains("items") ? "items" : (j.contains("questions") ? "questions" : "");
        if (!itemsKey.empty()) {
            mQuestions.clear();
            for (const auto& qj : j[itemsKey]) {
                ScaleQuestion q;
                if (qj.contains("id")) q.id = qj["id"];
                if (qj.contains("text_key")) q.text_key = qj["text_key"];
                if (qj.contains("type")) q.type = qj["type"];
                if (qj.contains("dimension") && !qj["dimension"].is_null()) {
                    q.dimension = qj["dimension"];
                }
                if (qj.contains("coding")) q.coding = qj["coding"];
                if (qj.contains("random_group")) {
                    q.random_group = qj["random_group"];
                } else {
                    // Auto-default: inst and items with visible_when get group 0 (fixed)
                    if (q.type == "inst" || qj.contains("visible_when")) {
                        q.random_group = 0;
                    }
                }

                // Required state
                if (qj.contains("required")) {
                    if (qj["required"].is_boolean()) {
                        q.required_state = qj["required"].get<bool>() ? 1 : 0;
                    }
                }

                // Input validation (C9) — new multi-constraint flat format
                if (qj.contains("validation") && qj["validation"].is_object()) {
                    auto& vj = qj["validation"];
                    auto& val = q.validation;
                    // New format fields
                    if (vj.contains("min_length"))        val.min_length       = vj["min_length"].get<int>();
                    if (vj.contains("max_length"))        val.max_length       = vj["max_length"].get<int>();
                    if (vj.contains("min_length_error"))  val.min_length_error = vj["min_length_error"].get<std::string>();
                    if (vj.contains("max_length_error"))  val.max_length_error = vj["max_length_error"].get<std::string>();
                    if (vj.contains("min_words"))         val.min_words        = vj["min_words"].get<int>();
                    if (vj.contains("max_words"))         val.max_words        = vj["max_words"].get<int>();
                    if (vj.contains("min_words_error"))   val.min_words_error  = vj["min_words_error"].get<std::string>();
                    if (vj.contains("max_words_error"))   val.max_words_error  = vj["max_words_error"].get<std::string>();
                    if (vj.contains("number_min"))  { val.number_min_set = true; val.number_min = vj["number_min"].get<double>(); }
                    if (vj.contains("number_max"))  { val.number_max_set = true; val.number_max = vj["number_max"].get<double>(); }
                    if (vj.contains("number_min_error"))  val.number_min_error = vj["number_min_error"].get<std::string>();
                    if (vj.contains("number_max_error"))  val.number_max_error = vj["number_max_error"].get<std::string>();
                    if (vj.contains("pattern"))           val.pattern          = vj["pattern"].get<std::string>();
                    if (vj.contains("pattern_error"))     val.pattern_error    = vj["pattern_error"].get<std::string>();
                    if (vj.contains("min_selected"))      val.min_selected     = vj["min_selected"].get<int>();
                    if (vj.contains("max_selected"))      val.max_selected     = vj["max_selected"].get<int>();
                    if (vj.contains("min_selected_error")) val.min_selected_error = vj["min_selected_error"].get<std::string>();
                    if (vj.contains("max_selected_error")) val.max_selected_error = vj["max_selected_error"].get<std::string>();
                    // Legacy single-type format — convert on load
                    if (vj.contains("type")) {
                        std::string legacyType = vj["type"].get<std::string>();
                        double legacyMin = vj.value("min", 0.0);
                        double legacyMax = vj.value("max", 0.0);
                        int legacyValue  = vj.value("value", 0);
                        std::string legacyErrKey = vj.value("error_key", std::string(""));
                        if (legacyType == "selection") {
                            if (legacyMin > 0) { val.min_selected = (int)legacyMin; val.min_selected_error = legacyErrKey; }
                            if (legacyMax > 0) { val.max_selected = (int)legacyMax; val.max_selected_error = legacyErrKey; }
                        } else if (legacyType == "number") {
                            if (legacyMin != 0 || legacyMax != 0) {
                                val.number_min_set = true; val.number_min = legacyMin; val.number_min_error = legacyErrKey;
                                val.number_max_set = true; val.number_max = legacyMax; val.number_max_error = legacyErrKey;
                            }
                        } else if (legacyType == "min_length") {
                            val.min_length = legacyValue; val.min_length_error = legacyErrKey;
                        } else if (legacyType == "max_length") {
                            val.max_length = legacyValue; val.max_length_error = legacyErrKey;
                        } else if (legacyType == "pattern") {
                            val.pattern = vj.value("regex", std::string("")); val.pattern_error = legacyErrKey;
                        }
                    }
                }

                // Conditional display
                if (qj.contains("visible_when") && !qj["visible_when"].is_null()) {
                    ParseVisibleWhen(qj["visible_when"],
                                     q.has_visible_when,
                                     q.visible_when_logic,
                                     q.visible_when_simple,
                                     q.visible_when_is_complex);
                }

                // Type-specific fields
                if (qj.contains("likert_points")) q.likert_points = qj["likert_points"];
                if (qj.contains("likert_min")) q.likert_min = qj["likert_min"];
                if (qj.contains("likert_max")) q.likert_max = qj["likert_max"];
                if (qj.contains("likert_reverse")) q.likert_reverse = qj["likert_reverse"].get<bool>();
                if (qj.contains("randomize_options")) q.randomize_options = qj["randomize_options"].get<bool>();
                if (qj.contains("likert_labels") && qj["likert_labels"].is_array()) {
                    q.likert_labels = qj["likert_labels"].get<std::vector<std::string>>();
                }
                if (qj.contains("min")) q.min_value = qj["min"];
                if (qj.contains("max")) q.max_value = qj["max"];
                if (qj.contains("left")) q.left_label = qj["left"];
                else if (qj.contains("min_label")) q.left_label = qj["min_label"];
                if (qj.contains("right")) q.right_label = qj["right"];
                else if (qj.contains("max_label")) q.right_label = qj["max_label"];
                if (qj.contains("orientation")) q.vas_orientation = qj["orientation"];
                if (qj.contains("anchors") && qj["anchors"].is_array()) {
                    q.vas_anchors.clear();
                    for (const auto& a : qj["anchors"]) {
                        ScaleQuestion::VasAnchor va;
                        if (a.contains("value")) va.value = a["value"].get<double>();
                        if (a.contains("label")) va.label = a["label"];
                        q.vas_anchors.push_back(va);
                    }
                }
                if (qj.contains("options")) {
                    // Options can be plain strings OR objects {text_key, value, ...}
                    q.options.clear();
                    for (const auto& opt : qj["options"]) {
                        if (opt.is_string()) {
                            q.options.push_back(opt.get<std::string>());
                        } else if (opt.is_object() && opt.contains("text_key")) {
                            q.options.push_back(opt["text_key"].get<std::string>());
                        }
                    }
                }
                if (qj.contains("correct")) {
                    q.correct = qj["correct"].get<std::vector<std::string>>();
                }
                if (qj.contains("image")) q.image = qj["image"];
                if (qj.contains("columns")) {
                    q.columns = qj["columns"].get<std::vector<std::string>>();
                }
                if (qj.contains("rows")) {
                    q.rows = qj["rows"].get<std::vector<std::string>>();
                }

                // Answer alias (S3 answer piping)
                if (qj.contains("answer_alias") && qj["answer_alias"].is_string())
                    q.answer_alias = qj["answer_alias"];
                if (qj.contains("question_head") && qj["question_head"].is_string())
                    q.question_head = qj["question_head"];

                // Gate (blocking item)
                if (qj.contains("gate") && qj["gate"].is_object()) {
                    const auto& gj = qj["gate"];
                    q.has_gate = true;
                    if (gj.contains("required_value")) q.gate_required_value = gj["required_value"];
                    if (gj.contains("operator")) q.gate_operator = gj["operator"];
                    if (gj.contains("value") && gj["value"].is_number()) q.gate_value = gj["value"].get<double>();
                    if (gj.contains("terminate_message_key")) q.gate_terminate_message_key = gj["terminate_message_key"];
                }

                // Section revisable (section-type only, default true)
                if (q.type == "section" && qj.contains("revisable") && qj["revisable"].is_boolean())
                    q.revisable = qj["revisable"].get<bool>();

                // Section randomize (section-type only, default false)
                if (q.type == "section" && qj.contains("randomize") && qj["randomize"].is_object()) {
                    auto& r = qj["randomize"];
                    if (r.contains("method") && r["method"].is_string() && r["method"] == "shuffle")
                        q.section_randomize = true;
                    if (r.contains("fixed") && r["fixed"].is_array())
                        for (auto& f : r["fixed"]) if (f.is_string()) q.section_randomize_fixed.push_back(f);
                }

                mQuestions.push_back(q);
            }
        }

        // Load scoring
        if (j.contains("scoring")) {
            mScoring.clear();
            for (auto& [key, value] : j["scoring"].items()) {
                DimensionScoring ds;
                if (value.contains("method")) ds.method = value["method"];
                if (value.contains("items")) {
                    ds.items = value["items"].get<std::vector<std::string>>();
                }
                if (value.contains("description")) ds.description = value["description"];
                if (value.contains("weights")) {
                    for (auto& [wkey, wval] : value["weights"].items()) {
                        ds.weights[wkey] = wval.get<double>();
                    }
                }
                if (value.contains("item_coding")) {
                    for (auto& [ikey, ival] : value["item_coding"].items()) {
                        ds.item_coding[ikey] = ival.get<int>();
                    }
                }
                if (value.contains("correct_answers")) {
                    for (auto& [cakey, caval] : value["correct_answers"].items()) {
                        ds.correct_answers[cakey] = caval.get<std::vector<std::string>>();
                    }
                }
                if (value.contains("norms") && value["norms"].contains("thresholds")) {
                    for (const auto& t : value["norms"]["thresholds"]) {
                        NormThreshold nt;
                        if (t.contains("min")) nt.min = t["min"].get<double>();
                        if (t.contains("max")) nt.max = t["max"].get<double>();
                        if (t.contains("label_key")) nt.label = t["label_key"].get<std::string>();
                        else if (t.contains("label")) nt.label = t["label"].get<std::string>();
                        ds.norms.push_back(nt);
                    }
                }
                if (value.contains("transform") && value["transform"].is_array()) {
                    for (const auto& step : value["transform"]) {
                        TransformStep ts;
                        if (step.contains("op")) ts.op = step["op"].get<std::string>();
                        if (step.contains("value") && step["value"].is_number()) {
                            ts.value = step["value"].get<double>();
                        }
                        ds.transform.push_back(ts);
                    }
                }
                if (value.contains("scores")) {
                    ds.scores = value["scores"].get<std::vector<std::string>>();
                }
                mScoring[key] = ds;
            }
        }

        // Load computed variables
        if (j.contains("computed")) {
            mComputed.clear();
            for (auto& [key, value] : j["computed"].items()) {
                ComputedVariable cv;
                if (value.contains("expression")) cv.expression = value["expression"];
                if (value.contains("type")) cv.type = value["type"];
                if (value.contains("norms") && value["norms"].contains("thresholds")) {
                    for (const auto& t : value["norms"]["thresholds"]) {
                        NormThreshold nt;
                        if (t.contains("min")) nt.min = t["min"].get<double>();
                        if (t.contains("max")) nt.max = t["max"].get<double>();
                        if (t.contains("label_key")) nt.label = t["label_key"].get<std::string>();
                        else if (t.contains("label")) nt.label = t["label"].get<std::string>();
                        cv.norms.push_back(nt);
                    }
                }
                mComputed[key] = cv;
            }
        }

        // Load report
        if (j.contains("report")) {
            auto& rj = j["report"];
            if (rj.contains("template")) mReportConfig.template_type = rj["template"];
            if (rj.contains("include")) {
                mReportConfig.include = rj["include"].get<std::vector<std::string>>();
            }
            if (rj.contains("header")) mReportConfig.header = rj["header"];
            if (rj.contains("footer_refs")) {
                mReportConfig.footer_refs = rj["footer_refs"].get<std::vector<std::string>>();
            }
        }

        // Load data_output
        if (j.contains("data_output")) {
            auto& dj = j["data_output"];
            if (dj.contains("individual_file")) mDataOutput.individual_file = dj["individual_file"];
            if (dj.contains("pooled_file")) mDataOutput.pooled_file = dj["pooled_file"];
            if (dj.contains("report_file")) mDataOutput.report_file = dj["report_file"];
            if (dj.contains("columns")) mDataOutput.columns = dj["columns"];
            if (dj.contains("pooled_columns")) mDataOutput.pooled_columns = dj["pooled_columns"];
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing scale definition JSON: " << e.what() << std::endl;
        return false;
    }
}

bool ScaleDefinition::LoadFromOSDBundlePath(const std::string& osdPath)
{
    try {
        std::ifstream file(osdPath);
        if (!file.is_open()) {
            return false;
        }
        json bundle;
        file >> bundle;

        if (!bundle.contains("definition")) {
            return false;
        }
        if (!ParseDefinitionFromJSON(bundle["definition"])) {
            return false;
        }

        // Load translations from the bundle
        if (bundle.contains("translations") && bundle["translations"].is_object()) {
            for (auto& [lang, trans] : bundle["translations"].items()) {
                if (trans.is_object()) {
                    for (auto& [key, value] : trans.items()) {
                        if (value.is_string()) {
                            mTranslations[lang][key] = value.get<std::string>();
                        }
                    }
                }
            }
        }

        mSourceIsOSD = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading OSD bundle: " << e.what() << std::endl;
        return false;
    }
}

bool ScaleDefinition::LoadTranslationJSON(const std::string& jsonPath, const std::string& language)
{
    try {
        std::ifstream file(jsonPath);
        if (!file.is_open()) {
            return false;
        }

        json j;
        file >> j;

        // Load all key-value pairs for this language
        for (auto& [key, value] : j.items()) {
            if (value.is_string()) {
                mTranslations[language][key] = value.get<std::string>();
            }
        }

        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool ScaleDefinition::BuildDefinitionJSONObject(json& outJSON) const
{
    try {
        // Start with raw JSON from original file to preserve unknown fields
        // (pages, response_footer, min_label/max_label, coding, etc.)
        json j = mRawDefinition;

        // Overlay known structured fields from C++ model

        // Save scale_info
        j["scale_info"] = {
            {"name", mScaleInfo.name},
            {"code", mScaleInfo.code},
            {"abbreviation", mScaleInfo.abbreviation},
            {"description", mScaleInfo.description},
            {"citation", mScaleInfo.citation},
            {"license", mScaleInfo.license},
            {"license_explanation", mScaleInfo.license_explanation},
            {"license_url", mScaleInfo.license_url},
            {"version", mScaleInfo.version},
            {"url", mScaleInfo.url}
        };
        if (!mScaleInfo.domain.empty()) {
            j["scale_info"]["domain"] = mScaleInfo.domain;
        }

        // Save parameters
        if (!mParameters.empty()) {
            j["parameters"] = json::object();
            for (const auto& [key, param] : mParameters) {
                j["parameters"][key] = {
                    {"type", param.type},
                    {"default", param.defaultValue},
                    {"description", param.description}
                };
                if (!param.options.empty()) {
                    j["parameters"][key]["options"] = param.options;
                }
            }
        }

        // Save likert_options - merge with existing to preserve unknown keys
        // like response_footer
        if (!j.contains("likert_options")) {
            j["likert_options"] = json::object();
        }
        j["likert_options"]["points"] = mLikertOptions.points;
        // Write question_head unless the source explicitly had a likert_options block
        // without a question_head key (e.g. KDQOL-36 which has "likert_options": {}).
        // New scales (mRawDefinition is empty) always get question_head written.
        bool rawHadLikertOptions = mRawDefinition.contains("likert_options");
        if (!rawHadLikertOptions || mRawDefinition["likert_options"].contains("question_head")) {
            j["likert_options"]["question_head"] = mLikertOptions.question_head;
        }
        j["likert_options"]["labels"] = mLikertOptions.labels;
        if (mLikertOptions.min != -1) {
            j["likert_options"]["min"] = mLikertOptions.min;
        }
        if (mLikertOptions.max != -1) {
            j["likert_options"]["max"] = mLikertOptions.max;
        }

        // Save dimensions
        if (!mDimensions.empty()) {
            j["dimensions"] = json::array();
            for (const auto& d : mDimensions) {
                json dj = {
                    {"id", d.id},
                    {"name", d.name},
                    {"abbreviation", d.abbreviation},
                    {"description", d.description},
                    {"enabled_param", d.enabled_param.empty() ? nullptr : json(d.enabled_param)}
                };
                if (d.selectable) {
                    dj["selectable"] = true;
                    dj["default_enabled"] = d.default_enabled;
                }
                if (d.has_visible_when && !d.visible_when.empty()) {
                    dj["visible_when"] = SerializeVisibleWhen(d.visible_when_logic, d.visible_when);
                }
                j["dimensions"].push_back(dj);
            }
        }

        // Save questions - merge per-question to preserve unknown fields
        // (min_label, max_label, coding, dimension, etc.)
        {
            // Build a map of raw questions by id for merging
            std::map<std::string, json> rawQuestionMap;
            const std::string rawKey = mRawDefinition.contains("items") ? "items" :
                                       (mRawDefinition.contains("questions") ? "questions" : "");
            if (!rawKey.empty()) {
                for (const auto& rq : mRawDefinition[rawKey]) {
                    if (rq.contains("id")) {
                        rawQuestionMap[rq["id"].get<std::string>()] = rq;
                    }
                }
            }

            j["items"] = json::array();
            for (const auto& q : mQuestions) {
                // Start with raw question data if it exists (preserves unknown fields)
                json qj;
                auto it = rawQuestionMap.find(q.id);
                if (it != rawQuestionMap.end()) {
                    qj = it->second;
                }

                // Overlay known fields
                qj["id"] = q.id;
                qj["text_key"] = q.text_key;
                qj["type"] = q.type;
                qj["random_group"] = q.random_group;

                // Required state
                if (q.required_state >= 0) {
                    qj["required"] = (q.required_state == 1);
                } else {
                    qj.erase("required");  // Don't write if using default
                }

                // Input validation (C9) — new multi-constraint flat format
                if (q.validation.HasAnyValidation()) {
                    nlohmann::json vj;
                    const auto& val = q.validation;
                    if (val.min_length >= 0)   { vj["min_length"] = val.min_length; if (!val.min_length_error.empty()) vj["min_length_error"] = val.min_length_error; }
                    if (val.max_length >= 0)   { vj["max_length"] = val.max_length; if (!val.max_length_error.empty()) vj["max_length_error"] = val.max_length_error; }
                    if (val.min_words >= 0)    { vj["min_words"]  = val.min_words;  if (!val.min_words_error.empty())  vj["min_words_error"]  = val.min_words_error; }
                    if (val.max_words >= 0)    { vj["max_words"]  = val.max_words;  if (!val.max_words_error.empty())  vj["max_words_error"]  = val.max_words_error; }
                    if (val.number_min_set)    { vj["number_min"] = val.number_min; if (!val.number_min_error.empty()) vj["number_min_error"] = val.number_min_error; }
                    if (val.number_max_set)    { vj["number_max"] = val.number_max; if (!val.number_max_error.empty()) vj["number_max_error"] = val.number_max_error; }
                    if (!val.pattern.empty())  { vj["pattern"]    = val.pattern;    if (!val.pattern_error.empty())    vj["pattern_error"]    = val.pattern_error; }
                    if (val.min_selected >= 0) { vj["min_selected"] = val.min_selected; if (!val.min_selected_error.empty()) vj["min_selected_error"] = val.min_selected_error; }
                    if (val.max_selected >= 0) { vj["max_selected"] = val.max_selected; if (!val.max_selected_error.empty()) vj["max_selected_error"] = val.max_selected_error; }
                    qj["validation"] = vj;
                } else {
                    qj.erase("validation");
                }

                // Type-specific fields
                if (q.type == "likert") {
                    if (q.likert_points != -1) {
                        qj["likert_points"] = q.likert_points;
                    }
                    if (q.likert_min != -1) {
                        qj["likert_min"] = q.likert_min;
                    }
                    if (q.likert_max != -1) {
                        qj["likert_max"] = q.likert_max;
                    }
                    if (q.likert_reverse) {
                        qj["likert_reverse"] = true;
                    }
                    if (!q.likert_labels.empty()) {
                        qj["likert_labels"] = q.likert_labels;
                    }
                }
                if (q.type == "vas") {
                    qj["min"] = q.min_value;
                    qj["max"] = q.max_value;
                    if (!q.left_label.empty()) {
                        qj["min_label"] = q.left_label;
                    }
                    if (!q.right_label.empty()) {
                        qj["max_label"] = q.right_label;
                    }
                    if (!q.vas_orientation.empty() && q.vas_orientation != "horizontal") {
                        qj["orientation"] = q.vas_orientation;
                    }
                    if (!q.vas_anchors.empty()) {
                        nlohmann::json anchorsArr = nlohmann::json::array();
                        for (const auto& a : q.vas_anchors) {
                            anchorsArr.push_back({{"value", a.value}, {"label", a.label}});
                        }
                        qj["anchors"] = anchorsArr;
                    }
                }
                if (q.type == "multi" || q.type == "multicheck") {
                    if (!q.options.empty()) {
                        // If the original options were objects {text_key, value, ...}, preserve
                        // them from mRawDefinition so scoring values are not lost.
                        bool originalHasObjectOptions = false;
                        auto rawIt = rawQuestionMap.find(q.id);
                        if (rawIt != rawQuestionMap.end()
                            && rawIt->second.contains("options")
                            && rawIt->second["options"].is_array()
                            && !rawIt->second["options"].empty()
                            && rawIt->second["options"][0].is_object()) {
                            originalHasObjectOptions = true;
                        }
                        if (!originalHasObjectOptions) {
                            qj["options"] = q.options;
                        }
                        // If object-format, qj already has the original from rawQuestionMap
                    }
                    if (q.randomize_options) {
                        qj["randomize_options"] = true;
                    }
                    if (!q.correct.empty()) {
                        qj["correct"] = q.correct;
                    }
                }
                if (q.type == "image" || q.type == "imageresponse") {
                    if (!q.image.empty()) {
                        qj["image"] = q.image;
                    }
                }
                if (q.type == "grid") {
                    if (!q.columns.empty()) {
                        qj["columns"] = q.columns;
                    }
                    if (!q.rows.empty()) {
                        qj["rows"] = q.rows;
                    }
                }

                // Conditional display (visible_when)
                if (!q.has_visible_when) {
                    qj.erase("visible_when");
                } else if (q.visible_when_is_complex) {
                    // Leave qj["visible_when"] untouched (from raw JSON)
                } else if (!q.visible_when_simple.empty()) {
                    qj["visible_when"] = SerializeVisibleWhen(q.visible_when_logic, q.visible_when_simple);
                }

                // Answer alias (S3 answer piping)
                if (!q.answer_alias.empty())
                    qj["answer_alias"] = q.answer_alias;
                if (!q.question_head.empty())
                    qj["question_head"] = q.question_head;
                else
                    qj.erase("answer_alias");

                // Gate (blocking item)
                if (q.has_gate && (!q.gate_required_value.empty() || !q.gate_operator.empty())) {
                    json gateObj;
                    if (!q.gate_operator.empty()) {
                        gateObj["operator"] = q.gate_operator;
                        gateObj["value"] = q.gate_value;
                    } else {
                        gateObj["required_value"] = q.gate_required_value;
                    }
                    gateObj["terminate_message_key"] = q.gate_terminate_message_key;
                    qj["gate"] = gateObj;
                } else {
                    qj.erase("gate");
                }

                // Section revisable — only write when false (true is default, omit it)
                if (q.type == "section" && !q.revisable)
                    qj["revisable"] = false;
                else
                    qj.erase("revisable");

                // Section randomize — only write when true (false is default, omit it)
                if (q.type == "section" && q.section_randomize) {
                    nlohmann::json rdm;
                    rdm["method"] = "shuffle";
                    if (!q.section_randomize_fixed.empty())
                        rdm["fixed"] = q.section_randomize_fixed;
                    qj["randomize"] = rdm;
                } else {
                    qj.erase("randomize");
                }

                j["items"].push_back(qj);
            }
        }

        // Save default_required
        if (mDefaultRequired >= 0) {
            j["default_required"] = (mDefaultRequired == 1);
        } else {
            j.erase("default_required");
        }

        // Save scoring
        if (!mScoring.empty()) {
            j["scoring"] = json::object();
            for (const auto& [key, ds] : mScoring) {
                json sj = {
                    {"method", ds.method},
                    {"items", ds.items},
                    {"description", ds.description}
                };
                if (!ds.weights.empty()) {
                    sj["weights"] = ds.weights;
                }
                if (!ds.item_coding.empty()) {
                    sj["item_coding"] = ds.item_coding;
                }
                if (!ds.correct_answers.empty()) {
                    sj["correct_answers"] = json::object();
                    for (const auto& [cakey, caval] : ds.correct_answers) {
                        sj["correct_answers"][cakey] = caval;
                    }
                }
                if (!ds.norms.empty()) {
                    nlohmann::json thresholdsArr = nlohmann::json::array();
                    for (const auto& t : ds.norms) {
                        thresholdsArr.push_back({{"min", t.min}, {"max", t.max}, {"label_key", t.label}});
                    }
                    sj["norms"] = {{"thresholds", thresholdsArr}};
                }
                if (!ds.transform.empty()) {
                    nlohmann::json tArr = nlohmann::json::array();
                    for (const auto& ts : ds.transform) {
                        tArr.push_back({{"op", ts.op}, {"value", ts.value}});
                    }
                    sj["transform"] = tArr;
                }
                if (!ds.scores.empty()) {
                    sj["scores"] = ds.scores;
                }
                j["scoring"][key] = sj;
            }
        }

        // Save computed variables
        if (!mComputed.empty()) {
            j["computed"] = nlohmann::json::object();
            for (const auto& [key, cv] : mComputed) {
                nlohmann::json cvj = {
                    {"expression", cv.expression},
                    {"type", cv.type}
                };
                if (!cv.norms.empty()) {
                    nlohmann::json thresholdsArr = nlohmann::json::array();
                    for (const auto& t : cv.norms) {
                        thresholdsArr.push_back({{"min", t.min}, {"max", t.max}, {"label_key", t.label}});
                    }
                    cvj["norms"] = {{"thresholds", thresholdsArr}};
                }
                j["computed"][key] = cvj;
            }
        }

        // Save report (only if we have meaningful data or it was in original)
        if (!mReportConfig.template_type.empty() || mRawDefinition.contains("report")) {
            j["report"] = {
                {"template", mReportConfig.template_type},
                {"include", mReportConfig.include},
                {"header", mReportConfig.header},
                {"footer_refs", mReportConfig.footer_refs}
            };
        }

        // Save data_output (only if we have meaningful data or it was in original)
        if (!mDataOutput.individual_file.empty() || mRawDefinition.contains("data_output")) {
            j["data_output"] = {
                {"individual_file", mDataOutput.individual_file},
                {"pooled_file", mDataOutput.pooled_file},
                {"report_file", mDataOutput.report_file},
                {"columns", mDataOutput.columns},
                {"pooled_columns", mDataOutput.pooled_columns}
            };
        }

        // Note: "pages", "response_footer" (in likert_options), "min_label",
        // "max_label", "coding", "dimension" and other OSD format fields
        // are preserved automatically from mRawDefinition

        outJSON = std::move(j);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error building scale definition JSON: " << e.what() << std::endl;
        return false;
    }
}

bool ScaleDefinition::SaveDefinitionJSON(const std::string& jsonPath)
{
    try {
        json j;
        if (!BuildDefinitionJSONObject(j)) {
            return false;
        }
        std::ofstream file(jsonPath);
        if (!file.is_open()) {
            return false;
        }
        file << j.dump(2);  // 2-space indent
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving scale definition: " << e.what() << std::endl;
        return false;
    }
}

bool ScaleDefinition::SaveTranslationJSON(const std::string& jsonPath, const std::string& language)
{
    try {
        if (mTranslations.find(language) == mTranslations.end()) {
            return false;  // No translations for this language
        }

        json j;
        for (const auto& [key, value] : mTranslations[language]) {
            j[key] = value;
        }

        std::ofstream file(jsonPath);
        if (!file.is_open()) {
            return false;
        }
        file << j.dump(2);
        file.close();

        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void ScaleDefinition::AddQuestion(const ScaleQuestion& question)
{
    mQuestions.push_back(question);
    mDirty = true;
}

void ScaleDefinition::InsertQuestion(int index, const ScaleQuestion& question)
{
    if (index < 0) index = 0;
    if (index > (int)mQuestions.size()) index = (int)mQuestions.size();
    mQuestions.insert(mQuestions.begin() + index, question);
    mDirty = true;
}

void ScaleDefinition::RemoveQuestion(const std::string& questionID)
{
    auto it = std::remove_if(mQuestions.begin(), mQuestions.end(),
        [&questionID](const ScaleQuestion& q) { return q.id == questionID; });

    if (it != mQuestions.end()) {
        mQuestions.erase(it, mQuestions.end());
        mDirty = true;
    }
}

void ScaleDefinition::MoveQuestion(int fromIndex, int toIndex)
{
    if (fromIndex < 0 || fromIndex >= (int)mQuestions.size() ||
        toIndex < 0 || toIndex >= (int)mQuestions.size()) {
        return;
    }

    if (fromIndex == toIndex) {
        return;
    }

    ScaleQuestion q = mQuestions[fromIndex];
    mQuestions.erase(mQuestions.begin() + fromIndex);
    mQuestions.insert(mQuestions.begin() + toIndex, q);
    mDirty = true;
}

ScaleQuestion* ScaleDefinition::GetQuestion(const std::string& questionID)
{
    for (auto& q : mQuestions) {
        if (q.id == questionID) {
            return &q;
        }
    }
    return nullptr;
}

void ScaleDefinition::AddDimension(const ScaleDimension& dimension)
{
    mDimensions.push_back(dimension);
    mDirty = true;
}

void ScaleDefinition::RemoveDimension(const std::string& dimensionID)
{
    auto it = std::remove_if(mDimensions.begin(), mDimensions.end(),
        [&dimensionID](const ScaleDimension& d) { return d.id == dimensionID; });

    if (it != mDimensions.end()) {
        mDimensions.erase(it, mDimensions.end());
        mDirty = true;
    }
}

ScaleDimension* ScaleDefinition::GetDimension(const std::string& dimensionID)
{
    for (auto& d : mDimensions) {
        if (d.id == dimensionID) {
            return &d;
        }
    }
    return nullptr;
}

void ScaleDefinition::AddTranslation(const std::string& language, const std::string& key, const std::string& value)
{
    mTranslations[language][key] = value;
    mDirty = true;
}

void ScaleDefinition::RemoveTranslation(const std::string& language)
{
    mTranslations.erase(language);
    mDirty = true;
}

std::vector<std::string> ScaleDefinition::GetAvailableLanguages() const
{
    std::vector<std::string> languages;
    for (const auto& [lang, _] : mTranslations) {
        languages.push_back(lang);
    }
    return languages;
}

std::string ScaleDefinition::GetTranslation(const std::string& language, const std::string& key) const
{
    auto langIt = mTranslations.find(language);
    if (langIt != mTranslations.end()) {
        auto keyIt = langIt->second.find(key);
        if (keyIt != langIt->second.end()) {
            return keyIt->second;
        }
    }
    return "";
}

ScaleDefinition::ValidationResult ScaleDefinition::ValidateInternal() const
{
    ValidationResult result;

    // Check scale info
    if (mScaleInfo.code.empty()) {
        result.errors.push_back("Scale code is required");
    }
    if (mScaleInfo.name.empty()) {
        result.errors.push_back("Scale name is required");
    }

    // Check questions
    if (mQuestions.empty()) {
        result.errors.push_back("Scale must have at least one question");
    }

    // Check for duplicate question IDs
    std::map<std::string, int> idCounts;
    for (const auto& q : mQuestions) {
        if (q.id.empty()) {
            result.errors.push_back("Question missing ID");
        } else {
            idCounts[q.id]++;
        }
        if (q.text_key.empty()) {
            result.warnings.push_back("Question " + q.id + " missing text_key");
        }
        if (q.type.empty()) {
            result.errors.push_back("Question " + q.id + " missing type");
        }
    }

    for (const auto& [id, count] : idCounts) {
        if (count > 1) {
            result.errors.push_back("Duplicate question ID: " + id);
        }
    }

    // Check dimensions reference valid questions
    for (const auto& dim : mDimensions) {
        if (dim.id.empty()) {
            result.errors.push_back("Dimension missing ID");
        }
    }

    // Check scoring references valid questions
    for (const auto& [dimId, scoring] : mScoring) {
        for (const auto& qid : scoring.items) {
            bool found = false;
            for (const auto& q : mQuestions) {
                if (q.id == qid) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                result.warnings.push_back("Scoring " + dimId + " references non-existent question: " + qid);
            }
        }
    }

    return result;
}

bool ScaleDefinition::Validate(std::string& errorOutput)
{
    // Use internal validation
    ValidationResult result = ValidateInternal();

    std::stringstream ss;
    if (!result.errors.empty()) {
        ss << "Errors:\n";
        for (const auto& err : result.errors) {
            ss << "  - " << err << "\n";
        }
    }
    if (!result.warnings.empty()) {
        ss << "Warnings:\n";
        for (const auto& warn : result.warnings) {
            ss << "  - " << warn << "\n";
        }
    }

    errorOutput = ss.str();
    return result.IsValid();
}
