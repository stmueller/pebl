// SnapshotManager.cpp - PEBL snapshot creation and management
// Copyright (c) 2026 Shane T. Mueller
// Licensed under GPL

#include "SnapshotManager.h"
#include "Study.h"
#include "Chain.h"
#include "../../libs/json.hpp"
#include <sys/stat.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <filesystem>

namespace fs = std::filesystem;

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
// Undef Windows CopyFile macro to avoid conflict with our function
#ifdef CopyFile
#undef CopyFile
#endif
#ifndef stat
#define stat _stat
#endif
#define mkdir(path, mode) _mkdir(path)
#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & _S_IFMT) == _S_IFDIR)
#endif
#ifndef S_ISREG
#define S_ISREG(mode) (((mode) & _S_IFMT) == _S_IFREG)
#endif
#else
#include <dirent.h>
#include <sys/types.h>
#endif

using json = nlohmann::json;

SnapshotManager::SnapshotManager() {
}

SnapshotManager::~SnapshotManager() {
}

std::string SnapshotManager::CreateSnapshot(const std::string& studyPath,
                                             const std::string& snapshotsDir) {
    // Load study to get metadata
    auto study = Study::LoadFromDirectory(studyPath);
    if (!study) {
        return "";
    }

    // Generate snapshot name: studyname_vN_YYYY-MM-DD
    std::string snapshotName = GenerateSnapshotName(study->GetName(), study->GetVersion());
    std::string snapshotPath = snapshotsDir + "/" + snapshotName;

    // Check if snapshot already exists
    if (DirectoryExists(snapshotPath)) {
        // Append timestamp to make unique
        auto now = std::time(nullptr);
        std::ostringstream oss;
        oss << snapshotPath << "_" << now;
        snapshotPath = oss.str();
    }

    // Copy study directory, excluding data/ directories
    if (!CopyDirectory(studyPath, snapshotPath, true)) {
        return "";
    }

    return snapshotName;
}

SnapshotManager::ValidationResult SnapshotManager::ValidateSnapshot(const std::string& snapshotPath) {
    ValidationResult result;
    result.isValid = true;

    // Check if directory exists
    if (!DirectoryExists(snapshotPath)) {
        result.isValid = false;
        result.errors.push_back("Snapshot directory does not exist");
        return result;
    }

    // Check for study-info.json
    std::string studyInfoPath = snapshotPath + "/study-info.json";
    if (!FileExists(studyInfoPath)) {
        result.isValid = false;
        result.errors.push_back("study-info.json not found");
        return result;
    }

    // Try to load study
    auto study = Study::LoadFromDirectory(snapshotPath);
    if (!study) {
        result.isValid = false;
        result.errors.push_back("Failed to parse study-info.json");
        return result;
    }

    // Validate study structure
    auto studyValidation = study->Validate();
    result.errors.insert(result.errors.end(), studyValidation.errors.begin(), studyValidation.errors.end());
    result.warnings.insert(result.warnings.end(), studyValidation.warnings.begin(), studyValidation.warnings.end());

    // Check for chains/ directory
    std::string chainsDir = snapshotPath + "/chains";
    if (!DirectoryExists(chainsDir)) {
        result.warnings.push_back("chains/ directory not found");
    }

    // Check for tests/ directory
    std::string testsDir = snapshotPath + "/tests";
    if (!DirectoryExists(testsDir)) {
        result.warnings.push_back("tests/ directory not found");
    }

    // Verify no data/ directories (snapshots shouldn't have data)
    std::string dataDir = snapshotPath + "/data";
    if (DirectoryExists(dataDir)) {
        result.warnings.push_back("Snapshot contains data/ directory (should be excluded)");
    }

    // Check each test for data/ subdirectories
    for (const auto& test : study->GetTests()) {
        std::string testDataDir = snapshotPath + "/tests/" + test.testPath + "/data";
        if (DirectoryExists(testDataDir)) {
            result.warnings.push_back("Test " + test.testName + " contains data/ directory");
        }
    }

    result.isValid = result.errors.empty();
    return result;
}

