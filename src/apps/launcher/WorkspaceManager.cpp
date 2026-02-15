// WorkspaceManager.cpp - PEBL workspace initialization and management
// Copyright (c) 2026 Shane T. Mueller
// Licensed under GPL

#include "WorkspaceManager.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#include <shlobj.h>
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
#else
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>
#endif

WorkspaceManager::WorkspaceManager()
    : mInitialized(false)
{
    // In portable mode, use current directory; otherwise use Documents
    if (IsPortableMode()) {
        mWorkspacePath = GetPortableWorkspacePath();
    } else {
        mWorkspacePath = GetDocumentsPath() + "/pebl-exp." + PEBL_VERSION;
    }
}

WorkspaceManager::~WorkspaceManager() {
}

bool WorkspaceManager::IsPortableMode() const {
    // Priority 1: Check for explicit STANDALONE.txt marker file
    // This is the most reliable trigger - a batch file/script creates this
    // in the working directory before launching
    if (FileExists("./STANDALONE.txt") || FileExists("../STANDALONE.txt") || FileExists("../../STANDALONE.txt")) {
        return true;
    }

    // Priority 2: Check for PORTABLE marker file (legacy support)
    if (FileExists("./PORTABLE.txt") || FileExists("../PORTABLE.txt") || FileExists("../../PORTABLE.txt")) {
        return true;
    }

    // Priority 3: Check for PEBL_PORTABLE environment variable
    // Can be set by a batch file before launching
    const char* portableEnv = getenv("PEBL_PORTABLE");
    if (portableEnv && strcmp(portableEnv, "1") == 0) {
        return true;
    }

    // No automatic detection - only explicit marker files or environment variable
    return false;
}

std::string WorkspaceManager::GetPortableWorkspacePath() const {
    // Determine the correct workspace root for portable mode.
    // The workspace should be at the portable distribution root, not inside PEBL/bin/
    //
    // Expected structure:
    //   PEBL2.3_Portable/             <- workspace root (where STANDALONE.txt is)
    //   ├── STANDALONE.txt            <- REQUIRED marker file
    //   ├── PEBL/
    //   │   ├── bin/
    //   │   │   └── pebl-launcher.exe
    //   │   ├── battery/
    //   │   └── pebl-lib/
    //   ├── my_studies/
    //   ├── chains/
    //   ├── snapshots/
    //   └── runPEBL.bat

    // Determine workspace root based on marker file location
    // Only marker files determine the workspace location - no automatic detection

    // Check if marker file is in current directory (running from root)
    if (FileExists("./STANDALONE.txt") || FileExists("./PORTABLE.txt")) {
        return ".";
    }

    // Check if marker file is one level up
    if (FileExists("../STANDALONE.txt") || FileExists("../PORTABLE.txt")) {
        return "..";
    }

    // Check if marker file is two levels up (we're in PEBL/bin/)
    if (FileExists("../../STANDALONE.txt") || FileExists("../../PORTABLE.txt")) {
        return "../..";
    }

    // If PEBL_PORTABLE env var is set, use current directory
    // Fallback: current directory
    return ".";
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
        if (!CreateDir(mWorkspacePath)) {
            return false;
        }
    }

    // Create subdirectories
    std::vector<std::string> subdirs = {
        "/my_studies",
        "/snapshots",
        "/scales",
        "/doc",
        "/demo",
        "/tutorials",
        "/logs"
    };

    for (const auto& subdir : subdirs) {
        std::string fullPath = mWorkspacePath + subdir;
        if (!DirectoryExists(fullPath)) {
            if (!CreateDir(fullPath)) {
                return false;
            }
        }
    }

    // Create scales subdirectories
    std::string scalesPath = mWorkspacePath + "/scales";
    CreateDir(scalesPath + "/definitions");
    CreateDir(scalesPath + "/translations");

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
    CreateDir(docDest);

    std::vector<std::string> docFiles = {
        std::string("doc/pman/PEBLManual") + PEBL_VERSION + ".pdf",
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

        if (CopyFileContents(srcFile, destFile)) {
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

    try {
        for (const auto& entry : fs::directory_iterator(studiesPath)) {
            if (!entry.is_directory()) {
                continue;
            }

            std::string dirName = entry.path().filename().string();
            std::string fullPath = entry.path().string();

            // Check if directory contains study-info.json
            std::string studyInfoPath = fullPath + "/study-info.json";
            struct stat fileInfo;
            if (stat(studyInfoPath.c_str(), &fileInfo) == 0) {
                studies.push_back(dirName);
            }
        }
    } catch (const fs::filesystem_error&) {
        // Directory doesn't exist or can't be read
    }

    return studies;
}

std::vector<std::string> WorkspaceManager::GetSnapshotDirectories() const {
    std::vector<std::string> snapshots;
    std::string snapshotsPath = GetSnapshotsPath();

    try {
        for (const auto& entry : fs::directory_iterator(snapshotsPath)) {
            if (!entry.is_directory()) {
                continue;
            }

            std::string dirName = entry.path().filename().string();
            std::string fullPath = entry.path().string();

            // Check if directory contains study-info.json
            std::string studyInfoPath = fullPath + "/study-info.json";
            struct stat fileInfo;
            if (stat(studyInfoPath.c_str(), &fileInfo) == 0) {
                snapshots.push_back(dirName);
            }
        }
    } catch (const fs::filesystem_error&) {
        // Directory doesn't exist or can't be read
    }

    return snapshots;
}

bool WorkspaceManager::CreateStudyDirectory(const std::string& studyName) {
    std::string studyPath = GetStudiesPath() + "/" + studyName;

    if (DirectoryExists(studyPath)) {
        return false;  // Already exists
    }

    // Create study directory
    if (!CreateDir(studyPath)) {
        return false;
    }

    // Create subdirectories
    CreateDir(studyPath + "/chains");
    CreateDir(studyPath + "/tests");
    CreateDir(studyPath + "/data");

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

bool WorkspaceManager::CreateDir(const std::string& path) {
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
        if (!parentPath.empty() && CreateDir(parentPath)) {
            return mkdir(path.c_str(), 0755) == 0;
        }
    }

    return false;
}

bool WorkspaceManager::DirectoryExists(const std::string& path) const {
    struct stat info;
    return (stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR));
}

