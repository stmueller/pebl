// LauncherConfig.cpp - Configuration management implementation
// Copyright (c) 2026 Shane T. Mueller
// Licensed under GPL

#include "LauncherConfig.h"
#include "BinReloc.h"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#else
#include <unistd.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
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
    , mDataOutputPath("")
    , mFontSize(18)
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

    // Set workspace path to Documents/pebl-exp.2.3/
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
#ifdef _WIN32
    const char* separator = "\\";
#else
    const char* separator = "/";
#endif

    struct stat st;

    // Initialize BinReloc
    BrInitError error;
    if (br_init(&error) == 0) {
        printf("Warning: BinReloc initialization failed\n");
    }

    // Use BinReloc to find the prefix (one level up from bin/)
    // If launcher is at /path/to/pebl/bin/pebl-launcher, prefix is /path/to/pebl
    char* prefix = br_find_prefix("/usr/local");
    if (prefix) {
        std::string batteryPath = std::string(prefix) + separator + "battery";
        free(prefix);

        if (stat(batteryPath.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
            printf("Found PEBL battery at: %s\n", batteryPath.c_str());
            return batteryPath;
        }
    }

    // Fallback: Try to find executable directory directly
    char* exeDir = br_find_exe_dir("/usr/local/bin");
    if (exeDir) {
        // Check in exe directory first (portable mode - launcher and battery in same dir)
        std::string batteryPath = std::string(exeDir) + separator + "battery";
        if (stat(batteryPath.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
            free(exeDir);
            printf("Found PEBL battery at: %s\n", batteryPath.c_str());
            return batteryPath;
        }

        // Check one level up (if launcher is in bin/ subdirectory)
        std::string exeDirStr(exeDir);
        free(exeDir);

        size_t lastSep = exeDirStr.find_last_of("/\\");
        if (lastSep != std::string::npos) {
            std::string parentDir = exeDirStr.substr(0, lastSep);
            batteryPath = parentDir + separator + "battery";

            if (stat(batteryPath.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
                printf("Found PEBL battery at: %s\n", batteryPath.c_str());
                return batteryPath;
            }
        }
    }

#ifdef _WIN32
    // Check for Windows installed mode (Program Files)
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
    // On Linux, check standard installation paths
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
    // Store config file in Documents/pebl-exp.2.3/launcher.cfg
    std::string documentsPath = GetDocumentsPath();
    if (!documentsPath.empty()) {
#ifdef _WIN32
        return documentsPath + "\\pebl-exp.2.3\\launcher.cfg";
#else
        return documentsPath + "/pebl-exp.2.3/launcher.cfg";
#endif
    }

    // Fallback to ~/.pebl/launcher.cfg
#ifdef _WIN32
    // Windows: %APPDATA%\PEBL\launcher.cfg
    char appDataPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appDataPath))) {
        return std::string(appDataPath) + "\\PEBL\\launcher.cfg";
    }
    return "";
#else
    // Linux/macOS: ~/.pebl/launcher.cfg
    const char* homeDir = getenv("HOME");
    if (!homeDir) {
        struct passwd* pw = getpwuid(getuid());
        homeDir = pw->pw_dir;
    }
    return std::string(homeDir) + "/.pebl/launcher.cfg";
#endif
}

bool LauncherConfig::LoadConfig()
{
    std::string configPath = GetConfigFilePath();
    std::ifstream configFile(configPath);

    if (!configFile.is_open()) {
        // Config file doesn't exist yet, use defaults
        return false;
    }

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
                mCurrentStudyPath = value;
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

    // Create directory if it doesn't exist
#ifdef _WIN32
    size_t lastSlash = configPath.find_last_of('\\');
#else
    size_t lastSlash = configPath.find_last_of('/');
#endif

    if (lastSlash != std::string::npos) {
        std::string configDir = configPath.substr(0, lastSlash);
#ifdef _WIN32
        CreateDirectoryA(configDir.c_str(), NULL);
#else
        mkdir(configDir.c_str(), 0755);
#endif
    }

    std::ofstream configFile(configPath);
    if (!configFile.is_open()) {
        return false;
    }

    // Write configuration
    configFile << "# PEBL Launcher Configuration" << std::endl;
    configFile << "# Auto-generated - edit at your own risk" << std::endl;
    configFile << std::endl;

    configFile << "experiment_directory=" << mExperimentDirectory << std::endl;
    configFile << "subject_code=" << mSubjectCode << std::endl;
    configFile << "language=" << mLanguage << std::endl;
    configFile << "fullscreen=" << (mFullscreen ? "true" : "false") << std::endl;
    configFile << std::endl;

    configFile << "# File paths" << std::endl;
    configFile << "workspace_path=" << mWorkspacePath << std::endl;
    configFile << "battery_path=" << mBatteryPath << std::endl;
    configFile << "data_output_path=" << mDataOutputPath << std::endl;
    configFile << std::endl;

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
    configFile << "current_study_path=" << mCurrentStudyPath << std::endl;
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