bool SnapshotManager::ImportSnapshot(const std::string& snapshotPath,
                                      const std::string& studiesDir,
                                      const std::string& newStudyName) {
    std::string studyPath = studiesDir + "/" + newStudyName;

    // Check if study already exists
    if (DirectoryExists(studyPath)) {
        return false;
    }

    // Copy snapshot to studies directory
    // Note: For ZIP imports, format conversion happens before this is called
    // For directory imports, we need to convert after copying
    if (!CopyDirectory(snapshotPath, studyPath, false)) {
        return false;
    }

    // Create data/ directory (snapshots don't have it)
    std::string dataDir = studyPath + "/data";
    if (!DirectoryExists(dataDir)) {
        mkdir(dataDir.c_str(), 0755);
    }

    return true;
}

std::string SnapshotManager::GenerateSnapshotName(const std::string& studyName, int version) {
    // Replace spaces with hyphens, convert to lowercase
    std::string safeName = studyName;
    for (char& c : safeName) {
        if (c == ' ') c = '-';
        else c = std::tolower(c);
    }

    // Get current date
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);

    std::ostringstream oss;
    oss << safeName << "_v" << version << "_"
        << std::put_time(&tm, "%Y-%m-%d");

    return oss.str();
}

SnapshotManager::SnapshotInfo SnapshotManager::GetSnapshotInfo(const std::string& snapshotPath) {
    SnapshotInfo info;
    info.version = 0;
    info.testCount = 0;
    info.chainCount = 0;

    auto study = Study::LoadFromDirectory(snapshotPath);
    if (!study) {
        return info;
    }

    info.studyName = study->GetName();
    info.version = study->GetVersion();
    info.description = study->GetDescription();
    info.author = study->GetAuthor();
    info.createdDate = study->GetCreatedDate();
    info.testCount = static_cast<int>(study->GetTests().size());
    info.chainCount = study->GetChainCount();

    return info;
}

bool SnapshotManager::CopyDirectory(const std::string& source, const std::string& dest, bool excludeData) {
    // Create destination directory
    struct stat info;
    if (stat(dest.c_str(), &info) != 0) {
        if (mkdir(dest.c_str(), 0755) != 0) {
            return false;
        }
    }

    bool success = true;

    try {
        for (const auto& entry : fs::directory_iterator(source)) {
            std::string name = entry.path().filename().string();
            std::string sourcePath = entry.path().string();
            std::string destPath = dest + "/" + name;

            bool isDirectory = entry.is_directory();

            // Check if should exclude
            if (ShouldExcludeFromSnapshot(name, isDirectory) && excludeData) {
                continue;
            }

            if (isDirectory) {
                // Recursively copy directory
                if (!CopyDirectory(sourcePath, destPath, excludeData)) {
                    success = false;
                    break;
                }
            } else {
                // Copy file
                if (!CopyFileContents(sourcePath, destPath)) {
                    success = false;
                    break;
                }
            }
        }
    } catch (const fs::filesystem_error&) {
        return false;
    }

    return success;
}

bool SnapshotManager::CopyFileContents(const std::string& source, const std::string& dest) {
    std::ifstream src(source, std::ios::binary);
    if (!src.is_open()) {
        return false;
    }

    std::ofstream dst(dest, std::ios::binary);
    if (!dst.is_open()) {
        return false;
    }

    dst << src.rdbuf();

    src.close();
    dst.close();

#ifndef _WIN32
    // Copy file permissions (POSIX only)
    struct stat fileInfo;
    if (stat(source.c_str(), &fileInfo) == 0) {
        chmod(dest.c_str(), fileInfo.st_mode);
    }
#endif

    return true;
}

bool SnapshotManager::DirectoryExists(const std::string& path) const {
    struct stat info;
    return (stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR));
}

bool SnapshotManager::FileExists(const std::string& path) const {
    struct stat info;
    return (stat(path.c_str(), &info) == 0 && !(info.st_mode & S_IFDIR));
}

std::string SnapshotManager::GetCurrentDateString() const {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str();
}

bool SnapshotManager::ShouldExcludeFromSnapshot(const std::string& name, bool isDirectory) const {
    // Exclude data/ directories
    if (isDirectory && name == "data") {
        return true;
    }

    // Exclude hidden files/directories
    if (!name.empty() && name[0] == '.') {
        return true;
    }

    // Exclude temporary files
    if (name.find("~") != std::string::npos) {
        return true;
    }

    if (name.find(".tmp") != std::string::npos) {
        return true;
    }

    return false;
}

