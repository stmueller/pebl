// LauncherUI.h - User interface for PEBL Launcher
// Copyright (c) 2026 Shane T. Mueller
// Licensed under GPL

#ifndef LAUNCHER_UI_H
#define LAUNCHER_UI_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <SDL2/SDL.h>
#include "TextEditor.h"

class LauncherConfig;
class Study;
class Chain;
class ChainItem;
class WorkspaceManager;
class SnapshotManager;

struct ExperimentInfo {
    std::string path;
    std::string name;
    std::string directory;
    std::string description;
    std::string screenshotPath;
    bool hasParams;
    bool hasTranslations;
    bool hasScreenshot;
};

// Old ChainItem struct removed - using Chain.h ChainItem instead

// Page editor dialog state
struct PageEditorState {
    bool show;
    int editingIndex;  // -1 for new, >= 0 for editing existing
    char title[256];
    char content[4096];
    int pageType;  // 0=instruction, 1=consent, 2=completion
};

// Test editor dialog state (for adding/editing test items in chains)
struct TestEditorState {
    bool show;
    int editingIndex;  // -1 for new, >= 0 for editing existing
    int selectedTestIndex;  // Index in study's test list
    int selectedVariantIndex;  // Index in test's parameter variants list
    char language[16];
    int randomGroup;  // Randomization group ID (0 = no randomization)
};

// Translation editor dialog state
struct TranslationEditorState {
    bool show;
    int testIndex;  // Which test we're editing translations for
    char language[16];  // Target language to edit/create
    char testPath[512];  // Full path to test directory

    // In-app translation editor data
    bool dataLoaded;
    bool dirty;  // True if there are unsaved changes
    int selectedKeyIndex;  // Currently selected key in the list
    std::vector<std::string> keys;  // Keys in order
    std::map<std::string, std::string> englishValues;  // Key -> English text
    std::map<std::string, std::string> targetValues;   // Key -> Target language text (editable)

    TranslationEditorState() : show(false), testIndex(-1), dataLoaded(false), dirty(false), selectedKeyIndex(-1) {
        language[0] = '\0';
        testPath[0] = '\0';
    }

    void Clear() {
        dataLoaded = false;
        dirty = false;
        selectedKeyIndex = -1;
        keys.clear();
        englishValues.clear();
        targetValues.clear();
    }
};

struct Parameter {
    std::string name;
    std::string value;
    std::string defaultValue;
    std::string description;
    std::vector<std::string> options;  // Optional list of allowed values
};

class LauncherUI {
public:
    LauncherUI(LauncherConfig* config, SDL_Renderer* renderer);
    ~LauncherUI();

    void Render(bool* p_open);

private:
    // Main UI components
    void RenderMenuBar();
    void RenderStudyBar();
    void RenderTestsTab();
    void RenderChainsTab();
    void RenderRunTab();
    void RenderQuickLaunchTab();

    // Tests tab sub-components
    void RenderTestsInStudy();
    void RenderAddTestPanel();
    void RenderBatteryBrowser();
    void RenderFileImport();
    void RenderNewTestTemplate();

    // Dialogs
    void ShowAboutDialog();
    void ShowParameterEditor();
    void ShowVariantNameDialog();
    void ShowSettingsDialog();
    void ShowPageEditor();
    void ShowTestEditor();
    void ShowNewStudyDialog();
    void ShowNewChainDialog();
    void ShowStudySettingsDialog();
    void ShowFirstRunDialog();
    void ShowDuplicateSubjectWarning();
    void ShowEditParticipantCodeDialog();
    void ShowCodeEditor();
    void ShowTranslationEditorDialog();

    // Helper to load parameter editor after variant name is entered
    void LoadParameterEditorForVariant();

    // Legacy methods (will be refactored/removed)
    void RenderFilePanel();
    void RenderDetailsPanel();
    void RenderDetailsTab();
    void RenderStudyTab();
    void RenderChainTab();

    void ScanExperimentDirectory(const std::string& path);
    void LoadExperimentInfo(const std::string& scriptPath);
    void LoadScreenshot(const std::string& imagePath);
    void FreeScreenshot();
    void RunTest();  // Renamed from RunExperiment
    void OpenDirectoryInFileBrowser(const std::string& path);
    void OpenFileInTextEditor(const std::string& filePath);

    // Utility launchers
    void LaunchTranslationEditor();
    void LaunchDataCombiner(const std::string& workingDirectory = "");

    // Study management
    void CreateNewStudy();
    void LoadStudy(const std::string& studyPath);
    void AddTestToStudy();
    void AddTestFromFile(const std::string& filePath);
    void CreateTestFromTemplate(const std::string& testName, int templateType);
    void CreateTestFromGenericStudy(const std::string& testName);  // Copy complete battery/template structure
    void RemoveTestFromStudy(const std::string& testName);
    void ScanTemplates();  // Dynamically load available templates
    void EditTestParameters(int testIndex);
    void ScanParameterVariants(int testIndex);

