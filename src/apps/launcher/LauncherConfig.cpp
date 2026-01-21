// LauncherConfig.cpp - Configuration management implementation
// Copyright (c) 2026 Shane T. Mueller
// Licensed under GPL

#include "LauncherConfig.h"
#include "BinReloc.h"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <filesystem>

namespace fs = std::filesystem;

#include <sys/stat.h>
#include <sys/types.h>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#include <direct.h>
// Windows compatibility for stat
#ifndef stat
#define stat _stat
#endif
#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & _S_IFMT) == _S_IFDIR)
#endif
#else
#include <unistd.h>
#include <pwd.h>
#endif

LauncherConfig::LauncherConfig()
    : mExperimentDirectory("")
    , mSubjectCode("test")
    , mLanguage("en")
    , mFullscreen(false)
    , mAutoUpload(false)
    , mUploadToken("")
    , mUploadURL("https://peblhub.online/api/upload")
    , mWorkspacePath("")
    , mBatteryPath("")
    , mPeblExecutablePath("")
    , mDataOutputPath("")
    , mFontSize(16)
    , mWindowWidth(1280)
    , mWindowHeight(720)
    , mCurrentStudyPath("")
    , mCurrentChainName("")
{
    // Set platform-specific default external editor
#ifdef _WIN32
    mExternalEditor = "start";
#elif __APPLE__
    mExternalEditor = "open";
#else
    mExternalEditor = "xdg-open";
#endif

    // Detect battery path using BinReloc (relative to launcher executable)
    mBatteryPath = DetectPEBLInstallation();

    // For backward compatibility, also set experiment directory to battery path
    mExperimentDirectory = mBatteryPath;

    // Detect PEBL executable path (same directory as launcher)
#ifdef _WIN32
    char exePath[MAX_PATH];
    DWORD len = GetModuleFileNameA(NULL, exePath, MAX_PATH);
    if (len > 0 && len < MAX_PATH) {
        std::string exePathStr(exePath);
        size_t lastSep = exePathStr.find_last_of("/\\");
        if (lastSep != std::string::npos) {
            std::string exeDir = exePathStr.substr(0, lastSep);
            mPeblExecutablePath = exeDir + "\\pebl2.exe";
            struct stat st;
            if (stat(mPeblExecutablePath.c_str(), &st) == 0) {
                printf("Found PEBL executable at: %s\n", mPeblExecutablePath.c_str());
            } else {
                printf("Warning: PEBL executable not found at: %s\n", mPeblExecutablePath.c_str());
                mPeblExecutablePath = "";
            }
        }
    }
#else
    // On Linux, use BinReloc to find launcher location
    BrInitError error;
    if (br_init(&error) != 0) {
        char* exeDir = br_find_exe_dir("/usr/local/bin");
        if (exeDir) {
            mPeblExecutablePath = std::string(exeDir) + "/pebl2";
            free(exeDir);
            struct stat st;
            if (stat(mPeblExecutablePath.c_str(), &st) == 0) {
                printf("Found PEBL executable at: %s\n", mPeblExecutablePath.c_str());
            } else {
                printf("Warning: PEBL executable not found at: %s\n", mPeblExecutablePath.c_str());
                mPeblExecutablePath = "";
            }
        }
    }
#endif

    // Set workspace path based on mode
    if (IsPortableMode()) {
        // In portable mode, workspace is the portable root directory
        // This allows access to PEBL/battery, PEBL/demo, PEBL/tutorial, etc.
        std::string portableRoot = GetPortableWorkspacePath();

        // Convert relative path to absolute
#ifdef _WIN32
        char absPath[MAX_PATH];
        if (GetFullPathNameA(portableRoot.c_str(), MAX_PATH, absPath, NULL) > 0) {
            mWorkspacePath = absPath;
        } else {
            mWorkspacePath = portableRoot;
        }
#else
        char* absPath = realpath(portableRoot.c_str(), nullptr);
        if (absPath) {
            mWorkspacePath = absPath;
            free(absPath);
        } else {
            mWorkspacePath = portableRoot;
        }
#endif
        printf("Portable mode: workspace set to %s\n", mWorkspacePath.c_str());

        // In portable mode, data output goes to workspace/my_studies
        if (!mWorkspacePath.empty()) {
#ifdef _WIN32
            mDataOutputPath = mWorkspacePath + "\\my_studies";
#else
            mDataOutputPath = mWorkspacePath + "/my_studies";
#endif
        }
    } else {
        // Installed mode: use Documents/pebl-exp.2.3/
        std::string documentsPath = GetDocumentsPath();
        if (!documentsPath.empty()) {
#ifdef _WIN32
            mWorkspacePath = documentsPath + "\\pebl-exp.2.3";
#else
            mWorkspacePath = documentsPath + "/pebl-exp.2.3";
#endif
        }

        // Set default data output path (workspace/my_studies)
        if (!mWorkspacePath.empty()) {
#ifdef _WIN32
            mDataOutputPath = mWorkspacePath + "\\my_studies";
#else
            mDataOutputPath = mWorkspacePath + "/my_studies";
#endif
        }
    }
}

