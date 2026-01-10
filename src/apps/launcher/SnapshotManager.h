// SnapshotManager.h - PEBL snapshot creation and management
// Copyright (c) 2026 Shane T. Mueller
// Licensed under GPL

#ifndef SNAPSHOTMANAGER_H
#define SNAPSHOTMANAGER_H

#include <string>
#include <vector>
#include <memory>

class Study;

// Manages snapshot creation, validation, and import
class SnapshotManager {
public:
    SnapshotManager();
    ~SnapshotManager();

    // Create snapshot from study
    // Returns snapshot directory name (e.g., "study-name_v1_2026-01-09")
    std::string CreateSnapshot(const std::string& studyPath,
                                const std::string& snapshotsDir);

    // Validate snapshot directory
    struct ValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    ValidationResult ValidateSnapshot(const std::string& snapshotPath);

    // Import snapshot into my_studies
    bool ImportSnapshot(const std::string& snapshotPath,
                        const std::string& studiesDir,
                        const std::string& newStudyName);

    // Get snapshot name format: studyname_vN_YYYY-MM-DD
    static std::string GenerateSnapshotName(const std::string& studyName, int version);

    // Extract study metadata from snapshot
    struct SnapshotInfo {
        std::string studyName;
        int version;
        std::string description;
        std::string author;
        std::string createdDate;
        int testCount;
        int chainCount;
    };
    SnapshotInfo GetSnapshotInfo(const std::string& snapshotPath);

private:
    bool CopyDirectory(const std::string& source, const std::string& dest, bool excludeData);
    bool CopyFile(const std::string& source, const std::string& dest);
    bool DirectoryExists(const std::string& path) const;
    bool FileExists(const std::string& path) const;
    std::string GetCurrentDateString() const;  // Returns YYYY-MM-DD
    bool ShouldExcludeFromSnapshot(const std::string& name, bool isDirectory) const;
};

#endif // SNAPSHOTMANAGER_H