bool WorkspaceManager::FileExists(const std::string& path) const {
    struct stat info;
    return (stat(path.c_str(), &info) == 0 && !(info.st_mode & S_IFDIR));
}

bool WorkspaceManager::CopyDirectory(const std::string& source, const std::string& dest, bool excludeData, const std::vector<std::string>& excludeDirs) {
    // Create destination directory
    if (!CreateDir(dest)) {
        return false;
    }

    bool success = true;

    try {
        for (const auto& entry : fs::directory_iterator(source)) {
            std::string name = entry.path().filename().string();

            // Skip data/ directory if excludeData is true
            if (excludeData && name == "data" && entry.is_directory()) {
                continue;
            }

            // Skip directories in excludeDirs list
            bool skipDir = false;
            for (const auto& excludeDir : excludeDirs) {
                if (name == excludeDir) {
                    skipDir = true;
                    break;
                }
            }
            if (skipDir) {
                continue;
            }

            std::string sourcePath = entry.path().string();
            std::string destPath = dest + "/" + name;

            if (entry.is_directory()) {
                // Recursively copy directory
                if (!CopyDirectory(sourcePath, destPath, excludeData, excludeDirs)) {
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

bool WorkspaceManager::CopyFileContents(const std::string& source, const std::string& dest) {
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
#ifdef _WIN32
    // On Windows, use SHGetFolderPath to get Documents folder
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, path))) {
        return std::string(path);
    }
    // Fallback to USERPROFILE/Documents
    const char* userProfile = getenv("USERPROFILE");
    if (userProfile) {
        return std::string(userProfile) + "\\Documents";
    }
    return "C:\\Users\\Public\\Documents";
#else
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
#endif
}
