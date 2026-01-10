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
    std::string configFile = tempDir + "/chainpage-" + uuid + ".json";

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
    cmd << " -v subnum=" << subjectID;

    // Add language if specified
    if (!language.empty()) {
        cmd << " -v language=" << language;
    }

    // Add parameter variant file if not default
    if (!paramVariant.empty() && paramVariant != "default") {
        cmd << " -v gParamFile=" << paramVariant;
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

Chain::Chain() {
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

        // Load items
        mItems.clear();
        if (j.contains("items") && j["items"].is_array()) {
            for (const auto& itemJson : j["items"]) {
                std::string itemTypeStr = itemJson.value("item_type", "instruction");
                ItemType itemType = StringToItemType(itemTypeStr);

                ChainItem item(itemType);

                if (item.IsPageItem()) {
                    // Load page item fields
                    item.title = itemJson.value("title", "");
                    item.content = itemJson.value("content", "");

                } else {
                    // Load test item fields
                    item.testName = itemJson.value("test_name", "");
                    item.paramVariant = itemJson.value("param_variant", "default");
                    item.language = itemJson.value("language", "");
                    item.randomGroup = itemJson.value("random_group", 0);
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
