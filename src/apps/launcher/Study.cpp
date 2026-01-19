// Study.cpp - PEBL Study data model implementation
// Copyright (c) 2026 Shane T. Mueller
// Licensed under GPL

#include "Study.h"
#include "Chain.h"
#include "../../libs/json.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <sys/stat.h>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#ifndef stat
#define stat _stat
#endif
#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & _S_IFMT) == _S_IFDIR)
#endif
#ifndef S_ISREG
#define S_ISREG(mode) (((mode) & _S_IFMT) == _S_IFREG)
#endif
#ifndef S_IFDIR
#define S_IFDIR _S_IFDIR
#endif
#else
#include <dirent.h>
#endif

using json = nlohmann::json;
namespace fs = std::filesystem;

// ============================================================================
// Test Implementation
// ============================================================================

bool Test::Exists(const std::string& studyPath) const {
    std::string fullPath = studyPath + "/tests/" + testPath;
    struct stat info;
    return (stat(fullPath.c_str(), &info) == 0 && (info.st_mode & S_IFDIR));
}

std::vector<std::string> Test::GetAvailableLanguages(const std::string& studyPath) const {
    std::vector<std::string> languages;
    std::string translationsDir = studyPath + "/tests/" + testPath + "/translations";

    try {
        if (!fs::exists(translationsDir) || !fs::is_directory(translationsDir)) {
            return languages;
        }

        for (const auto& entry : fs::directory_iterator(translationsDir)) {
            if (!entry.is_regular_file()) continue;

            std::string filename = entry.path().filename().string();

            // Look for pattern: testname.pbl-LANG.json
            size_t dashPos = filename.rfind('-');
            size_t jsonPos = filename.rfind(".json");

            if (dashPos != std::string::npos && jsonPos != std::string::npos) {
                std::string lang = filename.substr(dashPos + 1, jsonPos - dashPos - 1);
                if (!lang.empty()) {
                    languages.push_back(lang);
                }
            }
        }
    } catch (const fs::filesystem_error&) {
        // Directory doesn't exist or can't be read
    }

    return languages;
}

const ParameterVariant* Test::GetVariant(const std::string& variantName) const {
    auto it = parameterVariants.find(variantName);
    if (it != parameterVariants.end()) {
        return &(it->second);
    }
    return nullptr;
}

// ============================================================================
// Study Implementation
// ============================================================================

Study::Study()
    : mVersion(1)
{
}

Study::~Study() {
}

std::shared_ptr<Study> Study::LoadFromDirectory(const std::string& path) {
    auto study = std::make_shared<Study>();
    study->mPath = path;

    std::string jsonPath = path + "/study-info.json";
    printf("  Looking for: %s\n", jsonPath.c_str());

    // Check if file exists
    std::ifstream testFile(jsonPath);
    if (!testFile.good()) {
        printf("  File does not exist or cannot be opened\n");
        return nullptr;
    }
    testFile.close();

    if (!study->LoadFromJSON(jsonPath)) {
        printf("  Failed to parse JSON from file\n");
        return nullptr;
    }

    printf("  Successfully loaded study: %s\n", study->GetName().c_str());
    return study;
}

bool Study::Save() {
    std::string jsonPath = mPath + "/study-info.json";
    return SaveToJSON(jsonPath);
}

std::shared_ptr<Study> Study::CreateNew(const std::string& path,
                                         const std::string& name,
                                         const std::string& author) {
    auto study = std::make_shared<Study>();
    study->mPath = path;
    study->mName = name;
    study->mAuthor = author;
    study->mVersion = 1;
    study->mCreatedDate = study->GetCurrentISO8601Time();
    study->mModifiedDate = study->mCreatedDate;

    // Create directory structure using filesystem API (handles paths with spaces)
    try {
        fs::create_directories(path);
        fs::create_directories(fs::path(path) / "chains");
        fs::create_directories(fs::path(path) / "tests");
        // Note: data/ directory removed - not needed in study structure
    } catch (const fs::filesystem_error& e) {
        printf("Error creating study directories: %s\n", e.what());
    }

    // Create default chain
    std::string chainPath = path + "/chains/Main.json";
    auto defaultChain = Chain::CreateNew(chainPath, "Main", "Default chain for this study");
    if (defaultChain) {
        defaultChain->Save();
        printf("Created default chain: Main.json\n");
    }

    return study;
}

bool Study::AddTest(const Test& test) {
    // Check if test already exists
    for (const auto& t : mTests) {
        if (t.testName == test.testName) {
            return false;  // Already exists
        }
    }

    mTests.push_back(test);
    UpdateModifiedDate();
    return true;
}

