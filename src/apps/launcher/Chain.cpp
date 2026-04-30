// Chain.cpp - PEBL Chain data model implementation
// Copyright (c) 2026 Shane T. Mueller
// Licensed under GPL

#include "Chain.h"
#include "Study.h"
#include "../../libs/json.hpp"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <sys/stat.h>

using json = nlohmann::json;

// ============================================================================
// ItemType Conversion
// ============================================================================

std::string ItemTypeToString(ItemType type) {
    switch (type) {
        case ItemType::Instruction:
            return "instruction";
        case ItemType::Consent:
            return "consent";
        case ItemType::Completion:
            return "completion";
        case ItemType::Test:
            return "test";
        default:
            return "unknown";
    }
}

ItemType StringToItemType(const std::string& str) {
    if (str == "instruction") return ItemType::Instruction;
    if (str == "consent") return ItemType::Consent;
    if (str == "completion") return ItemType::Completion;
    if (str == "test") return ItemType::Test;
    if (str == "scale") return ItemType::Test;  // Platform OSD scale items run as tests
    return ItemType::Instruction;  // Default fallback
}

// ============================================================================
// ChainItem Implementation
// ============================================================================

std::string ChainItem::CreateChainPageConfig(const std::string& tempDir) const {
    // Generate unique filename using timestamp + random
    std::srand(std::time(nullptr));
    int random = std::rand() % 10000;
    std::string uuid = std::to_string(std::time(nullptr)) + "-" + std::to_string(random);
#ifdef _WIN32
    std::string configFile = tempDir + "\\chainpage-" + uuid + ".json";
#else
    std::string configFile = tempDir + "/chainpage-" + uuid + ".json";
#endif

    try {
        json j;
        j["title"] = title;
        j["content"] = content;
        j["page_type"] = ItemTypeToString(type);

        std::ofstream file(configFile);
        if (!file.is_open()) {
            return "";
        }

        file << j.dump(2);  // Pretty print with 2-space indent
        file.close();

        return configFile;

    } catch (const std::exception& e) {
        return "";
    }
}

std::string ChainItem::BuildTestCommand(const std::string& studyPath,
                                         const std::string& subjectID) const {
    std::ostringstream cmd;

    // Base command: pebl2 testPath/testName.pbl
    cmd << "pebl2 " << studyPath << "/tests/" << testName << "/" << testName << ".pbl";

    // Add subject ID
    cmd << " -s " << subjectID;

    // Add language if specified
    if (!language.empty()) {
        cmd << " --language " << language;
    }

    // Note: Parameter variant handling requires Study context to look up the actual filename
    // from the ParameterVariant.file field. This method doesn't have that access.
    // Use LauncherUI::ExecuteChainItem() for proper parameter file handling.
    if (!paramVariant.empty() && paramVariant != "default") {
        // This is incomplete - would need: --pfile params/filename.par.json
        // where filename comes from ParameterVariant.file, not the variant name
        cmd << " --pfile params/" << paramVariant;  // Caller should pass actual filename
    }

    return cmd.str();
}

std::string ChainItem::GetDisplayName() const {
    if (IsPageItem()) {
        return title.empty() ? ItemTypeToString(type) : title;
    } else {
        return testName.empty() ? "Test" : testName;
    }
}

// ============================================================================
// Chain Implementation
// ============================================================================

Chain::Chain()
    : mParticipantCounter(1001), mUploadEnabled(false),
      mLSLEnabled(false), mLSLStreamName("PEBL_{test}")
{
}

Chain::~Chain() {
}

std::shared_ptr<Chain> Chain::LoadFromFile(const std::string& path) {
    auto chain = std::make_shared<Chain>();
    chain->mFilePath = path;

    if (!chain->LoadFromJSON(path)) {
        return nullptr;
    }

    return chain;
}

bool Chain::Save() {
    return SaveToJSON(mFilePath);
}

std::shared_ptr<Chain> Chain::CreateNew(const std::string& path,
                                         const std::string& name,
                                         const std::string& description) {
    auto chain = std::make_shared<Chain>();
    chain->mFilePath = path;
    chain->mName = name;
    chain->mDescription = description;

    return chain;
}

void Chain::AddItem(const ChainItem& item) {
    mItems.push_back(item);
}

