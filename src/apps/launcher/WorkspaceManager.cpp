// WorkspaceManager.cpp - PEBL workspace initialization and management
// Copyright (c) 2026 Shane T. Mueller
// Licensed under GPL

#include "WorkspaceManager.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

WorkspaceManager::WorkspaceManager()
    : mInitialized(false)
{
    // In portable mode, use current directory; otherwise use Documents
    if (IsPortableMode()) {
        mWorkspacePath = ".";  // Current directory
    } else {
        mWorkspacePath = GetDocumentsPath() + "/pebl-exp.2.3";
    }
}

WorkspaceManager::~WorkspaceManager() {
}

bool WorkspaceManager::IsPortableMode() const {
    // Check for PORTABLE marker file in executable directory
    if (DirectoryExists("./PORTABLE") || DirectoryExists("../PORTABLE")) {
        return true;
    }

    // Check for PEBL_PORTABLE environment variable
    const char* portableEnv = getenv("PEBL_PORTABLE");
    if (portableEnv && strcmp(portableEnv, "1") == 0) {
        return true;
    }

    return false;
}

bool WorkspaceManager::IsFirstRun() const {
    // In portable mode, workspace always "exists" (current directory)
    if (IsPortableMode()) {
        return false;
    }

    // Check if workspace directory exists and has content
    if (!DirectoryExists(mWorkspacePath)) {
        return true;
    }

    // Check if my_studies exists (primary indicator)
    std::string studiesPath = mWorkspacePath + "/my_studies";
    return !DirectoryExists(studiesPath);
}

bool WorkspaceManager::Initialize() {
    // Skip workspace creation in portable mode
    if (IsPortableMode()) {
        mInitialized = true;
        return true;
    }

    // Create main workspace directory
    if (!DirectoryExists(mWorkspacePath)) {
        if (!CreateDirectory(mWorkspacePath)) {
            return false;
        }
    }

    // Create subdirectories
    std::vector<std::string> subdirs = {
        "/my_studies",
        "/snapshots",
        "/doc",
        "/demo",
        "/tutorials",
        "/logs"
    };

    for (const auto& subdir : subdirs) {
        std::string fullPath = mWorkspacePath + subdir;
        if (!DirectoryExists(fullPath)) {
            if (!CreateDirectory(fullPath)) {
                return false;
            }
        }
    }

    mInitialized = true;
    return true;
}

bool WorkspaceManager::CopyResources(const std::string& installationPath) {
    if (installationPath.empty() || IsPortableMode()) {
        std::cout << "CopyResources: skipped (empty path or portable mode)" << std::endl;
        return false;
    }

    std::cout << "CopyResources: installation path = " << installationPath << std::endl;
    std::cout << "CopyResources: workspace path = " << mWorkspacePath << std::endl;

    // Copy specific documentation files
    std::string docDest = mWorkspacePath + "/doc";
    CreateDirectory(docDest);

    std::vector<std::string> docFiles = {
        "doc/pman/PEBLManual2.3.pdf",
        "doc/ReleaseNotes.txt",
        "Notes_for_LLMs.txt"
    };

    std::cout << "Copying documentation files..." << std::endl;
    for (const auto& relPath : docFiles) {
        std::string srcFile = installationPath + "/" + relPath;
        // Flatten the structure - put all files directly in doc/
        std::string filename = relPath;
        size_t lastSlash = filename.find_last_of('/');
        if (lastSlash != std::string::npos) {
            filename = filename.substr(lastSlash + 1);
        }
        std::string destFile = docDest + "/" + filename;

        if (CopyFile(srcFile, destFile)) {
            std::cout << "  ✓ Copied " << filename << std::endl;
        } else {
            std::cout << "  ✗ Failed to copy " << relPath << std::endl;
        }
    }

    // Copy demos (entire directory, excluding tests subdirectory)
    std::string demoSource = installationPath + "/demo";
    std::string demoDest = mWorkspacePath + "/demo";
    std::cout << "Checking for demo: " << demoSource << std::endl;
    if (DirectoryExists(demoSource)) {
        std::cout << "Copying demos from " << demoSource << " to " << demoDest << " (excluding tests/)" << std::endl;
        std::vector<std::string> excludeDirs = {"tests"};
        if (CopyDirectory(demoSource, demoDest, false, excludeDirs)) {
            std::cout << "  ✓ Demos copied successfully" << std::endl;
        } else {
            std::cout << "  ✗ Failed to copy demos" << std::endl;
        }
    } else {
        std::cout << "  Demo source not found" << std::endl;
    }

    // Copy tutorials (entire directory)
    std::string tutorialSource = installationPath + "/tutorials";
    std::string tutorialDest = mWorkspacePath + "/tutorials";
    std::cout << "Checking for tutorials: " << tutorialSource << std::endl;
    if (DirectoryExists(tutorialSource)) {
        std::cout << "Copying tutorials from " << tutorialSource << " to " << tutorialDest << std::endl;
        if (CopyDirectory(tutorialSource, tutorialDest, false)) {
            std::cout << "  ✓ Tutorials copied successfully" << std::endl;
        } else {
            std::cout << "  ✗ Failed to copy tutorials" << std::endl;
        }
    } else {
        std::cout << "  Tutorials source not found" << std::endl;
    }

    return true;
}

bool WorkspaceManager::IsInitialized() const {
    return mInitialized || DirectoryExists(mWorkspacePath);
}