LauncherConfig::~LauncherConfig()
{
}

std::string LauncherConfig::GetDocumentsPath() const
{
    std::string documentsPath;

#ifdef _WIN32
    // Windows: Documents folder
    char docPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, docPath))) {
        documentsPath = docPath;
    }
#else
    // Linux/macOS: ~/Documents or ~ if Documents doesn't exist
    const char* homeDir = getenv("HOME");
    if (!homeDir) {
        struct passwd* pw = getpwuid(getuid());
        homeDir = pw->pw_dir;
    }

    std::string docDir = std::string(homeDir) + "/Documents";
    struct stat st;
    if (stat(docDir.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
        documentsPath = docDir;
    } else {
        // Fallback to home directory
        documentsPath = homeDir;
    }
#endif

    return documentsPath;
}

std::string LauncherConfig::DetectPEBLInstallation() const
{
    struct stat st;

    // Only check for portable-style PEBL directory if explicitly in portable mode
    if (IsPortableMode()) {
#ifdef _WIN32
        const char* peblBattery = "PEBL\\battery";
        const char* parentPeblBattery = "..\\PEBL\\battery";
#else
        const char* peblBattery = "PEBL/battery";
        const char* parentPeblBattery = "../PEBL/battery";
#endif

        if (stat(peblBattery, &st) == 0 && S_ISDIR(st.st_mode)) {
            printf("Found PEBL battery in portable mode: %s\n", peblBattery);
            return peblBattery;
        }

        if (stat(parentPeblBattery, &st) == 0 && S_ISDIR(st.st_mode)) {
            printf("Found PEBL battery in portable mode: %s\n", parentPeblBattery);
            return parentPeblBattery;
        }
    }

#ifdef _WIN32
    // On Windows, use GetModuleFileName to find the launcher's location
    char exePath[MAX_PATH];
    DWORD len = GetModuleFileNameA(NULL, exePath, MAX_PATH);
    if (len > 0 && len < MAX_PATH) {
        std::string exePathStr(exePath);

        // Find the directory containing the executable
        size_t lastSep = exePathStr.find_last_of("/\\");
        if (lastSep != std::string::npos) {
            std::string exeDir = exePathStr.substr(0, lastSep);

            // Check for battery in same directory (non-portable installed mode)
            std::string batteryPath = exeDir + "\\battery";
            if (stat(batteryPath.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
                printf("Found PEBL battery at: %s\n", batteryPath.c_str());
                return batteryPath;
            }

            // Check one level up (if launcher is in bin/ subdirectory)
            size_t parentSep = exeDir.find_last_of("/\\");
            if (parentSep != std::string::npos) {
                std::string parentDir = exeDir.substr(0, parentSep);
                batteryPath = parentDir + "\\battery";
                if (stat(batteryPath.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
                    printf("Found PEBL battery at: %s\n", batteryPath.c_str());
                    return batteryPath;
                }
            }
        }
    }

    // Fallback: Check for Windows installed mode (Program Files)
    const char* programFiles[] = {
        "C:\\Program Files\\pebl2\\battery",
        "C:\\Program Files (x86)\\pebl2\\battery",
        "C:\\Program Files\\PEBL2\\battery",
        "C:\\Program Files (x86)\\PEBL2\\battery"
    };

    for (const char* path : programFiles) {
        if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
            printf("Found PEBL battery at: %s\n", path);
            return path;
        }
    }
#else
    // On Linux, use BinReloc to find the executable location
    BrInitError error;
    if (br_init(&error) != 0) {
        // BinReloc initialized successfully
        char* prefix = br_find_prefix("/usr/local");
        if (prefix) {
            std::string batteryPath = std::string(prefix) + "/battery";
            free(prefix);
            if (stat(batteryPath.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
                printf("Found PEBL battery at: %s\n", batteryPath.c_str());
                return batteryPath;
            }
        }

        // Try exe directory
        char* exeDir = br_find_exe_dir("/usr/local/bin");
        if (exeDir) {
            std::string batteryPath = std::string(exeDir) + "/battery";
            if (stat(batteryPath.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
                free(exeDir);
                printf("Found PEBL battery at: %s\n", batteryPath.c_str());
                return batteryPath;
            }

            // Check one level up
            std::string exeDirStr(exeDir);
            free(exeDir);
            size_t lastSep = exeDirStr.find_last_of('/');
            if (lastSep != std::string::npos) {
                std::string parentDir = exeDirStr.substr(0, lastSep);
                batteryPath = parentDir + "/battery";
                if (stat(batteryPath.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
                    printf("Found PEBL battery at: %s\n", batteryPath.c_str());
                    return batteryPath;
                }
            }
        }
    }

    // Fallback: check standard installation paths
    const char* linuxPaths[] = {
        "/usr/local/pebl2/battery",
        "/usr/pebl2/battery",
        "/usr/local/share/pebl2/battery",
        "/usr/share/pebl2/battery"
    };

    for (const char* path : linuxPaths) {
        if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
            printf("Found PEBL battery at: %s\n", path);
            return path;
        }
    }
#endif

    printf("Warning: Could not locate PEBL battery directory\n");
    return "";
}

std::string LauncherConfig::GetConfigFilePath() const
{
    // In portable mode, store config in workspace/settings/
    if (IsPortableMode()) {
        std::string portableWorkspace = GetPortableWorkspacePath();
#ifdef _WIN32
        return portableWorkspace + "\\settings\\launcher.cfg";
#else
        return portableWorkspace + "/settings/launcher.cfg";
#endif
    }

    // Store config file in Documents/pebl-exp.2.3/settings/launcher.cfg
    std::string documentsPath = GetDocumentsPath();
    if (!documentsPath.empty()) {
#ifdef _WIN32
        return documentsPath + "\\pebl-exp.2.3\\settings\\launcher.cfg";
#else
        return documentsPath + "/pebl-exp.2.3/settings/launcher.cfg";
#endif
    }

    // Fallback to ~/.pebl/settings/launcher.cfg
#ifdef _WIN32
    // Windows: %APPDATA%\PEBL\settings\launcher.cfg
    char appDataPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appDataPath))) {
        return std::string(appDataPath) + "\\PEBL\\settings\\launcher.cfg";
    }
    return "";
#else
    // Linux/macOS: ~/.pebl/settings/launcher.cfg
    const char* homeDir = getenv("HOME");
    if (!homeDir) {
        struct passwd* pw = getpwuid(getuid());
        homeDir = pw->pw_dir;
    }
    return std::string(homeDir) + "/.pebl/settings/launcher.cfg";
#endif
}

