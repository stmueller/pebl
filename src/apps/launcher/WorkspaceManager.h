// WorkspaceManager.h - PEBL workspace initialization and management
// Copyright (c) 2026 Shane T. Mueller
// Licensed under GPL

#ifndef WORKSPACEMANAGER_H
#define WORKSPACEMANAGER_H

#include <string>
#include <vector>

// Manages PEBL workspace directory structure in Documents/pebl-exp.2.3/
class WorkspaceManager {
public:
    WorkspaceManager();
    ~WorkspaceManager();

    // Check if running in portable mode
    bool IsPortableMode() const;

    // Check if this is first run (workspace doesn't exist)
    bool IsFirstRun() const;

    // Initialize workspace (creates directories on first run)
    bool Initialize();

    // Copy resources from installation (docs, demos, tutorials)
    bool CopyResources(const std::string& installationPath);

    // Check if workspace is initialized
    bool IsInitialized() const;

    // Set workspace path (override auto-detected path, e.g. from saved config)
    void SetWorkspacePath(const std::string& path) { mWorkspacePath = path; }

    // Get workspace paths
    std::string GetWorkspacePath() const { return mWorkspacePath; }
    std::string GetStudiesPath() const { return mWorkspacePath + "/my_studies"; }
    std::string GetSnapshotsPath() const { return mWorkspacePath + "/snapshots"; }
    std::string GetChainsPath() const { return mWorkspacePath + "/chains"; }
    std::string GetScalesPath() const { return mWorkspacePath + "/scales"; }
    std::string GetDocsPath() const { return mWorkspacePath + "/doc"; }
    std::string GetDemoPath() const { return mWorkspacePath + "/demo"; }
    std::string GetTutorialPath() const { return mWorkspacePath + "/tutorial"; }

    // Get list of studies
    std::vector<std::string> GetStudyDirectories() const;

    // Get list of snapshots
    std::vector<std::string> GetSnapshotDirectories() const;

    // Create new study directory
    bool CreateStudyDirectory(const std::string& studyName);

    // Create snapshot from study
    bool CreateSnapshot(const std::string& studyPath, const std::string& snapshotName);

    // Import snapshot into my_studies
    bool ImportSnapshot(const std::string& snapshotPath, const std::string& newStudyName);

private:
    bool CreateDir(const std::string& path);
    bool DirectoryExists(const std::string& path) const;
    bool FileExists(const std::string& path) const;
    bool CopyDirectory(const std::string& source, const std::string& dest, bool excludeData = false, const std::vector<std::string>& excludeDirs = {});
    bool CopyFileContents(const std::string& source, const std::string& dest);
    std::string GetDocumentsPath() const;
    std::string GetPortableWorkspacePath() const;

    std::string mWorkspacePath;  // e.g., /home/user/Documents/pebl-exp.2.3
    bool mInitialized;
};

#endif // WORKSPACEMANAGER_H