void Chain::InsertItem(int index, const ChainItem& item) {
    if (index < 0 || index > static_cast<int>(mItems.size())) {
        return;  // Invalid index
    }

    mItems.insert(mItems.begin() + index, item);
}

bool Chain::RemoveItem(int index) {
    if (index < 0 || index >= static_cast<int>(mItems.size())) {
        return false;  // Invalid index
    }

    mItems.erase(mItems.begin() + index);
    return true;
}

bool Chain::MoveItem(int fromIndex, int toIndex) {
    if (fromIndex < 0 || fromIndex >= static_cast<int>(mItems.size())) {
        return false;  // Invalid source index
    }

    if (toIndex < 0 || toIndex >= static_cast<int>(mItems.size())) {
        return false;  // Invalid destination index
    }

    if (fromIndex == toIndex) {
        return true;  // No-op
    }

    // Extract item
    ChainItem item = mItems[fromIndex];

    // Remove from old position
    mItems.erase(mItems.begin() + fromIndex);

    // Adjust toIndex if needed (removing item may shift indices)
    if (toIndex > fromIndex) {
        toIndex--;
    }

    // Insert at new position
    mItems.insert(mItems.begin() + toIndex, item);

    return true;
}

ChainItem* Chain::GetItem(int index) {
    if (index < 0 || index >= static_cast<int>(mItems.size())) {
        return nullptr;
    }

    return &mItems[index];
}

const ChainItem* Chain::GetItem(int index) const {
    if (index < 0 || index >= static_cast<int>(mItems.size())) {
        return nullptr;
    }

    return &mItems[index];
}

Chain::ValidationResult Chain::Validate(const Study* study) const {
    ValidationResult result;

    // Check required fields
    if (mName.empty()) {
        result.errors.push_back("Chain name is required");
    }

    if (mFilePath.empty()) {
        result.warnings.push_back("Chain file path not set");
    }

    // Check items
    if (mItems.empty()) {
        result.warnings.push_back("Chain has no items");
    }

    for (size_t i = 0; i < mItems.size(); i++) {
        const ChainItem& item = mItems[i];
        std::string prefix = "Item " + std::to_string(i + 1) + ": ";

        if (item.IsPageItem()) {
            // Validate page items
            if (item.title.empty()) {
                result.warnings.push_back(prefix + "Page has no title");
            }

            if (item.content.empty()) {
                result.warnings.push_back(prefix + "Page has no content");
            }

        } else {
            // Validate test items
            if (item.testName.empty()) {
                result.errors.push_back(prefix + "Test name is required");
            }

            // If study is provided, check if test exists in study
            if (study != nullptr) {
                const Test* test = study->GetTest(item.testName);
                if (test == nullptr) {
                    result.errors.push_back(prefix + "Test not found in study: " + item.testName);
                } else {
                    // Check if test is included
                    if (!test->included) {
                        result.warnings.push_back(prefix + "Test is marked as not included: " + item.testName);
                    }

                    // Check parameter variant
                    if (!item.paramVariant.empty() && item.paramVariant != "default") {
                        const ParameterVariant* variant = test->GetVariant(item.paramVariant);
                        if (variant == nullptr) {
                            result.errors.push_back(prefix + "Parameter variant not found: " + item.paramVariant);
                        }
                    }

                    // Check language
                    if (!item.language.empty()) {
                        std::vector<std::string> availableLangs = test->GetAvailableLanguages(study->GetPath());
                        bool langFound = false;
                        for (const auto& lang : availableLangs) {
                            if (lang == item.language) {
                                langFound = true;
                                break;
                            }
                        }
                        if (!langFound && !availableLangs.empty()) {
                            result.warnings.push_back(prefix + "Language not found in translations: " + item.language);
                        }
                    }
                }
            }
        }
    }

    return result;
}