bool LauncherConfig::IsPortableMode() const
{
    struct stat st;

    // Check for STANDALONE.txt marker file
    if (stat("./STANDALONE.txt", &st) == 0 || stat("../STANDALONE.txt", &st) == 0 ||
        stat("../../STANDALONE.txt", &st) == 0) {
        return true;
    }

    // Check for PORTABLE marker file
    if (stat("./PORTABLE.txt", &st) == 0 || stat("../PORTABLE.txt", &st) == 0 ||
        stat("../../PORTABLE.txt", &st) == 0) {
        return true;
    }

    // Check for PEBL_PORTABLE environment variable
    const char* portableEnv = getenv("PEBL_PORTABLE");
    if (portableEnv && strcmp(portableEnv, "1") == 0) {
        return true;
    }

    // No automatic detection - only explicit marker files or environment variable
    return false;
}

std::string LauncherConfig::GetPortableWorkspacePath() const
{
    struct stat st;

    // Check where the marker file is located to determine workspace root
    // Marker file location determines workspace location

    // Check if marker file is in current directory (running from root)
    if (stat("./STANDALONE.txt", &st) == 0 || stat("./PORTABLE.txt", &st) == 0) {
        return ".";
    }

    // Check if marker file is one level up
    if (stat("../STANDALONE.txt", &st) == 0 || stat("../PORTABLE.txt", &st) == 0) {
        return "..";
    }

    // Check if marker file is two levels up (we're in PEBL/bin/)
    if (stat("../../STANDALONE.txt", &st) == 0 || stat("../../PORTABLE.txt", &st) == 0) {
        return "../..";
    }

    // If PEBL_PORTABLE env var is set, use current directory
    // Fallback: current directory
    return ".";
}

