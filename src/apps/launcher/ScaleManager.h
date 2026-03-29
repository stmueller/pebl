// ScaleManager.h - PEBL Scale file management
// Copyright (c) 2026 Shane T. Mueller
// Licensed under GPL

#ifndef SCALE_MANAGER_H
#define SCALE_MANAGER_H

#include "ScaleDefinition.h"
#include <string>
#include <vector>
#include <memory>

// Manages scale files in media/apps/scales/ (read-only library) and workspace/scales/ (writable)
class ScaleManager {
public:
    ScaleManager(const std::string& batteryPath, const std::string& workspacePath = "");
    ~ScaleManager();

    // Scan available scales from library and workspace
    std::vector<std::string> GetAvailableScales();

    // Create new scale
    std::shared_ptr<ScaleDefinition> CreateScale(const std::string& code);

    // Load existing scale (definition + translations)
    std::shared_ptr<ScaleDefinition> LoadScale(const std::string& code);

    // Save scale (definition + translations)
    bool SaveScale(std::shared_ptr<ScaleDefinition> scale);

    // Delete scale (definition + translations)
    bool DeleteScale(const std::string& code);

    // Export scale to battery directory
    bool ExportToBattery(std::shared_ptr<ScaleDefinition> scale);

    // Import scale from file
    std::shared_ptr<ScaleDefinition> ImportFromFile(const std::string& filePath);

    // Loose OSD files — .osd files placed directly in the workspace scales directory,
    // not yet installed into a <code>/ subdirectory.
    struct LooseOSDEntry {
        std::string path;   // Full filesystem path to the .osd file
        std::string code;   // Scale code read from scale_info.code
        std::string name;   // Scale name read from scale_info.name
    };
    std::vector<LooseOSDEntry> GetLooseOSDEntries() const;

    // Install a loose .osd file: creates workspace/scales/<code>/ and moves the file there.
    // Returns the loaded ScaleDefinition on success, nullptr on failure.
    std::shared_ptr<ScaleDefinition> InstallLooseOSD(const std::string& osdPath);

    // Get paths
    std::string GetDefinitionPath(const std::string& code) const;
    std::string GetOSDPath(const std::string& code) const;
    std::string GetTranslationPath(const std::string& code, const std::string& lang) const;
    std::string GetBatteryPath() const { return mBatteryPath; }
    std::string GetWorkspacePath() const { return mWorkspacePath; }
    std::string GetBatteryScalesPath() const { return mBatteryScalesPath; }
    std::string GetWorkspaceScalesPath() const { return mWorkspaceScalesPath; }

    // Returns workspace path if available, else library path
    std::string GetScalesPath() const { return mWorkspaceScalesPath.empty() ? mBatteryScalesPath : mWorkspaceScalesPath; }
    std::string GetDefinitionsPath() const { return mWorkspaceDefinitionsPath.empty() ? mBatteryDefinitionsPath : mWorkspaceDefinitionsPath; }

    // Check if scale exists
    bool ScaleExists(const std::string& code) const;

    // Get scale metadata (without loading full scale)
    struct ScaleMetadata {
        std::string code;
        std::string name;
        std::string description;
        std::string author;
        int questionCount;
        std::vector<std::string> availableLanguages;
    };
    ScaleMetadata GetScaleMetadata(const std::string& code) const;

    // Create standalone study from scale in workspace
    bool CreateStudyFromScale(std::shared_ptr<ScaleDefinition> scale,
                              const std::string& workspaceStudiesPath,
                              const std::string& studyName = "");

    // Add scale as a test to an existing study
    bool AddScaleToStudy(std::shared_ptr<ScaleDefinition> scale,
                         const std::string& studyPath);

private:
    void EnsureDirectoriesExist();
    std::vector<std::string> GetTranslationFiles(const std::string& code) const;
    std::vector<std::string> GetTranslationFilesFromPath(const std::string& code, const std::string& translationsPath) const;
    bool ScaleExistsInPath(const std::string& code, const std::string& definitionsPath) const;

    // Generate or copy screenshot for deployed scale test
    bool GenerateScreenshot(const std::string& scaleCode, const std::string& testPath);

    // Generate .pbl.schema.json and .pbl.par.json from scale definition parameters.
    // Called at study/test creation time so OSD-defined params are immediately visible
    // in the parameter editor.  Also updates par.json with any new defaults without
    // overwriting values the user has already set.
    bool GenerateSchemaFiles(const ScaleDefinition& scale, const std::string& testPath);

    std::string mBatteryPath;
    std::string mWorkspacePath;

    // Library paths (read-only, shipped with PEBL): media/apps/scales/
    std::string mBatteryScalesPath;         // media/apps/scales/
    std::string mBatteryDefinitionsPath;    // media/apps/scales/definitions/

    // Workspace paths (writable, user-created)
    std::string mWorkspaceScalesPath;       // workspace/scales/
    std::string mWorkspaceDefinitionsPath;  // workspace/scales/definitions/
};

#endif // SCALE_MANAGER_H