std::vector<std::string> WorkspaceManager::GetStudyDirectories() const {
    std::vector<std::string> studies;
    std::string studiesPath = GetStudiesPath();

    DIR* dir = opendir(studiesPath.c_str());
    if (!dir) {
        return studies;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        // Skip . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        std::string fullPath = studiesPath + "/" + entry->d_name;
        struct stat info;
        if (stat(fullPath.c_str(), &info) == 0 && (info.st_mode & S_IFDIR)) {
            // Check if directory contains study-info.json
            std::string studyInfoPath = fullPath + "/study-info.json";
            if (DirectoryExists(fullPath)) {
                struct stat fileInfo;
                if (stat(studyInfoPath.c_str(), &fileInfo) == 0) {
                    studies.push_back(entry->d_name);
                }
            }
        }
    }

    closedir(dir);
    return studies;
}

std::vector<std::string> WorkspaceManager::GetSnapshotDirectories() const {
    std::vector<std::string> snapshots;
    std::string snapshotsPath = GetSnapshotsPath();

    DIR* dir = opendir(snapshotsPath.c_str());
    if (!dir) {
        return snapshots;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        // Skip . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        std::string fullPath = snapshotsPath + "/" + entry->d_name;
        struct stat info;
        if (stat(fullPath.c_str(), &info) == 0 && (info.st_mode & S_IFDIR)) {
            // Check if directory contains study-info.json
            std::string studyInfoPath = fullPath + "/study-info.json";
            struct stat fileInfo;
            if (stat(studyInfoPath.c_str(), &fileInfo) == 0) {
                snapshots.push_back(entry->d_name);
            }
        }
    }

    closedir(dir);
    return snapshots;
}

bool WorkspaceManager::CreateStudyDirectory(const std::string& studyName) {
    std::string studyPath = GetStudiesPath() + "/" + studyName;

    if (DirectoryExists(studyPath)) {
        return false;  // Already exists
    }

    // Create study directory
    if (!CreateDirectory(studyPath)) {
        return false;
    }

    // Create subdirectories
    CreateDirectory(studyPath + "/chains");
    CreateDirectory(studyPath + "/tests");
    CreateDirectory(studyPath + "/data");

    return true;
}

bool WorkspaceManager::CreateSnapshot(const std::string& studyPath, const std::string& snapshotName) {
    std::string snapshotPath = GetSnapshotsPath() + "/" + snapshotName;

    if (DirectoryExists(snapshotPath)) {
        return false;  // Already exists
    }

    // Copy study directory, excluding data/ subdirectories
    return CopyDirectory(studyPath, snapshotPath, true);
}

bool WorkspaceManager::ImportSnapshot(const std::string& snapshotPath, const std::string& newStudyName) {
    std::string studyPath = GetStudiesPath() + "/" + newStudyName;

    if (DirectoryExists(studyPath)) {
        return false;  // Already exists
    }

    // Copy snapshot directory (includes everything since snapshots don't have data/)
    return CopyDirectory(snapshotPath, studyPath, false);
}

bool WorkspaceManager::CreateDirectory(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) == 0) {
        return (info.st_mode & S_IFDIR) != 0;  // Already exists
    }

    // Create directory with permissions 0755
    if (mkdir(path.c_str(), 0755) == 0) {
        return true;
    }

    // Try to create parent directories recursively
    size_t pos = path.rfind('/');
    if (pos != std::string::npos) {
        std::string parentPath = path.substr(0, pos);
        if (!parentPath.empty() && CreateDirectory(parentPath)) {
            return mkdir(path.c_str(), 0755) == 0;
        }
    }

    return false;
}

bool WorkspaceManager::DirectoryExists(const std::string& path) const {
    struct stat info;
    return (stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR));
}

bool WorkspaceManager::CopyDirectory(const std::string& source, const std::string& dest, bool excludeData, const std::vector<std::string>& excludeDirs) {
    // Create destination directory
    if (!CreateDirectory(dest)) {
        return false;
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

        // Skip data/ directory if excludeData is true
        if (excludeData && strcmp(entry->d_name, "data") == 0) {
            continue;
        }

        // Skip directories in excludeDirs list
        bool skipDir = false;
        for (const auto& excludeDir : excludeDirs) {
            if (strcmp(entry->d_name, excludeDir.c_str()) == 0) {
                skipDir = true;
                break;
            }
        }
        if (skipDir) {
            continue;
        }

        std::string sourcePath = source + "/" + entry->d_name;
        std::string destPath = dest + "/" + entry->d_name;

        struct stat info;
        if (stat(sourcePath.c_str(), &info) == 0) {
            if (info.st_mode & S_IFDIR) {
                // Recursively copy directory
                if (!CopyDirectory(sourcePath, destPath, excludeData, excludeDirs)) {
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

bool WorkspaceManager::CopyFile(const std::string& source, const std::string& dest) {
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

    return true;
}

std::string WorkspaceManager::GetDocumentsPath() const {
    // Try to get Documents directory from environment or standard locations
    const char* home = getenv("HOME");
    if (home) {
        // Check for XDG_DOCUMENTS_DIR first
        std::string xdgConfigPath = std::string(home) + "/.config/user-dirs.dirs";
        std::ifstream xdgConfig(xdgConfigPath);
        if (xdgConfig.is_open()) {
            std::string line;
            while (std::getline(xdgConfig, line)) {
                if (line.find("XDG_DOCUMENTS_DIR=") == 0) {
                    size_t start = line.find('"');
                    size_t end = line.rfind('"');
                    if (start != std::string::npos && end != std::string::npos && end > start) {
                        std::string docPath = line.substr(start + 1, end - start - 1);
                        // Replace $HOME with actual home path
                        size_t homePos = docPath.find("$HOME");
                        if (homePos != std::string::npos) {
                            docPath.replace(homePos, 5, home);
                        }
                        return docPath;
                    }
                }
            }
        }

        // Fallback to ~/Documents
        return std::string(home) + "/Documents";
    }

    // Last resort fallback
    return "/tmp/pebl-workspace";
}
