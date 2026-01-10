// SnapshotManager.cpp - PEBL snapshot creation and management
// Copyright (c) 2026 Shane T. Mueller
// Licensed under GPL

#include "SnapshotManager.h"
#include "Study.h"
#include "Chain.h"
#include "../../libs/json.hpp"
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

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

    // Validate snapshot first
    auto validation = ValidateSnapshot(snapshotPath);
    if (!validation.isValid) {
        return false;
    }

    // Copy snapshot to studies directory
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

    DIR* dir = opendir(source.c_str());
    if (!dir) {
        return false;
    }

    struct dirent* entry;
    bool success = true;

    while ((entry = readdir(dir)) != nullptr) {
        // Skip . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        std::string sourcePath = source + "/" + entry->d_name;
        std::string destPath = dest + "/" + entry->d_name;

        struct stat fileInfo;
        if (stat(sourcePath.c_str(), &fileInfo) == 0) {
            bool isDirectory = (fileInfo.st_mode & S_IFDIR);

            // Check if should exclude
            if (ShouldExcludeFromSnapshot(entry->d_name, isDirectory) && excludeData) {
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
                if (!CopyFile(sourcePath, destPath)) {
                    success = false;
                    break;
                }
            }
        }
    }

    closedir(dir);
    return success;
}

bool SnapshotManager::CopyFile(const std::string& source, const std::string& dest) {
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

    // Copy file permissions
    struct stat fileInfo;
    if (stat(source.c_str(), &fileInfo) == 0) {
        chmod(dest.c_str(), fileInfo.st_mode);
    }

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
