// LauncherConfig.h - Configuration management for PEBL Launcher
// Copyright (c) 2026 Shane T. Mueller
// Licensed under GPL

#ifndef LAUNCHER_CONFIG_H
#define LAUNCHER_CONFIG_H

#include <string>
#include <vector>

// Version and prefix constants - defined here to avoid Makefile quoting issues
#ifndef PEBL_VERSION
#define PEBL_VERSION "2.4"
#endif

#ifndef PREFIX
#define PREFIX "/usr/local"
#endif

struct RecentExperiment {
    std::string path;
    std::string name;
    time_t lastRun;
};

class LauncherConfig {
public:
    LauncherConfig();
    ~LauncherConfig();

    bool LoadConfig();
    bool SaveConfig();

    // Getters
    std::string GetExperimentDirectory() const { return mExperimentDirectory; }
    std::string GetSubjectCode() const { return mSubjectCode; }
    std::string GetLanguage() const { return mLanguage; }
    bool GetFullscreen() const { return mFullscreen; }
    const std::vector<RecentExperiment>& GetRecentExperiments() const { return mRecentExperiments; }
    bool GetAutoUpload() const { return mAutoUpload; }
    std::string GetUploadToken() const { return mUploadToken; }
    std::string GetUploadURL() const { return mUploadURL; }
    std::string GetWorkspacePath() const { return mWorkspacePath; }
    std::string GetBatteryPath() const { return mBatteryPath; }
    std::string GetPeblExecutablePath() const { return mPeblExecutablePath; }
    std::string GetDataOutputPath() const { return mDataOutputPath; }
    int GetFontSize() const { return mFontSize; }
    std::string GetCurrentStudyPath() const { return mCurrentStudyPath; }
    std::string GetCurrentChainName() const { return mCurrentChainName; }
    int GetWindowWidth() const { return mWindowWidth; }
    int GetWindowHeight() const { return mWindowHeight; }
    std::string GetExternalEditor() const { return mExternalEditor; }

    // Setters
    void SetExperimentDirectory(const std::string& dir) { mExperimentDirectory = dir; }
    void SetSubjectCode(const std::string& code) { mSubjectCode = code; }
    void SetLanguage(const std::string& lang) { mLanguage = lang; }
    void SetFullscreen(bool fullscreen) { mFullscreen = fullscreen; }
    void SetAutoUpload(bool autoUpload) { mAutoUpload = autoUpload; }
    void SetUploadToken(const std::string& token) { mUploadToken = token; }
    void SetUploadURL(const std::string& url) { mUploadURL = url; }
    void SetWorkspacePath(const std::string& path) { mWorkspacePath = path; }
    void SetBatteryPath(const std::string& path) { mBatteryPath = path; }
    void SetPeblExecutablePath(const std::string& path) { mPeblExecutablePath = path; }
    void SetDataOutputPath(const std::string& path) { mDataOutputPath = path; }
    void SetFontSize(int size) { mFontSize = size; }
    void SetCurrentStudyPath(const std::string& path) { mCurrentStudyPath = path; }
    void SetCurrentChainName(const std::string& name) { mCurrentChainName = name; }
    void SetWindowWidth(int width) { mWindowWidth = width; }
    void SetWindowHeight(int height) { mWindowHeight = height; }
    void SetExternalEditor(const std::string& editor) { mExternalEditor = editor; }

    void AddRecentExperiment(const std::string& path, const std::string& name);

private:
    std::string GetConfigFilePath() const;
    std::string GetDocumentsPath() const;
    std::string DetectPEBLInstallation() const;
    bool IsPortableMode() const;
    std::string GetPortableWorkspacePath() const;

    std::string mExperimentDirectory;
    std::string mSubjectCode;
    std::string mLanguage;
    bool mFullscreen;

    // Upload settings
    bool mAutoUpload;
    std::string mUploadToken;
    std::string mUploadURL;

    // File paths
    std::string mWorkspacePath;      // Documents/pebl-exp.2.4/
    std::string mBatteryPath;        // /usr/local/share/pebl/battery/ or auto-detect
    std::string mPeblExecutablePath; // Path to pebl2 executable
    std::string mDataOutputPath;     // Where to save data files

    // UI settings
    int mFontSize;                   // Font size (default 16)
    int mWindowWidth;                // Window width (default 1280)
    int mWindowHeight;               // Window height (default 720)
    std::string mExternalEditor;     // External text editor command

    // Session state (selected study and chain)
    std::string mCurrentStudyPath;   // Path to currently selected study
    std::string mCurrentChainName;   // Name of currently selected chain file

    std::vector<RecentExperiment> mRecentExperiments;
    static const int MAX_RECENT_EXPERIMENTS = 10;
};

#endif // LAUNCHER_CONFIG_H