bool Study::RemoveTest(const std::string& testName) {
    auto it = std::find_if(mTests.begin(), mTests.end(),
                          [&testName](const Test& t) { return t.testName == testName; });

    if (it != mTests.end()) {
        mTests.erase(it);
        UpdateModifiedDate();
        return true;
    }

    return false;
}

Test* Study::GetTest(const std::string& testName) {
    auto it = std::find_if(mTests.begin(), mTests.end(),
                          [&testName](const Test& t) { return t.testName == testName; });

    if (it != mTests.end()) {
        return &(*it);
    }

    return nullptr;
}

const Test* Study::GetTest(const std::string& testName) const {
    auto it = std::find_if(mTests.begin(), mTests.end(),
                          [&testName](const Test& t) { return t.testName == testName; });

    if (it != mTests.end()) {
        return &(*it);
    }

    return nullptr;
}

std::vector<std::string> Study::GetChainFiles() const {
    std::vector<std::string> chains;
    std::string chainsDir = mPath + "/chains";

    try {
        if (!fs::exists(chainsDir) || !fs::is_directory(chainsDir)) {
            return chains;
        }

        for (const auto& entry : fs::directory_iterator(chainsDir)) {
            if (!entry.is_regular_file()) continue;

            std::string filename = entry.path().filename().string();

            // Look for .json files
            if (filename.size() > 5 && filename.substr(filename.size() - 5) == ".json") {
                chains.push_back(filename);
            }
        }
    } catch (const fs::filesystem_error&) {
        // Directory doesn't exist or can't be read
    }

    return chains;
}

int Study::GetChainCount() const {
    return static_cast<int>(GetChainFiles().size());
}

Study::ValidationResult Study::Validate() const {
    ValidationResult result;

    // Check required fields
    if (mName.empty()) {
        result.errors.push_back("Study name is required");
    }

    if (mPath.empty()) {
        result.errors.push_back("Study path is required");
    }

    if (mVersion < 1) {
        result.errors.push_back("Study version must be >= 1");
    }

    // Check directory structure
    struct stat info;
    if (stat(mPath.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR)) {
        result.errors.push_back("Study directory does not exist: " + mPath);
    } else {
        // Check subdirectories
        std::string chainsDir = mPath + "/chains";
        if (stat(chainsDir.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR)) {
            result.warnings.push_back("chains/ directory missing");
        }

        std::string testsDir = mPath + "/tests";
        if (stat(testsDir.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR)) {
            result.warnings.push_back("tests/ directory missing");
        }
    }

    // Validate tests
    for (const auto& test : mTests) {
        if (test.testName.empty()) {
            result.errors.push_back("Test name cannot be empty");
        }

        if (test.testPath.empty()) {
            result.errors.push_back("Test path cannot be empty for test: " + test.testName);
        }

        // Check if test directory exists
        if (!test.Exists(mPath)) {
            result.warnings.push_back("Test directory not found: " + test.testPath);
        }
    }

    return result;
}

bool Study::LoadFromJSON(const std::string& jsonPath) {
    try {
        std::ifstream file(jsonPath);
        if (!file.is_open()) {
            return false;
        }

        json j;
        file >> j;

        // Load required fields
        mName = j.value("study_name", "");
        mVersion = j.value("version", 1);

        // Load optional fields
        mDescription = j.value("description", "");
        mAuthor = j.value("author", "");
        mStudyToken = j.value("study_token", "");
        mUploadServerURL = j.value("upload_server_url", "");
        mCreatedDate = j.value("created_date", "");
        mModifiedDate = j.value("modified_date", "");

        // Load tests
        mTests.clear();
        if (j.contains("tests") && j["tests"].is_array()) {
            for (const auto& testJson : j["tests"]) {
                Test test;
                test.testName = testJson.value("test_name", "");
                test.displayName = testJson.value("display_name", testJson.value("test_name", ""));
                test.testPath = testJson.value("test_path", "");
                test.included = testJson.value("included", true);

                // Load parameter variants
                if (testJson.contains("parameter_variants") && testJson["parameter_variants"].is_object()) {
                    for (auto& [key, value] : testJson["parameter_variants"].items()) {
                        ParameterVariant variant;
                        variant.description = value.value("description", "");
                        variant.file = value.value("file", "");
                        test.parameterVariants[key] = variant;
                    }
                }

                mTests.push_back(test);
            }
        }

        return true;

    } catch (const std::exception& e) {
        // JSON parsing error
        return false;
    }
}

