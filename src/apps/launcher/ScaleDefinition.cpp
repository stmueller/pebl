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

ScaleDefinition::ScaleDefinition()
    : mDirty(false)
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

bool ScaleDefinition::LoadFromScalesDir(const std::string& basePath, const std::string& scaleCode)
{
    // Load from <basePath>/<code>/<code>.json
    std::string scaleDir = basePath + "/" + scaleCode;
    std::string defPath = scaleDir + "/" + scaleCode + ".json";

    if (!LoadDefinitionJSON(defPath)) {
        return false;
    }

    // Load translations from the same directory
    std::string prefix = scaleCode + ".pbl-";

    try {
        if (fs::exists(scaleDir)) {
            for (const auto& entry : fs::directory_iterator(scaleDir)) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();
                    if (filename.find(prefix) == 0 &&
                        filename.size() >= 5 &&
                        filename.substr(filename.size() - 5) == ".json") {
                        std::string lang = filename.substr(prefix.length());
                        lang = lang.substr(0, lang.length() - 5);
                        LoadTranslationJSON(entry.path().string(), lang);
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

    // Save translations
    for (const auto& [lang, translations] : mTranslations) {
        std::string transFile = translationsPath + "/" + mScaleInfo.code + ".pbl-" + lang + ".json";
        if (!SaveTranslationJSON(transFile, lang)) {
            return false;
        }
    }

    mDirty = false;
    return true;
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

        // Load scale_info
        if (j.contains("scale_info")) {
            auto& info = j["scale_info"];
            if (info.contains("name")) mScaleInfo.name = info["name"];
            if (info.contains("code")) mScaleInfo.code = info["code"];
            if (info.contains("abbreviation")) mScaleInfo.abbreviation = info["abbreviation"];
            if (info.contains("description")) mScaleInfo.description = info["description"];
            if (info.contains("citation")) mScaleInfo.citation = info["citation"];
            if (info.contains("license")) mScaleInfo.license = info["license"];
            if (info.contains("version")) mScaleInfo.version = info["version"];
            if (info.contains("url")) mScaleInfo.url = info["url"];
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
                mDimensions.push_back(d);
            }
        }

        // Load questions
        if (j.contains("questions")) {
            mQuestions.clear();
            for (const auto& qj : j["questions"]) {
                ScaleQuestion q;
                if (qj.contains("id")) q.id = qj["id"];
                if (qj.contains("text_key")) q.text_key = qj["text_key"];
                if (qj.contains("type")) q.type = qj["type"];
                if (qj.contains("dimension") && !qj["dimension"].is_null()) {
                    q.dimension = qj["dimension"];
                }
                if (qj.contains("coding")) q.coding = qj["coding"];

                // Type-specific fields
                if (qj.contains("likert_points")) q.likert_points = qj["likert_points"];
                if (qj.contains("likert_min")) q.likert_min = qj["likert_min"];
                if (qj.contains("likert_max")) q.likert_max = qj["likert_max"];
                if (qj.contains("likert_labels") && qj["likert_labels"].is_array()) {
                    q.likert_labels = qj["likert_labels"].get<std::vector<std::string>>();
                }
                if (qj.contains("min")) q.min_value = qj["min"];
                if (qj.contains("max")) q.max_value = qj["max"];
                if (qj.contains("left")) q.left_label = qj["left"];
                if (qj.contains("right")) q.right_label = qj["right"];
                if (qj.contains("options")) {
                    q.options = qj["options"].get<std::vector<std::string>>();
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
                mScoring[key] = ds;
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
        std::cerr << "Error loading scale definition: " << e.what() << std::endl;
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

bool ScaleDefinition::SaveDefinitionJSON(const std::string& jsonPath)
{
    try {
        json j;

        // Save scale_info
        j["scale_info"] = {
            {"name", mScaleInfo.name},
            {"code", mScaleInfo.code},
            {"abbreviation", mScaleInfo.abbreviation},
            {"description", mScaleInfo.description},
            {"citation", mScaleInfo.citation},
            {"license", mScaleInfo.license},
            {"version", mScaleInfo.version},
            {"url", mScaleInfo.url}
        };

        // Save parameters
        if (!mParameters.empty()) {
            j["parameters"] = json::object();
            for (const auto& [key, param] : mParameters) {
                j["parameters"][key] = {
                    {"type", param.type},
                    {"default", param.defaultValue},
                    {"description", param.description}
                };
            }
        }

        // Save likert_options
        j["likert_options"] = {
            {"points", mLikertOptions.points},
            {"question_head", mLikertOptions.question_head},
            {"labels", mLikertOptions.labels}
        };
        // Only save min/max if explicitly set (not using defaults)
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
                j["dimensions"].push_back(dj);
            }
        }

        // Save questions
        j["questions"] = json::array();
        for (const auto& q : mQuestions) {
            json qj = {
                {"id", q.id},
                {"text_key", q.text_key},
                {"type", q.type}
            };

            // Type-specific fields
            if (q.type == "likert") {
                qj["likert_points"] = q.likert_points;
                // Only save min/max if explicitly set (not using defaults)
                if (q.likert_min != -1) {
                    qj["likert_min"] = q.likert_min;
                }
                if (q.likert_max != -1) {
                    qj["likert_max"] = q.likert_max;
                }
                // Only save labels if explicitly set (not empty = use scale default)
                if (!q.likert_labels.empty()) {
                    qj["likert_labels"] = q.likert_labels;
                }
            }
            if (q.type == "vas") {
                qj["min"] = q.min_value;
                qj["max"] = q.max_value;
                qj["left"] = q.left_label;
                qj["right"] = q.right_label;
            }
            if (q.type == "multi" || q.type == "multicheck") {
                if (!q.options.empty()) {
                    qj["options"] = q.options;
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

            j["questions"].push_back(qj);
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
                j["scoring"][key] = sj;
            }
        }

        // Save report
        j["report"] = {
            {"template", mReportConfig.template_type},
            {"include", mReportConfig.include},
            {"header", mReportConfig.header},
            {"footer_refs", mReportConfig.footer_refs}
        };

        // Save data_output
        j["data_output"] = {
            {"individual_file", mDataOutput.individual_file},
            {"pooled_file", mDataOutput.pooled_file},
            {"report_file", mDataOutput.report_file},
            {"columns", mDataOutput.columns},
            {"pooled_columns", mDataOutput.pooled_columns}
        };

        // Write to file with pretty printing
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