bool Chain::LoadFromJSON(const std::string& jsonPath) {
    try {
        std::ifstream file(jsonPath);
        if (!file.is_open()) {
            return false;
        }

        json j;
        file >> j;

        // Load metadata
        mName = j.value("chain_name", "");
        mDescription = j.value("description", "");
        mParticipantCounter = j.value("participant_counter", 1001);
        mUploadEnabled = j.value("upload_enabled", false);
        mLSLEnabled = j.value("lsl_enabled", false);
        mLSLStreamName = j.value("lsl_stream_name", "PEBL_{test}");

        // Load items
        mItems.clear();
        if (j.contains("items") && j["items"].is_array()) {
            for (const auto& itemJson : j["items"]) {
                std::string itemTypeStr = itemJson.value("item_type", "instruction");
                ItemType itemType = StringToItemType(itemTypeStr);

                ChainItem item(itemType);

                if (item.IsPageItem()) {
                    // Load page item fields (handle null values)
                    item.title = (itemJson.contains("title") && !itemJson["title"].is_null())
                                 ? itemJson["title"].get<std::string>() : "";
                    item.content = (itemJson.contains("content") && !itemJson["content"].is_null())
                                   ? itemJson["content"].get<std::string>() : "";

                } else {
                    // Load test item fields (handle null values)
                    // Platform "scale" items use scale_code + scale_runner instead of test_name.
                    // JS runner scales cannot run in native launcher — skip them.
                    if (itemTypeStr == "scale") {
                        std::string runner = (itemJson.contains("scale_runner") && !itemJson["scale_runner"].is_null())
                                             ? itemJson["scale_runner"].get<std::string>() : "pebl";
                        if (runner == "js") {
                            // JS scales are web-only; skip this item entirely
                            continue;
                        }
                        // PEBL scale: test directory is tests/osd_{CODE}/, .pbl is {CODE}.pbl
                        std::string scaleCode = (itemJson.contains("scale_code") && !itemJson["scale_code"].is_null())
                                                ? itemJson["scale_code"].get<std::string>() : "";
                        item.testName = scaleCode.empty() ? "" : "osd_" + scaleCode;
                    } else {
                        item.testName = (itemJson.contains("test_name") && !itemJson["test_name"].is_null())
                                        ? itemJson["test_name"].get<std::string>() : "";
                    }

                    item.paramVariant = (itemJson.contains("param_variant") && !itemJson["param_variant"].is_null())
                                        ? itemJson["param_variant"].get<std::string>() : "default";
                    item.language = (itemJson.contains("language") && !itemJson["language"].is_null())
                                    ? itemJson["language"].get<std::string>() : "";
                    // Support both "random_group" (launcher) and "randomization_group" (platform)
                    if (itemJson.contains("random_group") && !itemJson["random_group"].is_null()) {
                        item.randomGroup = itemJson["random_group"].get<int>();
                    } else if (itemJson.contains("randomization_group") && !itemJson["randomization_group"].is_null()) {
                        item.randomGroup = itemJson["randomization_group"].get<int>();
                    } else {
                        item.randomGroup = 0;
                    }
                }

                mItems.push_back(item);
            }
        }

        return true;

    } catch (const std::exception& e) {
        return false;
    }
}

bool Chain::SaveToJSON(const std::string& jsonPath) {
    try {
        json j;

        // Save metadata
        j["chain_name"] = mName;
        j["description"] = mDescription;
        j["participant_counter"] = mParticipantCounter;
        j["upload_enabled"] = mUploadEnabled;
        j["lsl_enabled"] = mLSLEnabled;
        j["lsl_stream_name"] = mLSLStreamName;

        // Save items
        json itemsArray = json::array();
        for (const auto& item : mItems) {
            json itemJson;
            itemJson["item_type"] = ItemTypeToString(item.type);

            if (item.IsPageItem()) {
                // Save page item fields
                itemJson["title"] = item.title;
                itemJson["content"] = item.content;

            } else {
                // Save test item fields
                itemJson["test_name"] = item.testName;
                itemJson["param_variant"] = item.paramVariant;
                itemJson["language"] = item.language;
                itemJson["random_group"] = item.randomGroup;
            }

            itemsArray.push_back(itemJson);
        }
        j["items"] = itemsArray;

        // Write to file with pretty printing
        std::ofstream file(jsonPath);
        if (!file.is_open()) {
            return false;
        }

        file << j.dump(2);  // 2-space indentation
        file.close();

        return true;

    } catch (const std::exception& e) {
        return false;
    }
}

void Chain::IncrementParticipantCounter() {
    mParticipantCounter++;
    Save();
}