bool Study::SaveToJSON(const std::string& jsonPath) {
    try {
        json j;

        // Save metadata
        j["study_name"] = mName;
        j["description"] = mDescription;
        j["version"] = mVersion;
        j["author"] = mAuthor;
        j["study_token"] = mStudyToken;
        j["upload_server_url"] = mUploadServerURL;
        j["created_date"] = mCreatedDate;
        j["modified_date"] = mModifiedDate;

        // Save tests
        json testsArray = json::array();
        for (const auto& test : mTests) {
            json testJson;
            testJson["test_name"] = test.testName;
            testJson["display_name"] = test.displayName.empty() ? test.testName : test.displayName;
            testJson["test_path"] = test.testPath;
            testJson["included"] = test.included;

            // Save parameter variants
            if (!test.parameterVariants.empty()) {
                json variantsJson;
                for (const auto& [key, variant] : test.parameterVariants) {
                    json variantJson;
                    variantJson["description"] = variant.description;
                    if (!variant.file.empty()) {
                        variantJson["file"] = variant.file;
                    } else {
                        variantJson["file"] = nullptr;
                    }
                    variantsJson[key] = variantJson;
                }
                testJson["parameter_variants"] = variantsJson;
            }

            testsArray.push_back(testJson);
        }
        j["tests"] = testsArray;

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

void Study::UpdateModifiedDate() {
    mModifiedDate = GetCurrentISO8601Time();
}

std::string Study::GetCurrentISO8601Time() const {
    auto now = std::time(nullptr);
    auto tm = *std::gmtime(&now);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

std::string Study::GetStudyCode() const {
    std::string code;
    code.reserve(4);

    // Extract first 4 alphanumeric characters from study name
    for (char c : mName) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            code += std::toupper(static_cast<unsigned char>(c));
            if (code.length() >= 4) {
                break;
            }
        }
    }

    // Pad with 'X' if less than 4 characters
    while (code.length() < 4) {
        code += 'X';
    }

    return code;
}

// ============================================================================
// Upload Configuration Management
// ============================================================================

std::string Study::GetUploadConfigPath(const std::string& testName) const {
    return mPath + "/tests/" + testName + "/upload.json";
}

bool Study::TestHasUploadConfig(const std::string& testName) const {
    std::string uploadPath = GetUploadConfigPath(testName);
    struct stat info;
    return (stat(uploadPath.c_str(), &info) == 0);
}

bool Study::CreateUploadConfigForTest(const std::string& testName) {
    // Validate inputs
    if (mStudyToken.empty()) {
        printf("Cannot create upload config: study token not set\n");
        return false;
    }

    if (mUploadServerURL.empty()) {
        printf("Cannot create upload config: upload server URL not set\n");
        return false;
    }

    // Get test
    const Test* test = GetTest(testName);
    if (!test) {
        printf("Cannot create upload config: test not found: %s\n", testName.c_str());
        return false;
    }

    // Parse server URL to extract host, port, path
    std::string host, page;
    int port = 443;  // Default HTTPS port

    std::string url = mUploadServerURL;

    // Remove protocol
    if (url.find("https://") == 0) {
        url = url.substr(8);
        port = 443;
    } else if (url.find("http://") == 0) {
        url = url.substr(7);
        port = 80;
    }

    // Find first slash to separate host from path
    size_t slashPos = url.find('/');
    if (slashPos != std::string::npos) {
        host = url.substr(0, slashPos);
        page = url.substr(slashPos);
    } else {
        host = url;
        page = "/scripts/uploadPEBL_token.php";  // Default page
    }

    // Check for port in host (host:port format)
    size_t colonPos = host.find(':');
    if (colonPos != std::string::npos) {
        std::string portStr = host.substr(colonPos + 1);
        port = std::atoi(portStr.c_str());
        host = host.substr(0, colonPos);
    }

    // Create upload.json content
    try {
        json j;
        j["host"] = host;
        j["page"] = page;
        j["subnumpage"] = "/api/getNewParticipantId.php";  // For server-assigned participant IDs
        j["port"] = port;
        j["token"] = mStudyToken;
        // Use display name if available, otherwise fall back to test ID
        j["taskname"] = test->displayName.empty() ? testName : test->displayName;
        j["username"] = mStudyToken;
        j["uploadpassword"] = mStudyToken;

        // Write to file
        std::string uploadPath = GetUploadConfigPath(testName);

        // Ensure test directory exists
        std::string testDir = mPath + "/tests/" + testName;
        try {
            fs::create_directories(testDir);
        } catch (const fs::filesystem_error& e) {
            printf("Error creating test directory: %s\n", e.what());
            return false;
        }

        std::ofstream file(uploadPath);
        if (!file.is_open()) {
            printf("Failed to create upload config file: %s\n", uploadPath.c_str());
            return false;
        }

        file << j.dump(2);  // Pretty print with 2-space indent
        file.close();

        printf("Created upload config: %s\n", uploadPath.c_str());
        return true;

    } catch (const std::exception& e) {
        printf("Error creating upload config: %s\n", e.what());
        return false;
    }
}