    // Chain management (new system)
    void CreateNewChain();
    void LoadChain(const std::string& chainPath);
    void SaveCurrentChain();
    void AddInstructionPage();
    void AddConsentPage();
    void AddCompletionPage();
    void AddTestToChain();
    void RemoveChainItem(int index);
    void MoveChainItemUp(int index);
    void MoveChainItemDown(int index);
    void EditChainItem(int index);
    void TestChainItem(int index);
    void RunChain();
    void RunChainConfirmed();  // Actually runs chain after duplicate check
    void ExecuteChainItem(int index);
    std::vector<std::string> CheckExistingSubjectCodes();
    std::vector<std::string> BuildAdditionalArguments();

    // File dialogs
    std::string OpenDirectoryDialog(const std::string& title = "Select Directory", const std::string& startDir = "");
    std::string OpenFileDialog(const std::string& title = "Select File", const std::string& filter = "");
    std::string SaveFileDialog(const std::string& title = "Save File", const std::string& defaultName = "");

    LauncherConfig* mConfig;
    SDL_Renderer* mRenderer;
    std::vector<ExperimentInfo> mExperiments;
    int mSelectedExperiment;

    // UI State
    char mSubjectCode[256];  // Legacy - now called participant code
    char mParticipantCode[256];  // Auto-generated from study code + counter
    char mStudyCode[5];  // 4-character study code prefix (editable)
    char mLanguageCode[16];
    bool mFullscreen;
    int mScreenResolution;  // Index into resolution list
    bool mVSync;
    char mGraphicsDriver[256];
    char mCustomArguments[512];
    bool mShowAbout;
    char mExperimentDir[512];

    // Available languages (populated from selected experiment)
    std::vector<std::string> mAvailableLanguages;

    // Screenshot display
    SDL_Texture* mScreenshotTexture;
    int mScreenshotWidth;
    int mScreenshotHeight;

    // Running experiment tracking
    class ExperimentRunner* mRunningExperiment;
    bool mShowStderr;  // Toggle between stdout and stderr display
    bool mOutputExpanded;  // Toggle output window expansion

    // Chain execution accumulated output (preserved across chain items)
    std::string mChainAccumulatedStdout;
    std::string mChainAccumulatedStderr;

    // Study system
    std::shared_ptr<Study> mCurrentStudy;
    std::shared_ptr<Chain> mCurrentChain;
    std::shared_ptr<WorkspaceManager> mWorkspace;
    std::shared_ptr<SnapshotManager> mSnapshots;

    // Study/chain UI state
    std::vector<std::string> mStudyList;
    int mSelectedStudyIndex;
    std::vector<std::string> mChainList;
    int mSelectedChainIndex;

    // Chain execution
    bool mRunningChain;
    int mCurrentChainItemIndex;
    std::vector<int> mChainExecutionOrder;  // Randomized execution order (indices into chain items)

    // Parameter editor
    bool mShowParameterEditor;
    bool mShowVariantNameDialog;  // Prompt for variant name before editing
    char mVariantName[256];  // User input for variant name (e.g., "mousebutton", "touchscreen")
    int mEditingTestIndex;  // Which test we're creating a variant for
    std::vector<Parameter> mParameters;
    std::string mParameterFile;

    // Subject code duplicate warning
    bool mShowDuplicateSubjectWarning;
    std::vector<std::string> mDuplicateWarningCodes;

    // Dialogs
    bool mShowSettings;
    bool mShowNewStudyDialog;
    bool mShowNewChainDialog;
    bool mShowStudySettingsDialog;
    bool mShowFirstRunDialog;
    bool mShowEditParticipantCodeDialog;
    PageEditorState mPageEditor;
    TestEditorState mTestEditor;
    TranslationEditorState mTranslationEditor;

    // Top-level tab state
    int mTopLevelTab;  // 0=Study, 1=Quick Launch

    // Tests tab state
    int mAddTestSubTab;  // 0=Battery, 1=File, 2=New
    char mNewStudyName[256];
    char mNewStudyDescription[1024];
    char mNewStudyAuthor[256];
    char mNewChainName[256];
    char mNewChainDescription[1024];

    // Quick Launch tab state
    char mQuickLaunchPath[512];
    char mQuickLaunchParamFile[512];
    std::string mQuickLaunchDirectory;
    std::vector<std::string> mQuickLaunchFiles;
    int mQuickLaunchSelectedFile;

    // Template system (data-driven)
    std::vector<std::string> mTemplateNames;  // Display names
    std::vector<std::string> mTemplateFiles;  // Filenames
    std::string mBatteryPath;  // Path to battery directory

    // Code editor
    bool mShowCodeEditor;
    std::string mCodeEditorFilePath;
    TextEditor mCodeEditor;
};


#endif // LAUNCHER_UI_H