bool LauncherConfig::LoadConfig()
{
    std::string configPath = GetConfigFilePath();
    std::ifstream configFile(configPath);

    if (!configFile.is_open()) {
        // Config file doesn't exist yet, use defaults
        return false;
    }

    // In portable mode, skip loading path-related settings
    // They should be re-detected each time based on current location
    bool portable = IsPortableMode();

    std::string line;
    std::string currentSection = "";

    while (std::getline(configFile, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // Check for section headers
        if (line[0] == '[' && line[line.length()-1] == ']') {
            currentSection = line.substr(1, line.length()-2);
            continue;
        }

        // Parse key=value pairs
        size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) {
            continue;
        }

        std::string key = line.substr(0, equalPos);
        std::string value = line.substr(equalPos + 1);

        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        // Apply configuration values based on section
        if (currentSection == "") {
            // In portable mode, skip path-related settings (except relative study paths)
            if (portable) {
                if (key == "experiment_directory" || key == "workspace_path" ||
                    key == "battery_path" || key == "pebl_executable_path" ||
                    key == "data_output_path") {
                    continue;  // Skip - will be re-detected
                }
                // Note: current_study_path is handled specially in its own case below
            }

            if (key == "experiment_directory") {
                mExperimentDirectory = value;
            } else if (key == "subject_code") {
                mSubjectCode = value;
            } else if (key == "language") {
                mLanguage = value;
            } else if (key == "fullscreen") {
                mFullscreen = (value == "true" || value == "1");
            } else if (key == "auto_upload") {
                mAutoUpload = (value == "true" || value == "1");
            } else if (key == "upload_token") {
                mUploadToken = value;
            } else if (key == "upload_url") {
                mUploadURL = value;
            } else if (key == "workspace_path") {
                mWorkspacePath = value;
            } else if (key == "battery_path") {
                mBatteryPath = value;
            } else if (key == "pebl_executable_path") {
                mPeblExecutablePath = value;
            } else if (key == "data_output_path") {
                mDataOutputPath = value;
            } else if (key == "font_size") {
                try {
                    mFontSize = std::stoi(value);
                } catch (...) {
                    printf("Warning: Invalid font_size value, using default\n");
                }
            } else if (key == "window_width") {
                try {
                    mWindowWidth = std::stoi(value);
                } catch (...) {
                    printf("Warning: Invalid window_width value, using default\n");
                }
            } else if (key == "window_height") {
                try {
                    mWindowHeight = std::stoi(value);
                } catch (...) {
                    printf("Warning: Invalid window_height value, using default\n");
                }
            } else if (key == "current_study_path") {
                if (portable && !value.empty()) {
                    // In portable mode, convert relative path to absolute
                    // Check if it's already absolute
                    if (value[0] != '/' && !(value.length() > 1 && value[1] == ':')) {
                        // It's a relative path - make it absolute relative to workspace
                        try {
                            fs::path relativePath(value);
                            fs::path absolutePath = fs::absolute(fs::path(mWorkspacePath) / relativePath);
                            mCurrentStudyPath = absolutePath.string();
                        } catch (...) {
                            mCurrentStudyPath = value;  // Fallback to as-is
                        }
                    } else {
                        // Absolute path in portable mode - skip it
                        mCurrentStudyPath = "";
                    }
                } else {
                    mCurrentStudyPath = value;
                }
            } else if (key == "current_chain_name") {
                mCurrentChainName = value;
            } else if (key == "external_editor") {
                mExternalEditor = value;
            }
        } else if (currentSection == "recent") {
            // Parse recent experiment entry: name|path|timestamp
            size_t pipe1 = value.find('|');
            if (pipe1 != std::string::npos) {
                size_t pipe2 = value.find('|', pipe1 + 1);
                if (pipe2 != std::string::npos) {
                    try {
                        std::string timestampStr = value.substr(pipe2 + 1);
                        // Skip if timestamp is empty or invalid
                        if (timestampStr.empty()) {
                            continue;
                        }

                        RecentExperiment recent;
                        recent.name = value.substr(0, pipe1);
                        recent.path = value.substr(pipe1 + 1, pipe2 - pipe1 - 1);
                        recent.lastRun = std::stol(timestampStr);
                        mRecentExperiments.push_back(recent);
                    } catch (const std::invalid_argument& e) {
                        // Skip malformed entry
                        printf("Warning: Skipping malformed recent experiment entry: %s\n", value.c_str());
                    } catch (const std::out_of_range& e) {
                        // Skip out of range timestamp
                        printf("Warning: Skipping out-of-range timestamp in recent experiment entry: %s\n", value.c_str());
                    }
                }
            }
        }
    }

    configFile.close();
    return true;
}