bool SnapshotManager::ConvertSnapshotFormat(const std::string& studyPath) {
    /**
     * Convert PEBLOnlinePlatform snapshot format to launcher format
     * Platform format has:
     *   - version: string (e.g., "Version 3")
     *   - test_id instead of test_name in tests array
     *   - test_name as display name
     * Launcher format needs:
     *   - version: integer
     *   - test_name as identifier
     *   - display_name as readable name
     */
    try {
        std::string jsonPath = studyPath + "/study-info.json";

        // Read existing study-info.json
        std::ifstream file(jsonPath);
        if (!file.is_open()) {
            return false;
        }

        json platformJson;
        file >> platformJson;
        file.close();

        // Create launcher-compatible JSON
        json launcherJson;

        // Convert metadata
        launcherJson["study_name"] = platformJson.value("study_name", "Imported Study");
        launcherJson["description"] = platformJson.value("description", "");
        launcherJson["author"] = platformJson.value("created_by", "");

        // Convert version from string to integer
        std::string versionStr = platformJson.value("version", "1");
        int versionInt = 1;
        // Try to extract number from strings like "Version 3"
        size_t lastSpace = versionStr.rfind(' ');
        if (lastSpace != std::string::npos) {
            std::string numPart = versionStr.substr(lastSpace + 1);
            versionInt = std::atoi(numPart.c_str());
            if (versionInt == 0) versionInt = 1;
        }
        launcherJson["version"] = versionInt;

        // Convert tests array
        json testsArray = json::array();
        if (platformJson.contains("tests") && platformJson["tests"].is_array()) {
            for (const auto& platformTest : platformJson["tests"]) {
                json launcherTest;

                // In platform format: test_id is the identifier, test_name is display name
                std::string testId = platformTest.value("test_id", "");
                std::string testDisplayName = platformTest.value("test_name", testId);

                launcherTest["test_name"] = testId;  // Launcher uses test_name as identifier
                launcherTest["display_name"] = testDisplayName;
                launcherTest["test_path"] = testId;  // Path is same as identifier
                launcherTest["included"] = true;

                // Store main_file so ExecuteChainItem can find the correct .pbl
                // (OSD scales: test dir is osd_CODE but .pbl is CODE.pbl)
                std::string mainFile = platformTest.value("main_file", "");
                if (!mainFile.empty()) {
                    launcherTest["main_file"] = mainFile;
                }

                // Build parameter_variants by scanning params/ directory for .par.json files
                json paramVariants = json::object();
                std::string paramsDir = studyPath + "/tests/" + testId + "/params";
                if (DirectoryExists(paramsDir)) {
                    // Always add a "default" variant
                    json defaultVariant;
                    defaultVariant["description"] = "Default parameters";
                    defaultVariant["file"] = nullptr;
                    paramVariants["default"] = defaultVariant;

                    // Scan for .par.json files
                    for (const auto& entry : fs::directory_iterator(paramsDir)) {
                        if (entry.is_regular_file()) {
                            std::string filename = entry.path().filename().string();
                            // Look for .par.json files
                            if (filename.size() > 9 && filename.substr(filename.size() - 9) == ".par.json") {
                                // Extract variant name (remove .par.json extension)
                                std::string variantName = filename.substr(0, filename.size() - 9);
                                json variant;
                                variant["description"] = variantName;
                                variant["file"] = filename;
                                paramVariants[variantName] = variant;
                                printf("Found parameter variant: %s -> %s\n", variantName.c_str(), filename.c_str());
                            }
                        }
                    }
                }
                launcherTest["parameter_variants"] = paramVariants;

                testsArray.push_back(launcherTest);
            }
        }
        launcherJson["tests"] = testsArray;

        // Upload config must be set intentionally via Study Settings;
        // do not auto-populate from upload.json files in test directories
        launcherJson["study_token"] = "";
        launcherJson["upload_server_url"] = "";
        launcherJson["created_date"] = platformJson.value("created_at", "");
        launcherJson["modified_date"] = platformJson.value("created_at", "");

        // Write converted JSON back to file
        std::ofstream outFile(jsonPath);
        if (!outFile.is_open()) {
            return false;
        }

        outFile << launcherJson.dump(2);  // 2-space indentation
        outFile.close();

        printf("Converted snapshot format to launcher format\n");
        return true;

    } catch (const std::exception& e) {
        printf("Error converting snapshot format: %s\n", e.what());
        return false;
    }
}