bool LauncherConfig::SaveConfig()
{
    std::string configPath = GetConfigFilePath();
    bool portable = IsPortableMode();

    // Create directory (and any parent directories) if it doesn't exist
    fs::path configDir = fs::path(configPath).parent_path();
    if (!configDir.empty()) {
        try {
            fs::create_directories(configDir);
        } catch (const fs::filesystem_error& e) {
            printf("Warning: Could not create config directory: %s\n", e.what());
        }
    }

    std::ofstream configFile(configPath);
    if (!configFile.is_open()) {
        return false;
    }

    // Write configuration
    configFile << "# PEBL Launcher Configuration" << std::endl;
    configFile << "# Auto-generated - edit at your own risk" << std::endl;
    if (portable) {
        configFile << "# Portable mode - path settings are auto-detected on launch" << std::endl;
    }
    configFile << std::endl;

    // In portable mode, don't save absolute path settings
    // They will be re-detected based on the launcher's location
    if (!portable) {
        configFile << "experiment_directory=" << mExperimentDirectory << std::endl;
    }
    configFile << "subject_code=" << mSubjectCode << std::endl;
    configFile << "language=" << mLanguage << std::endl;
    configFile << "fullscreen=" << (mFullscreen ? "true" : "false") << std::endl;
    configFile << std::endl;

    // In portable mode, skip path settings entirely
    if (!portable) {
        configFile << "# File paths" << std::endl;
        configFile << "workspace_path=" << mWorkspacePath << std::endl;
        configFile << "battery_path=" << mBatteryPath << std::endl;
        configFile << "pebl_executable_path=" << mPeblExecutablePath << std::endl;
        configFile << "data_output_path=" << mDataOutputPath << std::endl;
        configFile << std::endl;
    }

    configFile << "# Upload settings" << std::endl;
    configFile << "auto_upload=" << (mAutoUpload ? "true" : "false") << std::endl;
    configFile << "upload_token=" << mUploadToken << std::endl;
    configFile << "upload_url=" << mUploadURL << std::endl;
    configFile << std::endl;

    configFile << "# UI settings" << std::endl;
    configFile << "font_size=" << mFontSize << std::endl;
    configFile << "window_width=" << mWindowWidth << std::endl;
    configFile << "window_height=" << mWindowHeight << std::endl;
    configFile << "external_editor=" << mExternalEditor << std::endl;
    configFile << std::endl;

    configFile << "# Session state" << std::endl;
    // Save study path - in portable mode, convert to relative path
    if (!mCurrentStudyPath.empty()) {
        if (portable) {
            // Convert absolute path to relative path for portable mode
            try {
                fs::path studyPath(mCurrentStudyPath);
                fs::path workspacePath(mWorkspacePath);
                fs::path relativePath = fs::relative(studyPath, workspacePath);
                configFile << "current_study_path=" << relativePath.string() << std::endl;
            } catch (...) {
                // If relative path conversion fails, skip saving
                printf("Warning: Could not convert study path to relative path\n");
            }
        } else {
            configFile << "current_study_path=" << mCurrentStudyPath << std::endl;
        }
    }
    configFile << "current_chain_name=" << mCurrentChainName << std::endl;

    // Save recent experiments
    if (!mRecentExperiments.empty()) {
        configFile << std::endl;
        configFile << "[recent]" << std::endl;
        for (size_t i = 0; i < mRecentExperiments.size(); i++) {
            const RecentExperiment& recent = mRecentExperiments[i];
            configFile << "recent" << i << "=" << recent.name << "|"
                      << recent.path << "|" << recent.lastRun << std::endl;
        }
    }

    configFile.close();
    return true;
}

void LauncherConfig::AddRecentExperiment(const std::string& path, const std::string& name)
{
    // Check if already in list
    for (auto it = mRecentExperiments.begin(); it != mRecentExperiments.end(); ++it) {
        if (it->path == path) {
            // Update timestamp and move to front
            it->lastRun = time(nullptr);
            RecentExperiment recent = *it;
            mRecentExperiments.erase(it);
            mRecentExperiments.insert(mRecentExperiments.begin(), recent);
            return;
        }
    }

    // Add new entry at front
    RecentExperiment recent;
    recent.path = path;
    recent.name = name;
    recent.lastRun = time(nullptr);
    mRecentExperiments.insert(mRecentExperiments.begin(), recent);

    // Limit to MAX_RECENT_EXPERIMENTS
    if ((int)mRecentExperiments.size() > MAX_RECENT_EXPERIMENTS) {
        mRecentExperiments.resize(MAX_RECENT_EXPERIMENTS);
    }
}
