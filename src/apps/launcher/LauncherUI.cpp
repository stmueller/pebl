// LauncherUI.cpp - User interface implementation for PEBL Launcher
// Copyright (c) 2026 Shane T. Mueller
// Licensed under GPL

#include "LauncherUI.h"
#include "LauncherConfig.h"
#include "ExperimentRunner.h"
#include "Study.h"
#include "Chain.h"
#include "WorkspaceManager.h"
#include "SnapshotManager.h"
#include "ZipExtractor.h"
#include "ScaleDefinition.h"
#include "ScaleManager.h"
#include "../../utility/BinReloc.h"
#include "imgui.h"
#include <SDL2/SDL_image.h>
#include <cstring>
#include <cctype>
#include <ctime>
#include <algorithm>
#include <random>
#include <map>
#include <set>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <numeric>
#include <sys/stat.h>
#include <json.hpp>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#include <commdlg.h>
#include <direct.h>
#include <io.h>
#define mkdir(path, mode) _mkdir(path)
#ifndef stat
#define stat _stat
#endif
#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & _S_IFMT) == _S_IFDIR)
#endif
#ifndef S_ISREG
#define S_ISREG(mode) (((mode) & _S_IFMT) == _S_IFREG)
#endif
#else
#include <dirent.h>
#include <unistd.h>
#endif

namespace fs = std::filesystem;

// Helper function to get (and create if needed) temp directory in workspace
static std::string GetWorkspaceTempDirectory(const std::string& workspacePath)
{
    std::string tempDir;

    if (!workspacePath.empty()) {
#ifdef _WIN32
        tempDir = workspacePath + "\\temp";
#else
        tempDir = workspacePath + "/temp";
#endif
        // Create the temp directory if it doesn't exist
        struct stat st;
        if (stat(tempDir.c_str(), &st) != 0) {
#ifdef _WIN32
            _mkdir(tempDir.c_str());
#else
            mkdir(tempDir.c_str(), 0755);
#endif
            printf("Created temp directory: %s\n", tempDir.c_str());
        }
    } else {
        // Fallback to system temp if no workspace
#ifdef _WIN32
        char tempPath[MAX_PATH];
        DWORD len = GetTempPathA(MAX_PATH, tempPath);
        if (len > 0 && len < MAX_PATH) {
            tempDir = tempPath;
            // Remove trailing backslash if present
            if (!tempDir.empty() && (tempDir.back() == '\\' || tempDir.back() == '/')) {
                tempDir.pop_back();
            }
        } else {
            tempDir = "C:\\Windows\\Temp";
        }
#else
        const char* tmpdir = getenv("TMPDIR");
        tempDir = tmpdir ? tmpdir : "/tmp";
#endif
    }

    return tempDir;
}

// Helper function to get the PEBL media directory from executable path
static std::string GetPEBLMediaPath(const std::string& peblExePath)
{
    if (peblExePath.empty()) {
        return "";
    }

    // Find the directory containing the executable
    size_t lastSep = peblExePath.find_last_of("/\\");
    if (lastSep == std::string::npos) {
        return "";
    }

    std::string exeDir = peblExePath.substr(0, lastSep);

    // Go up one level (from bin/ to PEBL root)
    size_t parentSep = exeDir.find_last_of("/\\");
    std::string peblRoot;
    if (parentSep != std::string::npos) {
        peblRoot = exeDir.substr(0, parentSep);
    } else {
        peblRoot = ".";
    }

#ifdef _WIN32
    return peblRoot + "\\media";
#else
    return peblRoot + "/media";
#endif
}

LauncherUI::LauncherUI(LauncherConfig* config, SDL_Renderer* renderer)
    : mConfig(config)
    , mRenderer(renderer)
    , mSelectedExperiment(-1)
    , mFullscreen(false)
    , mShowAbout(false)
    , mScreenshotTexture(nullptr)
    , mScreenshotWidth(0)
    , mScreenshotHeight(0)
    , mRunningExperiment(nullptr)
    , mShowStderr(false)
    , mOutputExpanded(false)
    , mSelectedStudyIndex(-1)
    , mSelectedChainIndex(-1)
    , mSelectedStudyTestIndex(-1)
    , mStudyTestScreenshot(nullptr)
    , mStudyTestScreenshotW(0)
    , mStudyTestScreenshotH(0)
    , mRunningChain(false)
    , mCurrentChainItemIndex(-1)
    , mShowParameterEditor(false)
    , mShowVariantNameDialog(false)
    , mEditingTestIndex(-1)
    , mEditingDefaultParams(true)
    , mShowDuplicateSubjectWarning(false)
    , mShowSnapshotCreated(false)
    , mShowSettings(false)
    , mShowNewStudyDialog(false)
    , mShowNewChainDialog(false)
    , mShowStudySettingsDialog(false)
    , mShowFirstRunDialog(false)
    , mShowGettingStartedDialog(false)
    , mShowEditParticipantCodeDialog(false)
    , mAddTestSubTab(0)
    , mQuickLaunchSelectedFile(-1)
    , mShowCodeEditor(false)
    , mShowScaleBuilder(false)
    , mSelectedScaleIndex(-1)
    , mSelectedDimensionIndex(-1)
    , mScaleBrowserScreenshot(nullptr)
    , mScaleBrowserScreenshotW(0)
    , mScaleBrowserScreenshotH(0)
    , mScaleBrowserScreenshotForIndex(-1)
    , mScaleTransSelectedKey(-1)
{
    mScaleTransLanguage[0] = '\0';
    // Configure PEBL syntax highlighting based on doc/pebl.lang
    auto lang = TextEditor::LanguageDefinition();

    lang.mName = "PEBL";

    // PEBL keywords (case-insensitive in actual language, but TextEditor is case-sensitive)
    // Include both cases for common usage
    lang.mKeywords = {
        "if", "If", "IF",
        "elseif", "ElseIf", "ELSEIF",
        "else", "Else", "ELSE",
        "loop", "Loop", "LOOP",
        "while", "While", "WHILE",
        "return", "Return", "RETURN",
        "define", "Define", "DEFINE",
        "and", "And", "AND",
        "or", "Or", "OR",
        "not", "Not", "NOT",
        "break", "Break", "BREAK"
    };

    // PEBL uses # for single-line comments (shell-like)
    lang.mSingleLineComment = "#";
    lang.mCommentStart = "";  // No multi-line comments in PEBL
    lang.mCommentEnd = "";

    // PEBL-specific token regex patterns (regex, palette index)
    lang.mTokenRegexStrings = {
        { "\\b[0-9]*\\.[0-9]+\\b", TextEditor::PaletteIndex::Number },  // Floats first
        { "\\b[0-9]+\\b", TextEditor::PaletteIndex::Number },           // Then integers
        { "\\bg[A-Za-z0-9_]*(\\.[A-Za-z0-9_]+)?\\b", TextEditor::PaletteIndex::Identifier },  // Global variables
        { "\\b[a-fh-z][A-Za-z0-9_]*(\\.[A-Za-z0-9_]+)?\\b", TextEditor::PaletteIndex::Identifier },  // Local variables
        { ":?[A-Z][A-Za-z0-9_]*(?=\\s*\\()", TextEditor::PaletteIndex::KnownIdentifier },  // Function names
        { "<-", TextEditor::PaletteIndex::Preprocessor },  // Assignment operator
    };

    lang.mCaseSensitive = true;  // For variable names (g vs others)
    lang.mAutoIndentation = true;

    mCodeEditor.SetLanguageDefinition(lang);
    mCodeEditor.SetShowWhitespaces(false);
    mCodeEditor.SetTabSize(4);
    mCodeEditor.SetPalette(TextEditor::GetDarkPalette());
    // Initialize UI state from config
    std::strncpy(mSubjectCode, config->GetSubjectCode().c_str(), sizeof(mSubjectCode) - 1);
    mSubjectCode[sizeof(mSubjectCode) - 1] = '\0';
    mParticipantCode[0] = '\0';
    mStudyCode[0] = '\0';
    mQuickLaunchPath[0] = '\0';
    mQuickLaunchParamFile[0] = '\0';
    mLastSnapshotName[0] = '\0';
    mLastSnapshotPath[0] = '\0';

    // Set Quick Launch to start in workspace directory
    // Portable mode: portable root directory (for access to PEBL/battery, demo, tutorial)
    // Installed mode: Documents/pebl-exp.2.3
    std::string peblExpPath = config->GetWorkspacePath();
    if (!peblExpPath.empty()) {
        try {
            if (fs::exists(peblExpPath) && fs::is_directory(peblExpPath)) {
                mQuickLaunchDirectory = peblExpPath;

                // Scan for .pbl files on startup
                for (const auto& entry : fs::directory_iterator(peblExpPath)) {
                    if (!entry.is_regular_file()) continue;
                    std::string name = entry.path().filename().string();
                    if (name.length() > 4 && name.substr(name.length() - 4) == ".pbl") {
                        mQuickLaunchFiles.push_back(name);
                    }
                }
                std::sort(mQuickLaunchFiles.begin(), mQuickLaunchFiles.end());
            } else {
                // Fall back to config directory
                mQuickLaunchDirectory = config->GetExperimentDirectory();
            }
        } catch (const fs::filesystem_error&) {
            // Fall back to config directory
            mQuickLaunchDirectory = config->GetExperimentDirectory();
        }
    }

    std::strncpy(mLanguageCode, config->GetLanguage().c_str(), sizeof(mLanguageCode) - 1);
    mLanguageCode[sizeof(mLanguageCode) - 1] = '\0';
    std::strncpy(mExperimentDir, config->GetExperimentDirectory().c_str(), sizeof(mExperimentDir) - 1);
    mExperimentDir[sizeof(mExperimentDir) - 1] = '\0';
    mFullscreen = config->GetFullscreen();

    // Initialize additional run settings
    mScreenResolution = 0;  // Default to first resolution (auto)
    mVSync = false;
    mGraphicsDriver[0] = '\0';
    mCustomArguments[0] = '\0';

    // Initialize study system
    mWorkspace = std::make_shared<WorkspaceManager>();
    mSnapshots = std::make_shared<SnapshotManager>();

    // If the config has an explicit workspace path (loaded from saved settings), use that.
    // This keeps WorkspaceManager in sync with LauncherConfig so scale tests and the UI
    // settings panel both operate on the same directory.
    if (!config->GetWorkspacePath().empty()) {
        mWorkspace->SetWorkspacePath(config->GetWorkspacePath());
    }

    // Initialize scale manager with battery and workspace paths
    std::string batteryPathForScales = config->GetBatteryPath();
    std::string workspacePathForScales = mWorkspace->GetWorkspacePath();
    if (!batteryPathForScales.empty()) {
        mScaleManager = std::make_shared<ScaleManager>(batteryPathForScales, workspacePathForScales);
        printf("Initialized Scale Manager with battery path: %s, workspace path: %s\n",
               batteryPathForScales.c_str(), workspacePathForScales.c_str());
        fflush(stdout);
    } else {
        printf("Warning: Battery path not set, Scale Builder will not be available\n");
        fflush(stdout);
    }

    // Check for first run
    if (mWorkspace->IsFirstRun()) {
        mShowFirstRunDialog = true;
    } else {
        // Initialize workspace (creates directories) only if not first run
        // On first run, we wait for user confirmation in the dialog
        if (!mWorkspace->Initialize()) {
            printf("Warning: Failed to initialize workspace\n");
        }
    }

    // Initialize page editor state
    mPageEditor.show = false;
    mPageEditor.editingIndex = -1;
    mPageEditor.title[0] = '\0';
    mPageEditor.content[0] = '\0';
    mPageEditor.pageType = 0;

    // Initialize test editor state
    mTestEditor.show = false;
    mTestEditor.editingIndex = -1;
    mTestEditor.selectedTestIndex = -1;
    mTestEditor.selectedVariantIndex = 0;  // Default variant
    mTestEditor.language[0] = '\0';
    mTestEditor.randomGroup = 0;  // No randomization by default

    // Initialize translation editor state (handled by constructor now)
    // mTranslationEditor is default-constructed

    // Initialize variant naming dialog
    mVariantName[0] = '\0';

    // Initialize top-level tab (default to Study)
    mTopLevelTab = 0;

    // Initialize new study dialog state
    mNewStudyName[0] = '\0';
    mNewStudyDescription[0] = '\0';
    mNewStudyAuthor[0] = '\0';

    // Initialize new chain dialog state
    mNewChainName[0] = '\0';
    mNewChainDescription[0] = '\0';

    // Scan battery directory for tests
    // Use battery path from config, fall back to experiment directory
    std::string batteryPath = config->GetBatteryPath();
    if (batteryPath.empty()) {
        batteryPath = mExperimentDir;
    }

    // Store battery path for template loading
    mBatteryPath = batteryPath;

    if (!batteryPath.empty() && batteryPath.length() < sizeof(mExperimentDir)) {
        std::strncpy(mExperimentDir, batteryPath.c_str(), sizeof(mExperimentDir) - 1);
        mExperimentDir[sizeof(mExperimentDir) - 1] = '\0';
        ScanExperimentDirectory(batteryPath);
        printf("Scanned battery directory: %s - found %zu tests\n",
               batteryPath.c_str(), mExperiments.size());
        fflush(stdout);
    }

    // Scan templates directory
    ScanTemplates();

    // Restore previously selected study and chain
    std::string lastStudyPath = config->GetCurrentStudyPath();
    if (!lastStudyPath.empty()) {
        printf("Restoring last study: %s\n", lastStudyPath.c_str());
        LoadStudy(lastStudyPath);

        // If study loaded successfully, restore the chain
        if (mCurrentStudy) {
            std::string lastChainName = config->GetCurrentChainName();
            if (!lastChainName.empty()) {
                std::string chainPath = lastStudyPath + "/chains/" + lastChainName;
                printf("Restoring last chain: %s\n", chainPath.c_str());
                LoadChain(chainPath);
            }
        }
    }
}

LauncherUI::~LauncherUI()
{
    FreeScreenshot();
    FreeStudyTestScreenshot();

    if (mScaleBrowserScreenshot) {
        SDL_DestroyTexture(mScaleBrowserScreenshot);
        mScaleBrowserScreenshot = nullptr;
    }

    // Clean up running experiment if any
    if (mRunningExperiment) {
        delete mRunningExperiment;
        mRunningExperiment = nullptr;
    }
}

void LauncherUI::Render(bool* p_open)
{
    // Update running experiment output if any
    if (mRunningExperiment && mRunningExperiment->IsRunning()) {
        mRunningExperiment->UpdateOutput();
    }

    // Check if chain execution needs to advance to next item
    if (mRunningChain && mRunningExperiment) {
        bool isRunning = mRunningExperiment->IsRunning();
        if (!isRunning) {
            printf("DEBUG: Chain item finished (IsRunning=false), advancing...\n");

            // Check exit code - only abort chain if it's a consent form with exit code 1
            // Exit code 1 = user explicitly declined consent
            // Other non-zero codes = crash or error, not consent decline
            int exitCode = mRunningExperiment->GetExitCode();

            // Debug logging to file and stdout
            FILE* debugLog = fopen("chain_debug.log", "a");
            if (debugLog) {
                fprintf(debugLog, "=== Chain item finished ===\n");
                fprintf(debugLog, "  Exit code: %d\n", exitCode);
                fprintf(debugLog, "  mCurrentChainItemIndex: %d\n", mCurrentChainItemIndex);
                fflush(debugLog);
            }
            printf("=== Chain item finished ===\n");
            printf("  Exit code from GetExitCode(): %d\n", exitCode);
            printf("  mCurrentChainItemIndex: %d\n", mCurrentChainItemIndex);

            // Get the current chain item to check its type
            bool shouldAbortChain = false;
            bool isConsentDecline = false;
            if (debugLog) {
                fprintf(debugLog, "  Checking: exitCode=%d, chainItemIndex=%d\n", exitCode, mCurrentChainItemIndex);
            }
            if (exitCode != 0 && mCurrentChain && mCurrentChainItemIndex >= 0 &&
                mCurrentChainItemIndex < (int)mCurrentChain->GetItems().size()) {
                const ChainItem& currentItem = mCurrentChain->GetItems()[mCurrentChainItemIndex];
                if (debugLog) {
                    fprintf(debugLog, "  Item type=%d (Consent=%d)\n", (int)currentItem.type, (int)ItemType::Consent);
                    fflush(debugLog);
                }

                // Only treat exit code 1 on consent forms as "declined"
                // Other exit codes (crashes, errors) should not abort the chain as "declined"
                if (currentItem.type == ItemType::Consent && exitCode == 1) {
                    shouldAbortChain = true;
                    isConsentDecline = true;
                    if (debugLog) fprintf(debugLog, "  -> CONSENT DECLINED, aborting chain\n");
                } else if (currentItem.type == ItemType::Consent && exitCode != 0) {
                    if (debugLog) fprintf(debugLog, "  -> Consent error (code %d), continuing\n", exitCode);
                } else {
                    if (debugLog) fprintf(debugLog, "  -> Non-consent item (code %d), continuing\n", exitCode);
                }
            } else {
                if (debugLog) fprintf(debugLog, "  exitCode==0 or invalid index, continuing chain\n");
            }
            if (debugLog) {
                fflush(debugLog);
                fclose(debugLog);
            }

            if (shouldAbortChain && isConsentDecline) {
                // Exit code 1 on consent form - user explicitly declined consent
                printf("Chain terminated: User declined consent\n");

                // Accumulate output from this final item
                mChainAccumulatedStdout += mRunningExperiment->GetStdout();
                mChainAccumulatedStderr += mRunningExperiment->GetStderr();
                mChainAccumulatedStdout += "\n=== Chain terminated: User declined consent ===\n";

                // Stop chain execution
                mRunningChain = false;
                mCurrentChainItemIndex = -1;

                // Clean up runner
                delete mRunningExperiment;
                mRunningExperiment = nullptr;

                // Return early - don't continue to next item
                goto render_ui;
            }

            // Accumulate output from completed item BEFORE deleting runner
            mChainAccumulatedStdout += mRunningExperiment->GetStdout();
            mChainAccumulatedStderr += mRunningExperiment->GetStderr();

            // Add separator between items for clarity
            mChainAccumulatedStdout += "\n=== End of item " + std::to_string(mCurrentChainItemIndex + 1) + " ===\n\n";
            mChainAccumulatedStderr += "\n=== End of item " + std::to_string(mCurrentChainItemIndex + 1) + " ===\n\n";

            // Current item has finished, advance to next
            printf("Chain item %d finished, advancing...\n", mCurrentChainItemIndex + 1);
            mCurrentChainItemIndex++;

            if (mCurrentChain && mCurrentChainItemIndex < (int)mCurrentChain->GetItems().size()) {
                // Execute next item
                const ChainItem& item = mCurrentChain->GetItems()[mCurrentChainItemIndex];
                printf("Advancing to chain item %d/%zu: %s\n",
                       mCurrentChainItemIndex + 1,
                       mCurrentChain->GetItems().size(),
                       item.GetDisplayName().c_str());

                // Clean up previous runner (output already accumulated)
                delete mRunningExperiment;
                mRunningExperiment = nullptr;

                // Execute the next item
                ExecuteChainItem(mCurrentChainItemIndex);
            } else {
                // Chain completed
                printf("Chain execution completed (all %zu items finished)\n", mCurrentChain->GetItems().size());
                mRunningChain = false;
                mCurrentChainItemIndex = -1;

                // Increment participant counter for next run
                if (mCurrentChain) {
                    mCurrentChain->IncrementParticipantCounter();
                    printf("Participant counter incremented to: %d\n", mCurrentChain->GetParticipantCounter());
                }

                // Clean up runner (output already accumulated)
                if (mRunningExperiment) {
                    delete mRunningExperiment;
                    mRunningExperiment = nullptr;
                }
            }
        }
    }

render_ui:
    // Main window takes up full viewport
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar |
                                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("PEBL Launcher", p_open, window_flags);

    RenderMenuBar();

    // Reserve space at bottom for output panel, then wrap tab content
    float outputPanelHeight = mOutputExpanded ? 250.0f : 30.0f;
    float contentHeight = ImGui::GetContentRegionAvail().y - outputPanelHeight;
    ImGui::BeginChild("MainTabArea", ImVec2(0, contentHeight));

    // Top-level tabbed interface: Manage Studies vs Quick Launch
    // Make tab headers larger and more prominent (colors, padding, and larger font)
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(30, 8));   // Larger horizontal and vertical padding
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 8));    // More spacing around tabs
    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 8.0f);             // Rounded corners for pill effect
    ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.35f, 0.40f, 0.48f, 1.0f));           // Darker blue for inactive
    ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.35f, 0.60f, 0.85f, 1.0f));    // Brighter blue on hover
    ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.20f, 0.60f, 0.95f, 1.0f));     // Vivid blue for active

    if (ImGui::BeginTabBar("TopLevelTabs", ImGuiTabBarFlags_None)) {
        // Manage Studies tab
        ImGui::SetWindowFontScale(1.5f);  // Large font for tab header
        if (ImGui::BeginTabItem("Manage Studies")) {
            if (mTopLevelTab != 1) {  // Don't override if we're switching to Quick Launch
                mTopLevelTab = 0;
            }
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar(3);

            // Show study bar (study selector, new study button, etc.)
            RenderStudyBar();

            // Small spacing before second-level tabs
            ImGui::Dummy(ImVec2(0, 5));

            // Second-level tab styling
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 6));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 6));
            ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 6.0f);
            ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.35f, 0.40f, 0.48f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.35f, 0.60f, 0.85f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.20f, 0.60f, 0.95f, 1.0f));

            if (ImGui::BeginTabBar("StudyTabs", ImGuiTabBarFlags_None)) {
                ImGui::SetWindowFontScale(1.3f);  // Medium font for second-level tabs
                if (ImGui::BeginTabItem("Tests")) {
                    ImGui::SetWindowFontScale(1.0f);
                    ImGui::PopStyleColor(3);
                    ImGui::PopStyleVar(3);

                    RenderTestsTab();
                    ImGui::EndTabItem();

                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 6));
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 6));
                    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 6.0f);
                    ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.35f, 0.40f, 0.48f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.35f, 0.60f, 0.85f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.20f, 0.60f, 0.95f, 1.0f));
                    ImGui::SetWindowFontScale(1.3f);
                }

                if (ImGui::BeginTabItem("Chains")) {
                    ImGui::SetWindowFontScale(1.0f);
                    ImGui::PopStyleColor(3);
                    ImGui::PopStyleVar(3);

                    RenderChainsTab();
                    ImGui::EndTabItem();

                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 6));
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 6));
                    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 6.0f);
                    ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.35f, 0.40f, 0.48f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.35f, 0.60f, 0.85f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.20f, 0.60f, 0.95f, 1.0f));
                    ImGui::SetWindowFontScale(1.3f);
                }

                if (ImGui::BeginTabItem("Run")) {
                    ImGui::SetWindowFontScale(1.0f);
                    ImGui::PopStyleColor(3);
                    ImGui::PopStyleVar(3);

                    RenderRunTab();
                    ImGui::EndTabItem();

                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 6));
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 6));
                    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 6.0f);
                    ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.35f, 0.40f, 0.48f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.35f, 0.60f, 0.85f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.20f, 0.60f, 0.95f, 1.0f));
                }

                // Final cleanup for second-level tabs
                ImGui::SetWindowFontScale(1.0f);
                ImGui::PopStyleColor(3);
                ImGui::PopStyleVar(3);

                ImGui::EndTabBar();
            } else {
                // If tab bar didn't begin, clean up styles
                ImGui::SetWindowFontScale(1.0f);
                ImGui::PopStyleColor(3);
                ImGui::PopStyleVar(3);
            }

            ImGui::EndTabItem();

            // Restore styles for next top-level tab header
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(30, 8));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 8));
            ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 8.0f);
            ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.35f, 0.40f, 0.48f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.35f, 0.60f, 0.85f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.20f, 0.60f, 0.95f, 1.0f));
            ImGui::SetWindowFontScale(1.5f);
        }

        // Quick Launch tab
        ImGuiTabItemFlags quickLaunchFlags = (mTopLevelTab == 1) ? ImGuiTabItemFlags_SetSelected : 0;
        if (ImGui::BeginTabItem("Quick Launch", nullptr, quickLaunchFlags)) {
            if (mTopLevelTab == 1) {
                mTopLevelTab = -1;  // Reset flag after first frame
            }
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar(3);

            RenderQuickLaunchTab();
            ImGui::EndTabItem();

            // Restore styles for next top-level tab header
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(30, 8));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 8));
            ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 8.0f);
            ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.35f, 0.40f, 0.48f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.35f, 0.60f, 0.85f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.20f, 0.60f, 0.95f, 1.0f));
            ImGui::SetWindowFontScale(1.5f);
        }

        // Scales/Surveys tab
        ImGuiTabItemFlags scaleBuilderFlags = (mTopLevelTab == 2) ? ImGuiTabItemFlags_SetSelected : 0;
        if (ImGui::BeginTabItem("Scales/Surveys", nullptr, scaleBuilderFlags)) {
            if (mTopLevelTab == 2) {
                mTopLevelTab = -1;  // Reset flag after first frame
            }
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar(3);

            ShowScaleBuilder();
            ImGui::EndTabItem();

            // Restore styles for consistency
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(30, 8));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 8));
            ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 8.0f);
            ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.35f, 0.40f, 0.48f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.35f, 0.60f, 0.85f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.20f, 0.60f, 0.95f, 1.0f));
        }

        // Final cleanup - pop the styles that are still on the stack
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(3);

        ImGui::EndTabBar();
    } else {
        // If tab bar didn't begin, clean up styles
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(3);
    }

    ImGui::EndChild();  // MainTabArea

    // Output panel at bottom of window
    RenderOutputPanel();

    ImGui::End();

    // Show about dialog if requested
    if (mShowAbout) {
        ShowAboutDialog();
    }

    // Show variant naming dialog if requested
    if (mShowVariantNameDialog) {
        ShowVariantNameDialog();
    }

    // Show parameter editor if requested
    if (mShowParameterEditor) {
        ShowParameterEditor();
    }

    // Show settings dialog if requested
    if (mShowSettings) {
        ShowSettingsDialog();
    }

    // Show page editor if requested
    if (mPageEditor.show) {
        ShowPageEditor();
    }

    // Show test editor if requested
    if (mTestEditor.show) {
        ShowTestEditor();
    }

    // Show code editor if requested
    if (mShowCodeEditor) {
        ShowCodeEditor();
    }

    // Show question editor dialog if requested
    if (mQuestionEditor.show) {
        ShowQuestionEditor();
    }

    // Show batch import dialog if requested
    if (mBatchImport.show) {
        ShowBatchImportDialog();
    }

    // Show dimension editor dialog if requested
    if (mDimensionEditor.show) {
        ShowDimensionEditor();
    }

    // Show create study from scale dialog if requested
    if (mCreateStudyDialog.show) {
        ShowCreateStudyFromScaleDialog();
    }

    // Show correct answers editor dialog if requested
    if (mCorrectAnswersEditor.show) {
        ShowCorrectAnswersEditor();
    }

    // Show norms editor dialog if requested
    if (mNormsEditor.show) {
        ShowNormsEditor();
    }

    // Show translation editor dialog if requested
    bool translationEditorWasShown = mTranslationEditor.show;
    if (mTranslationEditor.show) {
        ShowTranslationEditorDialog();
    }
    // When scale-mode translation editor closes, reload translations from disk into mCurrentScale
    if (translationEditorWasShown && !mTranslationEditor.show &&
        mTranslationEditor.scaleMode && mCurrentScale && mScaleManager) {
        auto reloaded = mScaleManager->LoadScale(mCurrentScale->GetScaleInfo().code);
        if (reloaded) {
            mCurrentScale->GetTranslations() = reloaded->GetTranslations();
        }
        mTranslationEditor.ClearScaleMode();
    }

    // Show new study dialog if requested
    if (mShowNewStudyDialog) {
        ShowNewStudyDialog();
    }

    // Show new chain dialog if requested
    if (mShowNewChainDialog) {
        ShowNewChainDialog();
    }

    // Show study settings dialog if requested
    if (mShowStudySettingsDialog) {
        ShowStudySettingsDialog();
    }

    // Show first run dialog if requested
    if (mShowFirstRunDialog) {
        ShowFirstRunDialog();
    }

    // Show getting started dialog if there are no studies
    if (mShowGettingStartedDialog) {
        ShowGettingStartedDialog();
    }

    // Show duplicate subject code warning if requested
    if (mShowDuplicateSubjectWarning) {
        ShowDuplicateSubjectWarning();
    }

    // Show edit participant code dialog if requested
    if (mShowEditParticipantCodeDialog) {
        ShowEditParticipantCodeDialog();
    }

    // Show snapshot created dialog if requested
    if (mShowSnapshotCreated) {
        ShowSnapshotCreatedDialog();
    }
}

void LauncherUI::RenderMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Study...", "Ctrl+N")) {
                mShowNewStudyDialog = true;
            }

            if (ImGui::MenuItem("Open Study...", "Ctrl+O")) {
                // Start in workspace studies directory
                std::string studiesPath = mWorkspace->GetStudiesPath();

                #ifdef __linux__
                std::string command = "zenity --file-selection --directory --title=\"Select Study Directory\" --filename=\"" + studiesPath + "/\" 2>/dev/null";
                FILE* pipe = popen(command.c_str(), "r");
                std::string studyPath;
                if (pipe) {
                    char buffer[1024];
                    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                        studyPath = buffer;
                        if (!studyPath.empty() && studyPath[studyPath.length()-1] == '\n') {
                            studyPath.erase(studyPath.length()-1);
                        }
                    }
                    pclose(pipe);
                }
                #else
                std::string studyPath = OpenDirectoryDialog("Select Study Directory");
                #endif

                if (!studyPath.empty()) {
                    LoadStudy(studyPath);
                }
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Settings...", "Ctrl+,")) {
                mShowSettings = true;
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                SDL_Event quit_event;
                quit_event.type = SDL_QUIT;
                SDL_PushEvent(&quit_event);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Study"))
        {
            bool hasStudy = (mCurrentStudy != nullptr);

            if (ImGui::MenuItem("New Study...")) {
                mShowNewStudyDialog = true;
            }

            if (ImGui::MenuItem("Load Study...")) {
                std::string studiesPath = mWorkspace->GetStudiesPath();

                #ifdef __linux__
                std::string command = "zenity --file-selection --directory --title=\"Select Study Directory\" --filename=\"" + studiesPath + "/\" 2>/dev/null";
                FILE* pipe = popen(command.c_str(), "r");
                std::string studyPath;
                if (pipe) {
                    char buffer[1024];
                    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                        studyPath = buffer;
                        if (!studyPath.empty() && studyPath[studyPath.length()-1] == '\n') {
                            studyPath.erase(studyPath.length()-1);
                        }
                    }
                    pclose(pipe);
                }
                #else
                std::string studyPath = OpenDirectoryDialog("Select Study Directory");
                #endif

                if (!studyPath.empty()) {
                    LoadStudy(studyPath);
                }
            }

            if (ImGui::MenuItem("Open Study Directory...", nullptr, false, hasStudy)) {
                if (mCurrentStudy) {
                    std::string studyPath = mCurrentStudy->GetPath();
                    OpenDirectoryInFileBrowser(studyPath);
                }
            }

            if (ImGui::MenuItem("Study Settings...", nullptr, false, hasStudy)) {
                mShowStudySettingsDialog = true;
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Create Snapshot...", nullptr, false, hasStudy)) {
                if (mCurrentStudy && mSnapshots) {
                    // Create snapshot in workspace/snapshots directory
                    std::string snapshotsDir = mWorkspace->GetWorkspacePath() + "/snapshots";

                    // Create snapshots directory if it doesn't exist
                    if (!fs::exists(snapshotsDir)) {
                        fs::create_directories(snapshotsDir);
                    }

                    std::string snapshotName = mSnapshots->CreateSnapshot(mCurrentStudy->GetPath(), snapshotsDir);

                    if (!snapshotName.empty()) {
                        std::string snapshotPath = snapshotsDir + "/" + snapshotName;
                        printf("Created snapshot: %s at %s\n", snapshotName.c_str(), snapshotPath.c_str());

                        // Store snapshot info for dialog
                        std::strncpy(mLastSnapshotName, snapshotName.c_str(), sizeof(mLastSnapshotName) - 1);
                        mLastSnapshotName[sizeof(mLastSnapshotName) - 1] = '\0';
                        std::strncpy(mLastSnapshotPath, snapshotPath.c_str(), sizeof(mLastSnapshotPath) - 1);
                        mLastSnapshotPath[sizeof(mLastSnapshotPath) - 1] = '\0';

                        // Show success dialog
                        mShowSnapshotCreated = true;
                    } else {
                        printf("Failed to create snapshot\n");
                    }
                }
            }

            if (ImGui::MenuItem("Import Snapshot...")) {
                std::string snapshotPath = OpenDirectoryDialog("Select Snapshot Directory");
                if (!snapshotPath.empty() && mSnapshots) {
                    ImportSnapshotFromPath(snapshotPath);
                }
            }

            if (ImGui::MenuItem("Import Snapshot ZIP...")) {
                std::string zipPath = OpenFileDialog("Select Snapshot ZIP", "*.zip", mWorkspace->GetSnapshotsPath());
                if (!zipPath.empty() && mSnapshots) {
                    // Create temporary directory for extraction
                    std::string tempDir = "/tmp/pebl_snapshot_import_" + std::to_string(std::time(nullptr));
                    mkdir(tempDir.c_str(), 0755);

                    // Extract ZIP
                    printf("Extracting snapshot ZIP...\n");
                    auto extractResult = ZipExtractor::ExtractAll(zipPath, tempDir);
                    if (extractResult.success) {
                        // Determine snapshot directory
                        std::string snapshotPath;

                        // Check if tempDir itself is the snapshot (has study-info.json)
                        std::string studyInfoPath = tempDir + "/study-info.json";
                        if (fs::exists(studyInfoPath)) {
                            // Files extracted directly to tempDir
                            snapshotPath = tempDir;
                            printf("Snapshot extracted directly to temp directory\n");
                        } else {
                            // Look for subdirectory containing study-info.json
                            try {
                                for (const auto& entry : fs::directory_iterator(tempDir)) {
                                    if (!entry.is_directory()) continue;
                                    std::string candidatePath = entry.path().string();
                                    std::string candidateStudyInfo = candidatePath + "/study-info.json";
                                    if (fs::exists(candidateStudyInfo)) {
                                        snapshotPath = candidatePath;
                                        printf("Snapshot found in subdirectory: %s\n", entry.path().filename().string().c_str());
                                        break;
                                    }
                                }
                            } catch (const fs::filesystem_error&) {
                                // Directory doesn't exist or can't be read
                            }
                        }

                        if (!snapshotPath.empty()) {
                            ImportSnapshotFromPath(snapshotPath);
                        } else {
                            printf("Could not locate snapshot directory in extracted ZIP\n");
                        }

                        // Clean up temp directory
                        std::string cleanupCmd = "rm -rf " + tempDir;
                        system(cleanupCmd.c_str());
                    } else {
                        printf("Failed to extract ZIP file: %s\n", extractResult.error.c_str());
                        rmdir(tempDir.c_str());
                    }
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Tools"))
        {
            if (ImGui::MenuItem("Open Battery Directory...")) {
                std::string batteryPath = mConfig->GetBatteryPath();
                if (!batteryPath.empty()) {
                    OpenDirectoryInFileBrowser(batteryPath);
                }
            }

            if (ImGui::MenuItem("Open Workspace Directory...")) {
                std::string workspacePath = mWorkspace->GetWorkspacePath();
                if (!workspacePath.empty()) {
                    OpenDirectoryInFileBrowser(workspacePath);
                }
            }

            bool hasStudy = (mCurrentStudy != nullptr);
            if (ImGui::MenuItem("Open Study Directory...", nullptr, false, hasStudy)) {
                if (mCurrentStudy) {
                    std::string studyPath = mCurrentStudy->GetPath();
                    OpenDirectoryInFileBrowser(studyPath);
                }
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Data Combiner...")) {
                // Start directory picker in current study's data directory if available
                std::string startDir;
                if (mCurrentStudy) {
                    startDir = mCurrentStudy->GetPath() + "/data";
                } else {
                    startDir = mWorkspace->GetWorkspacePath();
                }

                std::string selectedDir = OpenDirectoryDialog("Select Directory for Data Combiner", startDir);
                if (!selectedDir.empty()) {
                    LaunchDataCombiner(selectedDir);
                }
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Combine data files from a directory");
            }

            if (ImGui::MenuItem("Scales/Surveys...", nullptr, false, mScaleManager != nullptr)) {
                mTopLevelTab = 2;  // Switch to Scales/Surveys tab
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Create and edit psychological scales/surveys");
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Refresh Battery Tests")) {
                std::string batteryPath = mConfig->GetBatteryPath();
                if (!batteryPath.empty()) {
                    ScanExperimentDirectory(batteryPath);
                    printf("Refreshed battery: %zu tests found\n", mExperiments.size());
                }
            }

            if (ImGui::MenuItem("View Launch Log")) {
                std::string logPath = ExperimentRunner::GetLaunchLogPath();
                printf("Opening launch log: %s\n", logPath.c_str());
                #ifdef __linux__
                system(("xdg-open \"" + logPath + "\" &").c_str());
                #elif defined(_WIN32)
                system(("start \"\" \"" + logPath + "\"").c_str());
                #elif defined(__APPLE__)
                system(("open \"" + logPath + "\"").c_str());
                #endif
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("View log of launched PEBL processes");
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About PEBL Launcher")) {
                mShowAbout = true;
            }

            if (ImGui::MenuItem("PEBL Documentation")) {
                #ifdef __linux__
                system("xdg-open https://pebl.sourceforge.net/documentation.html &");
                #elif defined(_WIN32)
                system("start https://pebl.sourceforge.net/documentation.html");
                #elif defined(__APPLE__)
                system("open https://pebl.sourceforge.net/documentation.html");
                #endif
            }

            if (ImGui::MenuItem("PEBL Manual (PDF)")) {
                // Open local PDF manual
                std::string manualName = std::string("PEBLManual") + PEBL_VERSION + ".pdf";
                std::string workspacePath = mConfig->GetWorkspacePath();
                std::string manualPath;

                // Portable mode: manual is at portable root (e.g., PEBL2.3_Portable/PEBLManual2.3.pdf)
                // Installed mode: manual is in Documents/pebl-exp.2.3/doc/PEBLManual2.3.pdf
                std::vector<std::string> possiblePaths = {
                    (fs::path(workspacePath) / manualName).string(),  // Portable: root
                    (fs::path(workspacePath) / "doc" / manualName).string(),  // Installed: doc subfolder
                };

                for (const auto& path : possiblePaths) {
                    if (fs::exists(path)) {
                        manualPath = path;
                        break;
                    }
                }

                if (manualPath.empty()) {
                    manualPath = possiblePaths[0];
                    printf("Warning: Manual not found at expected locations\n");
                }

                #ifdef __linux__
                system(("xdg-open \"" + manualPath + "\" &").c_str());
                #elif defined(_WIN32)
                system(("start \"\" \"" + manualPath + "\"").c_str());
                #elif defined(__APPLE__)
                system(("open \"" + manualPath + "\"").c_str());
                #endif
            }

            if (ImGui::MenuItem("Function Reference")) {
                #ifdef __linux__
                system("xdg-open https://pebl.sourceforge.net/function-reference/index.html &");
                #elif defined(_WIN32)
                system("start https://pebl.sourceforge.net/function-reference/index.html");
                #elif defined(__APPLE__)
                system("open https://pebl.sourceforge.net/function-reference/index.html");
                #endif
            }

            if (ImGui::MenuItem("PEBL Website")) {
                #ifdef __linux__
                system("xdg-open https://pebl.sourceforge.net &");
                #elif defined(_WIN32)
                system("start https://pebl.sourceforge.net");
                #elif defined(__APPLE__)
                system("open https://pebl.sourceforge.net");
                #endif
            }

            if (ImGui::MenuItem("PEBL Online Hub")) {
                #ifdef __linux__
                system("xdg-open https://peblhub.online &");
                #elif defined(_WIN32)
                system("start https://peblhub.online");
                #elif defined(__APPLE__)
                system("open https://peblhub.online");
                #endif
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Show First-Run Dialog")) {
                mShowFirstRunDialog = true;
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void LauncherUI::RenderFilePanel()
{
    ImGui::Text("Experiment Directory:");
    ImGui::PushItemWidth(-1);
    if (ImGui::InputText("##ExperimentDir", mExperimentDir, sizeof(mExperimentDir),
                         ImGuiInputTextFlags_EnterReturnsTrue)) {
        ScanExperimentDirectory(mExperimentDir);
        mConfig->SetExperimentDirectory(mExperimentDir);
    }
    ImGui::PopItemWidth();

    ImGui::SameLine();
    if (ImGui::Button("Browse...")) {
        std::string dir = OpenDirectoryDialog("Select Experiment Directory");
        if (!dir.empty()) {
            std::strcpy(mExperimentDir, dir.c_str());
            ScanExperimentDirectory(mExperimentDir);
            mConfig->SetExperimentDirectory(mExperimentDir);
        }
    }

    ImGui::Separator();

    // Recent tests section
    const std::vector<RecentExperiment>& recent = mConfig->GetRecentExperiments();
    if (!recent.empty()) {
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Recent Tests:");

        ImGui::BeginChild("RecentList", ImVec2(0, 120), true);
        ImGui::SetWindowFontScale(1.3f);

        for (const auto& exp : recent) {
            // Show just the name, with timestamp as tooltip
            if (ImGui::Selectable(exp.name.c_str())) {
                // Find this experiment in our main list
                for (int i = 0; i < (int)mExperiments.size(); i++) {
                    if (mExperiments[i].path == exp.path) {
                        mSelectedExperiment = i;
                        LoadExperimentInfo(exp.path);
                        break;
                    }
                }
            }

            if (ImGui::IsItemHovered()) {
                // Format timestamp
                char timeBuf[64];
                struct tm* timeinfo = localtime(&exp.lastRun);
                strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", timeinfo);
                ImGui::SetTooltip("Last run: %s\n%s", timeBuf, exp.path.c_str());
            }
        }

        ImGui::SetWindowFontScale(1.0f);
        ImGui::EndChild();

        ImGui::Separator();
    }

    ImGui::Text("Tests (%zu found):", mExperiments.size());

    // Filter box
    static char filter[256] = "";
    ImGui::PushItemWidth(-1);
    ImGui::InputTextWithHint("##Filter", "Filter tests...", filter, sizeof(filter));
    ImGui::PopItemWidth();

    ImGui::Separator();

    // Scrollable experiment list with larger font
    ImGui::BeginChild("ExperimentList", ImVec2(0, 0), false);

    // Increase font scale for better readability
    ImGui::SetWindowFontScale(1.3f);

    for (int i = 0; i < (int)mExperiments.size(); i++) {
        const ExperimentInfo& exp = mExperiments[i];

        // Apply filter
        if (strlen(filter) > 0 &&
            exp.name.find(filter) == std::string::npos) {
            continue;
        }

        bool is_selected = (mSelectedExperiment == i);
        if (ImGui::Selectable(exp.name.c_str(), is_selected)) {
            mSelectedExperiment = i;
            LoadExperimentInfo(exp.path);
        }

        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", exp.path.c_str());
        }
    }

    // Reset font scale
    ImGui::SetWindowFontScale(1.0f);

    ImGui::EndChild();
}

void LauncherUI::RenderDetailsPanel()
{
    // Create tabbed interface
    if (ImGui::BeginTabBar("DetailsTabs")) {
        // Details Tab
        if (ImGui::BeginTabItem("Details")) {
            RenderDetailsTab();
            ImGui::EndTabItem();
        }

        // Study Tab
        if (ImGui::BeginTabItem("Study")) {
            RenderStudyTab();
            ImGui::EndTabItem();
        }

        // Chain Tab
        if (ImGui::BeginTabItem("Chain")) {
            RenderChainTab();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

void LauncherUI::RenderDetailsTab()
{
    if (mSelectedExperiment < 0 || mSelectedExperiment >= (int)mExperiments.size()) {
        ImGui::TextWrapped("Select a test from the list on the left to view its details.");
        return;
    }

    const ExperimentInfo& exp = mExperiments[mSelectedExperiment];

    // Calculate layout heights
    float availHeight = ImGui::GetContentRegionAvail().y;
    float topSectionHeight = availHeight * 0.45f; // Top section takes 45% of height
    float panelWidth = ImGui::GetContentRegionAvail().x;
    float screenshotWidth = panelWidth * 0.5f;

    // Top section: Screenshot (left) and Info (right) side-by-side
    ImGui::BeginChild("TopSection", ImVec2(0, topSectionHeight), false);

    // Left side: Screenshot
    ImGui::BeginChild("ScreenshotPanel", ImVec2(screenshotWidth, 0), false, ImGuiWindowFlags_NoScrollbar);

    if (mScreenshotTexture) {
        // Calculate display size while maintaining aspect ratio
        float aspectRatio = (float)mScreenshotHeight / (float)mScreenshotWidth;
        float displayWidth = screenshotWidth - 20; // Leave some padding
        float displayHeight = displayWidth * aspectRatio;

        // Limit height to available space
        float maxHeight = topSectionHeight - 20;
        if (displayHeight > maxHeight) {
            displayHeight = maxHeight;
            displayWidth = displayHeight / aspectRatio;
        }

        ImGui::Image((ImTextureID)(intptr_t)mScreenshotTexture,
                     ImVec2(displayWidth, displayHeight));
    } else {
        ImGui::TextDisabled("No screenshot available");
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // Right side: Experiment info
    ImGui::BeginChild("InfoPanel", ImVec2(0, 0), false);

    // Experiment name
    ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "%s", exp.name.c_str());
    ImGui::Separator();
    ImGui::Spacing();

    // Basic information
    ImGui::Text("Path:");
    ImGui::TextWrapped("%s", exp.directory.c_str());

    if (ImGui::Button("Open Experiment Folder", ImVec2(-1, 0))) {
        OpenDirectoryInFileBrowser(exp.directory);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Open this experiment's folder in file browser");
    }

    ImGui::Spacing();

    // Parameters
    if (exp.hasParams) {
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "✓ Parameters available");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("This experiment has configurable parameters");
        }
    } else {
        ImGui::TextDisabled("No parameters");
    }

    // Translations/Languages
    if (exp.hasTranslations && !mAvailableLanguages.empty()) {
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "✓ Translations: %zu languages",
                          mAvailableLanguages.size());

        if (ImGui::IsItemHovered()) {
            std::string tooltip = "Available languages:\n";
            for (size_t i = 0; i < mAvailableLanguages.size(); i++) {
                tooltip += mAvailableLanguages[i];
                if (i < mAvailableLanguages.size() - 1) tooltip += ", ";
            }
            ImGui::SetTooltip("%s", tooltip.c_str());
        }
    } else {
        ImGui::TextDisabled("No translations");
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Description
    ImGui::Text("Description:");
    ImGui::BeginChild("Description", ImVec2(0, 100), true);
    if (!exp.description.empty()) {
        ImGui::TextWrapped("%s", exp.description.c_str());
    } else {
        ImGui::TextDisabled("No description available");
    }
    ImGui::EndChild();

    ImGui::EndChild(); // End InfoPanel
    ImGui::EndChild(); // End TopSection

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Configuration section
    ImGui::Text("Configuration:");

    // Subject code and language on same line (compact layout)
    float itemWidth = ImGui::GetContentRegionAvail().x * 0.45f;

    ImGui::Text("Subject Code:");
    ImGui::SameLine(150);
    ImGui::PushItemWidth(itemWidth);
    if (ImGui::InputText("##SubjectCode", mSubjectCode, sizeof(mSubjectCode))) {
        mConfig->SetSubjectCode(mSubjectCode);
    }
    ImGui::PopItemWidth();

    // Language selection (dropdown)
    if (exp.hasTranslations && !mAvailableLanguages.empty()) {
        ImGui::SameLine();
        ImGui::Text("Language:");
        ImGui::SameLine();
        ImGui::PushItemWidth(itemWidth);
        if (ImGui::BeginCombo("##Language", mLanguageCode)) {
            for (const auto& lang : mAvailableLanguages) {
                bool is_selected = (strcmp(mLanguageCode, lang.c_str()) == 0);
                if (ImGui::Selectable(lang.c_str(), is_selected)) {
                    std::strcpy(mLanguageCode, lang.c_str());
                    mConfig->SetLanguage(mLanguageCode);
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();
    }

    ImGui::Spacing();

    // Fullscreen toggle
    if (ImGui::Checkbox("Fullscreen Mode", &mFullscreen)) {
        mConfig->SetFullscreen(mFullscreen);
    }

    ImGui::Spacing();

    // Parameter editor button (if experiment has parameters)
    if (exp.hasParams) {
        if (ImGui::Button("Edit Parameters", ImVec2(-1, 0))) {
            // Sync schema from scale definition if this is a scale-based test
            std::string expScaleCode = fs::path(exp.path).stem().string();
            SyncScaleSchema(exp.directory, expScaleCode);

            // Load parameters from schema file
            mParameters.clear();

            fs::path schemaPath = fs::path(exp.directory) / "params" / (fs::path(exp.path).filename().string() + ".schema.json");

            if (fs::exists(schemaPath)) {
                try {
                    std::ifstream schemaFile(schemaPath);
                    if (!schemaFile.is_open()) {
                        printf("ERROR: Could not open schema file: %s\n", schemaPath.string().c_str());
                    } else {
                        // Parse JSON schema file properly
                        nlohmann::json schemaJson;
                        schemaFile >> schemaJson;
                        schemaFile.close();

                        if (!schemaJson.contains("parameters") || !schemaJson["parameters"].is_array()) {
                            printf("ERROR: Schema file does not contain 'parameters' array\n");
                        } else {
                            // Extract parameters from JSON
                            for (const auto& paramJson : schemaJson["parameters"]) {
                                if (!paramJson.contains("name") || !paramJson.contains("default")) {
                                    continue;  // Skip invalid entries
                                }

                                Parameter param;
                                param.name = paramJson["name"].get<std::string>();

                                // Convert default value to string
                                if (paramJson["default"].is_string()) {
                                    param.defaultValue = paramJson["default"].get<std::string>();
                                } else if (paramJson["default"].is_number_integer()) {
                                    param.defaultValue = std::to_string(paramJson["default"].get<int>());
                                } else if (paramJson["default"].is_number_float()) {
                                    param.defaultValue = std::to_string(paramJson["default"].get<double>());
                                } else if (paramJson["default"].is_boolean()) {
                                    param.defaultValue = paramJson["default"].get<bool>() ? "true" : "false";
                                } else if (paramJson["default"].is_array()) {
                                    param.defaultValue = paramJson["default"].dump();
                                } else {
                                    param.defaultValue = paramJson["default"].dump();
                                }

                                // Extract description
                                if (paramJson.contains("description")) {
                                    param.description = paramJson["description"].get<std::string>();
                                }

                                // Extract options if available
                                if (paramJson.contains("options") && paramJson["options"].is_array()) {
                                    for (const auto& opt : paramJson["options"]) {
                                        if (opt.is_string()) {
                                            param.options.push_back(opt.get<std::string>());
                                        } else {
                                            param.options.push_back(opt.dump());
                                        }
                                    }
                                }

                                param.value = param.defaultValue;  // Initialize to default
                                mParameters.push_back(param);
                            }
                            printf("Loaded %zu parameters from schema\n", mParameters.size());
                        }
                    }
                } catch (const std::exception& e) {
                    printf("ERROR parsing schema JSON: %s\n", e.what());
                }
            }

            // Set parameter file path
            mParameterFile = (fs::path(exp.directory) / (fs::path(exp.path).stem().string() + ".par.json")).string();

            // Load existing parameter file if it exists
            if (fs::exists(mParameterFile)) {
                try {
                    std::ifstream paramFile(mParameterFile);
                    if (paramFile.is_open()) {
                        nlohmann::json paramJson;
                        paramFile >> paramJson;
                        paramFile.close();

                        // Update parameter values from JSON object
                        for (auto& param : mParameters) {
                            if (paramJson.contains(param.name)) {
                                // Convert JSON value to string
                                if (paramJson[param.name].is_string()) {
                                    param.value = paramJson[param.name].get<std::string>();
                                } else if (paramJson[param.name].is_number_integer()) {
                                    param.value = std::to_string(paramJson[param.name].get<int>());
                                } else if (paramJson[param.name].is_number_float()) {
                                    param.value = std::to_string(paramJson[param.name].get<double>());
                                } else if (paramJson[param.name].is_boolean()) {
                                    param.value = paramJson[param.name].get<bool>() ? "true" : "false";
                                } else {
                                    param.value = paramJson[param.name].dump();
                                }
                            }
                        }
                        printf("Loaded parameter values from: %s\n", mParameterFile.c_str());
                    }
                } catch (const std::exception& e) {
                    printf("ERROR parsing parameter file JSON: %s\n", e.what());
                }
            }

            mShowParameterEditor = true;
        }
        ImGui::Spacing();
    }

    ImGui::Separator();
    ImGui::Spacing();

    // Add to Study button
    float buttonWidth = ImGui::GetContentRegionAvail().x * 0.48f;
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.6f, 0.9f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.4f, 0.7f, 1.0f));

    if (ImGui::Button("Add to Study", ImVec2(buttonWidth, 40))) {
        AddTestToStudy();
    }

    ImGui::PopStyleColor(3);

    ImGui::SameLine();

    // Run Test button
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));

    if (ImGui::Button("Run Test", ImVec2(buttonWidth, 40))) {
        RunTest();
    }

    ImGui::PopStyleColor(3);

    // Status/info text and folder button
    ImGui::Spacing();
    ImGui::TextDisabled("Path: %s", exp.directory.c_str());
    ImGui::SameLine();
    if (ImGui::SmallButton("Open Folder")) {
        OpenDirectoryInFileBrowser(exp.directory);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Open experiment folder in file browser");
    }
}

void LauncherUI::RenderChainTab()
{

    // Top section: Chain selector and info
    ImGui::BeginChild("ChainSelector", ImVec2(0, 150), true);

    // Chain selection dropdown
    ImGui::Text("Current Chain:");
    ImGui::SameLine();

    const char* currentChainName = mCurrentChain ? mCurrentChain->GetName().c_str() : "None";
    ImGui::PushItemWidth(250);
    if (ImGui::BeginCombo("##ChainSelect", currentChainName)) {
        // List chains from current study
        if (mCurrentStudy) {
            auto chainFiles = mCurrentStudy->GetChainFiles();

            // Only show "None" option if no chains exist
            if (chainFiles.empty()) {
                if (ImGui::Selectable("None", !mCurrentChain)) {
                    mCurrentChain.reset();
                }
            }

            for (size_t i = 0; i < chainFiles.size(); i++) {
                std::string chainName = fs::path(chainFiles[i]).stem().string();
                bool is_selected = (mCurrentChain &&
                                  fs::path(mCurrentChain->GetFilePath()).stem().string() == chainName);

                if (ImGui::Selectable(chainName.c_str(), is_selected)) {
                    // Construct full path to chain file
                    std::string fullChainPath = mCurrentStudy->GetPath() + "/chains/" + chainFiles[i];
                    LoadChain(fullChainPath);
                    printf("Loaded chain from dropdown: %s\n", chainName.c_str());
                }
            }
        } else {
            // No study loaded - show "None" option
            if (ImGui::Selectable("None", !mCurrentChain)) {
                mCurrentChain.reset();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();

    ImGui::SameLine();

    // New chain button
    if (ImGui::Button("New Chain...")) {
        CreateNewChain();
    }

    ImGui::SameLine();

    // Save chain button
    if (mCurrentChain) {
        if (ImGui::Button("Save Chain")) {
            SaveCurrentChain();
        }

        ImGui::SameLine();

        // Copy chain button
        if (ImGui::Button("Copy Chain...")) {
            ImGui::OpenPopup("Copy Chain");
        }

        ImGui::SameLine();

        // Delete chain button
        if (ImGui::Button("Delete Chain")) {
            ImGui::OpenPopup("Confirm Delete Chain");
        }
    }

    // Copy Chain popup
    if (ImGui::BeginPopupModal("Copy Chain", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Create a copy of '%s'", mCurrentChain ? mCurrentChain->GetName().c_str() : "");
        ImGui::Spacing();

        static char copyName[256] = "";
        if (ImGui::IsWindowAppearing()) {
            // Pre-fill with original name + "_copy"
            if (mCurrentChain) {
                std::string defaultName = mCurrentChain->GetName() + "_copy";
                std::strncpy(copyName, defaultName.c_str(), sizeof(copyName) - 1);
                copyName[sizeof(copyName) - 1] = '\0';
            }
            ImGui::SetKeyboardFocusHere();
        }

        ImGui::Text("New chain name:");
        ImGui::InputText("##CopyChainName", copyName, sizeof(copyName));

        ImGui::Spacing();

        if (ImGui::Button("Copy", ImVec2(120, 0))) {
            if (strlen(copyName) > 0 && mCurrentChain && mCurrentStudy) {
                // Create a copy of the chain with the new name
                std::string studyPath = mCurrentStudy->GetPath();
                std::string newChainPath = (fs::path(studyPath) / "chains" / (std::string(copyName) + ".json")).string();

                // Copy the current chain file
                std::string oldChainPath = mCurrentChain->GetFilePath();
                try {
                    fs::copy_file(oldChainPath, newChainPath, fs::copy_options::overwrite_existing);

                    // Load the new chain and update its name
                    auto newChain = Chain::LoadFromFile(newChainPath);
                    if (newChain) {
                        newChain->SetName(copyName);
                        newChain->Save();

                        // Select the new chain
                        mCurrentChain = newChain;
                        mConfig->SetCurrentChainName(copyName);
                        printf("Created copy of chain: %s\n", copyName);
                    }
                } catch (const std::exception& e) {
                    printf("Error copying chain: %s\n", e.what());
                }

                copyName[0] = '\0';
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            copyName[0] = '\0';
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    // Delete Chain confirmation popup
    if (ImGui::BeginPopupModal("Confirm Delete Chain", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure you want to delete the chain '%s'?", mCurrentChain ? mCurrentChain->GetName().c_str() : "");
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "This action cannot be undone.");
        ImGui::Spacing();

        if (ImGui::Button("Delete", ImVec2(120, 0))) {
            if (mCurrentChain && mCurrentStudy) {
                std::string chainPath = mCurrentChain->GetFilePath();
                std::string chainName = mCurrentChain->GetName();
                try {
                    fs::remove(chainPath);
                    mCurrentChain.reset();
                    mConfig->SetCurrentChainName("");
                    printf("Deleted chain: %s\n", chainName.c_str());
                } catch (const std::exception& e) {
                    printf("Error deleting chain: %s\n", e.what());
                }
            }
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::Spacing();

    // Chain info
    if (mCurrentChain) {
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "%s", mCurrentChain->GetName().c_str());
        ImGui::Text("Description: %s", mCurrentChain->GetDescription().c_str());
        ImGui::Text("Items in chain: %zu", mCurrentChain->GetItems().size());

        ImGui::Spacing();

        // Upload configuration checkbox
        bool uploadEnabled = mCurrentChain->GetUploadEnabled();
        if (ImGui::Checkbox("Upload data to server", &uploadEnabled)) {
            mCurrentChain->SetUploadEnabled(uploadEnabled);

            // If enabling upload, ensure study has upload config
            if (uploadEnabled && mCurrentStudy) {
                bool hasConfig = !mCurrentStudy->GetStudyToken().empty() &&
                               !mCurrentStudy->GetUploadServerURL().empty();

                if (!hasConfig) {
                    ImGui::OpenPopup("Upload Config Required");
                } else {
                    // Create/update upload.json for all tests in chain
                    int created = 0;
                    for (const auto& item : mCurrentChain->GetItems()) {
                        if (!item.IsPageItem()) {
                            if (mCurrentStudy->CreateUploadConfigForTest(item.testName)) {
                                created++;
                            }
                        }
                    }
                    if (created > 0) {
                        printf("Created %d upload.json file(s) for chain tests\n", created);
                    }
                }
            }

            mCurrentChain->Save();
        }

        // Warning popup if upload config is missing
        if (ImGui::BeginPopupModal("Upload Config Required", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::TextWrapped("To enable data upload, you must first configure the study's upload settings.");
            ImGui::Spacing();
            ImGui::TextWrapped("Please go to Study Settings and enter:");
            ImGui::BulletText("Upload Server URL");
            ImGui::BulletText("Study Token");
            ImGui::Spacing();

            if (ImGui::Button("OK", ImVec2(120, 0))) {
                // Revert the checkbox
                mCurrentChain->SetUploadEnabled(false);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (uploadEnabled) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "(Enabled)");
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // LSL configuration checkbox
        bool lslEnabled = mCurrentChain->GetLSLEnabled();
        if (ImGui::Checkbox("Enable LSL streaming", &lslEnabled)) {
            mCurrentChain->SetLSLEnabled(lslEnabled);
            mCurrentChain->Save();
        }

        if (lslEnabled) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "(Enabled)");
        }

        // LSL stream name input (shown when LSL is enabled)
        if (lslEnabled) {
            ImGui::Indent(20.0f);
            ImGui::Text("LSL Stream Name:");
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text("Stream name template for LSL outlets.");
                ImGui::Text("Available placeholders:");
                ImGui::BulletText("{test} - Test name (e.g., 'gonogo')");
                ImGui::BulletText("{subject} - Subject ID");
                ImGui::Text("\nExample: PEBL_{test} becomes PEBL_gonogo");
                ImGui::EndTooltip();
            }

            char streamName[256];
            std::strncpy(streamName, mCurrentChain->GetLSLStreamName().c_str(), sizeof(streamName) - 1);
            streamName[sizeof(streamName) - 1] = '\0';

            if (ImGui::InputText("##lsl_stream_name", streamName, sizeof(streamName))) {
                mCurrentChain->SetLSLStreamName(std::string(streamName));
                mCurrentChain->Save();
            }

            ImGui::Unindent(20.0f);
        }
    } else {
        ImGui::TextDisabled("No chain loaded. Create a new chain or select an existing one.");
    }

    ImGui::EndChild();

    ImGui::Spacing();

    // Add item buttons
    if (!mCurrentChain || !mCurrentStudy) {
        ImGui::BeginDisabled();
    }

    float buttonWidth = (ImGui::GetContentRegionAvail().x - 20) / 4.0f;

    if (ImGui::Button("Add Instruction", ImVec2(buttonWidth, 0))) {
        AddInstructionPage();
    }

    ImGui::SameLine();

    if (ImGui::Button("Add Consent", ImVec2(buttonWidth, 0))) {
        AddConsentPage();
    }

    ImGui::SameLine();

    if (ImGui::Button("Add Completion", ImVec2(buttonWidth, 0))) {
        AddCompletionPage();
    }

    ImGui::SameLine();

    if (ImGui::Button("Add Test", ImVec2(buttonWidth, 0))) {
        AddTestToChain();
    }

    if (!mCurrentChain || !mCurrentStudy) {
        ImGui::EndDisabled();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Chain items list
    ImGui::Text("Chain Items:");
    ImGui::BeginChild("ChainItemsList", ImVec2(0, -80), true);

    if (!mCurrentChain) {
        ImGui::TextDisabled("Load a chain to view items");
    } else if (mCurrentChain->GetItems().empty()) {
        ImGui::TextDisabled("No items in this chain. Use the buttons above to add items.");
    } else {
        // Display chain items
        const auto& items = mCurrentChain->GetItems();
        for (size_t i = 0; i < items.size(); i++) {
            const ChainItem& item = items[i];

            ImGui::PushID((int)i);

            // Drag handle — source and target for reordering
            ImGui::Selectable("::##cdh", false, ImGuiSelectableFlags_AllowOverlap);
            if (ImGui::IsItemHovered())
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
            if (ImGui::BeginDragDropSource()) {
                int dragIdx = (int)i;
                ImGui::SetDragDropPayload("CHAIN_ITEM", &dragIdx, sizeof(int));
                ImGui::Text("Move: %s", item.GetDisplayName().c_str());
                ImGui::EndDragDropSource();
            }
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* pl = ImGui::AcceptDragDropPayload("CHAIN_ITEM")) {
                    int srcIdx = *(const int*)pl->Data;
                    if (srcIdx != (int)i) {
                        MoveChainItemTo(srcIdx, (int)i);
                        ImGui::PopID();
                        break;
                    }
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::SameLine();

            // Item number
            ImGui::Text("%zu.", i + 1);
            ImGui::SameLine();

            // Type icon/label
            const char* typeLabel = "";
            ImVec4 typeColor(1.0f, 1.0f, 1.0f, 1.0f);

            if (item.type == ItemType::Instruction) {
                typeLabel = "[INS]";
                typeColor = ImVec4(0.4f, 0.7f, 1.0f, 1.0f);
            } else if (item.type == ItemType::Consent) {
                typeLabel = "[CON]";
                typeColor = ImVec4(0.7f, 0.4f, 1.0f, 1.0f);
            } else if (item.type == ItemType::Completion) {
                typeLabel = "[CMP]";
                typeColor = ImVec4(0.4f, 1.0f, 0.7f, 1.0f);
            } else if (item.type == ItemType::Test) {
                typeLabel = "[TST]";
                typeColor = ImVec4(1.0f, 0.7f, 0.4f, 1.0f);
            }

            ImGui::TextColored(typeColor, "%s", typeLabel);
            ImGui::SameLine();

            // Item name/title
            std::string displayName = item.GetDisplayName();
            ImGui::Text("%s", displayName.c_str());

            // Show test details if it's a test item
            if (item.type == ItemType::Test) {
                ImGui::Indent(40);
                ImGui::TextDisabled("Test: %s | Variant: %s",
                                   item.testName.c_str(),
                                   item.paramVariant.empty() ? "default" : item.paramVariant.c_str());

                // Randomization group dropdown (inline editing)
                ImGui::SameLine();
                ImGui::TextDisabled(" | Group:");
                ImGui::SameLine();

                ImGui::PushItemWidth(80);
                const char* groupOptions[] = {"None", "1", "2", "3"};
                int currentGroup = item.randomGroup;
                if (ImGui::Combo("##randomgroup", &currentGroup, groupOptions, 4)) {
                    // Update the item's random group
                    ChainItem* mutableItem = mCurrentChain->GetItem(i);
                    if (mutableItem) {
                        mutableItem->randomGroup = currentGroup;
                        SaveCurrentChain();
                        printf("Updated randomization group for item %zu to %d\n", i, currentGroup);
                    }
                }
                ImGui::PopItemWidth();

                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Randomization group (0=None, 1-3=Group number)\n"
                                    "Tests with same non-zero group will be randomized together\n"
                                    "Group 0 (None) keeps test in fixed position");
                }

                ImGui::Unindent(40);
            }

            // Control buttons (on the right)
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - 250);

            // Up button
            if (i == 0) {
                ImGui::BeginDisabled();
            }
            if (ImGui::ArrowButton("##up", ImGuiDir_Up)) {
                MoveChainItemUp(i);
                ImGui::PopID();
                break; // Exit loop after moving to avoid iterator issues
            }
            if (i == 0) {
                ImGui::EndDisabled();
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Move up");
            }

            ImGui::SameLine();

            // Down button
            if (i >= items.size() - 1) {
                ImGui::BeginDisabled();
            }
            if (ImGui::ArrowButton("##down", ImGuiDir_Down)) {
                MoveChainItemDown(i);
                ImGui::PopID();
                break; // Exit loop after moving to avoid iterator issues
            }
            if (i >= items.size() - 1) {
                ImGui::EndDisabled();
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Move down");
            }

            ImGui::SameLine();

            // Test button
            if (ImGui::SmallButton("Test")) {
                TestChainItem(i);
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Test run this item");
            }

            ImGui::SameLine();

            // Edit button
            if (ImGui::SmallButton("Edit")) {
                EditChainItem(i);
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Edit item");
            }

            ImGui::SameLine();

            // Remove button
            if (ImGui::SmallButton("Remove")) {
                RemoveChainItem(i);
                ImGui::PopID();
                break; // Exit loop after removing to avoid iterator issues
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Remove from chain");
            }

            ImGui::Spacing();
            ImGui::PopID();
        }
    }

    ImGui::EndChild();

    ImGui::Spacing();
    ImGui::TextDisabled("Use the 'Test' button to run individual items, or go to the Run tab to execute the full chain.");
}

void LauncherUI::ShowAboutDialog()
{
    ImGui::OpenPopup("About PEBL Launcher");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("About PEBL Launcher", &mShowAbout, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("PEBL Experiment Launcher");
        ImGui::Separator();
        ImGui::Text("Version: %s (Prototype)", PEBL_VERSION);
        ImGui::Text("Built with Dear ImGui and SDL2");
        ImGui::Spacing();
        ImGui::Text("Copyright (c) 2026 Shane T. Mueller");
        ImGui::Text("Licensed under GPL");
        ImGui::Spacing();

        if (ImGui::Button("Close", ImVec2(120, 0))) {
            mShowAbout = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void LauncherUI::ShowSettingsDialog()
{
    ImGui::OpenPopup("Settings");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);

    if (ImGui::BeginPopupModal("Settings", &mShowSettings, 0))
    {
        ImGui::Text("Configure PEBL Launcher settings");
        ImGui::Separator();
        ImGui::Spacing();

        // Tabbed interface for different settings categories
        if (ImGui::BeginTabBar("SettingsTabs", ImGuiTabBarFlags_None))
        {
            // ============================================================
            // GENERAL TAB
            // ============================================================
            if (ImGui::BeginTabItem("General"))
            {
                ImGui::Spacing();

                // Default subject code
                ImGui::Text("Default Subject Code:");
                char subjectCode[256];
                std::strncpy(subjectCode, mConfig->GetSubjectCode().c_str(), sizeof(subjectCode) - 1);
                subjectCode[sizeof(subjectCode) - 1] = '\0';
                ImGui::PushItemWidth(200);
                if (ImGui::InputText("##SubjectCode", subjectCode, sizeof(subjectCode))) {
                    mConfig->SetSubjectCode(subjectCode);
                }
                ImGui::PopItemWidth();

                ImGui::Spacing();

                // Default language
                ImGui::Text("Default Language:");
                char language[16];
                std::strncpy(language, mConfig->GetLanguage().c_str(), sizeof(language) - 1);
                language[sizeof(language) - 1] = '\0';
                ImGui::PushItemWidth(100);
                if (ImGui::InputText("##Language", language, sizeof(language))) {
                    mConfig->SetLanguage(language);
                }
                ImGui::PopItemWidth();
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Two-letter language code (en, de, es, fr, etc.)");
                }

                ImGui::Spacing();

                // Fullscreen mode
                bool fullscreen = mConfig->GetFullscreen();
                if (ImGui::Checkbox("Run tests in fullscreen mode by default", &fullscreen)) {
                    mConfig->SetFullscreen(fullscreen);
                }

                ImGui::Spacing();

                // Font size
                ImGui::Text("Font Size:");
                int fontSize = mConfig->GetFontSize();
                ImGui::PushItemWidth(100);
                if (ImGui::SliderInt("##FontSize", &fontSize, 10, 24)) {
                    mConfig->SetFontSize(fontSize);
                }
                ImGui::PopItemWidth();
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Requires restart to take effect");
                }

                ImGui::EndTabItem();
            }

            // ============================================================
            // FILE PATHS TAB
            // ============================================================
            if (ImGui::BeginTabItem("File Paths"))
            {
                ImGui::Spacing();
                ImGui::TextWrapped("Configure file system locations for PEBL. Leave blank for auto-detection.");
                ImGui::Separator();
                ImGui::Spacing();

                // Workspace path
                ImGui::Text("Workspace Path:");
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Main workspace directory (typically Documents/pebl-exp.%s/)", PEBL_VERSION);
                }
                char workspacePath[512];
                std::strncpy(workspacePath, mConfig->GetWorkspacePath().c_str(), sizeof(workspacePath) - 1);
                workspacePath[sizeof(workspacePath) - 1] = '\0';
                ImGui::PushItemWidth(-100);
                if (ImGui::InputText("##WorkspacePath", workspacePath, sizeof(workspacePath))) {
                    mConfig->SetWorkspacePath(workspacePath);
                }
                ImGui::PopItemWidth();
                ImGui::SameLine();
                if (ImGui::Button("Browse##Workspace")) {
                    std::string path = OpenDirectoryDialog("Select Workspace Directory");
                    if (!path.empty()) {
                        mConfig->SetWorkspacePath(path);
                    }
                }

                ImGui::Spacing();

                // Battery path
                ImGui::Text("Battery Path:");
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Directory containing battery tests (e.g., /usr/local/share/pebl/battery/)");
                }
                char batteryPath[512];
                std::strncpy(batteryPath, mConfig->GetBatteryPath().c_str(), sizeof(batteryPath) - 1);
                batteryPath[sizeof(batteryPath) - 1] = '\0';
                ImGui::PushItemWidth(-100);
                if (ImGui::InputText("##BatteryPath", batteryPath, sizeof(batteryPath))) {
                    mConfig->SetBatteryPath(batteryPath);
                }
                ImGui::PopItemWidth();
                ImGui::SameLine();
                if (ImGui::Button("Browse##Battery")) {
                    std::string path = OpenDirectoryDialog("Select Battery Directory");
                    if (!path.empty()) {
                        mConfig->SetBatteryPath(path);
                    }
                }

                ImGui::Spacing();

                // PEBL executable path
                ImGui::Text("PEBL Executable Path:");
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Location of the pebl2 executable (auto-detected on startup)");
                }
                char peblExePath[512];
                std::strncpy(peblExePath, mConfig->GetPeblExecutablePath().c_str(), sizeof(peblExePath) - 1);
                peblExePath[sizeof(peblExePath) - 1] = '\0';
                ImGui::PushItemWidth(-100);
                if (ImGui::InputText("##PeblExePath", peblExePath, sizeof(peblExePath))) {
                    mConfig->SetPeblExecutablePath(peblExePath);
                }
                ImGui::PopItemWidth();
                ImGui::SameLine();
                if (ImGui::Button("Browse##PeblExe")) {
                    std::string path = OpenFileDialog("Select PEBL Executable");
                    if (!path.empty()) {
                        mConfig->SetPeblExecutablePath(path);
                    }
                }

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                ImGui::TextWrapped("Note: Restart the launcher after changing paths for changes to take full effect.");

                ImGui::EndTabItem();
            }

            // ============================================================
            // UPLOAD TAB
            // ============================================================
            if (ImGui::BeginTabItem("Upload"))
            {
                ImGui::Spacing();
                ImGui::TextWrapped("Configure automatic data upload to PEBL Hub or custom server.");
                ImGui::Separator();
                ImGui::Spacing();

                // Auto-upload checkbox
                bool autoUpload = mConfig->GetAutoUpload();
                if (ImGui::Checkbox("Enable auto-upload after test completion", &autoUpload)) {
                    mConfig->SetAutoUpload(autoUpload);
                }

                ImGui::Spacing();

                // Upload URL
                ImGui::Text("Upload URL:");
                char uploadURL[512];
                std::strncpy(uploadURL, mConfig->GetUploadURL().c_str(), sizeof(uploadURL) - 1);
                uploadURL[sizeof(uploadURL) - 1] = '\0';
                ImGui::PushItemWidth(-1);
                if (ImGui::InputText("##UploadURL", uploadURL, sizeof(uploadURL))) {
                    mConfig->SetUploadURL(uploadURL);
                }
                ImGui::PopItemWidth();
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Default: https://peblhub.online/api/upload");
                }

                ImGui::Spacing();

                // Upload token
                ImGui::Text("Authentication Token:");
                char uploadToken[256];
                std::strncpy(uploadToken, mConfig->GetUploadToken().c_str(), sizeof(uploadToken) - 1);
                uploadToken[sizeof(uploadToken) - 1] = '\0';
                ImGui::PushItemWidth(-1);
                if (ImGui::InputText("##UploadToken", uploadToken, sizeof(uploadToken), ImGuiInputTextFlags_Password)) {
                    mConfig->SetUploadToken(uploadToken);
                }
                ImGui::PopItemWidth();
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Get your token from peblhub.online/account");
                }

                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
                    "When enabled, data files will be automatically uploaded after each test completes.");

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Bottom buttons
        if (ImGui::Button("Save", ImVec2(120, 0))) {
            mConfig->SaveConfig();
            mShowSettings = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            // Reload config to revert changes
            mConfig->LoadConfig();
            mShowSettings = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 120);

        if (ImGui::Button("Apply", ImVec2(120, 0))) {
            mConfig->SaveConfig();
        }

        ImGui::EndPopup();
    }
}

void LauncherUI::ShowParameterEditor()
{
    ImGui::OpenPopup("Parameter Editor");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_FirstUseEver);

    if (ImGui::BeginPopupModal("Parameter Editor", &mShowParameterEditor, 0))
    {
        // Header row: description on left, Variants button on right
        if (mEditingDefaultParams) {
            ImGui::Text("Editing default parameters");
        } else {
            ImGui::Text("Editing variant: %s", mVariantName);
        }
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 80);
        if (ImGui::SmallButton("Variants...")) {
            mShowVariantNameDialog = true;
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Create or switch to a named parameter variant\n(e.g., for different input devices or conditions)");
        }
        if (mEditingDefaultParams) {
            ImGui::TextDisabled("Values here override the built-in defaults from the test definition.");
        } else {
            ImGui::TextDisabled("Variants are alternate parameter sets for different conditions.");
        }
        ImGui::Separator();
        ImGui::Spacing();

        // Scrollable parameter list in table format
        ImGui::BeginChild("ParameterList", ImVec2(0, -40), true);

        if (mParameters.empty()) {
            ImGui::TextDisabled("No parameters defined for this experiment");
        } else {
            // Table header
            if (ImGui::BeginTable("ParameterTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Parameter", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                ImGui::TableSetupColumn("Description (built-in)", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Reset", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                ImGui::TableHeadersRow();

                // Parameter rows
                bool shouldFocusFirst = ImGui::IsWindowAppearing();
                for (size_t i = 0; i < mParameters.size(); i++) {
                    Parameter& param = mParameters[i];

                    ImGui::PushID((int)i);
                    ImGui::TableNextRow();

                    // Column 1: Parameter name
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextWrapped("%s", param.name.c_str());

                    // Column 2: Editable value — use combo/checkbox when options available
                    ImGui::TableSetColumnIndex(1);
                    ImGui::PushItemWidth(-1);

                    if (!param.options.empty()) {
                        // Combo box for parameters with defined options
                        // Find current selection index
                        int currentIdx = -1;
                        for (size_t j = 0; j < param.options.size(); j++) {
                            if (param.options[j] == param.value) {
                                currentIdx = (int)j;
                                break;
                            }
                        }
                        std::string preview = (currentIdx >= 0) ? param.options[currentIdx] : param.value;
                        if (ImGui::BeginCombo("##value", preview.c_str())) {
                            for (size_t j = 0; j < param.options.size(); j++) {
                                bool isSelected = ((int)j == currentIdx);
                                if (ImGui::Selectable(param.options[j].c_str(), isSelected)) {
                                    param.value = param.options[j];
                                }
                                if (isSelected) {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                            ImGui::EndCombo();
                        }
                    } else {
                        // Free text input
                        char buffer[256];
                        std::strncpy(buffer, param.value.c_str(), sizeof(buffer) - 1);
                        buffer[sizeof(buffer) - 1] = '\0';

                        if (shouldFocusFirst && i == 0) {
                            ImGui::SetKeyboardFocusHere();
                        }
                        if (ImGui::InputText("##value", buffer, sizeof(buffer))) {
                            param.value = buffer;
                        }
                    }

                    ImGui::PopItemWidth();

                    // Column 3: Description with default value
                    ImGui::TableSetColumnIndex(2);
                    std::string descText = param.description;
                    if (!param.defaultValue.empty()) {
                        descText += " (built-in: " + param.defaultValue + ")";
                    }
                    // Show options if available
                    if (!param.options.empty()) {
                        descText += " Options: ";
                        for (size_t j = 0; j < param.options.size(); j++) {
                            if (j > 0) descText += ", ";
                            descText += param.options[j];
                        }
                    }
                    ImGui::TextWrapped("%s", descText.c_str());

                    // Column 4: Reset button
                    ImGui::TableSetColumnIndex(3);
                    if (ImGui::SmallButton("Reset")) {
                        param.value = param.defaultValue;
                    }

                    ImGui::PopID();
                }

                ImGui::EndTable();
            }
        }

        ImGui::EndChild();

        ImGui::Spacing();

        // Buttons
        if (ImGui::Button("Save", ImVec2(120, 0))) {
            // Save parameters to file
            if (!mParameterFile.empty()) {
                std::ofstream file(mParameterFile);
                if (file.is_open()) {
                    file << "{" << std::endl;
                    for (size_t i = 0; i < mParameters.size(); i++) {
                        file << "  \"" << mParameters[i].name << "\": \"" << mParameters[i].value << "\"";
                        if (i < mParameters.size() - 1) {
                            file << ",";
                        }
                        file << std::endl;
                    }
                    file << "}" << std::endl;
                    file.close();
                    printf("Parameters saved to: %s\n", mParameterFile.c_str());

                    // Register named variants with the study (skip for default par.json)
                    if (!mEditingDefaultParams) {
                        size_t lastSlash = mParameterFile.find_last_of("/\\");
                        std::string filename = (lastSlash != std::string::npos)
                                             ? mParameterFile.substr(lastSlash + 1)
                                             : mParameterFile;
                        size_t dotPar = filename.find(".par.json");
                        if (dotPar != std::string::npos) {
                            std::string variantName = filename.substr(0, dotPar);

                            size_t testsPos = mParameterFile.find("/tests/");
                            if (testsPos != std::string::npos && mCurrentStudy) {
                                size_t testNameStart = testsPos + 7;
                                size_t testNameEnd = mParameterFile.find("/", testNameStart);
                                if (testNameEnd != std::string::npos) {
                                    std::string testName = mParameterFile.substr(testNameStart, testNameEnd - testNameStart);
                                    Test* test = mCurrentStudy->GetTest(testName);
                                    if (test) {
                                        ParameterVariant variant;
                                        variant.file = variantName + ".par.json";
                                        variant.description = "Custom variant";
                                        test->parameterVariants[variantName] = variant;
                                        mCurrentStudy->Save();
                                        printf("Registered variant '%s' for test '%s'\n", variantName.c_str(), testName.c_str());
                                    }
                                }
                            }
                        }
                    }
                }
            }
            mShowParameterEditor = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            mShowParameterEditor = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void LauncherUI::ShowVariantNameDialog()
{
    ImGui::OpenPopup("Parameter Variants");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);

    if (ImGui::BeginPopupModal("Parameter Variants", &mShowVariantNameDialog, 0))
    {
        // Get test name and variants
        std::string testName = "";
        const std::map<std::string, ParameterVariant>* variants = nullptr;

        if (mCurrentStudy && mEditingTestIndex >= 0) {
            const auto& tests = mCurrentStudy->GetTests();
            if (mEditingTestIndex < (int)tests.size()) {
                testName = tests[mEditingTestIndex].testName;
                const Test* test = mCurrentStudy->GetTest(testName);
                if (test) {
                    variants = &test->parameterVariants;
                }
            }
        }

        ImGui::TextWrapped("Parameter sets for test: %s", testName.c_str());
        ImGui::TextDisabled("The default parameter set is edited directly. Variants are named alternate configurations.");
        ImGui::Separator();
        ImGui::Spacing();

        // Edit Default button (prominent)
        if (ImGui::Button("Edit Default Parameters", ImVec2(-1, 35))) {
            mVariantName[0] = '\0';
            LoadParameterEditorForVariant();
            mShowVariantNameDialog = false;
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Edit the default %s.pbl.par.json file.\nThese are the parameters used when no variant is specified.", testName.c_str());
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Show existing named variants
        if (variants && !variants->empty()) {
            ImGui::Text("Named Variants:");
            ImGui::Spacing();

            ImGui::BeginChild("ExistingVariants", ImVec2(0, 120), true);
            for (const auto& pair : *variants) {
                ImGui::PushID(pair.first.c_str());

                if (ImGui::Selectable(pair.first.c_str(), false, 0, ImVec2(0, 26))) {
                    std::strncpy(mVariantName, pair.first.c_str(), sizeof(mVariantName) - 1);
                    mVariantName[sizeof(mVariantName) - 1] = '\0';
                    LoadParameterEditorForVariant();
                    mShowVariantNameDialog = false;
                    ImGui::PopID();
                    ImGui::CloseCurrentPopup();
                    break;
                }

                ImGui::SameLine();
                ImGui::TextDisabled("(%s)", pair.second.file.c_str());

                ImGui::PopID();
            }
            ImGui::EndChild();

            ImGui::Spacing();
        }

        // Create new variant section
        ImGui::Text("Create New Variant:");
        ImGui::Spacing();

        ImGui::Text("Name:");
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Examples: mousebutton, touchscreen, leftclick, arrowLR\n"
                            "The variant will be saved as %s-<name>.par.json", testName.c_str());
        }
        ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 130);
        ImGui::InputText("##variantname", mVariantName, sizeof(mVariantName));
        ImGui::PopItemWidth();
        ImGui::SameLine();

        bool canCreate = strlen(mVariantName) > 0;
        if (!canCreate) {
            ImGui::BeginDisabled();
        }
        if (ImGui::Button("Create", ImVec2(120, 0))) {
            LoadParameterEditorForVariant();
            mShowVariantNameDialog = false;
            ImGui::CloseCurrentPopup();
        }
        if (!canCreate) {
            ImGui::EndDisabled();
        }

        ImGui::Spacing();

        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            mShowVariantNameDialog = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void LauncherUI::LoadParameterEditorForVariant()
{
    if (!mCurrentStudy || mEditingTestIndex < 0) {
        printf("Error: Invalid state for loading parameter editor\n");
        return;
    }

    const auto& tests = mCurrentStudy->GetTests();
    if (mEditingTestIndex >= (int)tests.size()) {
        printf("Error: Invalid test index\n");
        return;
    }

    const Test& test = tests[mEditingTestIndex];
    std::string studyPath = mCurrentStudy->GetPath();
    std::string testPath = studyPath + "/tests/" + test.testPath;
    std::string schemaPath = testPath + "/params/" + test.testName + ".pbl.schema.json";

    // Sync schema from scale definition if this is a scale-based test
    SyncScaleSchema(testPath, test.testName);

    printf("Loading parameter schema from: %s\n", schemaPath.c_str());

    // Check if schema file exists
    if (!fs::exists(schemaPath)) {
        printf("ERROR: Schema file does not exist at: %s\n", schemaPath.c_str());
        printf("Test path: %s\n", testPath.c_str());
        printf("Test name: %s\n", test.testName.c_str());
        printf("Test testPath: %s\n", test.testPath.c_str());

        // List what files DO exist in params directory
        std::string paramsDir = testPath + "/params";
        if (fs::exists(paramsDir) && fs::is_directory(paramsDir)) {
            printf("Files in params directory:\n");
            for (const auto& entry : fs::directory_iterator(paramsDir)) {
                printf("  - %s\n", entry.path().filename().string().c_str());
            }
        } else {
            printf("Params directory does not exist: %s\n", paramsDir.c_str());
        }
        return;
    }

    // Load schema to get parameter definitions
    try {
        std::ifstream schemaFile(schemaPath);
        if (!schemaFile.is_open()) {
            printf("ERROR: Could not open schema file (even though it exists): %s\n", schemaPath.c_str());
            return;
        }

        // Parse JSON schema file
        mParameters.clear();
        nlohmann::json schemaJson;
        schemaFile >> schemaJson;
        schemaFile.close();

        if (!schemaJson.contains("parameters") || !schemaJson["parameters"].is_array()) {
            printf("ERROR: Schema file does not contain 'parameters' array\n");
            return;
        }

        // Extract parameters from JSON
        for (const auto& paramJson : schemaJson["parameters"]) {
            if (!paramJson.contains("name") || !paramJson.contains("default")) {
                continue;  // Skip invalid entries
            }

            Parameter param;
            param.name = paramJson["name"].get<std::string>();

            // Convert default value to string
            if (paramJson["default"].is_string()) {
                param.defaultValue = paramJson["default"].get<std::string>();
            } else if (paramJson["default"].is_number_integer()) {
                param.defaultValue = std::to_string(paramJson["default"].get<int>());
            } else if (paramJson["default"].is_number_float()) {
                param.defaultValue = std::to_string(paramJson["default"].get<double>());
            } else if (paramJson["default"].is_boolean()) {
                param.defaultValue = paramJson["default"].get<bool>() ? "true" : "false";
            } else {
                param.defaultValue = paramJson["default"].dump();
            }

            if (paramJson.contains("description")) {
                param.description = paramJson["description"].get<std::string>();
            }

            // Extract options if available
            if (paramJson.contains("options") && paramJson["options"].is_array()) {
                for (const auto& opt : paramJson["options"]) {
                    if (opt.is_string()) {
                        param.options.push_back(opt.get<std::string>());
                    } else {
                        param.options.push_back(opt.dump());
                    }
                }
            }

            param.value = param.defaultValue;  // Initialize to default
            mParameters.push_back(param);
        }

        printf("Loaded %zu parameters from schema\n", mParameters.size());

        // Build parameter file path
        // Empty variant name = default parameter file (testname.pbl.par.json)
        // Non-empty variant name = named variant (testname-variantname.par.json)
        std::string variantFileName;
        if (strlen(mVariantName) == 0) {
            variantFileName = test.testName + ".pbl.par.json";
            mEditingDefaultParams = true;
        } else {
            variantFileName = test.testName + "-" + std::string(mVariantName) + ".par.json";
            mEditingDefaultParams = false;
        }
        mParameterFile = testPath + "/params/" + variantFileName;
        printf("Parameter file: %s\n", mParameterFile.c_str());

        // If parameter file already exists, load its values
        if (fs::exists(mParameterFile)) {
            printf("Loading existing parameter values from file\n");
            std::ifstream paramFile(mParameterFile);
            if (paramFile.is_open()) {
                // Read JSON file (simple key-value format)
                std::string paramLine;
                while (std::getline(paramFile, paramLine)) {
                    // Parse: "name": "value"
                    size_t colonPos = paramLine.find(':');
                    if (colonPos != std::string::npos) {
                        // Extract name (between first " and second ")
                        size_t nameStart = paramLine.find('"');
                        size_t nameEnd = paramLine.find('"', nameStart + 1);
                        if (nameStart != std::string::npos && nameEnd != std::string::npos) {
                            std::string paramName = paramLine.substr(nameStart + 1, nameEnd - nameStart - 1);

                            // Extract value (between third " and fourth ")
                            size_t valueStart = paramLine.find('"', nameEnd + 1);
                            size_t valueEnd = paramLine.find('"', valueStart + 1);
                            if (valueStart != std::string::npos && valueEnd != std::string::npos) {
                                std::string paramValue = paramLine.substr(valueStart + 1, valueEnd - valueStart - 1);

                                // Find and update parameter
                                for (auto& param : mParameters) {
                                    if (param.name == paramName) {
                                        param.value = paramValue;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                paramFile.close();
            }
        }

        // Show parameter editor
        mShowParameterEditor = true;

    } catch (const std::exception& e) {
        printf("Error loading parameter schema: %s\n", e.what());
    }
}

void LauncherUI::ScanExperimentDirectory(const std::string& path)
{
    mExperiments.clear();
    mSelectedExperiment = -1;
    FreeScreenshot();

    if (!fs::exists(path) || !fs::is_directory(path)) {
        return;
    }

    try {
        for (const auto& entry : fs::recursive_directory_iterator(path)) {
            if (entry.is_regular_file() && entry.path().extension() == ".pbl") {
                // Only include tests that have a .about.txt file
                // This filters out helper scripts, stimulus generators, and experimental files
                fs::path aboutPath = entry.path().parent_path() / (entry.path().filename().string() + ".about.txt");
                if (!fs::exists(aboutPath)) {
                    continue;  // Skip this .pbl file - not a proper battery test
                }

                ExperimentInfo info;
                info.path = entry.path().string();

                // Use parent directory name + filename for display
                // This helps differentiate tests with same name in different directories
                std::string parentDir = entry.path().parent_path().filename().string();
                std::string filename = entry.path().stem().string();

                if (parentDir != filename) {
                    // Show as "parentdir/filename" if they differ
                    info.name = parentDir + "/" + filename;
                } else {
                    // Just show filename if parent directory has same name
                    info.name = filename;
                }

                info.directory = entry.path().parent_path().string();
                info.description = "";

                // Check for parameter schema
                fs::path paramsDir = entry.path().parent_path() / "params";
                fs::path schemaFile = paramsDir / (entry.path().filename().string() + ".schema.json");
                info.hasParams = fs::exists(schemaFile);

                // Check for translations
                fs::path translationsDir = entry.path().parent_path() / "translations";
                info.hasTranslations = fs::exists(translationsDir) && fs::is_directory(translationsDir);

                // Check for screenshot (e.g., taskname.pbl.png)
                fs::path screenshotPath = entry.path().parent_path() / (entry.path().filename().string() + ".png");
                if (fs::exists(screenshotPath)) {
                    info.hasScreenshot = true;
                    info.screenshotPath = screenshotPath.string();
                } else {
                    info.hasScreenshot = false;
                    info.screenshotPath = "";
                }

                mExperiments.push_back(info);
            }
        }

        // Sort alphabetically (case-insensitive)
        std::sort(mExperiments.begin(), mExperiments.end(),
                  [](const ExperimentInfo& a, const ExperimentInfo& b) {
                      std::string aLower = a.name;
                      std::string bLower = b.name;
                      std::transform(aLower.begin(), aLower.end(), aLower.begin(), ::tolower);
                      std::transform(bLower.begin(), bLower.end(), bLower.begin(), ::tolower);
                      return aLower < bLower;
                  });

    } catch (const fs::filesystem_error& e) {
        printf("Error scanning directory: %s\n", e.what());
    }
}

void LauncherUI::ScanTemplates()
{
    mTemplateNames.clear();
    mTemplateFiles.clear();

    // Check media/templates directory (new location)
    std::string templatesPath = mBatteryPath + "/../media/templates";

    if (!fs::exists(templatesPath) || !fs::is_directory(templatesPath)) {
        printf("Warning: Templates directory not found: %s\n", templatesPath.c_str());
        return;
    }

    try {
        for (const auto& entry : fs::directory_iterator(templatesPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".pbl") {
                std::string filename = entry.path().filename().string();
                std::string displayName = filename.substr(0, filename.length() - 4);  // Remove .pbl

                // Convert filename to display name (e.g., "simple-rt" -> "Simple RT")
                std::string niceName;
                bool capitalizeNext = true;
                for (char c : displayName) {
                    if (c == '-' || c == '_') {
                        niceName += ' ';
                        capitalizeNext = true;
                    } else if (capitalizeNext) {
                        niceName += toupper(c);
                        capitalizeNext = false;
                    } else {
                        niceName += c;
                    }
                }

                mTemplateFiles.push_back(filename);
                mTemplateNames.push_back(niceName);
            }
        }

        // Sort alphabetically
        std::vector<size_t> indices(mTemplateNames.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::sort(indices.begin(), indices.end(),
                  [this](size_t a, size_t b) { return mTemplateNames[a] < mTemplateNames[b]; });

        std::vector<std::string> sortedNames;
        std::vector<std::string> sortedFiles;
        for (size_t i : indices) {
            sortedNames.push_back(mTemplateNames[i]);
            sortedFiles.push_back(mTemplateFiles[i]);
        }
        mTemplateNames = sortedNames;
        mTemplateFiles = sortedFiles;

        printf("Scanned templates: found %zu templates\n", mTemplateFiles.size());

    } catch (const fs::filesystem_error& e) {
        printf("Error scanning templates directory: %s\n", e.what());
    }
}

void LauncherUI::LoadScreenshot(const std::string& imagePath)
{
    // Free previous screenshot if any
    FreeScreenshot();

    if (imagePath.empty() || !fs::exists(imagePath)) {
        return;
    }

    // Load image using SDL_image
    SDL_Surface* surface = IMG_Load(imagePath.c_str());
    if (!surface) {
        printf("Failed to load screenshot: %s\n", IMG_GetError());
        return;
    }

    // Create texture from surface
    mScreenshotTexture = SDL_CreateTextureFromSurface(mRenderer, surface);
    if (!mScreenshotTexture) {
        printf("Failed to create texture: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    mScreenshotWidth = surface->w;
    mScreenshotHeight = surface->h;

    SDL_FreeSurface(surface);
}

void LauncherUI::FreeScreenshot()
{
    if (mScreenshotTexture) {
        SDL_DestroyTexture(mScreenshotTexture);
        mScreenshotTexture = nullptr;
        mScreenshotWidth = 0;
        mScreenshotHeight = 0;
    }
}

void LauncherUI::FreeStudyTestScreenshot()
{
    if (mStudyTestScreenshot) {
        SDL_DestroyTexture(mStudyTestScreenshot);
        mStudyTestScreenshot = nullptr;
        mStudyTestScreenshotW = 0;
        mStudyTestScreenshotH = 0;
    }
}

void LauncherUI::LoadStudyTestPreview(int testIndex)
{
    FreeStudyTestScreenshot();
    mStudyTestDescription.clear();

    if (!mCurrentStudy || testIndex < 0) return;

    const auto& tests = mCurrentStudy->GetTests();
    if (testIndex >= (int)tests.size()) return;

    const std::string& testName = tests[testIndex].testName;
    std::string studyPath = mCurrentStudy->GetPath();
    std::string testDir = studyPath + "/tests/" + testName;

    printf("LoadStudyTestPreview: testName='%s' studyPath='%s' testDir='%s'\n",
           testName.c_str(), studyPath.c_str(), testDir.c_str());

    // Load about.txt from study's local test directory
    std::string aboutPath = testDir + "/" + testName + ".pbl.about.txt";
    printf("  aboutPath='%s' exists=%d\n", aboutPath.c_str(), fs::exists(aboutPath) ? 1 : 0);
    if (fs::exists(aboutPath)) {
        std::ifstream aboutFile(aboutPath);
        if (aboutFile.is_open()) {
            mStudyTestDescription = std::string(
                (std::istreambuf_iterator<char>(aboutFile)),
                std::istreambuf_iterator<char>());
            aboutFile.close();
        }
    }

    // If not found locally, try battery source
    if (mStudyTestDescription.empty()) {
        // Look in battery for matching test
        for (const auto& exp : mExperiments) {
            fs::path expPath(exp.path);
            std::string expName = expPath.stem().string();
            if (expName == testName && !exp.description.empty()) {
                mStudyTestDescription = exp.description;
                break;
            }
        }
    }

    // Load screenshot from study's local test directory
    std::string screenshotPath = testDir + "/" + testName + ".pbl.png";
    printf("  screenshotPath='%s' exists=%d\n", screenshotPath.c_str(), fs::exists(screenshotPath) ? 1 : 0);
    if (!fs::exists(screenshotPath)) {
        // Fall back to battery source
        for (const auto& exp : mExperiments) {
            fs::path expPath(exp.path);
            std::string expName = expPath.stem().string();
            if (expName == testName && exp.hasScreenshot) {
                screenshotPath = exp.screenshotPath;
                break;
            }
        }
    }

    if (fs::exists(screenshotPath)) {
        SDL_Surface* surface = IMG_Load(screenshotPath.c_str());
        if (surface) {
            mStudyTestScreenshot = SDL_CreateTextureFromSurface(mRenderer, surface);
            if (mStudyTestScreenshot) {
                mStudyTestScreenshotW = surface->w;
                mStudyTestScreenshotH = surface->h;
            }
            SDL_FreeSurface(surface);
        }
    }
}

void LauncherUI::LoadExperimentInfo(const std::string& scriptPath)
{
    if (mSelectedExperiment < 0 || mSelectedExperiment >= (int)mExperiments.size()) {
        return;
    }

    ExperimentInfo& exp = mExperiments[mSelectedExperiment];

    // Load description from .about.txt file
    fs::path aboutPath = fs::path(scriptPath).parent_path() /
                         (fs::path(scriptPath).filename().string() + ".about.txt");

    if (fs::exists(aboutPath)) {
        std::ifstream aboutFile(aboutPath);
        if (aboutFile.is_open()) {
            std::string content((std::istreambuf_iterator<char>(aboutFile)),
                                std::istreambuf_iterator<char>());
            exp.description = content;
            aboutFile.close();
        }
    }

    // Load screenshot
    if (exp.hasScreenshot) {
        LoadScreenshot(exp.screenshotPath);
    } else {
        FreeScreenshot();
    }

    // Scan for available translations
    mAvailableLanguages.clear();
    if (exp.hasTranslations) {
        fs::path translationsDir = fs::path(scriptPath).parent_path() / "translations";
        try {
            std::set<std::string> langSet;
            for (const auto& entry : fs::directory_iterator(translationsDir)) {
                if (entry.is_regular_file() && entry.path().extension() == ".json") {
                    std::string filename = entry.path().stem().string();
                    std::string langCode;
                    // Try dash-based format: "taskname.pbl-en"
                    size_t dashPos = filename.find_last_of('-');
                    if (dashPos != std::string::npos) {
                        langCode = filename.substr(dashPos + 1);
                    } else {
                        // Try dot-based format: "taskname.en"
                        size_t dotPos = filename.find_last_of('.');
                        if (dotPos != std::string::npos) {
                            langCode = filename.substr(dotPos + 1);
                        }
                    }
                    if (!langCode.empty()) {
                        langSet.insert(langCode);
                    }
                }
            }
            mAvailableLanguages.assign(langSet.begin(), langSet.end());

            // Sort language codes (already sorted by set, but be explicit)
            std::sort(mAvailableLanguages.begin(), mAvailableLanguages.end());

            // Default to first available language if current language not in list
            if (!mAvailableLanguages.empty()) {
                bool found = false;
                for (const auto& lang : mAvailableLanguages) {
                    if (lang == mLanguageCode) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    std::strcpy(mLanguageCode, mAvailableLanguages[0].c_str());
                }
            }

        } catch (const fs::filesystem_error& e) {
            printf("Error scanning translations: %s\n", e.what());
        }
    }
}

void LauncherUI::RunTest()
{
    if (mSelectedExperiment < 0 || mSelectedExperiment >= (int)mExperiments.size()) {
        return;
    }

    // Clean up any previous test
    if (mRunningExperiment) {
        if (mRunningExperiment->IsRunning()) {
            printf("Warning: Previous test still running\n");
            return;
        }
        delete mRunningExperiment;
        mRunningExperiment = nullptr;
    }

    const ExperimentInfo& exp = mExperiments[mSelectedExperiment];

    // Build command line arguments
    std::vector<std::string> args;

    // Subject code
    if (strlen(mSubjectCode) > 0) {
        args.push_back("-s");
        args.push_back(mSubjectCode);
    }

    // Language
    if (strlen(mLanguageCode) > 0 && exp.hasTranslations) {
        args.push_back("-v");
        args.push_back(std::string("gLanguage=") + mLanguageCode);
    }

    // Fullscreen
    if (mFullscreen) {
        args.push_back("--fullscreen");
    }

    // Create new experiment runner
    mRunningExperiment = new ExperimentRunner(mConfig);
    bool success = mRunningExperiment->RunExperiment(exp.path, args,
                                                      mSubjectCode,
                                                      mLanguageCode,
                                                      mFullscreen);

    if (success) {
        // Add to recent experiments list
        mConfig->AddRecentExperiment(exp.path, exp.name);
        mShowStderr = false;  // Start showing stdout
    } else {
        printf("Failed to run experiment: %s\n", exp.path.c_str());
        delete mRunningExperiment;
        mRunningExperiment = nullptr;
        // TODO: Show error dialog
    }
}

void LauncherUI::OpenDirectoryInFileBrowser(const std::string& path)
{
    if (path.empty()) {
        printf("Cannot open directory: empty path\n");
        return;
    }
    // Create the directory if it doesn't exist yet (e.g. data/ before first test run)
    if (!fs::exists(path)) {
        try {
            fs::create_directories(path);
            printf("Created directory: %s\n", path.c_str());
        } catch (const std::exception& e) {
            printf("Cannot open directory (does not exist and could not be created): %s\n", path.c_str());
            return;
        }
    }

#ifdef _WIN32
    // Windows: Use explorer - normalize path to use backslashes
    std::string winPath = path;
    for (char& c : winPath) {
        if (c == '/') c = '\\';
    }
    std::string command = "explorer \"" + winPath + "\"";
    printf("Opening directory: %s\n", winPath.c_str());
    system(command.c_str());
#elif __APPLE__
    // macOS: Use open
    std::string command = "open \"" + path + "\" &";
    system(command.c_str());
#else
    // Linux: try desktop file managers directly before falling back to xdg-open.
    // xdg-open often fails for directories on systems where the MIME association
    // for inode/directory is not configured (e.g. headless or minimal desktops).
    static const char* kManagers[] = {
        "nautilus", "dolphin", "nemo", "thunar", "pcmanfm", nullptr
    };
    bool opened = false;
    for (int i = 0; kManagers[i] != nullptr; i++) {
        std::string check = std::string("which ") + kManagers[i] + " >/dev/null 2>&1";
        if (system(check.c_str()) == 0) {
            std::string cmd = std::string(kManagers[i]) + " \"" + path + "\" >/dev/null 2>&1 &";
            system(cmd.c_str());
            printf("Opening directory with %s: %s\n", kManagers[i], path.c_str());
            opened = true;
            break;
        }
    }
    if (!opened) {
        // Last resort: xdg-open
        std::string command = "xdg-open \"" + path + "\" &";
        int result = system(command.c_str());
        if (result != 0) {
            printf("Failed to open directory in file browser: %s\n", path.c_str());
        }
    }
#endif
}

void LauncherUI::OpenFileInTextEditor(const std::string& filePath)
{
    if (filePath.empty() || !fs::exists(filePath)) {
        printf("Cannot open file: %s (file not found)\n", filePath.c_str());
        return;
    }

#ifdef _WIN32
    // Windows: Use notepad as fallback, or default text editor via start
    std::string command = "start \"\" \"" + filePath + "\"";
#elif __APPLE__
    // macOS: Use open which will use default text editor
    std::string command = "open \"" + filePath + "\"";
#else
    // Linux: Use xdg-open which respects default application settings
    std::string command = "xdg-open \"" + filePath + "\" &";
#endif

    printf("Opening file in text editor: %s\n", filePath.c_str());
    int result = system(command.c_str());
    if (result != 0) {
        printf("Failed to open file in text editor: %s\n", filePath.c_str());
    }
}

void LauncherUI::LaunchTranslationEditor()
{
    if (mSelectedExperiment < 0 || mSelectedExperiment >= (int)mExperiments.size()) {
        printf("ERROR: No experiment selected for translation editor\n");
        return;
    }

    const ExperimentInfo& exp = mExperiments[mSelectedExperiment];

    // Get PEBL executable path from config
    std::string peblExec = mConfig->GetPeblExecutablePath();
    if (peblExec.empty()) {
        peblExec = "pebl2";  // Fallback
    }

    // Find translatetest.pbl - look in various possible locations
    std::string translateScript;
    std::string batteryPath = mConfig->GetBatteryPath();
    fs::path peblDir = fs::path(peblExec).parent_path().parent_path();  // Go up from bin/
    fs::path binDir = fs::path(peblExec).parent_path();  // bin/ directory
    std::vector<std::string> possiblePaths = {
        (binDir / "translatetest.pbl").string(),                    // In bin/ alongside executable
        (peblDir / "pebl-lib" / "translatetest.pbl").string(),      // In pebl-lib/
        (fs::path(batteryPath) / "translatetest" / "translatetest.pbl").string(),
        (fs::path(batteryPath).parent_path() / "media" / "apps" / "translatetest" / "translatetest.pbl").string(),
        (peblDir / "media" / "apps" / "translatetest" / "translatetest.pbl").string(),
        (peblDir / "battery" / "translatetest" / "translatetest.pbl").string(),
    };
    for (const auto& path : possiblePaths) {
        if (fs::exists(path)) {
            translateScript = path;
            break;
        }
    }
    if (translateScript.empty()) {
        printf("ERROR: Could not find translatetest.pbl in any expected location\n");
        return;
    }

    // Build command to launch translatetest.pbl
    std::string scriptPath = exp.path;
    std::string lang = std::string(mLanguageCode);
    if (lang.empty()) {
        lang = "en";
    }

    std::string command = "\"" + peblExec + "\" \"" + translateScript + "\" -v \"" + scriptPath + "\" --language " + lang;

    printf("Launching translation editor: %s\n", command.c_str());

    // Launch in background
#ifdef __linux__
    command += " &";
    int result = system(command.c_str());
    if (result != 0) {
        printf("ERROR: Failed to launch translation editor (exit code %d)\n", result);
    }
#else
    system(command.c_str());
#endif
}

void LauncherUI::LaunchDataCombiner(const std::string& workingDirectory)
{
    // Get PEBL executable path (same logic as ExperimentRunner)
    std::string peblExec = "pebl2";  // Default fallback
#ifdef __linux__
    char exePath[1024];
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    if (len != -1) {
        exePath[len] = '\0';
        std::string path(exePath);
        size_t lastSlash = path.find_last_of('/');
        if (lastSlash != std::string::npos) {
            peblExec = path.substr(0, lastSlash + 1) + "pebl2";
        }
    }
#endif

    // Build command to launch combinedatafiles.pbl
    std::string command;

    // If working directory is specified, change to it first
    if (!workingDirectory.empty()) {
#ifdef __linux__
        command = "cd \"" + workingDirectory + "\" && " + peblExec + " combinedatafiles.pbl &";
#else
        command = "cd \"" + workingDirectory + "\" & " + peblExec + " combinedatafiles.pbl";
#endif
        printf("Launching data combiner in: %s\n", workingDirectory.c_str());
    } else {
        command = peblExec + " combinedatafiles.pbl";
#ifdef __linux__
        command += " &";
#endif
        printf("Launching data combiner\n");
    }

    printf("Command: %s\n", command.c_str());

    // Launch in background
    int result = system(command.c_str());
    if (result != 0) {
        printf("ERROR: Failed to launch data combiner (exit code %d)\n", result);
    }
}

void LauncherUI::RunChain()
{
    if (!mCurrentChain || mCurrentChain->GetItems().empty()) {
        printf("Cannot run chain: no chain loaded or chain is empty\n");
        return;
    }

    if (mRunningChain) {
        printf("Chain already running\n");
        return;
    }

    if (!mCurrentStudy) {
        printf("Cannot run chain: no study loaded\n");
        return;
    }

    // Check if subject code is empty
    if (strlen(mSubjectCode) == 0) {
        printf("ERROR: Subject code is required to run chain\n");
        return;
    }

    // Check for existing subject codes
    std::vector<std::string> existingCodes = CheckExistingSubjectCodes();
    std::string currentCode(mSubjectCode);

    bool codeExists = false;
    for (const auto& code : existingCodes) {
        if (code == currentCode) {
            codeExists = true;
            break;
        }
    }

    if (codeExists) {
        printf("WARNING: Subject code '%s' has already been used in this study!\n", mSubjectCode);
        printf("Existing subject codes: ");
        for (size_t i = 0; i < existingCodes.size(); i++) {
            printf("%s%s", existingCodes[i].c_str(), i < existingCodes.size() - 1 ? ", " : "\n");
        }

        // Show dialog to user
        mDuplicateWarningCodes = existingCodes;
        mShowDuplicateSubjectWarning = true;
        return;  // Don't start chain yet - wait for user confirmation
    }

    // No duplicate - proceed directly
    RunChainConfirmed();
}

void LauncherUI::RunChainConfirmed()
{
    // Clean up any previous experiment
    if (mRunningExperiment) {
        if (mRunningExperiment->IsRunning()) {
            printf("Warning: Previous experiment still running\n");
            return;
        }
        delete mRunningExperiment;
        mRunningExperiment = nullptr;
    }

    // Clear accumulated output for new chain run
    mChainAccumulatedStdout.clear();
    mChainAccumulatedStderr.clear();

    // Build execution order with randomization groups
    // Items with the same randomGroup > 0 are shuffled among themselves,
    // regardless of their position in the chain
    const auto& items = mCurrentChain->GetItems();
    mChainExecutionOrder.clear();
    mChainExecutionOrder.reserve(items.size());

    // First pass: collect all items by randomization group
    std::map<int, std::vector<int>> groupItems;  // groupId -> list of item indices
    for (size_t i = 0; i < items.size(); i++) {
        if (items[i].type == ItemType::Test && items[i].randomGroup > 0) {
            groupItems[items[i].randomGroup].push_back(i);
        }
    }

    // Shuffle each group
    std::random_device rd;
    std::mt19937 g(rd());
    std::map<int, size_t> groupNextIndex;  // Track which item to use next from each group
    for (auto& [groupId, indices] : groupItems) {
        std::shuffle(indices.begin(), indices.end(), g);
        groupNextIndex[groupId] = 0;
        printf("Randomized group %d (%zu tests)\n", groupId, indices.size());
    }

    // Second pass: build execution order
    // Non-grouped items stay in place, grouped items are replaced with shuffled order
    for (size_t i = 0; i < items.size(); i++) {
        if (items[i].type == ItemType::Test && items[i].randomGroup > 0) {
            int groupId = items[i].randomGroup;
            // Use next item from shuffled group
            size_t& nextIdx = groupNextIndex[groupId];
            if (nextIdx < groupItems[groupId].size()) {
                mChainExecutionOrder.push_back(groupItems[groupId][nextIdx]);
                nextIdx++;
            }
        } else {
            // Not in a randomization group - add as-is
            mChainExecutionOrder.push_back(i);
        }
    }

    mRunningChain = true;
    mCurrentChainItemIndex = 0;
    printf("Starting chain execution (%zu items)\n", mCurrentChain->GetItems().size());

    // Execute first item
    ExecuteChainItem(0);
}

void LauncherUI::ExecuteChainItem(int index)
{
    if (!mCurrentChain || index < 0 || index >= (int)mChainExecutionOrder.size()) {
        printf("Invalid chain item index: %d\n", index);
        mRunningChain = false;
        return;
    }

    // Map logical execution index to actual item index
    int itemIndex = mChainExecutionOrder[index];
    const ChainItem& item = mCurrentChain->GetItems()[itemIndex];
    printf("Executing chain item %d/%zu: %s (item #%d)\n", index + 1, mChainExecutionOrder.size(),
           item.GetDisplayName().c_str(), itemIndex + 1);

    // Create new experiment runner
    mRunningExperiment = new ExperimentRunner(mConfig);

    if (item.type == ItemType::Test) {
        // Execute test item - look up test from study to get correct path
        const Test* test = mCurrentStudy->GetTest(item.testName);
        if (!test) {
            printf("Error: Test '%s' not found in study\n", item.testName.c_str());
            delete mRunningExperiment;
            mRunningExperiment = nullptr;
            mRunningChain = false;
            return;
        }

        std::string studyPath = mCurrentStudy->GetPath();
        // Extract basename from test_name for .pbl filename
        std::string baseName = fs::path(item.testName).filename().string();
        std::string testPath = (fs::path(studyPath) / "tests" / test->testPath / (baseName + ".pbl")).string();

        std::vector<std::string> args;

        // Note: Language is handled by ExperimentRunner::RunExperiment via the language parameter

        // Add parameter file (variant or default)
        // Note: PEBL automatically looks in params/ directory, so just pass filename
        if (!item.paramVariant.empty()) {
            std::string paramFile;

            if (item.paramVariant == "default") {
                // Use default parameter file: testName.pbl.par.json
                paramFile = baseName + ".pbl.par.json";
            } else {
                // Look up the actual filename from the parameter variant
                const ParameterVariant* variant = test->GetVariant(item.paramVariant);
                if (variant && !variant->file.empty()) {
                    paramFile = variant->file;
                }
            }

            if (!paramFile.empty()) {
                args.push_back("--pfile");
                args.push_back(paramFile);
            }
        }

        // Add upload flag if chain has upload enabled
        if (mCurrentChain->GetUploadEnabled()) {
            std::string uploadPath = mCurrentStudy->GetUploadConfigPath(item.testName);

            // Create/update upload.json for this test
            mCurrentStudy->CreateUploadConfigForTest(item.testName);

            // Add --upload flag
            args.push_back("--upload");
            args.push_back(uploadPath);
            printf("Upload enabled: %s\n", uploadPath.c_str());
        }

        // Add LSL flag if chain has LSL enabled
        if (mCurrentChain->GetLSLEnabled()) {
            std::string streamName = mCurrentChain->GetLSLStreamName();

            // Replace placeholders: {test} -> test name, {subject} -> subject ID
            size_t pos;
            while ((pos = streamName.find("{test}")) != std::string::npos) {
                streamName.replace(pos, 6, item.testName);
            }
            while ((pos = streamName.find("{subject}")) != std::string::npos) {
                streamName.replace(pos, 9, mSubjectCode);
            }

            // Add --lsl flag
            args.push_back("--lsl");
            args.push_back(streamName);
            printf("LSL enabled: stream=%s\n", streamName.c_str());
        }

        // Add additional arguments from Run tab settings
        std::vector<std::string> additionalArgs = BuildAdditionalArguments();
        args.insert(args.end(), additionalArgs.begin(), additionalArgs.end());

        // Debug: print all arguments being passed
        printf("ExecuteChainItem: Passing %zu arguments to PEBL:\n", args.size());
        for (size_t i = 0; i < args.size(); i++) {
            printf("  arg[%zu]: %s\n", i, args[i].c_str());
        }
        fflush(stdout);

        bool success = mRunningExperiment->RunExperiment(testPath, args,
                                                          mSubjectCode,
                                                          item.language.empty() ? mLanguageCode : item.language,
                                                          mFullscreen);

        if (!success) {
            printf("Failed to run test: %s\n", item.testName.c_str());
            delete mRunningExperiment;
            mRunningExperiment = nullptr;
            mRunningChain = false;
        }

    } else {
        // Execute page item (instruction/consent/completion)
        // Use ChainPage.pbl to display the page
        std::string tmpDir = GetWorkspaceTempDirectory(mConfig->GetWorkspacePath());
        std::string configFile = item.CreateChainPageConfig(tmpDir);

        if (configFile.empty()) {
            printf("Failed to create page config in: %s\n", tmpDir.c_str());
            delete mRunningExperiment;
            mRunningExperiment = nullptr;
            mRunningChain = false;
            return;
        }

        // Run ChainPage.pbl with the config - use absolute path from PEBL install
        std::string mediaPath = GetPEBLMediaPath(mConfig->GetPeblExecutablePath());
        std::string chainPagePath;
        if (!mediaPath.empty()) {
#ifdef _WIN32
            chainPagePath = mediaPath + "\\apps\\ChainPage\\ChainPage.pbl";
#else
            chainPagePath = mediaPath + "/apps/ChainPage/ChainPage.pbl";
#endif
        } else {
            // Fallback to relative path (may work if CWD is PEBL root)
            chainPagePath = "media/apps/ChainPage/ChainPage.pbl";
            printf("Warning: Could not determine PEBL media path, using relative path\n");
        }

        std::vector<std::string> args;
        // -v flag passes positional argument to Start(p)
        args.push_back("-v");
        args.push_back(configFile);

        // Add additional arguments from Run tab settings
        std::vector<std::string> additionalArgs = BuildAdditionalArguments();
        args.insert(args.end(), additionalArgs.begin(), additionalArgs.end());

        bool success = mRunningExperiment->RunExperiment(chainPagePath, args,
                                                          mSubjectCode,
                                                          mLanguageCode,
                                                          mFullscreen);

        if (!success) {
            printf("Failed to run page: %s\n", item.GetDisplayName().c_str());
            delete mRunningExperiment;
            mRunningExperiment = nullptr;
            mRunningChain = false;
        }
    }
}

void LauncherUI::TestChainItem(int index)
{
    if (!mCurrentChain || index < 0 || index >= (int)mCurrentChain->GetItems().size()) {
        printf("Invalid chain item index: %d\n", index);
        return;
    }

    if (!mCurrentStudy) {
        printf("Cannot test chain item: no study loaded\n");
        return;
    }

    // Clean up any previous experiment
    if (mRunningExperiment) {
        if (mRunningExperiment->IsRunning()) {
            printf("Warning: Previous experiment still running\n");
            return;
        }
        delete mRunningExperiment;
        mRunningExperiment = nullptr;
    }

    const ChainItem& item = mCurrentChain->GetItems()[index];
    printf("Test running chain item: %s\n", item.GetDisplayName().c_str());

    // Create new experiment runner
    mRunningExperiment = new ExperimentRunner(mConfig);

    if (item.type == ItemType::Test) {
        // Execute test item - look up test from study to get correct path
        const Test* test = mCurrentStudy->GetTest(item.testName);
        if (!test) {
            printf("Error: Test '%s' not found in study\n", item.testName.c_str());
            return;
        }

        std::string studyPath = mCurrentStudy->GetPath();
        // Extract basename from test_name for .pbl filename
        std::string baseName = fs::path(item.testName).filename().string();
        std::string testPath = (fs::path(studyPath) / "tests" / test->testPath / (baseName + ".pbl")).string();

        std::vector<std::string> args;

        // Note: Language is handled by ExperimentRunner::RunExperiment via the language parameter

        // Add parameter variant if not default
        if (!item.paramVariant.empty() && item.paramVariant != "default") {
            // Look up the actual filename from the parameter variant
            const ParameterVariant* variant = test->GetVariant(item.paramVariant);
            if (variant && !variant->file.empty()) {
                // Just pass params/filename - working dir is set to test directory
                std::string paramFile = "params/" + variant->file;
                args.push_back("--pfile");
                args.push_back(paramFile);
            }
        }

        // Add additional arguments from Run tab settings
        std::vector<std::string> additionalArgs = BuildAdditionalArguments();
        args.insert(args.end(), additionalArgs.begin(), additionalArgs.end());

        bool success = mRunningExperiment->RunExperiment(testPath, args,
                                                          mSubjectCode,
                                                          item.language.empty() ? mLanguageCode : item.language,
                                                          mFullscreen);

        if (!success) {
            printf("Failed to run test: %s\n", item.testName.c_str());
            delete mRunningExperiment;
            mRunningExperiment = nullptr;
        }

    } else {
        // Execute page item (instruction/consent/completion)
        // Use ChainPage.pbl to display the page
        std::string tmpDir = GetWorkspaceTempDirectory(mConfig->GetWorkspacePath());
        std::string configFile = item.CreateChainPageConfig(tmpDir);

        if (configFile.empty()) {
            printf("Failed to create page config in: %s\n", tmpDir.c_str());
            delete mRunningExperiment;
            mRunningExperiment = nullptr;
            return;
        }

        // Run ChainPage.pbl with the config - use absolute path from PEBL install
        std::string mediaPath = GetPEBLMediaPath(mConfig->GetPeblExecutablePath());
        std::string chainPagePath;
        if (!mediaPath.empty()) {
#ifdef _WIN32
            chainPagePath = mediaPath + "\\apps\\ChainPage\\ChainPage.pbl";
#else
            chainPagePath = mediaPath + "/apps/ChainPage/ChainPage.pbl";
#endif
        } else {
            // Fallback to relative path (may work if CWD is PEBL root)
            chainPagePath = "media/apps/ChainPage/ChainPage.pbl";
            printf("Warning: Could not determine PEBL media path, using relative path\n");
        }

        std::vector<std::string> args;
        // -v flag passes positional argument to Start(p)
        args.push_back("-v");
        args.push_back(configFile);

        // Add additional arguments from Run tab settings
        std::vector<std::string> additionalArgs = BuildAdditionalArguments();
        args.insert(args.end(), additionalArgs.begin(), additionalArgs.end());

        bool success = mRunningExperiment->RunExperiment(chainPagePath, args,
                                                          mSubjectCode,
                                                          mLanguageCode,
                                                          mFullscreen);

        if (!success) {
            printf("Failed to run page: %s\n", item.GetDisplayName().c_str());
            delete mRunningExperiment;
            mRunningExperiment = nullptr;
        }
    }
}

std::string LauncherUI::OpenDirectoryDialog(const std::string& title, const std::string& startDir)
{
#ifdef _WIN32
    // Windows: Use IFileDialog (Vista+) for modern folder picker
    std::string result;

    // Initialize COM
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr)) {
        IFileDialog* pfd = nullptr;
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
                              IID_IFileDialog, reinterpret_cast<void**>(&pfd));

        if (SUCCEEDED(hr)) {
            // Set options to pick folders
            DWORD dwOptions;
            hr = pfd->GetOptions(&dwOptions);
            if (SUCCEEDED(hr)) {
                hr = pfd->SetOptions(dwOptions | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);
            }

            // Set title
            if (SUCCEEDED(hr) && !title.empty()) {
                std::wstring wtitle(title.begin(), title.end());
                pfd->SetTitle(wtitle.c_str());
            }

            // Set starting directory if provided
            if (SUCCEEDED(hr) && !startDir.empty()) {
                IShellItem* psiFolder = nullptr;
                std::wstring wstartDir(startDir.begin(), startDir.end());
                hr = SHCreateItemFromParsingName(wstartDir.c_str(), NULL, IID_PPV_ARGS(&psiFolder));
                if (SUCCEEDED(hr)) {
                    pfd->SetFolder(psiFolder);
                    psiFolder->Release();
                }
            }

            // Show the dialog
            hr = pfd->Show(NULL);
            if (SUCCEEDED(hr)) {
                IShellItem* psi = nullptr;
                hr = pfd->GetResult(&psi);
                if (SUCCEEDED(hr)) {
                    PWSTR pszPath = nullptr;
                    hr = psi->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);
                    if (SUCCEEDED(hr)) {
                        // Convert wide string to UTF-8
                        int size_needed = WideCharToMultiByte(CP_UTF8, 0, pszPath, -1, NULL, 0, NULL, NULL);
                        if (size_needed > 0) {
                            result.resize(size_needed - 1);
                            WideCharToMultiByte(CP_UTF8, 0, pszPath, -1, &result[0], size_needed, NULL, NULL);
                        }
                        CoTaskMemFree(pszPath);
                    }
                    psi->Release();
                }
            }
            pfd->Release();
        }
        CoUninitialize();
    }

    return result;
#elif __APPLE__
    // macOS: Use osascript
    std::string command = "osascript -e 'POSIX path of (choose folder";
    if (!startDir.empty()) {
        command += " default location (POSIX file \"" + startDir + "\")";
    }
    command += " with prompt \"" + title + "\")'";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return "";

    char buffer[1024];
    std::string result;
    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result = buffer;
        // Remove trailing newline
        if (!result.empty() && result[result.length()-1] == '\n') {
            result.erase(result.length()-1);
        }
    }
    pclose(pipe);
    return result;
#else
    // Linux: Use zenity
    std::string command = "zenity --file-selection --directory --title=\"" + title + "\"";
    if (!startDir.empty()) {
        command += " --filename=\"" + startDir + "/\"";
    }
    command += " 2>/dev/null";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return "";

    char buffer[1024];
    std::string result;
    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result = buffer;
        // Remove trailing newline
        if (!result.empty() && result[result.length()-1] == '\n') {
            result.erase(result.length()-1);
        }
    }
    pclose(pipe);
    return result;
#endif
}

std::string LauncherUI::OpenFileDialog(const std::string& title, const std::string& filter, const std::string& initialDir)
{
#ifdef _WIN32
    // Windows: Use GetOpenFileName
    char filename[MAX_PATH] = "";

    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;

    // Convert filter from "*.json" format to Windows format "JSON Files\0*.json\0All Files\0*.*\0\0"
    std::string winFilter;
    if (!filter.empty()) {
        // Create a description from the filter
        std::string desc = filter;
        if (desc.substr(0, 2) == "*.") {
            desc = desc.substr(2) + " files";
            // Capitalize first letter
            if (!desc.empty()) desc[0] = toupper(desc[0]);
        }
        winFilter = desc + '\0' + filter + '\0' + "All Files" + '\0' + "*.*" + '\0';
    } else {
        winFilter = "All Files\0*.*\0";
    }
    winFilter += '\0';  // Double null terminator
    ofn.lpstrFilter = winFilter.c_str();

    // Convert title
    ofn.lpstrTitle = title.c_str();

    // Set initial directory if provided
    if (!initialDir.empty()) {
        ofn.lpstrInitialDir = initialDir.c_str();
    }

    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn)) {
        return std::string(filename);
    }
    return "";
#elif __APPLE__
    std::string command = "osascript -e 'POSIX path of (choose file with prompt \"" + title + "\")'";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return "";

    char buffer[1024];
    std::string result;
    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result = buffer;
        if (!result.empty() && result[result.length()-1] == '\n') {
            result.erase(result.length()-1);
        }
    }
    pclose(pipe);
    return result;
#else
    // Linux: Use zenity
    std::string command = "zenity --file-selection --title=\"" + title + "\"";
    if (!filter.empty()) {
        command += " --file-filter=\"" + filter + "\"";
    }
    command += " 2>/dev/null";

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return "";

    char buffer[1024];
    std::string result;
    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result = buffer;
        if (!result.empty() && result[result.length()-1] == '\n') {
            result.erase(result.length()-1);
        }
    }
    pclose(pipe);
    return result;
#endif
}

std::string LauncherUI::SaveFileDialog(const std::string& title, const std::string& defaultName)
{
#ifdef _WIN32
    // Windows: Use GetSaveFileName
    char filename[MAX_PATH];
    strncpy(filename, defaultName.c_str(), MAX_PATH - 1);
    filename[MAX_PATH - 1] = '\0';

    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "All Files\0*.*\0";
    ofn.lpstrTitle = title.c_str();
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

    if (GetSaveFileNameA(&ofn)) {
        return std::string(filename);
    }
    return "";
#elif __APPLE__
    std::string command = "osascript -e 'POSIX path of (choose file name with prompt \"" + title + "\"";
    if (!defaultName.empty()) {
        command += " default name \"" + defaultName + "\"";
    }
    command += ")'";

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return "";

    char buffer[1024];
    std::string result;
    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result = buffer;
        if (!result.empty() && result[result.length()-1] == '\n') {
            result.erase(result.length()-1);
        }
    }
    pclose(pipe);
    return result;
#else
    // Linux: Use zenity
    std::string command = "zenity --file-selection --save --title=\"" + title + "\"";
    if (!defaultName.empty()) {
        command += " --filename=\"" + defaultName + "\"";
    }
    command += " 2>/dev/null";

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return "";

    char buffer[1024];
    std::string result;
    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result = buffer;
        if (!result.empty() && result[result.length()-1] == '\n') {
            result.erase(result.length()-1);
        }
    }
    pclose(pipe);
    return result;
#endif
}

// ============================================================================
// Study Tab Implementation
// ============================================================================

void LauncherUI::RenderStudyTab()
{
    ImGui::Text("Study Management");
    ImGui::Separator();
    ImGui::Spacing();

    // Top section: Study selector and info
    ImGui::BeginChild("StudySelector", ImVec2(0, 100), true);

    // Study selection dropdown
    ImGui::Text("Current Study:");
    ImGui::SameLine();

    const char* currentStudyName = mCurrentStudy ? mCurrentStudy->GetName().c_str() : "None";
    ImGui::PushItemWidth(250);
    if (ImGui::BeginCombo("##StudySelect", currentStudyName)) {
        // Refresh study list from disk each time the combo is opened
        mStudyList = mWorkspace->GetStudyDirectories();

        // "None" option
        if (ImGui::Selectable("None", !mCurrentStudy)) {
            mCurrentStudy.reset();
        }

        // List studies from workspace
        for (size_t i = 0; i < mStudyList.size(); i++) {
            bool is_selected = (mCurrentStudy && mCurrentStudy->GetName() == mStudyList[i]);
            if (ImGui::Selectable(mStudyList[i].c_str(), is_selected)) {
                mSelectedStudyIndex = i;
                LoadStudy(mStudyList[i]);
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();

    ImGui::SameLine();

    // New study button
    if (ImGui::Button("New Study...")) {
        CreateNewStudy();
    }

    ImGui::Spacing();

    // Study info
    if (mCurrentStudy) {
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "%s", mCurrentStudy->GetName().c_str());
        ImGui::Text("Description: %s", mCurrentStudy->GetDescription().c_str());
        ImGui::Text("Tests in study: %zu", mCurrentStudy->GetTests().size());
    } else {
        ImGui::TextDisabled("No study loaded. Create a new study or select an existing one.");
    }

    ImGui::EndChild();

    ImGui::Spacing();

    // Tests in study section - split view: list on left, preview on right
    ImGui::Text("Tests in Study:");

    float listWidth = ImGui::GetContentRegionAvail().x * 0.4f;

    // Left side: Test list
    ImGui::BeginChild("StudyTestsList", ImVec2(listWidth, -40), true);

    if (!mCurrentStudy) {
        ImGui::TextDisabled("Load a study to view tests");
    } else if (mCurrentStudy->GetTests().empty()) {
        ImGui::TextDisabled("No tests in this study. Use 'Add to Study' button on Details tab to add tests.");
    } else {
        // Display tests in study as selectable items
        const auto& tests = mCurrentStudy->GetTests();
        for (size_t i = 0; i < tests.size(); i++) {
            ImGui::PushID((int)i);

            // Display name (prefer displayName, fall back to testName)
            std::string displayLabel = tests[i].displayName.empty()
                ? tests[i].testName : tests[i].displayName;

            bool is_selected = (mSelectedStudyTestIndex == (int)i);
            if (ImGui::Selectable(displayLabel.c_str(), is_selected)) {
                if (mSelectedStudyTestIndex != (int)i) {
                    mSelectedStudyTestIndex = (int)i;
                    LoadStudyTestPreview((int)i);
                }
            }

            // Show parameter variants as tooltip
            if (ImGui::IsItemHovered() && !tests[i].parameterVariants.empty()) {
                ImGui::SetTooltip("%zu parameter variants", tests[i].parameterVariants.size());
            }

            ImGui::PopID();
        }
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // Right side: Test preview (screenshot + about.txt)
    ImGui::BeginChild("StudyTestPreview", ImVec2(0, -40), true);

    if (mCurrentStudy && mSelectedStudyTestIndex >= 0 &&
        mSelectedStudyTestIndex < (int)mCurrentStudy->GetTests().size()) {

        const auto& test = mCurrentStudy->GetTests()[mSelectedStudyTestIndex];

        // Test name header
        std::string headerName = test.displayName.empty() ? test.testName : test.displayName;
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "%s", headerName.c_str());
        ImGui::Separator();
        ImGui::Spacing();

        // Action buttons
        if (ImGui::SmallButton("Edit Params")) {
            EditTestParameters(mSelectedStudyTestIndex);
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Remove from Study")) {
            const std::string testName = test.testName;
            RemoveTestFromStudy(testName);
            // Reset selection if removed test was selected
            if (mSelectedStudyTestIndex >= (int)mCurrentStudy->GetTests().size()) {
                mSelectedStudyTestIndex = (int)mCurrentStudy->GetTests().size() - 1;
                if (mSelectedStudyTestIndex >= 0) {
                    LoadStudyTestPreview(mSelectedStudyTestIndex);
                } else {
                    FreeStudyTestScreenshot();
                    mStudyTestDescription.clear();
                }
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Screenshot
        if (mStudyTestScreenshot) {
            float aspectRatio = (float)mStudyTestScreenshotH / (float)mStudyTestScreenshotW;
            float displayWidth = ImGui::GetContentRegionAvail().x;
            float displayHeight = displayWidth * aspectRatio;

            if (displayHeight > 400) {
                displayHeight = 400;
                displayWidth = displayHeight / aspectRatio;
            }

            ImGui::Image((ImTextureID)(intptr_t)mStudyTestScreenshot,
                        ImVec2(displayWidth, displayHeight));
            ImGui::Spacing();
        }

        // Description
        if (!mStudyTestDescription.empty()) {
            ImGui::TextWrapped("%s", mStudyTestDescription.c_str());
        } else {
            ImGui::TextDisabled("No description available");
        }
    } else {
        ImGui::TextDisabled("Select a test to view details");
    }

    ImGui::EndChild();

    ImGui::Spacing();

    // Bottom buttons
    if (!mCurrentStudy) {
        ImGui::BeginDisabled();
    }

    if (ImGui::Button("Save Study", ImVec2(-1, 0))) {
        if (mCurrentStudy) {
            mCurrentStudy->Save();
            printf("Study saved: %s\n", mCurrentStudy->GetName().c_str());
        }
    }

    if (!mCurrentStudy) {
        ImGui::EndDisabled();
    }
}

// ============================================================================
// Page Editor Dialog Implementation
// ============================================================================

void LauncherUI::ShowPageEditor()
{
    ImGui::OpenPopup("Page Editor");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);

    if (ImGui::BeginPopupModal("Page Editor", &mPageEditor.show, 0))
    {
        const char* pageTypes[] = { "Instruction", "Consent", "Completion" };

        ImGui::Text("Page Type:");
        ImGui::SameLine();
        ImGui::PushItemWidth(150);
        ImGui::Combo("##PageType", &mPageEditor.pageType, pageTypes, 3);
        ImGui::PopItemWidth();

        ImGui::Spacing();

        // Title field
        ImGui::Text("Title:");
        ImGui::PushItemWidth(-1);
        if (ImGui::IsWindowAppearing()) {
            ImGui::SetKeyboardFocusHere();
        }
        ImGui::InputText("##Title", mPageEditor.title, sizeof(mPageEditor.title));
        ImGui::PopItemWidth();

        ImGui::Spacing();

        // Content field (large text area)
        ImGui::Text("Content:");
        ImGui::PushItemWidth(-1);
        ImGui::InputTextMultiline("##Content", mPageEditor.content, sizeof(mPageEditor.content),
                                   ImVec2(-1, 300));
        ImGui::PopItemWidth();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Buttons
        if (ImGui::Button("Save", ImVec2(120, 0))) {
            if (!mCurrentChain) {
                printf("Error: No chain loaded\n");
            } else {
                // Create ChainItem based on page type
                ChainItem item;
                if (mPageEditor.pageType == 0) {
                    item.type = ItemType::Instruction;
                } else if (mPageEditor.pageType == 1) {
                    item.type = ItemType::Consent;
                } else {
                    item.type = ItemType::Completion;
                }

                item.title = mPageEditor.title;
                item.content = mPageEditor.content;

                if (mPageEditor.editingIndex >= 0) {
                    // Update existing item
                    mCurrentChain->RemoveItem(mPageEditor.editingIndex);
                    mCurrentChain->InsertItem(mPageEditor.editingIndex, item);
                    printf("Updated page: %s\n", item.title.c_str());
                } else {
                    // Add new item
                    mCurrentChain->AddItem(item);
                    printf("Added page: %s\n", item.title.c_str());
                }

                // Auto-save the chain
                SaveCurrentChain();
            }

            mPageEditor.show = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            mPageEditor.show = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void LauncherUI::ShowTestEditor()
{
    const char* dialogTitle = mTestEditor.editingIndex >= 0 ? "Edit Test" : "Add Test to Chain";
    ImGui::OpenPopup(dialogTitle);

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);

    if (ImGui::BeginPopupModal(dialogTitle, &mTestEditor.show, 0))
    {
        if (!mCurrentStudy) {
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Error: No study loaded");
            if (ImGui::Button("Close", ImVec2(120, 0))) {
                mTestEditor.show = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
            return;
        }

        const auto& tests = mCurrentStudy->GetTests();
        if (tests.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.3f, 1.0f), "No tests in study");
            ImGui::TextWrapped("Add tests to the study in the Tests tab first.");
            if (ImGui::Button("Close", ImVec2(120, 0))) {
                mTestEditor.show = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
            return;
        }

        // In edit mode, just show which test is being edited (read-only)
        // In add mode, show test selection list
        if (mTestEditor.editingIndex >= 0) {
            // Edit mode - show test name as read-only
            ImGui::Text("Editing Test:");
            ImGui::SameLine();
            if (mTestEditor.selectedTestIndex >= 0 && mTestEditor.selectedTestIndex < (int)tests.size()) {
                ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "%s", tests[mTestEditor.selectedTestIndex].testName.c_str());
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "(test not found)");
            }
            ImGui::Spacing();
        } else {
            // Add mode - show test selection list
            ImGui::Text("Select Test:");
            ImGui::Spacing();

            ImGui::BeginChild("TestList", ImVec2(0, 150), true);
            for (size_t i = 0; i < tests.size(); i++) {
                bool isSelected = (mTestEditor.selectedTestIndex == (int)i);
                if (ImGui::Selectable(tests[i].testName.c_str(), isSelected)) {
                    mTestEditor.selectedTestIndex = i;
                    mTestEditor.selectedVariantIndex = 0;  // Reset variant selection
                }
            }
            ImGui::EndChild();

            ImGui::Spacing();
        }

        // Parameter variant selection (if test is selected)
        if (mTestEditor.selectedTestIndex >= 0 && mTestEditor.selectedTestIndex < (int)tests.size()) {
            const Test& selectedTest = tests[mTestEditor.selectedTestIndex];

            ImGui::Text("Parameter Variant:");
            ImGui::SameLine();
            ImGui::PushItemWidth(200);

            // Build list of variants
            std::vector<std::string> variantNames;
            variantNames.push_back("default");  // Always have default option
            for (const auto& [name, variant] : selectedTest.parameterVariants) {
                variantNames.push_back(name);
            }

            // Current variant name
            const char* currentVariant = mTestEditor.selectedVariantIndex < (int)variantNames.size()
                ? variantNames[mTestEditor.selectedVariantIndex].c_str()
                : "default";

            if (ImGui::BeginCombo("##Variant", currentVariant)) {
                for (size_t i = 0; i < variantNames.size(); i++) {
                    bool isSelected = (mTestEditor.selectedVariantIndex == (int)i);
                    if (ImGui::Selectable(variantNames[i].c_str(), isSelected)) {
                        mTestEditor.selectedVariantIndex = i;
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();

            ImGui::Spacing();

            // Language selection (optional) - scan for available language files
            ImGui::Text("Language (optional):");
            ImGui::SameLine();

            // Scan translations directory for available language files
            std::vector<std::string> availableLanguages;
            std::string studyPath = mCurrentStudy->GetPath();
            std::string testPath = studyPath + "/tests/" + selectedTest.testPath;
            std::string translationsPath = testPath + "/translations";

            if (fs::exists(translationsPath) && fs::is_directory(translationsPath)) {
                for (const auto& entry : fs::directory_iterator(translationsPath)) {
                    if (entry.is_regular_file()) {
                        std::string filename = entry.path().filename().string();
                        // Look for files like "testname.pbl-en.json"
                        size_t dashPos = filename.rfind('-');
                        size_t dotPos = filename.rfind(".json");
                        if (dashPos != std::string::npos && dotPos != std::string::npos && dotPos > dashPos) {
                            std::string lang = filename.substr(dashPos + 1, dotPos - dashPos - 1);
                            availableLanguages.push_back(lang);
                        }
                    }
                }
            }

            ImGui::PushItemWidth(150);
            if (ImGui::BeginCombo("##Language", mTestEditor.language)) {
                // Show available languages from translation files
                for (const auto& lang : availableLanguages) {
                    bool isSelected = (std::string(mTestEditor.language) == lang);
                    if (ImGui::Selectable(lang.c_str(), isSelected)) {
                        std::strncpy(mTestEditor.language, lang.c_str(), sizeof(mTestEditor.language) - 1);
                        mTestEditor.language[sizeof(mTestEditor.language) - 1] = '\0';
                    }
                }

                // Allow custom entry
                ImGui::Separator();
                ImGui::TextDisabled("Custom:");
                static char customLang[16] = "";
                if (ImGui::InputText("##CustomLang", customLang, sizeof(customLang), ImGuiInputTextFlags_EnterReturnsTrue)) {
                    std::strncpy(mTestEditor.language, customLang, sizeof(mTestEditor.language) - 1);
                    mTestEditor.language[sizeof(mTestEditor.language) - 1] = '\0';
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();

            if (!availableLanguages.empty()) {
                ImGui::SameLine();
                ImGui::TextDisabled("(%zu available)", availableLanguages.size());
            }

            // Translation editor button
            ImGui::SameLine();
            if (ImGui::SmallButton("Edit Translations...")) {
                // Open translation editor dialog
                std::string testPath = studyPath + "/tests/" + selectedTest.testPath;
                mTranslationEditor.testIndex = mTestEditor.selectedTestIndex;
                std::strncpy(mTranslationEditor.testPath, testPath.c_str(), sizeof(mTranslationEditor.testPath) - 1);
                mTranslationEditor.testPath[sizeof(mTranslationEditor.testPath) - 1] = '\0';

                // Pre-fill language if one is set in test editor
                if (std::strlen(mTestEditor.language) > 0) {
                    std::strncpy(mTranslationEditor.language, mTestEditor.language, sizeof(mTranslationEditor.language) - 1);
                    mTranslationEditor.language[sizeof(mTranslationEditor.language) - 1] = '\0';
                } else {
                    mTranslationEditor.language[0] = '\0';
                }

                mTranslationEditor.show = true;
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Open translation editor for this test");
            }

            ImGui::Spacing();

            // Randomization group (optional)
            ImGui::Text("Randomization Group:");
            ImGui::SameLine();
            ImGui::PushItemWidth(100);
            const char* groupOptions[] = {"None", "1", "2", "3"};
            ImGui::Combo("##RandomGroup", &mTestEditor.randomGroup, groupOptions, 4);
            ImGui::PopItemWidth();
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Group 0 (None) keeps test in fixed position\n"
                                "Groups 1-3 allow randomizing order with other tests in same group");
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Buttons
        bool canSave = mTestEditor.selectedTestIndex >= 0;
        if (!canSave) {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button("Save", ImVec2(120, 0))) {
            if (!mCurrentChain) {
                printf("Error: No chain loaded\n");
            } else {
                const Test& selectedTest = tests[mTestEditor.selectedTestIndex];

                // Create ChainItem for test
                ChainItem item(ItemType::Test);
                item.testName = selectedTest.testName;

                // Set parameter variant
                if (mTestEditor.selectedVariantIndex > 0) {
                    std::vector<std::string> variantNames;
                    for (const auto& [name, variant] : selectedTest.parameterVariants) {
                        variantNames.push_back(name);
                    }
                    if (mTestEditor.selectedVariantIndex - 1 < (int)variantNames.size()) {
                        item.paramVariant = variantNames[mTestEditor.selectedVariantIndex - 1];
                    }
                } else {
                    item.paramVariant = "default";
                }

                // Set language if specified
                if (std::strlen(mTestEditor.language) > 0) {
                    item.language = mTestEditor.language;
                }

                // Set randomization group
                item.randomGroup = mTestEditor.randomGroup;

                if (mTestEditor.editingIndex >= 0) {
                    // Update existing item
                    mCurrentChain->RemoveItem(mTestEditor.editingIndex);
                    mCurrentChain->InsertItem(mTestEditor.editingIndex, item);
                    printf("Updated test item: %s\n", item.testName.c_str());
                } else {
                    // Add new item
                    mCurrentChain->AddItem(item);
                    printf("Added test to chain: %s\n", item.testName.c_str());
                }

                // Auto-save the chain
                SaveCurrentChain();
            }

            mTestEditor.show = false;
            ImGui::CloseCurrentPopup();
        }

        if (!canSave) {
            ImGui::EndDisabled();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            mTestEditor.show = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

// ============================================================================
// Study Management Function Stubs
// ============================================================================

void LauncherUI::CreateNewStudy()
{
    // TODO: Show dialog to enter study name and description
    printf("CreateNewStudy() - not yet implemented\n");
}

void LauncherUI::ImportSnapshotFromPath(const std::string& snapshotPath)
{
    if (!mSnapshots) return;

    // Read study_name directly since full validation may fail on platform format
    std::string studyName = "imported_study";
    std::string studyInfoPath = snapshotPath + "/study-info.json";
    std::ifstream infoFile(studyInfoPath);
    if (infoFile.is_open()) {
        try {
            nlohmann::json j;
            infoFile >> j;
            studyName = j.value("study_name", "imported_study");
        } catch (...) {}
        infoFile.close();
    }

    // Import first (copies files), then convert
    std::string studiesDir = mWorkspace->GetStudiesPath();
    std::string newStudyName = studyName + "_imported";

    if (mSnapshots->ImportSnapshot(snapshotPath, studiesDir, newStudyName)) {
        std::string newStudyPath = studiesDir + "/" + newStudyName;

        // Convert platform format to launcher format (in place on copied study)
        if (!mSnapshots->ConvertSnapshotFormat(newStudyPath)) {
            printf("Warning: Failed to convert snapshot format, attempting to use as-is\n");
        }

        printf("Imported snapshot as: %s\n", newStudyName.c_str());

        // Load the newly imported study
        LoadStudy(newStudyPath);
    } else {
        printf("Failed to import snapshot\n");
    }
}

void LauncherUI::LoadStudy(const std::string& studyPath)
{
    printf("LoadStudy(%s)\n", studyPath.c_str());

    // Handle both absolute and relative paths
    std::string fullPath = studyPath;

    // If path doesn't start with / or contain : (Windows drive), assume it's relative to workspace
    if (!studyPath.empty() && studyPath[0] != '/' && studyPath.find(':') == std::string::npos) {
        // Try prepending workspace studies path
        std::string studiesPath = mWorkspace->GetStudiesPath();
        fullPath = studiesPath + "/" + studyPath;
        printf("Converted to full path: %s\n", fullPath.c_str());
    }

    mCurrentStudy = Study::LoadFromDirectory(fullPath);
    if (mCurrentStudy) {
        printf("Study loaded: %s\n", mCurrentStudy->GetName().c_str());

        // Reset study test preview
        mSelectedStudyTestIndex = -1;
        FreeStudyTestScreenshot();
        mStudyTestDescription.clear();

        // Save selected study to config
        mConfig->SetCurrentStudyPath(fullPath);
        mConfig->SaveConfig();

        // Auto-load a chain: prefer "Main.json", otherwise first alphabetically
        std::string mainChainPath = fullPath + "/chains/Main.json";
        if (fs::exists(mainChainPath)) {
            LoadChain(mainChainPath);
            printf("Auto-loaded Main chain\n");
        } else {
            // No Main chain - load first chain alphabetically
            auto chainFiles = mCurrentStudy->GetChainFiles();
            if (!chainFiles.empty()) {
                std::sort(chainFiles.begin(), chainFiles.end());
                std::string firstChainPath = fullPath + "/chains/" + chainFiles[0];
                LoadChain(firstChainPath);
                printf("Auto-loaded first chain: %s\n", chainFiles[0].c_str());
            } else {
                // No chains - clear current chain
                mCurrentChain.reset();
            }
        }
    } else {
        printf("Failed to load study from: %s\n", fullPath.c_str());

        // Show error message to user
        printf("Error: Could not find study-info.json in %s\n", fullPath.c_str());
        printf("Make sure the selected directory contains a valid PEBL study.\n");
    }
}

void LauncherUI::AddTestToStudy()
{
    if (!mCurrentStudy) {
        printf("Error: No study loaded. Create or load a study first.\n");
        return;
    }

    if (mSelectedExperiment < 0 || mSelectedExperiment >= (int)mExperiments.size()) {
        printf("Error: No experiment selected\n");
        return;
    }

    const ExperimentInfo& exp = mExperiments[mSelectedExperiment];

    // Copy test from battery to study/tests/
    std::string studyPath = mCurrentStudy->GetPath();

    // Use the parent directory name as the test folder name
    // This avoids nested directories when exp.name contains "/"
    fs::path sourceDir(exp.directory);
    std::string testFolderName = sourceDir.filename().string();
    std::string testDestDir = studyPath + "/tests/" + testFolderName;

    try {
        // Create test directory
        fs::create_directories(testDestDir);

        // Copy all files from source directory (not subdirectories)
        for (const auto& entry : fs::directory_iterator(sourceDir)) {
            if (entry.is_regular_file()) {
                fs::path destFile = fs::path(testDestDir) / entry.path().filename();
                fs::copy_file(entry.path(), destFile, fs::copy_options::overwrite_existing);
                printf("Copied %s\n", entry.path().filename().string().c_str());
            }
        }

        // Copy ALL subdirectories (params, translations, sounds, images, etc.)
        for (const auto& entry : fs::directory_iterator(sourceDir)) {
            if (entry.is_directory()) {
                std::string subDirName = entry.path().filename().string();
                // Skip data directory - that's for output, not resources
                if (subDirName == "data") continue;

                std::string subDirDest = testDestDir + "/" + subDirName;
                fs::create_directories(subDirDest);
                fs::copy(entry.path(), subDirDest, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
                printf("Copied %s directory\n", subDirName.c_str());
            }
        }

        // Add test entry to study
        // Use display name that includes parent/filename for disambiguation
        Test test;
        test.testName = exp.name;
        test.testPath = testFolderName;  // Relative path within study/tests/
        test.included = true;

        mCurrentStudy->AddTest(test);

        // Scan for parameter variants in the copied params directory
        int testIndex = (int)mCurrentStudy->GetTests().size() - 1;
        ScanParameterVariants(testIndex);

        mCurrentStudy->Save();  // Save study-info.json
        printf("Added test to study: %s\n", test.testName.c_str());

        // Also add to default "Main" chain if it exists
        std::string mainChainPath = studyPath + "/chains/Main.json";
        if (fs::exists(mainChainPath)) {
            ChainItem item(ItemType::Test);
            item.testName = test.testName;
            item.paramVariant = "default";
            item.language = "en";
            item.randomGroup = 0;

            // If mCurrentChain is the Main chain, add directly to it
            if (mCurrentChain && mCurrentChain->GetFilePath() == mainChainPath) {
                mCurrentChain->AddItem(item);
                mCurrentChain->Save();
                printf("Added test to Main chain (current chain)\n");
            } else {
                // Otherwise load Main chain separately
                auto mainChain = Chain::LoadFromFile(mainChainPath);
                if (mainChain) {
                    mainChain->AddItem(item);
                    mainChain->Save();
                    printf("Added test to Main chain\n");
                }
            }
        }

    } catch (const fs::filesystem_error& e) {
        printf("Error copying test files: %s\n", e.what());
    }
}

void LauncherUI::AddTestFromFile(const std::string& filePath)
{
    if (!mCurrentStudy) {
        printf("Error: No study loaded. Create or load a study first.\n");
        return;
    }

    if (!fs::exists(filePath)) {
        printf("Error: File does not exist: %s\n", filePath.c_str());
        return;
    }

    // Extract test name from filename (without .pbl extension)
    fs::path path(filePath);
    std::string testName = path.stem().string();
    std::string sourceDir = path.parent_path().string();

    // Create test directory in study/tests/
    std::string studyPath = mCurrentStudy->GetPath();
    std::string testDestDir = studyPath + "/tests/" + testName;

    try {
        // Create test directory
        fs::create_directories(testDestDir);

        // Copy all files from source directory (not subdirectories)
        for (const auto& entry : fs::directory_iterator(sourceDir)) {
            if (entry.is_regular_file()) {
                fs::path destFile = fs::path(testDestDir) / entry.path().filename();
                fs::copy_file(entry.path(), destFile, fs::copy_options::overwrite_existing);
                printf("Copied %s\n", entry.path().filename().string().c_str());
            }
        }

        // Copy ALL subdirectories (params, translations, sounds, images, etc.)
        for (const auto& entry : fs::directory_iterator(sourceDir)) {
            if (entry.is_directory()) {
                std::string subDirName = entry.path().filename().string();
                // Skip data directory - that's for output, not resources
                if (subDirName == "data") continue;

                std::string subDirDest = testDestDir + "/" + subDirName;
                fs::create_directories(subDirDest);
                fs::copy(entry.path(), subDirDest, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
                printf("Copied %s directory\n", subDirName.c_str());
            }
        }

        // Add test to study
        Test test;
        test.testName = testName;
        test.testPath = testName;  // Relative path within study/tests/
        test.included = true;

        mCurrentStudy->AddTest(test);
        mCurrentStudy->Save();  // Save study-info.json
        printf("Added test from file to study: %s\n", testName.c_str());

        // Also add to default "Main" chain if it exists
        std::string mainChainPath = studyPath + "/chains/Main.json";
        if (fs::exists(mainChainPath)) {
            ChainItem item(ItemType::Test);
            item.testName = testName;
            item.paramVariant = "default";
            item.language = "en";
            item.randomGroup = 0;

            // If mCurrentChain is the Main chain, add directly to it
            if (mCurrentChain && mCurrentChain->GetFilePath() == mainChainPath) {
                mCurrentChain->AddItem(item);
                mCurrentChain->Save();
                printf("Added test to Main chain (current chain)\n");
            } else {
                // Otherwise load Main chain separately
                auto mainChain = Chain::LoadFromFile(mainChainPath);
                if (mainChain) {
                    mainChain->AddItem(item);
                    mainChain->Save();
                    printf("Added test to Main chain\n");
                }
            }
        }

    } catch (const fs::filesystem_error& e) {
        printf("Error copying test files: %s\n", e.what());
    }
}

void LauncherUI::CreateTestFromTemplate(const std::string& testName, int templateType)
{
    if (!mCurrentStudy) {
        printf("Error: No study loaded. Create or load a study first.\n");
        return;
    }

    // Create test directory in study/tests/
    std::string studyPath = mCurrentStudy->GetPath();
    std::string testDir = studyPath + "/tests/" + testName;

    try {
        // Create test directory structure
        fs::create_directories(testDir);
        fs::create_directories(testDir + "/params");
        fs::create_directories(testDir + "/translations");

        // Get template filename from dynamic list
        std::string templateFilename;
        if (templateType >= 0 && templateType < (int)mTemplateFiles.size()) {
            templateFilename = mTemplateFiles[templateType];
        } else {
            printf("Error: Invalid template type: %d\n", templateType);
            return;
        }

        // Find template file in media/templates/
        std::string templatePath = mBatteryPath + "/../media/templates/" + templateFilename;

        // Try to read template file
        std::ifstream templateFile(templatePath);
        if (!templateFile.is_open()) {
            printf("Warning: Could not find template file: %s\n", templatePath.c_str());
            printf("Using minimal fallback template.\n");

            // Fallback to minimal template
            std::string pblFile = testDir + "/" + testName + ".pbl";
            std::ofstream out(pblFile);
            if (!out.is_open()) {
                printf("Error: Could not create .pbl file\n");
                return;
            }
            out << "## " << testName << " - PEBL Test\n";
            out << "## Generated from template\n\n";
            out << "define Start(p) {\n";
            out << "    gWin <- MakeWindow(\"grey40\")\n";
            out << "    gSleepEasy <- 1\n\n";
            out << "    if(gSubNum+\"\" == \"0\") {\n";
            out << "        gSubNum <- GetSubNum(gWin)\n";
            out << "    }\n\n";
            out << "    ## Your code here\n\n";
            out << "    MessageBox(\"Experiment complete. Thank you!\", gWin)\n";
            out << "    return(0)\n";
            out << "}\n";
            out.close();
        } else {
            // Read entire template file
            std::stringstream buffer;
            buffer << templateFile.rdbuf();
            std::string templateContent = buffer.str();
            templateFile.close();

            // Write template content to new test file
            std::string pblFile = testDir + "/" + testName + ".pbl";
            std::ofstream out(pblFile);
            if (!out.is_open()) {
                printf("Error: Could not create .pbl file\n");
                return;
            }

            // Replace template name placeholders with actual test name
            // (For now, just write the template as-is - user can customize the filename in the .pbl)
            out << templateContent;
            out.close();
        }

        printf("Created test from template: %s/%s.pbl\n", testDir.c_str(), testName.c_str());

        // Add test to study
        Test test;
        test.testName = testName;
        test.testPath = testName;  // Relative path within study/tests/
        test.included = true;

        mCurrentStudy->AddTest(test);
        mCurrentStudy->Save();  // Save study-info.json
        printf("Added new test to study: %s\n", testName.c_str());

    } catch (const fs::filesystem_error& e) {
        printf("Error creating test from template: %s\n", e.what());
    }
}

void LauncherUI::CreateTestFromGenericStudy(const std::string& testName)
{
    if (!mCurrentStudy) {
        printf("Error: No study loaded. Create or load a study first.\n");
        return;
    }

    // Create test directory in study/tests/
    std::string studyPath = mCurrentStudy->GetPath();
    std::string testDir = studyPath + "/tests/" + testName;

    try {
        // Source: battery/template/ directory
        std::string templateDir = mBatteryPath + "/template";

        if (!fs::exists(templateDir)) {
            printf("Error: Template directory not found: %s\n", templateDir.c_str());
            return;
        }

        // Copy entire directory structure recursively
        fs::create_directories(testDir);

        // Copy params/ directory
        if (fs::exists(templateDir + "/params")) {
            fs::copy(templateDir + "/params", testDir + "/params",
                    fs::copy_options::recursive | fs::copy_options::overwrite_existing);

            // Rename template.pbl.schema.json to testname.pbl.schema.json
            std::string oldSchema = testDir + "/params/template.pbl.schema.json";
            std::string newSchema = testDir + "/params/" + testName + ".pbl.schema.json";
            if (fs::exists(oldSchema)) {
                fs::rename(oldSchema, newSchema);
            }
        }

        // Copy translations/ directory
        if (fs::exists(templateDir + "/translations")) {
            fs::copy(templateDir + "/translations", testDir + "/translations",
                    fs::copy_options::recursive | fs::copy_options::overwrite_existing);

            // Rename template.pbl-en.json to testname.pbl-en.json
            std::string oldTranslation = testDir + "/translations/template.pbl-en.json";
            std::string newTranslation = testDir + "/translations/" + testName + ".pbl-en.json";
            if (fs::exists(oldTranslation)) {
                fs::rename(oldTranslation, newTranslation);
            }
        }

        // Create data/ directory (empty)
        fs::create_directories(testDir + "/data");

        // Copy template.pbl to testname.pbl
        std::string templatePbl = templateDir + "/template.pbl";
        std::string newPbl = testDir + "/" + testName + ".pbl";
        if (fs::exists(templatePbl)) {
            fs::copy(templatePbl, newPbl, fs::copy_options::overwrite_existing);
        }

        // Copy template.pbl.about.txt to testname.pbl.about.txt
        std::string templateAbout = templateDir + "/template.pbl.about.txt";
        std::string newAbout = testDir + "/" + testName + ".pbl.about.txt";
        if (fs::exists(templateAbout)) {
            fs::copy(templateAbout, newAbout, fs::copy_options::overwrite_existing);
        }

        printf("Created test from Generic Study Template: %s\n", testDir.c_str());
        printf("  ✓ Copied params/ directory\n");
        printf("  ✓ Copied translations/ directory\n");
        printf("  ✓ Created %s.pbl\n", testName.c_str());
        printf("  ✓ Created %s.pbl.about.txt\n", testName.c_str());

        // Add test to study
        Test test;
        test.testName = testName;
        test.testPath = testName;  // Relative path within study/tests/
        test.included = true;

        mCurrentStudy->AddTest(test);
        mCurrentStudy->Save();  // Save study-info.json
        printf("Added new test to study: %s\n", testName.c_str());

    } catch (const fs::filesystem_error& e) {
        printf("Error creating test from Generic Study Template: %s\n", e.what());
    }
}

void LauncherUI::RemoveTestFromStudy(const std::string& testName)
{
    if (!mCurrentStudy) {
        printf("Error: No study loaded\n");
        return;
    }

    mCurrentStudy->RemoveTest(testName);
    mCurrentStudy->Save();  // Save study-info.json
    printf("Removed test from study: %s\n", testName.c_str());
}

bool LauncherUI::SyncScaleSchema(const std::string& testDir, const std::string& scaleCode)
{
    // Look for scale definition JSON in the test directory.
    // Scale tests store definitions in subdirectories like:
    //   testDir/CODE/CODE.json  or  testDir/definitions/CODE.json
    std::string scaleJsonPath;
    std::vector<std::string> candidates = {
        testDir + "/" + scaleCode + "/" + scaleCode + ".json",
        testDir + "/definitions/" + scaleCode + ".json"
    };
    for (const auto& path : candidates) {
        if (fs::exists(path)) {
            scaleJsonPath = path;
            break;
        }
    }

    // Also check the scale library (original source with full options)
    if (mScaleManager) {
        std::string libPath = mScaleManager->GetDefinitionPath(scaleCode);
        if (!libPath.empty() && fs::exists(libPath)) {
            // Prefer the library source — it has the original options
            scaleJsonPath = libPath;
        }
    }

    if (scaleJsonPath.empty()) {
        return false;  // Not a scale-based test
    }

    // Parse scale definition to extract parameters
    nlohmann::json scaleDef;
    try {
        std::ifstream scaleFile(scaleJsonPath);
        if (!scaleFile.is_open()) return false;
        scaleFile >> scaleDef;
        scaleFile.close();
    } catch (const std::exception& e) {
        printf("Error parsing scale JSON %s: %s\n", scaleJsonPath.c_str(), e.what());
        return false;
    }

    printf("Syncing schema from scale definition: %s\n", scaleJsonPath.c_str());

    // Build schema JSON from scale parameters
    nlohmann::json schemaJson = {
        {"test", scaleCode},
        {"version", "1.0"},
        {"description", scaleCode + " Scale"}
    };
    nlohmann::json schemaParams = nlohmann::json::array();

    // Always include the scale selector (fixed to this scale, hidden from UI)
    schemaParams.push_back({
        {"name",        "scale"},
        {"type",        "string"},
        {"default",     scaleCode},
        {"description", "OSD scale code (reads definitions/{code}.json)"},
        {"hidden",      true}
    });

    // Build par.json defaults
    nlohmann::json parDefaults = {{"scale", scaleCode}};

    // Extract each parameter from scale definition (if any)
    if (scaleDef.contains("parameters"))
    for (auto& [pName, pDef] : scaleDef["parameters"].items()) {
        nlohmann::json sp;
        sp["name"] = pName;

        std::string pType = "string";
        if (pDef.contains("type")) pType = pDef["type"].get<std::string>();
        sp["type"] = pType;

        // Default value
        if (pDef.contains("default")) {
            sp["default"] = pDef["default"];
            // Also set in par defaults
            parDefaults[pName] = pDef["default"];
        }

        if (pDef.contains("description")) {
            sp["description"] = pDef["description"].get<std::string>();
        }

        // Options
        if (pDef.contains("options") && pDef["options"].is_array()) {
            sp["options"] = pDef["options"];
        } else if (pType == "boolean") {
            sp["options"] = nlohmann::json::array({0, 1});
        }

        schemaParams.push_back(sp);
    }

    // Always include shuffle_questions as a standard ScaleRunner parameter
    if (!parDefaults.contains("shuffle_questions")) {
        schemaParams.push_back({
            {"name",        "shuffle_questions"},
            {"type",        "boolean"},
            {"default",     0},
            {"options",     nlohmann::json::array({0, 1})},
            {"description", "Randomize item order within randomization groups"}
        });
        parDefaults["shuffle_questions"] = 0;
    }

    // Always include show_header as a standard ScaleRunner parameter
    if (!parDefaults.contains("show_header")) {
        schemaParams.push_back({
            {"name",        "show_header"},
            {"type",        "boolean"},
            {"default",     1},
            {"options",     nlohmann::json::array({0, 1})},
            {"description", "Display the scale title header above the questionnaire"}
        });
        parDefaults["show_header"] = 1;
    }

    schemaJson["parameters"] = schemaParams;

    // Write schema file
    std::string schemaPath = testDir + "/params/" + scaleCode + ".pbl.schema.json";
    fs::create_directories(testDir + "/params");
    std::ofstream schemaFile(schemaPath);
    if (schemaFile.is_open()) {
        schemaFile << schemaJson.dump(2);
        schemaFile.close();
        printf("Updated schema: %s\n", schemaPath.c_str());
    }

    // Create or update par.json — add missing parameters without overwriting existing values
    std::string parPath = testDir + "/params/" + scaleCode + ".pbl.par.json";
    nlohmann::json existingParams;
    if (fs::exists(parPath)) {
        try {
            std::ifstream existingFile(parPath);
            if (existingFile.is_open()) {
                existingFile >> existingParams;
                existingFile.close();
            }
        } catch (...) {
            existingParams = nlohmann::json::object();
        }
    }

    // Merge: add defaults for any parameters not already in the file
    bool updated = false;
    for (auto& [key, val] : parDefaults.items()) {
        if (!existingParams.contains(key)) {
            existingParams[key] = val;
            updated = true;
        }
    }

    if (updated || !fs::exists(parPath)) {
        std::ofstream parFile(parPath);
        if (parFile.is_open()) {
            parFile << existingParams.dump(2);
            parFile.close();
            printf("%s params: %s\n", updated ? "Updated" : "Created", parPath.c_str());
        }
    }

    return true;
}


void LauncherUI::EditTestParameters(int testIndex)
{
    if (!mCurrentStudy) {
        printf("Error: No study loaded\n");
        return;
    }

    const auto& tests = mCurrentStudy->GetTests();
    if (testIndex < 0 || testIndex >= (int)tests.size()) {
        printf("Error: Invalid test index\n");
        return;
    }

    const Test& test = tests[testIndex];
    std::string studyPath = mCurrentStudy->GetPath();
    std::string testPath = studyPath + "/tests/" + test.testPath;

    // Sync schema from scale definition if this is a scale-based test
    SyncScaleSchema(testPath, test.testName);

    std::string schemaPath = testPath + "/params/" + test.testName + ".pbl.schema.json";

    // Check if schema file exists
    struct stat st;
    if (stat(schemaPath.c_str(), &st) != 0) {
        printf("Warning: No parameter schema found at %s\n", schemaPath.c_str());
        printf("Searched at: %s\n", schemaPath.c_str());
        printf("Test may not have configurable parameters, or test files may not have been copied to study.\n");
        return;
    }

    // Store test index for later use
    mEditingTestIndex = testIndex;

    // Scan for existing parameter variants in the params directory
    ScanParameterVariants(testIndex);

    // Load the default parameter set directly (skip variant dialog)
    mVariantName[0] = '\0';
    LoadParameterEditorForVariant();
}

void LauncherUI::ScanParameterVariants(int testIndex)
{
    if (!mCurrentStudy) return;

    const auto& tests = mCurrentStudy->GetTests();
    if (testIndex < 0 || testIndex >= (int)tests.size()) return;

    Test* test = mCurrentStudy->GetTest(tests[testIndex].testName);
    if (!test) return;

    std::string studyPath = mCurrentStudy->GetPath();
    std::string paramsDir = studyPath + "/tests/" + test->testPath + "/params";

    // Clear existing variants
    test->parameterVariants.clear();

    try {
        if (!fs::exists(paramsDir) || !fs::is_directory(paramsDir)) {
            printf("No params directory found at %s\n", paramsDir.c_str());
            return;
        }

        // Scan for .par.json files
        for (const auto& entry : fs::directory_iterator(paramsDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string filename = entry.path().filename().string();

                // Look for pattern: testname-variantname.par.json
                if (filename.find(".par.json") != std::string::npos) {
                    // Extract variant name
                    size_t dashPos = filename.find('-');
                    size_t parPos = filename.find(".par.json");

                    if (dashPos != std::string::npos && parPos != std::string::npos && dashPos < parPos) {
                        std::string variantName = filename.substr(dashPos + 1, parPos - dashPos - 1);

                        ParameterVariant variant;
                        variant.file = filename;
                        variant.description = "Parameter set: " + variantName;

                        test->parameterVariants[variantName] = variant;
                        printf("Found parameter variant: %s\n", variantName.c_str());
                    }
                }
            }
        }

        printf("Scanned %zu parameter variants for test %s\n",
               test->parameterVariants.size(), test->testName.c_str());

        // Save study to persist the scanned variants
        mCurrentStudy->Save();

    } catch (const fs::filesystem_error& e) {
        printf("Error scanning parameter variants: %s\n", e.what());
    }
}

// ============================================================================
// Chain Management Function Stubs
// ============================================================================

void LauncherUI::CreateNewChain()
{
    if (!mCurrentStudy) {
        printf("Error: No study loaded. Create or load a study first.\n");
        return;
    }

    mShowNewChainDialog = true;
}

void LauncherUI::LoadChain(const std::string& chainPath)
{
    printf("LoadChain(%s)\n", chainPath.c_str());

    mCurrentChain = Chain::LoadFromFile(chainPath);
    if (mCurrentChain) {
        printf("Chain loaded: %s\n", mCurrentChain->GetName().c_str());

        // Save selected chain to config (save just the filename, not full path)
        size_t lastSlash = chainPath.find_last_of("/\\");
        std::string chainName = (lastSlash != std::string::npos)
                                ? chainPath.substr(lastSlash + 1)
                                : chainPath;
        mConfig->SetCurrentChainName(chainName);
        mConfig->SaveConfig();
    } else {
        printf("Failed to load chain from: %s\n", chainPath.c_str());
    }
}

void LauncherUI::SaveCurrentChain()
{
    if (!mCurrentChain) {
        printf("Error: No chain loaded\n");
        return;
    }

    if (mCurrentChain->Save()) {
        printf("Chain saved: %s\n", mCurrentChain->GetName().c_str());
    } else {
        printf("Failed to save chain\n");
    }
}

void LauncherUI::AddInstructionPage()
{
    mPageEditor.show = true;
    mPageEditor.editingIndex = -1;
    mPageEditor.pageType = 0; // Instruction
    mPageEditor.title[0] = '\0';
    mPageEditor.content[0] = '\0';
}

void LauncherUI::AddConsentPage()
{
    mPageEditor.show = true;
    mPageEditor.editingIndex = -1;
    mPageEditor.pageType = 1; // Consent
    mPageEditor.title[0] = '\0';
    mPageEditor.content[0] = '\0';
}

void LauncherUI::AddCompletionPage()
{
    mPageEditor.show = true;
    mPageEditor.editingIndex = -1;
    mPageEditor.pageType = 2; // Completion
    mPageEditor.title[0] = '\0';
    mPageEditor.content[0] = '\0';
}

void LauncherUI::AddTestToChain()
{
    if (!mCurrentChain) {
        printf("Error: No chain loaded\n");
        return;
    }

    if (!mCurrentStudy) {
        printf("Error: No study loaded. Tests must come from a study.\n");
        return;
    }

    // Open test editor dialog for adding new test
    mTestEditor.show = true;
    mTestEditor.editingIndex = -1;  // -1 means adding new item
    mTestEditor.selectedTestIndex = -1;
    mTestEditor.selectedVariantIndex = 0;
    mTestEditor.language[0] = '\0';
}

void LauncherUI::RemoveChainItem(int index)
{
    if (!mCurrentChain) {
        printf("Error: No chain loaded\n");
        return;
    }

    mCurrentChain->RemoveItem(index);
    SaveCurrentChain();
    printf("Removed chain item at index: %d\n", index);
}

void LauncherUI::MoveChainItemUp(int index)
{
    if (!mCurrentChain) {
        printf("Error: No chain loaded\n");
        return;
    }

    if (index > 0) {
        mCurrentChain->MoveItem(index, index - 1);
        SaveCurrentChain();
        printf("Moved chain item up from index %d to %d\n", index, index - 1);
    }
}

void LauncherUI::MoveChainItemDown(int index)
{
    if (!mCurrentChain) {
        printf("Error: No chain loaded\n");
        return;
    }

    if (index < (int)mCurrentChain->GetItems().size() - 1) {
        // Swap with next item (MoveItem doesn't work correctly for moving forward)
        ChainItem* item1 = mCurrentChain->GetItem(index);
        ChainItem* item2 = mCurrentChain->GetItem(index + 1);
        if (item1 && item2) {
            ChainItem temp = *item1;
            *item1 = *item2;
            *item2 = temp;
            SaveCurrentChain();
            printf("Moved chain item down from index %d to %d\n", index, index + 1);
        }
    }
}

void LauncherUI::MoveChainItemTo(int from, int to)
{
    if (!mCurrentChain) return;
    int n = (int)mCurrentChain->GetItems().size();
    if (from == to || from < 0 || from >= n || to < 0 || to >= n) return;
    int step = (to > from) ? 1 : -1;
    for (int i = from; i != to; i += step) {
        ChainItem* a = mCurrentChain->GetItem(i);
        ChainItem* b = mCurrentChain->GetItem(i + step);
        if (a && b) std::swap(*a, *b);
    }
    SaveCurrentChain();
}

void LauncherUI::EditChainItem(int index)
{
    if (!mCurrentChain) {
        printf("Error: No chain loaded\n");
        return;
    }

    const auto& items = mCurrentChain->GetItems();
    if (index < 0 || index >= (int)items.size()) {
        printf("Error: Invalid chain item index: %d\n", index);
        return;
    }

    const ChainItem& item = items[index];

    // Only page items can be edited in the page editor
    if (item.type == ItemType::Instruction ||
        item.type == ItemType::Consent ||
        item.type == ItemType::Completion) {

        mPageEditor.show = true;
        mPageEditor.editingIndex = index;

        if (item.type == ItemType::Instruction) {
            mPageEditor.pageType = 0;
        } else if (item.type == ItemType::Consent) {
            mPageEditor.pageType = 1;
        } else {
            mPageEditor.pageType = 2;
        }

        std::strncpy(mPageEditor.title, item.title.c_str(), sizeof(mPageEditor.title) - 1);
        std::strncpy(mPageEditor.content, item.content.c_str(), sizeof(mPageEditor.content) - 1);
    } else if (item.type == ItemType::Test) {
        // Test items - show test editor
        if (!mCurrentStudy) {
            printf("Error: No study loaded - cannot edit test item\n");
            return;
        }

        // Find test in study's test list
        const auto& tests = mCurrentStudy->GetTests();
        int testIndex = -1;
        for (size_t i = 0; i < tests.size(); i++) {
            if (tests[i].testName == item.testName) {
                testIndex = i;
                break;
            }
        }

        if (testIndex < 0) {
            printf("Warning: Test '%s' not found in study\n", item.testName.c_str());
            // Still allow editing, but select first test as fallback
            testIndex = 0;
        }

        // Find parameter variant index
        int variantIndex = 0;  // Default to "default" variant
        if (!item.paramVariant.empty() && item.paramVariant != "default") {
            const Test& test = tests[testIndex];
            int idx = 1;  // Start at 1 since 0 is "default"
            for (const auto& [name, variant] : test.parameterVariants) {
                if (name == item.paramVariant) {
                    variantIndex = idx;
                    break;
                }
                idx++;
            }
        }

        // Set up test editor state
        mTestEditor.show = true;
        mTestEditor.editingIndex = index;
        mTestEditor.selectedTestIndex = testIndex;
        mTestEditor.selectedVariantIndex = variantIndex;
        std::strncpy(mTestEditor.language, item.language.c_str(), sizeof(mTestEditor.language) - 1);
        mTestEditor.language[sizeof(mTestEditor.language) - 1] = '\0';
        mTestEditor.randomGroup = item.randomGroup;
    }
}

// ============================================================================
// New Study-Centric UI Implementation
// ============================================================================

void LauncherUI::RenderStudyBar()
{
    ImGui::Spacing();

    // Study selector and control buttons
    ImGui::Text("Study:");
    ImGui::SameLine();

    const char* currentStudyName = mCurrentStudy ? mCurrentStudy->GetName().c_str() : "No study loaded";
    ImGui::PushItemWidth(300);
    if (ImGui::BeginCombo("##StudySelect", currentStudyName)) {
        // List studies from workspace
        auto studyDirs = mWorkspace->GetStudyDirectories();
        for (size_t i = 0; i < studyDirs.size(); i++) {
            // Extract study name from path
            std::string studyName = fs::path(studyDirs[i]).filename().string();
            bool is_selected = (mCurrentStudy && mCurrentStudy->GetName() == studyName);
            if (ImGui::Selectable(studyName.c_str(), is_selected)) {
                LoadStudy(studyDirs[i]);
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();

    ImGui::SameLine();
    if (ImGui::Button("New Study")) {
        mShowNewStudyDialog = true;
    }

    ImGui::SameLine();
    if (mCurrentStudy) {
        if (ImGui::Button("Open Directory")) {
            std::string studyPath = mCurrentStudy->GetPath();
            OpenDirectoryInFileBrowser(studyPath);
        }

        ImGui::SameLine();
        if (ImGui::Button("Study Settings")) {
            mShowStudySettingsDialog = true;
        }
    }

    // Show study info if loaded
    if (mCurrentStudy) {
        ImGui::SameLine();
        ImGui::TextDisabled("| %s | %zu tests | %d chains",
                           mCurrentStudy->GetName().c_str(),
                           mCurrentStudy->GetTests().size(),
                           mCurrentStudy->GetChainCount());
    }

    ImGui::Separator();
    ImGui::Spacing();  // Add spacing after separator
}

void LauncherUI::RenderTestsTab()
{
    if (!mCurrentStudy) {
        // No study loaded - show full-width battery browser for exploration
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f));
        ImGui::TextWrapped("No study loaded. Browse available tests below, then create or open a study to add them.");
        ImGui::PopStyleColor();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Show battery browser full-width (without "Add to Study" button)
        RenderBatteryBrowser();
        return;
    }

    // Two-panel layout: Tests in Study (left) | Add Test to Study (right)
    float panelWidth = ImGui::GetContentRegionAvail().x * 0.35f;

    // Left panel: Tests in Study
    ImGui::BeginChild("TestsInStudy", ImVec2(panelWidth, 0), true);
    RenderTestsInStudy();
    ImGui::EndChild();

    ImGui::SameLine();

    // Right panel: Add Test to Study
    ImGui::BeginChild("AddTestPanel", ImVec2(0, 0), true);
    RenderAddTestPanel();
    ImGui::EndChild();
}

void LauncherUI::RenderTestsInStudy()
{
    ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Tests in Study");
    ImGui::Separator();
    ImGui::Spacing();

    const auto& tests = mCurrentStudy->GetTests();

    if (tests.empty()) {
        ImGui::TextDisabled("No tests in this study yet.\nUse the panel on the right to add tests.");
        return;
    }

    // Scrollable list of tests - reserve space for preview below when a test is selected
    float listHeight = (mSelectedStudyTestIndex >= 0) ? ImGui::GetContentRegionAvail().y * 0.4f : -1;
    ImGui::BeginChild("TestList", ImVec2(0, listHeight), false);

    for (size_t i = 0; i < tests.size(); i++) {
        ImGui::PushID((int)i);

        ImGui::Text("%zu.", i + 1);
        ImGui::SameLine();

        // Calculate available width for test name (leaving 50px for menu button on the right)
        float availableWidth = ImGui::GetContentRegionAvail().x - 50;
        std::string testName = tests[i].testName;

        // Add variant count if present
        if (!tests[i].parameterVariants.empty()) {
            testName += " (" + std::to_string(tests[i].parameterVariants.size()) + " variants)";
        }

        // Truncate if too long
        std::string displayName = testName;
        if (ImGui::CalcTextSize(displayName.c_str()).x > availableWidth) {
            while (displayName.length() > 3 && ImGui::CalcTextSize((displayName + "...").c_str()).x > availableWidth) {
                displayName.pop_back();
            }
            displayName += "...";
        }

        // Make test name clickable to show preview
        bool is_selected = (mSelectedStudyTestIndex == (int)i);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.7f, 1.0f, 1.0f));  // Blue color
        if (ImGui::Selectable(displayName.c_str(), is_selected, ImGuiSelectableFlags_None, ImVec2(availableWidth, 0))) {
            mSelectedStudyTestIndex = (int)i;
            LoadStudyTestPreview((int)i);
        }
        ImGui::PopStyleColor();
        if (ImGui::IsItemHovered()) {
            if (displayName != testName) {
                ImGui::SetTooltip("%s\nClick to view test details", testName.c_str());
            } else {
                ImGui::SetTooltip("Click to view test details");
            }
        }

        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 50);

        // Menu button with all options
        if (ImGui::SmallButton("...")) {
            ImGui::OpenPopup("TestMenu");
        }

        if (ImGui::BeginPopup("TestMenu")) {
            std::string studyPath = mCurrentStudy->GetPath();
            fs::path testPath = fs::path(studyPath) / "tests" / tests[i].testPath;
            // Extract basename from test_name for .pbl filename
            std::string baseName = fs::path(tests[i].testName).filename().string();
            std::string pblFile = (testPath / (baseName + ".pbl")).string();

            // Quick Launch
            if (ImGui::MenuItem("Quick Launch")) {
                // Open Quick Launch tab with this test selected
                std::ifstream file(pblFile);
                if (file.is_open()) {
                    file.close();

                    // Set Quick Launch path to this test
                    std::strncpy(mQuickLaunchPath, pblFile.c_str(), sizeof(mQuickLaunchPath) - 1);
                    mQuickLaunchPath[sizeof(mQuickLaunchPath) - 1] = '\0';

                    // Update Quick Launch directory to parent of selected file
                    mQuickLaunchDirectory = fs::path(pblFile).parent_path().string();

                    // Switch to Quick Launch tab
                    mTopLevelTab = 1;

                    printf("Switched to Quick Launch with test: %s\n", baseName.c_str());
                } else {
                    printf("Error: Could not find test file: %s\n", pblFile.c_str());
                }
            }

            ImGui::Separator();

            // Edit code
            if (ImGui::MenuItem("Edit Code")) {
                std::ifstream file(pblFile);
                if (file.is_open()) {
                    std::stringstream buffer;
                    buffer << file.rdbuf();
                    file.close();

                    mCodeEditorFilePath = pblFile;
                    mCodeEditor.SetText(buffer.str());
                    mShowCodeEditor = true;
                } else {
                    printf("Error: Could not open file for editing: %s\n", pblFile.c_str());
                }
            }

            // Edit parameters
            if (ImGui::MenuItem("Edit Parameters...")) {
                EditTestParameters(i);
            }

            // Edit translations
            if (ImGui::MenuItem("Edit Translations...")) {
                // Open translation editor dialog
                mTranslationEditor.testIndex = i;
                std::string testPathStr = testPath.string();
                std::strncpy(mTranslationEditor.testPath, testPathStr.c_str(), sizeof(mTranslationEditor.testPath) - 1);
                mTranslationEditor.testPath[sizeof(mTranslationEditor.testPath) - 1] = '\0';
                mTranslationEditor.language[0] = '\0';  // Start with no language selected
                mTranslationEditor.show = true;
            }

            ImGui::Separator();

            // Open test directory
            if (ImGui::MenuItem("Open Test Directory")) {
                OpenDirectoryInFileBrowser(testPath.string());
            }

            // Combine data
            if (ImGui::MenuItem("Combine Data Files...")) {
                std::string dataPath = (testPath / "data").string();

                // Create data directory if it doesn't exist
                if (!fs::exists(dataPath)) {
                    fs::create_directories(dataPath);
                }

                LaunchDataCombiner(dataPath);
            }

            ImGui::Separator();

            // Remove test
            if (ImGui::MenuItem("Remove from Study")) {
                const std::string testName = tests[i].testName;
                RemoveTestFromStudy(testName);
                ImGui::EndPopup();
                ImGui::PopID();
                break;
            }

            ImGui::EndPopup();
        }

        ImGui::Spacing();
        ImGui::PopID();
    }

    ImGui::EndChild();

    // Preview section for selected study test
    if (mSelectedStudyTestIndex >= 0 && mSelectedStudyTestIndex < (int)tests.size()) {
        ImGui::Separator();
        ImGui::Spacing();

        // Screenshot
        if (mStudyTestScreenshot) {
            float aspectRatio = (float)mStudyTestScreenshotH / (float)mStudyTestScreenshotW;
            float displayWidth = ImGui::GetContentRegionAvail().x;
            float displayHeight = displayWidth * aspectRatio;

            if (displayHeight > 300) {
                displayHeight = 300;
                displayWidth = displayHeight / aspectRatio;
            }

            ImGui::Image((ImTextureID)(intptr_t)mStudyTestScreenshot,
                        ImVec2(displayWidth, displayHeight));
            ImGui::Spacing();
        }

        // Description
        if (!mStudyTestDescription.empty()) {
            ImGui::TextWrapped("%s", mStudyTestDescription.c_str());
        }
    }
}

void LauncherUI::RenderAddTestPanel()
{
    ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Add Test to Study");
    ImGui::Separator();
    ImGui::Spacing();

    // Four sub-tabs: Battery, Scale, File, New
    if (ImGui::BeginTabBar("AddTestTabs")) {
        if (ImGui::BeginTabItem("Battery")) {
            mAddTestSubTab = 0;
            RenderBatteryBrowser();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Scale")) {
            mAddTestSubTab = 1;
            RenderScaleBrowser();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("File")) {
            mAddTestSubTab = 2;
            RenderFileImport();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("New")) {
            mAddTestSubTab = 3;
            RenderNewTestTemplate();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

void LauncherUI::RenderBatteryBrowser()
{
    // This is essentially the old file panel + details panel combined
    // Filter box
    static char filter[256] = "";
    ImGui::PushItemWidth(-1);
    ImGui::InputTextWithHint("##Filter", "Filter tests...", filter, sizeof(filter));
    ImGui::PopItemWidth();

    ImGui::Spacing();

    // Split into test list (left) and details (right)
    float listWidth = ImGui::GetContentRegionAvail().x * 0.4f;

    ImGui::BeginChild("BatteryTestList", ImVec2(listWidth, 0), true);

    ImGui::Text("Battery Tests (%zu found):", mExperiments.size());
    ImGui::Separator();

    // Scrollable test list
    for (int i = 0; i < (int)mExperiments.size(); i++) {
        const ExperimentInfo& exp = mExperiments[i];

        // Apply filter
        if (strlen(filter) > 0 &&
            exp.name.find(filter) == std::string::npos) {
            continue;
        }

        bool is_selected = (mSelectedExperiment == i);
        if (ImGui::Selectable(exp.name.c_str(), is_selected)) {
            mSelectedExperiment = i;
            LoadExperimentInfo(exp.path);
        }

        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", exp.path.c_str());
        }
    }

    // Keyboard navigation - detect arrow keys and load details
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
        int newSelection = mSelectedExperiment;

        if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {
            // Find next visible item after current selection
            bool foundCurrent = (mSelectedExperiment < 0);
            for (int i = 0; i < (int)mExperiments.size(); i++) {
                if (strlen(filter) > 0 && mExperiments[i].name.find(filter) == std::string::npos) {
                    continue;
                }
                if (foundCurrent) {
                    newSelection = i;
                    break;
                }
                if (i == mSelectedExperiment) {
                    foundCurrent = true;
                }
            }
        } else if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {
            // Find previous visible item before current selection
            for (int i = (int)mExperiments.size() - 1; i >= 0; i--) {
                if (strlen(filter) > 0 && mExperiments[i].name.find(filter) == std::string::npos) {
                    continue;
                }
                if (i < mSelectedExperiment) {
                    newSelection = i;
                    break;
                }
            }
        }

        // If selection changed via keyboard, load the details
        if (newSelection != mSelectedExperiment && newSelection >= 0 && newSelection < (int)mExperiments.size()) {
            mSelectedExperiment = newSelection;
            LoadExperimentInfo(mExperiments[newSelection].path);
        }
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // Right side: Test details
    ImGui::BeginChild("BatteryTestDetails", ImVec2(0, 0), true);

    if (mSelectedExperiment < 0 || mSelectedExperiment >= (int)mExperiments.size()) {
        ImGui::TextDisabled("Select a test to view details");
    } else {
        const ExperimentInfo& exp = mExperiments[mSelectedExperiment];

        // Test name
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "%s", exp.name.c_str());
        ImGui::Separator();
        ImGui::Spacing();

        // Add to Study button (only available when study is loaded)
        if (mCurrentStudy) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));

            if (ImGui::Button("Add to Study", ImVec2(-1, 40))) {
                AddTestToStudy();
            }

            ImGui::PopStyleColor(3);
        } else {
            // No study loaded - show disabled button with tooltip
            ImGui::BeginDisabled();
            ImGui::Button("Create or Open a Study First", ImVec2(-1, 40));
            ImGui::EndDisabled();
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("Create or open a study to add tests");
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Screenshot
        if (mScreenshotTexture) {
            float aspectRatio = (float)mScreenshotHeight / (float)mScreenshotWidth;
            float displayWidth = ImGui::GetContentRegionAvail().x;
            float displayHeight = displayWidth * aspectRatio;

            // Cap height at a reasonable max (e.g., 400px) but let it fill width
            if (displayHeight > 400) {
                displayHeight = 400;
                displayWidth = displayHeight / aspectRatio;
            }

            ImGui::Image((ImTextureID)(intptr_t)mScreenshotTexture,
                        ImVec2(displayWidth, displayHeight));
            ImGui::Spacing();
        }

        // Description
        if (!exp.description.empty()) {
            ImGui::TextWrapped("%s", exp.description.c_str());
        } else {
            ImGui::TextDisabled("No description available");
        }

        ImGui::Spacing();

        // Info badges
        if (exp.hasParams) {
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "✓ Parameters");
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("This test has configurable parameters");
            }
            ImGui::SameLine();
        }
        if (exp.hasTranslations) {
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "✓ Translations");
            if (ImGui::IsItemHovered()) {
                if (!mAvailableLanguages.empty()) {
                    std::string tooltip = "Available languages: ";
                    for (size_t i = 0; i < mAvailableLanguages.size(); i++) {
                        tooltip += mAvailableLanguages[i];
                        if (i < mAvailableLanguages.size() - 1) {
                            tooltip += ", ";
                        }
                    }
                    ImGui::SetTooltip("%s", tooltip.c_str());
                } else {
                    ImGui::SetTooltip("This test has translation support");
                }
            }
        }
    }

    ImGui::EndChild();
}

void LauncherUI::RenderScaleBrowser()
{
    // Filter box
    static char filter[256] = "";
    ImGui::PushItemWidth(-1);
    ImGui::InputTextWithHint("##ScaleFilter", "Filter scales...", filter, sizeof(filter));
    ImGui::PopItemWidth();

    ImGui::Spacing();

    // Ensure scale list is loaded
    if (mScaleList.empty()) {
        mScaleList = mScaleManager->GetAvailableScales();
    }

    // Split into scale list (left) and details (right)
    float listWidth = ImGui::GetContentRegionAvail().x * 0.4f;

    ImGui::BeginChild("ScaleList", ImVec2(listWidth, 0), true);

    ImGui::Text("Available Scales (%zu found):", mScaleList.size());
    ImGui::Separator();

    // Scrollable scale list
    for (int i = 0; i < (int)mScaleList.size(); i++) {
        const std::string& scaleName = mScaleList[i];

        // Apply filter
        if (strlen(filter) > 0 &&
            scaleName.find(filter) == std::string::npos) {
            continue;
        }

        bool is_selected = (mSelectedScaleIndex == i);
        if (ImGui::Selectable(scaleName.c_str(), is_selected)) {
            mSelectedScaleIndex = i;
        }
    }

    // Keyboard navigation
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
        int newSelection = mSelectedScaleIndex;

        if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {
            // Find next visible item after current selection
            bool foundCurrent = (mSelectedScaleIndex < 0);
            for (int i = 0; i < (int)mScaleList.size(); i++) {
                if (strlen(filter) > 0 && mScaleList[i].find(filter) == std::string::npos) {
                    continue;
                }
                if (foundCurrent) {
                    newSelection = i;
                    break;
                }
                if (i == mSelectedScaleIndex) {
                    foundCurrent = true;
                }
            }
        } else if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {
            // Find previous visible item before current selection
            for (int i = (int)mScaleList.size() - 1; i >= 0; i--) {
                if (strlen(filter) > 0 && mScaleList[i].find(filter) == std::string::npos) {
                    continue;
                }
                if (i < mSelectedScaleIndex) {
                    newSelection = i;
                    break;
                }
            }
        }

        // Update selection if changed
        if (newSelection != mSelectedScaleIndex && newSelection >= 0 && newSelection < (int)mScaleList.size()) {
            mSelectedScaleIndex = newSelection;
        }
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // Right side: Scale details
    ImGui::BeginChild("ScaleDetails", ImVec2(0, 0), true);

    if (mSelectedScaleIndex < 0 || mSelectedScaleIndex >= (int)mScaleList.size()) {
        ImGui::TextDisabled("Select a scale to view details");
    } else {
        const std::string& scaleCode = mScaleList[mSelectedScaleIndex];
        auto metadata = mScaleManager->GetScaleMetadata(scaleCode);

        // Scale name
        if (!metadata.name.empty()) {
            ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "%s", metadata.name.c_str());
        } else {
            ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "%s", scaleCode.c_str());
        }
        ImGui::Separator();
        ImGui::Spacing();

        // Load screenshot if selection changed
        if (mSelectedScaleIndex != mScaleBrowserScreenshotForIndex) {
            if (mScaleBrowserScreenshot) {
                SDL_DestroyTexture(mScaleBrowserScreenshot);
                mScaleBrowserScreenshot = nullptr;
                mScaleBrowserScreenshotW = 0;
                mScaleBrowserScreenshotH = 0;
            }

            // Get definition path and derive parent directory
            std::string defPath = mScaleManager->GetDefinitionPath(scaleCode);
            fs::path defDir = fs::path(defPath).parent_path();
            std::string screenshotPath = (defDir / (scaleCode + ".pbl.png")).string();

            if (fs::exists(screenshotPath)) {
                SDL_Surface* surface = IMG_Load(screenshotPath.c_str());
                if (surface) {
                    mScaleBrowserScreenshot = SDL_CreateTextureFromSurface(mRenderer, surface);
                    if (mScaleBrowserScreenshot) {
                        mScaleBrowserScreenshotW = surface->w;
                        mScaleBrowserScreenshotH = surface->h;
                    }
                    SDL_FreeSurface(surface);
                }
            }

            mScaleBrowserScreenshotForIndex = mSelectedScaleIndex;
        }

        // Display screenshot
        if (mScaleBrowserScreenshot) {
            float aspectRatio = (float)mScaleBrowserScreenshotH / (float)mScaleBrowserScreenshotW;
            float displayWidth = ImGui::GetContentRegionAvail().x;
            float displayHeight = displayWidth * aspectRatio;

            if (displayHeight > 300.0f) {
                displayHeight = 300.0f;
                displayWidth = displayHeight / aspectRatio;
            }

            ImGui::Image((ImTextureID)(intptr_t)mScaleBrowserScreenshot,
                        ImVec2(displayWidth, displayHeight));
            ImGui::Spacing();
        }

        // Add to Study button (only available when study is loaded)
        if (mCurrentStudy) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));

            if (ImGui::Button("Add to Study", ImVec2(-1, 40))) {
                // Load the scale and add it to the study
                auto scale = mScaleManager->LoadScale(scaleCode);
                if (!scale) {
                    printf("Failed to load scale '%s'\n", scaleCode.c_str());
                } else {
                    std::string studyPath = mCurrentStudy->GetPath();
                    std::string testDir = studyPath + "/tests/" + scaleCode;

                    try {
                        // Create test directory structure
                        fs::create_directories(testDir);
                        fs::create_directories(testDir + "/definitions");
                        fs::create_directories(testDir + "/translations");
                        fs::create_directories(testDir + "/params");

                        // Copy ScaleRunner.pbl and rename to scalecode.pbl
                        std::string scaleRunnerSource = mBatteryPath + "/../media/apps/scales/ScaleRunner.pbl";
                        std::string scaleRunnerDest = testDir + "/" + scaleCode + ".pbl";

                        if (!fs::exists(scaleRunnerSource)) {
                            printf("Error: ScaleRunner.pbl not found at: %s\n", scaleRunnerSource.c_str());
                        } else {
                            fs::copy_file(scaleRunnerSource, scaleRunnerDest, fs::copy_options::overwrite_existing);
                            printf("Copied ScaleRunner.pbl to %s\n", scaleRunnerDest.c_str());

                            // Export scale definition and translations
                            std::string destDefPath = testDir + "/definitions";
                            std::string destTransPath = testDir + "/translations";

                            if (!scale->ExportToJSON(destDefPath, destTransPath)) {
                                printf("Error: Failed to export scale files\n");
                            } else {
                                printf("Exported scale definition and translations\n");

                                // Generate schema and default params from scale definition
                                SyncScaleSchema(testDir, scaleCode);

                                // Add test to study
                                Test test;
                                test.testName = scaleCode;
                                test.displayName = metadata.name.empty() ? scaleCode : metadata.name;
                                test.testPath = scaleCode;
                                test.included = true;

                                mCurrentStudy->AddTest(test);
                                mCurrentStudy->Save();

                                // Add to Main chain if it exists
                                std::string mainChainPath = studyPath + "/chains/Main.json";
                                if (fs::exists(mainChainPath)) {
                                    ChainItem item(ItemType::Test);
                                    item.testName = scaleCode;
                                    item.paramVariant = "default";
                                    item.language = "en";
                                    item.randomGroup = 0;

                                    if (mCurrentChain && mCurrentChain->GetFilePath() == mainChainPath) {
                                        mCurrentChain->AddItem(item);
                                        mCurrentChain->Save();
                                        printf("Added test to Main chain (current chain)\n");
                                    } else {
                                        auto mainChain = Chain::LoadFromFile(mainChainPath);
                                        if (mainChain) {
                                            mainChain->AddItem(item);
                                            mainChain->Save();
                                            printf("Added test to Main chain\n");
                                        }
                                    }
                                }

                                printf("Added scale '%s' to study\n", scaleCode.c_str());
                            }
                        }
                    } catch (const fs::filesystem_error& e) {
                        printf("Error adding scale to study: %s\n", e.what());
                    }
                }
            }

            ImGui::PopStyleColor(3);
        } else {
            // No study loaded - show disabled button with tooltip
            ImGui::BeginDisabled();
            ImGui::Button("Create or Open a Study First", ImVec2(-1, 40));
            ImGui::EndDisabled();
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("Create or open a study to add scales");
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Scale description
        if (!metadata.description.empty()) {
            ImGui::TextWrapped("%s", metadata.description.c_str());
            ImGui::Spacing();
        }

        // Scale info
        ImGui::Text("Code: %s", scaleCode.c_str());

        if (!metadata.author.empty()) {
            ImGui::Text("Author: %s", metadata.author.c_str());
        }

        ImGui::Text("Questions: %d", metadata.questionCount);

        if (!metadata.availableLanguages.empty()) {
            ImGui::Text("Languages: ");
            ImGui::SameLine();
            for (size_t i = 0; i < metadata.availableLanguages.size(); i++) {
                ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "%s", metadata.availableLanguages[i].c_str());
                if (i < metadata.availableLanguages.size() - 1) {
                    ImGui::SameLine();
                    ImGui::Text(",");
                    ImGui::SameLine();
                }
            }
        }
    }

    ImGui::EndChild();
}

void LauncherUI::RenderFileImport()
{
    ImGui::TextWrapped("Import a test from a .pbl file on your computer.");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    static char filePath[512] = "";
    ImGui::Text("Select .pbl file:");
    ImGui::PushItemWidth(-100);
    ImGui::InputText("##FilePath", filePath, sizeof(filePath));
    ImGui::PopItemWidth();

    ImGui::SameLine();
    if (ImGui::Button("Browse...")) {
        std::string selected = OpenFileDialog("Select PEBL Test", "*.pbl");
        if (!selected.empty()) {
            std::strncpy(filePath, selected.c_str(), sizeof(filePath) - 1);
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (strlen(filePath) > 0 && fs::exists(filePath)) {
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "File found: %s", filePath);

        ImGui::Spacing();

        if (ImGui::Button("Add to Study", ImVec2(200, 40))) {
            AddTestFromFile(filePath);
            filePath[0] = '\0';  // Clear the input after adding
        }
    } else if (strlen(filePath) > 0) {
        ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "File not found");
    }
}

void LauncherUI::RenderNewTestTemplate()
{
    ImGui::TextWrapped("Create a new test from a template.");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Generic Study Template option (complete directory structure)
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 0.4f, 1.0f), "Complete Study Template:");
    ImGui::TextWrapped("Creates a complete test with params/, translations/, and example parameter files.");
    ImGui::Spacing();

    static char genericTestName[128] = "";
    ImGui::Text("Test Name:");
    ImGui::InputText("##GenericTestName", genericTestName, sizeof(genericTestName));

    if (strlen(genericTestName) > 0) {
        if (ImGui::Button("Create from Generic Study Template", ImVec2(300, 40))) {
            CreateTestFromGenericStudy(genericTestName);
            genericTestName[0] = '\0';  // Clear the input after creating
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Individual test templates (simple .pbl files)
    ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Simple Test Templates:");
    ImGui::TextWrapped("Creates a single .pbl file from a template.");
    ImGui::Spacing();

    if (mTemplateNames.empty()) {
        ImGui::TextColored(ImVec4(0.8f, 0.4f, 0.2f, 1.0f),
                          "No templates found. Check media/templates/ directory.");
        return;
    }

    static int selectedTemplate = 0;

    // Convert vector<string> to vector<const char*> for ImGui::Combo
    std::vector<const char*> templateCStrings;
    for (const auto& name : mTemplateNames) {
        templateCStrings.push_back(name.c_str());
    }

    ImGui::Text("Template:");
    ImGui::Combo("##Template", &selectedTemplate, templateCStrings.data(), (int)templateCStrings.size());

    ImGui::Spacing();

    static char testName[128] = "";
    ImGui::Text("Test Name:");
    ImGui::InputText("##TestName", testName, sizeof(testName));

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (strlen(testName) > 0) {
        if (ImGui::Button("Create Test", ImVec2(200, 40))) {
            CreateTestFromTemplate(testName, selectedTemplate);
            testName[0] = '\0';  // Clear the input after creating
        }
    }
}

void LauncherUI::RenderChainsTab()
{
    // Use existing RenderChainTab implementation but require study
    if (!mCurrentStudy) {
        ImGui::TextWrapped("No study loaded. Chains are associated with studies.");
        return;
    }

    RenderChainTab();
}

void LauncherUI::RenderRunTab()
{
    if (!mCurrentStudy) {
        ImGui::TextWrapped("No study loaded. Load or create a study to run tests.");
        return;
    }
    ImGui::Separator();
    ImGui::Spacing();

    // Participant code - two-part editable field
    if (mCurrentStudy && mCurrentChain) {
        // Initialize study code if not set
        if (mStudyCode[0] == '\0') {
            std::string studyCode = mCurrentStudy->GetStudyCode();
            std::strncpy(mStudyCode, studyCode.c_str(), sizeof(mStudyCode) - 1);
            mStudyCode[sizeof(mStudyCode) - 1] = '\0';
        }

        // Get current counter
        int counter = mCurrentChain->GetParticipantCounter();
        char counterStr[16];
        snprintf(counterStr, sizeof(counterStr), "%d", counter);

        ImGui::Text("Participant Code:");
        ImGui::SameLine();

        // Study code prefix (editable)
        ImGui::PushItemWidth(80);
        if (ImGui::InputText("##StudyCodePrefix", mStudyCode, sizeof(mStudyCode))) {
            // Study code edited - no need to save yet, just updates the display
        }
        ImGui::PopItemWidth();

        ImGui::SameLine();
        ImGui::Text("_");
        ImGui::SameLine();

        // Counter number (editable with validation)
        ImGui::PushItemWidth(80);
        static char counterBuffer[16] = "";
        static bool counterBufferInitialized = false;

        // Initialize counter buffer on first render or when chain changes
        if (!counterBufferInitialized || strcmp(counterBuffer, counterStr) != 0) {
            strncpy(counterBuffer, counterStr, sizeof(counterBuffer) - 1);
            counterBuffer[sizeof(counterBuffer) - 1] = '\0';
            counterBufferInitialized = true;
        }

        if (ImGui::InputText("##CounterNumber", counterBuffer, sizeof(counterBuffer), ImGuiInputTextFlags_CharsDecimal)) {
            // Validate and update counter
            if (strlen(counterBuffer) > 0) {
                int newCounter = atoi(counterBuffer);
                if (newCounter < 1) newCounter = 1;
                mCurrentChain->SetParticipantCounter(newCounter);
                mCurrentChain->Save();
                // Update buffer to reflect validated value
                snprintf(counterBuffer, sizeof(counterBuffer), "%d", newCounter);
            }
        }
        ImGui::PopItemWidth();

        ImGui::SameLine();
        ImGui::Text("=");
        ImGui::SameLine();

        // Combined result (read-only display)
        std::string participantCode = std::string(mStudyCode) + "_" + counterBuffer;
        ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "%s", participantCode.c_str());

        // Update internal fields for compatibility
        std::strncpy(mParticipantCode, participantCode.c_str(), sizeof(mParticipantCode) - 1);
        mParticipantCode[sizeof(mParticipantCode) - 1] = '\0';
        std::strncpy(mSubjectCode, mParticipantCode, sizeof(mSubjectCode) - 1);
        mSubjectCode[sizeof(mSubjectCode) - 1] = '\0';
    } else {
        // Fallback: no study/chain loaded
        ImGui::Text("Participant Code:");
        ImGui::SameLine();
        ImGui::PushItemWidth(200);
        ImGui::InputText("##ParticipantCodeFallback", mSubjectCode, sizeof(mSubjectCode));
        ImGui::PopItemWidth();
    }

    // Check if subject code already exists and show warning (cached to avoid scanning every frame)
    if (mCurrentChain && strlen(mSubjectCode) > 0) {
        static std::vector<std::string> cachedExistingCodes;
        static std::string lastStudyPath;
        static std::string lastChainName;
        static bool cacheInitialized = false;

        // Refresh cache only when study/chain changes
        std::string currentStudyPath = mCurrentStudy ? mCurrentStudy->GetPath() : "";
        std::string currentChainName = mCurrentChain ? mCurrentChain->GetName() : "";

        if (!cacheInitialized || lastStudyPath != currentStudyPath || lastChainName != currentChainName) {
            cachedExistingCodes = CheckExistingSubjectCodes();
            lastStudyPath = currentStudyPath;
            lastChainName = currentChainName;
            cacheInitialized = true;
        }

        std::string currentCode(mSubjectCode);
        bool codeExists = false;
        for (const auto& code : cachedExistingCodes) {
            if (code == currentCode) {
                codeExists = true;
                break;
            }
        }

        if (codeExists) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.0f, 1.0f), "⚠ Code already used!");
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("This subject code has already been used in this study.\nData may be overwritten!");
            }
        }

        // Show existing codes if any
        if (!cachedExistingCodes.empty()) {
            ImGui::Indent(20);
            ImGui::TextDisabled("Existing codes:");
            ImGui::SameLine();
            std::string codesList;
            for (size_t i = 0; i < cachedExistingCodes.size() && i < 10; i++) {
                if (i > 0) codesList += ", ";
                codesList += cachedExistingCodes[i];
            }
            if (cachedExistingCodes.size() > 10) {
                codesList += "... (" + std::to_string(cachedExistingCodes.size()) + " total)";
            }
            ImGui::TextDisabled("%s", codesList.c_str());
            ImGui::Unindent(20);
        }
    }

    ImGui::Spacing();

    // Two-column layout for settings
    float columnWidth = ImGui::GetContentRegionAvail().x * 0.5f;

    // Left column
    ImGui::BeginChild("SettingsLeft", ImVec2(columnWidth - 5, 85), false);

    // Language
    ImGui::Text("Language:");
    ImGui::SameLine();
    ImGui::PushItemWidth(60);
    ImGui::InputText("##Language", mLanguageCode, sizeof(mLanguageCode));
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::TextDisabled("(en, es, de, fr...)");

    // Fullscreen
    ImGui::Checkbox("Fullscreen Mode", &mFullscreen);

    // VSync
    ImGui::Checkbox("Enable VSync", &mVSync);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Synchronize with monitor refresh rate");
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // Right column
    ImGui::BeginChild("SettingsRight", ImVec2(0, 85), false);

    // Screen Resolution
    ImGui::Text("Resolution:");
    ImGui::SameLine();
    ImGui::PushItemWidth(150);
    const char* resolutions[] = {
        "Auto (Current)",
        "1920x1080 (Full HD)",
        "1680x1050",
        "1440x900",
        "1366x768",
        "1280x1024",
        "1280x800",
        "1280x720 (HD)",
        "1024x768",
        "800x600"
    };
    if (ImGui::BeginCombo("##Resolution", resolutions[mScreenResolution])) {
        for (int i = 0; i < 10; i++) {
            bool is_selected = (mScreenResolution == i);
            if (ImGui::Selectable(resolutions[i], is_selected)) {
                mScreenResolution = i;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();

    // Advanced Options (compact)
    if (ImGui::TreeNode("Advanced")) {
        ImGui::Text("Driver:");
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::InputText("##Driver", mGraphicsDriver, sizeof(mGraphicsDriver));
        ImGui::PopItemWidth();

        ImGui::Text("Args:");
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::InputText("##CustomArgs", mCustomArguments, sizeof(mCustomArguments));
        ImGui::PopItemWidth();

        ImGui::TreePop();
    }

    ImGui::EndChild();

    ImGui::Separator();
    ImGui::Spacing();

    // Chain selector
    ImGui::Text("Select Chain:");

    // List chains from study
    auto chainFiles = mCurrentStudy->GetChainFiles();
    if (chainFiles.empty()) {
        ImGui::TextDisabled("No chains defined. Create a chain in the Chains tab.");
    } else {
        for (size_t i = 0; i < chainFiles.size(); i++) {
            std::string chainName = fs::path(chainFiles[i]).stem().string();

            // Highlight selected chain
            bool isSelected = (mCurrentChain &&
                              fs::path(mCurrentChain->GetFilePath()).stem().string() == chainName);
            if (isSelected) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
            }

            if (ImGui::Button(chainName.c_str(), ImVec2(200, 0))) {
                // Construct full path to chain file
                std::string fullChainPath = mCurrentStudy->GetPath() + "/chains/" + chainFiles[i];
                LoadChain(fullChainPath);
                printf("Loaded chain: %s\n", chainName.c_str());
            }

            if (isSelected) {
                ImGui::PopStyleColor();
            }
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Run button
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));

    bool canRun = mCurrentChain && !mCurrentChain->GetItems().empty() && !mRunningChain;
    if (!canRun) {
        ImGui::BeginDisabled();
    }

    const char* buttonLabel = mRunningChain ? "Running..." : "Run Selected Chain";
    if (ImGui::Button(buttonLabel, ImVec2(-1, 50))) {
        RunChain();
    }

    if (!canRun) {
        ImGui::EndDisabled();
    }

    ImGui::PopStyleColor(3);
}

void LauncherUI::RenderQuickLaunchTab()
{
    ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Quick Launch");
    ImGui::Separator();
    ImGui::Spacing();

    // Two-column layout for top section: Instructions + Directory (left) | Recent tests (right)
    float topLeftWidth = ImGui::GetContentRegionAvail().x * 0.5f;

    // Left column: Instructions + Directory (compact - 4 lines)
    ImGui::BeginChild("InstructionsColumn", ImVec2(topLeftWidth, 100), false);

    ImGui::Text("Browse and run .pbl scripts.");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Current directory display (read-only)
    ImGui::Text("Directory:");
    ImGui::PushItemWidth(-100);  // Leave room for browse button
    ImGui::InputText("##QuickLaunchDir", &mQuickLaunchDirectory[0], 512, ImGuiInputTextFlags_ReadOnly);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::Button("Browse...", ImVec2(90, 0))) {
        std::string dir = OpenDirectoryDialog("Select Directory for Quick Launch");
        if (!dir.empty()) {
            mQuickLaunchDirectory = dir;
            mQuickLaunchSelectedFile = -1;
            mQuickLaunchPath[0] = '\0';
        }
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // Right column: Recent tests (compact - 4 lines)
    ImGui::BeginChild("RecentTestsColumn", ImVec2(0, 100), false);

    const std::vector<RecentExperiment>& recent = mConfig->GetRecentExperiments();
    if (!recent.empty()) {
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Recent Tests:");
        ImGui::Spacing();

        ImGui::BeginChild("RecentList", ImVec2(0, 0), true);

        for (size_t i = 0; i < recent.size(); i++) {
            const auto& exp = recent[i];
            // Use index as unique ID to handle duplicate names
            ImGui::PushID(static_cast<int>(i));

            // Show just the name, with timestamp as tooltip
            if (ImGui::Selectable(exp.name.c_str())) {
                // Set the quick launch path to this experiment
                std::strncpy(mQuickLaunchPath, exp.path.c_str(), sizeof(mQuickLaunchPath) - 1);
                mQuickLaunchPath[sizeof(mQuickLaunchPath) - 1] = '\0';

                // Update directory to parent of selected file
                fs::path filePath(exp.path);
                mQuickLaunchDirectory = filePath.parent_path().string();
            }

            if (ImGui::IsItemHovered()) {
                // Format timestamp
                char timeBuf[64];
                struct tm* timeinfo = localtime(&exp.lastRun);
                strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", timeinfo);
                ImGui::SetTooltip("Last run: %s\n%s", timeBuf, exp.path.c_str());
            }

            ImGui::PopID();
        }

        ImGui::EndChild();
    } else {
        ImGui::TextDisabled("No recent tests");
    }

    ImGui::EndChild();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // File list (left) and configuration (right)
    float leftWidth = ImGui::GetContentRegionAvail().x * 0.5f;

    // Left: File browser (compact)
    ImGui::BeginChild("QuickLaunchFiles", ImVec2(leftWidth, 170), true);
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 1.0f, 1.0f), "PEBL Scripts");
    ImGui::Separator();

    // Scan current directory for directories and .pbl files
    std::vector<std::string> directories;
    std::vector<std::string> pblFiles;

    try {
        // Always add ".." for parent directory navigation
        directories.push_back("..");

        for (const auto& entry : fs::directory_iterator(mQuickLaunchDirectory)) {
            std::string name = entry.path().filename().string();

            if (entry.is_directory()) {
                directories.push_back(name);
            } else if (entry.is_regular_file() && name.length() > 4 && name.substr(name.length() - 4) == ".pbl") {
                pblFiles.push_back(name);
            }
        }

        std::sort(directories.begin(), directories.end());
        std::sort(pblFiles.begin(), pblFiles.end());
    } catch (const fs::filesystem_error&) {
        // Directory doesn't exist or can't be read
    }

    // If mQuickLaunchPath is set, find its index in the file list
    static std::string lastProcessedPath;
    if (strlen(mQuickLaunchPath) > 0 && std::string(mQuickLaunchPath) != lastProcessedPath) {
        std::string targetFile = mQuickLaunchPath;
        lastProcessedPath = targetFile;  // Remember we processed this path

        // Extract just the filename from the full path
        targetFile = fs::path(targetFile).filename().string();
        // Find index in pblFiles
        for (int i = 0; i < (int)pblFiles.size(); i++) {
            if (pblFiles[i] == targetFile) {
                mQuickLaunchSelectedFile = i;
                break;
            }
        }
    }

    // Display directories first with folder icon
    int dirIndex = 0;
    for (const auto& dir : directories) {
        std::string displayName = (dir == "..") ? "[UP] .." : "[DIR] " + dir;
        bool isParentDir = (dir == "..");

        if (ImGui::Selectable(displayName.c_str(), false, 0, ImVec2(ImGui::GetContentRegionAvail().x - 60, 0))) {
            // Navigate into directory
            if (isParentDir) {
                // Go up one level using fs::path for cross-platform support
                fs::path currentPath(mQuickLaunchDirectory);
                fs::path parentPath = currentPath.parent_path();
                if (!parentPath.empty() && parentPath != currentPath) {
                    mQuickLaunchDirectory = parentPath.string();
                }
            } else {
                // Navigate into subdirectory using fs::path
                mQuickLaunchDirectory = (fs::path(mQuickLaunchDirectory) / dir).string();
            }
            mQuickLaunchSelectedFile = -1;
            mQuickLaunchPath[0] = '\0';
        }

        // Add Open button for actual directories (not "..")
        if (!isParentDir) {
            ImGui::SameLine();
            ImGui::PushID(1000 + dirIndex);  // Use offset to avoid ID collision with files
            if (ImGui::SmallButton("Open")) {
                std::string fullPath = (fs::path(mQuickLaunchDirectory) / dir).string();
                OpenDirectoryInFileBrowser(fullPath);
            }
            ImGui::PopID();
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Open directory in file browser");
            }
        }

        dirIndex++;
    }

    // Display .pbl files
    int fileIndex = 0;
    for (const auto& file : pblFiles) {
        bool is_selected = (mQuickLaunchSelectedFile == fileIndex);

        // Make filename selectable
        if (ImGui::Selectable(file.c_str(), is_selected, 0, ImVec2(ImGui::GetContentRegionAvail().x - 60, 0))) {
            // Set as selected and update path
            mQuickLaunchSelectedFile = fileIndex;
            std::string fullPath = (fs::path(mQuickLaunchDirectory) / file).string();
            std::strncpy(mQuickLaunchPath, fullPath.c_str(), sizeof(mQuickLaunchPath) - 1);
            mQuickLaunchPath[sizeof(mQuickLaunchPath) - 1] = '\0';
        }

        // Add Edit button on the same line
        ImGui::SameLine();
        ImGui::PushID(fileIndex);
        if (ImGui::SmallButton("Edit")) {
            std::string fullPath = (fs::path(mQuickLaunchDirectory) / file).string();

            // Open in code editor
            std::ifstream fileStream(fullPath);
            if (fileStream.is_open()) {
                std::stringstream buffer;
                buffer << fileStream.rdbuf();
                fileStream.close();

                mCodeEditorFilePath = fullPath;
                mCodeEditor.SetText(buffer.str());
                mShowCodeEditor = true;
            } else {
                printf("Error: Could not open file for editing: %s\n", fullPath.c_str());
            }
        }
        ImGui::PopID();
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Open file in code editor");
        }

        fileIndex++;
    }

    if (directories.empty() && pblFiles.empty()) {
        ImGui::TextDisabled("Empty directory");
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // Right: Configuration (compact)
    ImGui::BeginChild("QuickLaunchConfig", ImVec2(0, 170), true);
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 1.0f, 1.0f), "Configuration");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Subject Code:");
    ImGui::PushItemWidth(-1);
    ImGui::InputText("##QLSubject", mSubjectCode, sizeof(mSubjectCode));
    ImGui::PopItemWidth();

    ImGui::Spacing();

    ImGui::Text("Language:");
    ImGui::PushItemWidth(-1);
    ImGui::InputText("##QLLanguage", mLanguageCode, sizeof(mLanguageCode));
    ImGui::PopItemWidth();

    ImGui::Spacing();

    ImGui::Text("Parameter File (optional):");
    ImGui::PushItemWidth(-80);
    ImGui::InputText("##QLParams", mQuickLaunchParamFile, sizeof(mQuickLaunchParamFile));
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::Button("...##ParamBrowse", ImVec2(70, 0))) {
        std::string file = OpenFileDialog("Select Parameter File", "*.json");
        if (!file.empty()) {
            std::strncpy(mQuickLaunchParamFile, file.c_str(), sizeof(mQuickLaunchParamFile) - 1);
            mQuickLaunchParamFile[sizeof(mQuickLaunchParamFile) - 1] = '\0';
        }
    }

    ImGui::Spacing();

    ImGui::Checkbox("Fullscreen", &mFullscreen);

    ImGui::EndChild();

    ImGui::Spacing();

    // Run button
    bool canRun = (mQuickLaunchPath[0] != '\0');
    if (!canRun) {
        ImGui::BeginDisabled();
    }

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));

    if (ImGui::Button("Run Script", ImVec2(-1, 50))) {
        // Build extra arguments (subject code and language are handled by RunExperiment)
        std::vector<std::string> args;
        if (strlen(mQuickLaunchParamFile) > 0) {
            args.push_back("--pfile");
            args.push_back(mQuickLaunchParamFile);
        }

        // Clean up any previous experiment
        if (mRunningExperiment) {
            delete mRunningExperiment;
        }

        // Run the experiment
        mRunningExperiment = new ExperimentRunner(mConfig);
        bool success = mRunningExperiment->RunExperiment(mQuickLaunchPath, args,
                                                          mSubjectCode, mLanguageCode,
                                                          mFullscreen);
        if (success) {
            // Add to recent experiments list
            std::string scriptPath = mQuickLaunchPath;
            std::string scriptName = fs::path(scriptPath).filename().string();
            mConfig->AddRecentExperiment(scriptPath, scriptName);
            mShowStderr = false;  // Start showing stdout
        } else {
            printf("Failed to run: %s\n", mQuickLaunchPath);
        }
    }

    ImGui::PopStyleColor(3);

    if (!canRun) {
        ImGui::EndDisabled();
    }
}

void LauncherUI::RenderOutputPanel()
{
    ImGui::Separator();

    // Header bar - always visible
    // Expand/collapse toggle with arrow indicator
    const char* toggleLabel = mOutputExpanded ? "v Output" : "> Output";
    if (ImGui::Button(toggleLabel, ImVec2(100, 0))) {
        mOutputExpanded = !mOutputExpanded;
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip(mOutputExpanded ? "Collapse output panel" : "Expand output panel");
    }

    if (!mOutputExpanded) {
        // Collapsed - just show a brief status on the same line
        ImGui::SameLine();
        if (mRunningExperiment && mRunningExperiment->IsRunning()) {
            ImGui::TextDisabled("(running...)");
        } else if (mRunningExperiment || !mChainAccumulatedStdout.empty() || !mChainAccumulatedStderr.empty()) {
            ImGui::TextDisabled("(click to expand)");
        }
        return;
    }

    // Expanded - show stdout/stderr toggle and controls
    ImGui::SameLine();
    if (ImGui::RadioButton("stdout##bottom", !mShowStderr)) {
        mShowStderr = false;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("stderr##bottom", mShowStderr)) {
        mShowStderr = true;
    }

    // "Open in Editor" button
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 135);
    if (ImGui::Button("Open in Editor##bottom", ImVec2(130, 0))) {
        std::string output;
        if (mRunningExperiment) {
            if (mRunningChain) {
                output = mShowStderr ? mChainAccumulatedStderr : mChainAccumulatedStdout;
                const std::string& currentOutput = mShowStderr ? mRunningExperiment->GetStderr() :
                                                                 mRunningExperiment->GetStdout();
                output += currentOutput;
            } else {
                output = mShowStderr ? mRunningExperiment->GetStderr() :
                                       mRunningExperiment->GetStdout();
            }
        } else if (!mChainAccumulatedStdout.empty() || !mChainAccumulatedStderr.empty()) {
            output = mShowStderr ? mChainAccumulatedStderr : mChainAccumulatedStdout;
        }

        if (!output.empty()) {
            mCodeEditor.SetText(output);
            mCodeEditorFilePath = "";
            mShowCodeEditor = true;
        }
    }

    // Scrollable output window - fills remaining space
    ImGui::BeginChild("BottomOutputPanel", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

    if (mRunningExperiment) {
        std::string output;

        // If running a chain, show accumulated output from all items plus current item
        if (mRunningChain) {
            output = mShowStderr ? mChainAccumulatedStderr : mChainAccumulatedStdout;
            const std::string& currentOutput = mShowStderr ? mRunningExperiment->GetStderr() :
                                                             mRunningExperiment->GetStdout();
            output += currentOutput;
        } else {
            output = mShowStderr ? mRunningExperiment->GetStderr() :
                                   mRunningExperiment->GetStdout();
        }

        if (!output.empty()) {
            ImGui::InputTextMultiline("##bottomoutput",
                                       const_cast<char*>(output.c_str()),
                                       output.size() + 1,
                                       ImVec2(-1, -1),
                                       ImGuiInputTextFlags_ReadOnly);
        } else if (mRunningExperiment->IsRunning()) {
            ImGui::TextDisabled("Waiting for output...");
        } else {
            ImGui::TextDisabled("No output captured");
        }
    } else if (!mChainAccumulatedStdout.empty() || !mChainAccumulatedStderr.empty()) {
        // Chain completed - show final accumulated output
        const std::string& output = mShowStderr ? mChainAccumulatedStderr : mChainAccumulatedStdout;
        ImGui::InputTextMultiline("##bottomoutput",
                                   const_cast<char*>(output.c_str()),
                                   output.size() + 1,
                                   ImVec2(-1, -1),
                                   ImGuiInputTextFlags_ReadOnly);
    } else {
        ImGui::TextDisabled("Run a test or chain to see output here");
    }

    ImGui::EndChild();
}

void LauncherUI::ShowNewStudyDialog()
{
    ImGui::OpenPopup("New Study");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(660, 470), ImGuiCond_Always);

    if (ImGui::BeginPopupModal("New Study", &mShowNewStudyDialog, 0))
    {
        ImGui::Text("Create a new study");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Study Name:");
        ImGui::PushItemWidth(-1);
        if (ImGui::IsWindowAppearing()) {
            ImGui::SetKeyboardFocusHere();
        }
        ImGui::InputText("##StudyName", mNewStudyName, sizeof(mNewStudyName));
        ImGui::PopItemWidth();

        ImGui::Spacing();

        ImGui::Text("Description:");
        ImGui::PushItemWidth(-1);
        ImGui::InputTextMultiline("##StudyDesc", mNewStudyDescription, sizeof(mNewStudyDescription),
                                   ImVec2(-1, 100));
        ImGui::PopItemWidth();

        ImGui::Spacing();

        ImGui::Text("Author:");
        ImGui::PushItemWidth(-1);
        ImGui::InputText("##StudyAuthor", mNewStudyAuthor, sizeof(mNewStudyAuthor));
        ImGui::PopItemWidth();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Create", ImVec2(120, 0))) {
            if (strlen(mNewStudyName) > 0) {
                // Create new study
                std::string studyPath = mWorkspace->GetStudiesPath() + "/" + mNewStudyName;
                mCurrentStudy = Study::CreateNew(studyPath, mNewStudyName, mNewStudyAuthor);

                if (mCurrentStudy) {
                    mCurrentStudy->SetDescription(mNewStudyDescription);
                    mCurrentStudy->Save();
                    printf("Created new study: %s\n", mNewStudyName);

                    // Save selected study to config
                    mConfig->SetCurrentStudyPath(studyPath);
                    mConfig->SaveConfig();

                    // Auto-load Main chain (created by Study::CreateNew)
                    std::string mainChainPath = studyPath + "/chains/Main.json";
                    if (fs::exists(mainChainPath)) {
                        LoadChain(mainChainPath);
                        printf("Auto-loaded Main chain for new study\n");
                    }
                }

                // Clear form
                mNewStudyName[0] = '\0';
                mNewStudyDescription[0] = '\0';
                mNewStudyAuthor[0] = '\0';

                mShowNewStudyDialog = false;
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            mNewStudyName[0] = '\0';
            mNewStudyDescription[0] = '\0';
            mNewStudyAuthor[0] = '\0';
            mShowNewStudyDialog = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void LauncherUI::ShowNewChainDialog()
{
    ImGui::OpenPopup("New Chain");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_FirstUseEver);

    if (ImGui::BeginPopupModal("New Chain", &mShowNewChainDialog, 0))
    {
        ImGui::Text("Create a new chain");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Chain Name:");
        ImGui::PushItemWidth(-1);
        if (ImGui::IsWindowAppearing()) {
            ImGui::SetKeyboardFocusHere();
        }
        ImGui::InputText("##ChainName", mNewChainName, sizeof(mNewChainName));
        ImGui::PopItemWidth();

        ImGui::Spacing();

        ImGui::Text("Description (optional):");
        ImGui::PushItemWidth(-1);
        ImGui::InputText("##ChainDesc", mNewChainDescription, sizeof(mNewChainDescription));
        ImGui::PopItemWidth();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Create", ImVec2(120, 0))) {
            if (strlen(mNewChainName) > 0) {
                // Create chain file path
                std::string studyPath = mCurrentStudy->GetPath();
                std::string chainPath = studyPath + "/chains/" + std::string(mNewChainName) + ".json";

                // Create new chain
                mCurrentChain = Chain::CreateNew(chainPath, mNewChainName, mNewChainDescription);

                if (mCurrentChain) {
                    mCurrentChain->Save();
                    printf("Created new chain: %s\n", mNewChainName);

                    // Save selected chain to config
                    std::string chainFileName = std::string(mNewChainName) + ".json";
                    mConfig->SetCurrentChainName(chainFileName);
                    mConfig->SaveConfig();
                }

                // Clear form
                mNewChainName[0] = '\0';
                mNewChainDescription[0] = '\0';

                mShowNewChainDialog = false;
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            mNewChainName[0] = '\0';
            mNewChainDescription[0] = '\0';
            mShowNewChainDialog = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void LauncherUI::ShowStudySettingsDialog()
{
    if (!mCurrentStudy) {
        mShowStudySettingsDialog = false;
        return;
    }

    ImGui::OpenPopup("Study Settings");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_Always);

    if (ImGui::BeginPopupModal("Study Settings", &mShowStudySettingsDialog, 0))
    {
        ImGui::Text("Study: %s", mCurrentStudy->GetName().c_str());
        ImGui::Separator();
        ImGui::Spacing();

        // Study metadata editing
        static char nameBuffer[256];
        static char descBuffer[1024];
        static char authorBuffer[256];
        static char uploadServerBuffer[512];
        static char studyTokenBuffer[256];
        static bool initialized = false;

        if (!initialized) {
            std::strncpy(nameBuffer, mCurrentStudy->GetName().c_str(), sizeof(nameBuffer) - 1);
            std::strncpy(descBuffer, mCurrentStudy->GetDescription().c_str(), sizeof(descBuffer) - 1);
            std::strncpy(authorBuffer, mCurrentStudy->GetAuthor().c_str(), sizeof(authorBuffer) - 1);
            std::strncpy(uploadServerBuffer, mCurrentStudy->GetUploadServerURL().c_str(), sizeof(uploadServerBuffer) - 1);
            std::strncpy(studyTokenBuffer, mCurrentStudy->GetStudyToken().c_str(), sizeof(studyTokenBuffer) - 1);
            initialized = true;
        }

        ImGui::Text("Name:");
        ImGui::PushItemWidth(-1);
        if (ImGui::IsWindowAppearing()) {
            ImGui::SetKeyboardFocusHere();
        }
        ImGui::InputText("##Name", nameBuffer, sizeof(nameBuffer));
        ImGui::PopItemWidth();

        ImGui::Spacing();

        ImGui::Text("Description:");
        ImGui::PushItemWidth(-1);
        ImGui::InputTextMultiline("##Desc", descBuffer, sizeof(descBuffer), ImVec2(-1, 150));
        ImGui::PopItemWidth();

        ImGui::Spacing();

        ImGui::Text("Author:");
        ImGui::PushItemWidth(-1);
        ImGui::InputText("##Author", authorBuffer, sizeof(authorBuffer));
        ImGui::PopItemWidth();

        ImGui::Spacing();
        ImGui::Text("Version: %d", mCurrentStudy->GetVersion());
        ImGui::Text("Created: %s", mCurrentStudy->GetCreatedDate().c_str());
        ImGui::Text("Modified: %s", mCurrentStudy->GetModifiedDate().c_str());

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Upload configuration
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Data Upload Configuration");
        ImGui::Spacing();
        ImGui::TextWrapped("Configure automatic data upload to PEBLOnlinePlatform or compatible server:");
        ImGui::Spacing();

        ImGui::Text("Upload Server URL:");
        ImGui::SameLine();
        if (ImGui::SmallButton("?##ServerHelp")) {
            ImGui::SetTooltip("Server URL (e.g., https://peblhub.online or http://localhost:8080)");
        }
        ImGui::PushItemWidth(-1);
        ImGui::InputText("##UploadServer", uploadServerBuffer, sizeof(uploadServerBuffer));
        ImGui::PopItemWidth();

        ImGui::Spacing();

        ImGui::Text("Study Token:");
        ImGui::SameLine();
        if (ImGui::SmallButton("?##TokenHelp")) {
            ImGui::SetTooltip("Study token from PEBLOnlinePlatform (e.g., STUDY_ABC123...)");
        }
        ImGui::PushItemWidth(-1);
        ImGui::InputText("##StudyToken", studyTokenBuffer, sizeof(studyTokenBuffer));
        ImGui::PopItemWidth();

        ImGui::Spacing();

        // Button to load from upload.json file
        if (ImGui::Button("Load from upload.json...", ImVec2(-1, 0))) {
            std::string uploadJsonPath = OpenFileDialog("Select upload.json", "*.json");
            if (!uploadJsonPath.empty()) {
                std::ifstream file(uploadJsonPath);
                if (file.is_open()) {
                    nlohmann::json uploadConfig;
                    try {
                        file >> uploadConfig;

                        // Extract server URL from host, port, and page
                        std::string host = uploadConfig.value("host", "");
                        int port = uploadConfig.value("port", 443);

                        // Construct server URL
                        std::string protocol = (port == 443) ? "https://" : "http://";
                        std::string serverUrl = protocol + host;
                        if ((port != 443 && port != 80) || (protocol == "http://" && port != 80)) {
                            serverUrl += ":" + std::to_string(port);
                        }

                        // Get token
                        std::string token = uploadConfig.value("token", "");

                        // Populate fields
                        std::strncpy(uploadServerBuffer, serverUrl.c_str(), sizeof(uploadServerBuffer) - 1);
                        uploadServerBuffer[sizeof(uploadServerBuffer) - 1] = '\0';

                        std::strncpy(studyTokenBuffer, token.c_str(), sizeof(studyTokenBuffer) - 1);
                        studyTokenBuffer[sizeof(studyTokenBuffer) - 1] = '\0';

                        printf("Loaded upload configuration from: %s\n", uploadJsonPath.c_str());
                    } catch (const std::exception& e) {
                        printf("Error parsing upload.json: %s\n", e.what());
                    }
                    file.close();
                } else {
                    printf("Failed to open upload.json file\n");
                }
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Save", ImVec2(120, 0))) {
            mCurrentStudy->SetName(nameBuffer);
            mCurrentStudy->SetDescription(descBuffer);
            mCurrentStudy->SetAuthor(authorBuffer);
            mCurrentStudy->SetUploadServerURL(uploadServerBuffer);
            mCurrentStudy->SetStudyToken(studyTokenBuffer);
            mCurrentStudy->Save();

            initialized = false;
            mShowStudySettingsDialog = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            initialized = false;
            mShowStudySettingsDialog = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void LauncherUI::ShowFirstRunDialog()
{
    ImGui::OpenPopup("Welcome to PEBL!");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_FirstUseEver);

    if (ImGui::BeginPopupModal("Welcome to PEBL!", nullptr, 0))
    {
        ImGui::TextWrapped("Welcome! This appears to be your first time running PEBL %s.", PEBL_VERSION);
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextWrapped("PEBL will create a workspace directory at:");
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "    %s", mWorkspace->GetWorkspacePath().c_str());
        ImGui::Spacing();

        ImGui::TextWrapped("This workspace will contain:");
        ImGui::BulletText("my_studies/ - Your study projects");
        ImGui::BulletText("snapshots/ - Study backups and imports");
        ImGui::BulletText("doc/ - Documentation");
        ImGui::BulletText("demo/ - Example experiments");
        ImGui::BulletText("tutorials/ - Tutorial materials");
        ImGui::BulletText("logs/ - Launch logs");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextWrapped("Resources will be copied from the installation. This may take a minute on first run.");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Center the button
        float buttonWidth = 200.0f;
        float windowWidth = ImGui::GetContentRegionAvail().x;
        ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

        if (ImGui::Button("Continue", ImVec2(buttonWidth, 40))) {
            // Initialize workspace (creates directories)
            if (!mWorkspace->Initialize()) {
                printf("ERROR: Failed to initialize workspace\n");
            } else {
                // Find installation path using BinReloc (AppImage-compatible)
                std::string installPath;

                #ifdef ENABLE_BINRELOC
                // Use BinReloc to find installation prefix
                BrInitError error;
                if (br_init(&error) != 0) {
                    char* prefix = br_find_prefix(PREFIX);
                    if (prefix) {
                        installPath = std::string(prefix);
                        free(prefix);
                        printf("BinReloc found installation at: %s\n", installPath.c_str());
                    }
                }
                #endif

                #ifndef _WIN32
                // Fallback 1: Try to find pebl2 executable via /proc/self/exe (Linux only)
                if (installPath.empty()) {
                    char exePath[1024];
                    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
                    if (len != -1) {
                        exePath[len] = '\0';
                        std::string path(exePath);
                        size_t lastSlash = path.find_last_of('/');
                        if (lastSlash != std::string::npos) {
                            installPath = path.substr(0, lastSlash);
                            // If we're in bin/, go up one level
                            if (installPath.find("/bin") != std::string::npos) {
                                lastSlash = installPath.find_last_of('/');
                                if (lastSlash != std::string::npos) {
                                    installPath = installPath.substr(0, lastSlash);
                                }
                            }
                            printf("/proc/self/exe derived installation at: %s\n", installPath.c_str());
                        }
                    }
                }
                #endif

                // Fallback 2: Use battery path from config
                if (installPath.empty()) {
                    std::string batteryPath = mConfig->GetBatteryPath();
                    if (!batteryPath.empty()) {
                        size_t lastSlash = batteryPath.find_last_of("/\\");
                        if (lastSlash != std::string::npos) {
                            installPath = batteryPath.substr(0, lastSlash);
                            printf("Config derived installation at: %s\n", installPath.c_str());
                        }
                    }
                }

                // Copy resources if we found installation
                if (!installPath.empty()) {
                    printf("Initializing workspace at: %s\n", mWorkspace->GetWorkspacePath().c_str());
                    printf("Copying resources from: %s\n", installPath.c_str());
                    mWorkspace->CopyResources(installPath);
                } else {
                    printf("WARNING: Could not determine installation path - resources not copied\n");
                }
            }

            mShowFirstRunDialog = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void LauncherUI::ShowGettingStartedDialog()
{
    ImGui::OpenPopup("Create a Study to Get Started");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_FirstUseEver);

    if (ImGui::BeginPopupModal("Create a Study to Get Started", nullptr, ImGuiWindowFlags_NoResize))
    {
        ImGui::Spacing();
        ImGui::TextWrapped("Welcome! To begin using PEBL, you need to create a study.");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextWrapped("A study is a container for:");
        ImGui::BulletText("Tests from the PEBL battery");
        ImGui::BulletText("Chains (sequences of tests and instructions)");
        ImGui::BulletText("Participant data and results");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Center the buttons
        float buttonWidth = 150.0f;
        float spacing = 20.0f;
        float totalWidth = buttonWidth * 2 + spacing;
        float windowWidth = ImGui::GetContentRegionAvail().x;
        ImGui::SetCursorPosX((windowWidth - totalWidth) * 0.5f);

        // New Study button (green)
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));

        if (ImGui::Button("New Study", ImVec2(buttonWidth, 40))) {
            mShowGettingStartedDialog = false;
            mShowNewStudyDialog = true;
            ImGui::CloseCurrentPopup();
        }

        ImGui::PopStyleColor(3);

        ImGui::SameLine(0, spacing);

        // Browse Tests button
        if (ImGui::Button("Browse Tests", ImVec2(buttonWidth, 40))) {
            mShowGettingStartedDialog = false;
            mTopLevelTab = 0;  // Switch to Manage Studies tab
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Explore available battery tests before creating a study");
        }

        ImGui::EndPopup();
    }
}

void LauncherUI::ShowDuplicateSubjectWarning()
{
    ImGui::OpenPopup("Duplicate Subject Code");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);

    if (ImGui::BeginPopupModal("Duplicate Subject Code", &mShowDuplicateSubjectWarning, 0))
    {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "⚠ Warning: Subject Code Already Used");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextWrapped("The subject code '%s' has already been used in this study.", mSubjectCode);
        ImGui::Spacing();
        ImGui::TextWrapped("Running the chain again with this code may overwrite existing data files!");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Show existing codes in a scrollable region
        ImGui::Text("Existing subject codes in this study:");
        ImGui::BeginChild("ExistingCodes", ImVec2(0, 150), true);
        for (const auto& code : mDuplicateWarningCodes) {
            if (code == std::string(mSubjectCode)) {
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "• %s (current code)", code.c_str());
            } else {
                ImGui::Text("• %s", code.c_str());
            }
        }
        ImGui::EndChild();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Buttons
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));
        if (ImGui::Button("Continue Anyway", ImVec2(150, 0))) {
            mShowDuplicateSubjectWarning = false;
            ImGui::CloseCurrentPopup();
            // Actually run the chain now
            RunChainConfirmed();
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
        if (ImGui::Button("Cancel", ImVec2(150, 0))) {
            mShowDuplicateSubjectWarning = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor(3);

        ImGui::EndPopup();
    }
}

void LauncherUI::ShowEditParticipantCodeDialog()
{
    ImGui::OpenPopup("Edit Participant Code");

    // Center dialog
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Edit Participant Code", &mShowEditParticipantCodeDialog, 0))
    {
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Edit Participant Code Components");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextWrapped("The participant code is generated from: STUDYCODE_COUNTER");
        ImGui::TextWrapped("Edit the study code (4 characters) and counter separately below.");
        ImGui::Spacing();

        // Study code (4 characters)
        ImGui::Text("Study Code (4 chars):");
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        if (ImGui::IsWindowAppearing()) {
            ImGui::SetKeyboardFocusHere();
        }
        ImGui::InputText("##StudyCode", mStudyCode, sizeof(mStudyCode));
        ImGui::PopItemWidth();

        ImGui::Spacing();

        // Participant counter
        if (mCurrentChain) {
            int counter = mCurrentChain->GetParticipantCounter();
            ImGui::Text("Counter:");
            ImGui::SameLine();
            ImGui::PushItemWidth(100);
            if (ImGui::InputInt("##Counter", &counter)) {
                if (counter < 1) counter = 1;
                mCurrentChain->SetParticipantCounter(counter);
                mCurrentChain->Save();
            }
            ImGui::PopItemWidth();

            ImGui::Spacing();

            // Preview
            std::string preview = std::string(mStudyCode) + "_" + std::to_string(counter);
            ImGui::TextColored(ImVec4(0.6f, 0.8f, 0.6f, 1.0f), "Preview:");
            ImGui::SameLine();
            ImGui::Text("%s", preview.c_str());
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Buttons
        if (ImGui::Button("Done", ImVec2(120, 0))) {
            mShowEditParticipantCodeDialog = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void LauncherUI::ShowCodeEditor()
{
    ImGui::SetNextWindowSize(ImVec2(1200, 800), ImGuiCond_FirstUseEver);

    bool open = true;
    if (ImGui::Begin("Code Editor", &open, ImGuiWindowFlags_MenuBar))
    {
        // Menu bar with file operations
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Save", "Ctrl+S")) {
                    // Save file
                    std::string text = mCodeEditor.GetText();
                    std::ofstream outFile(mCodeEditorFilePath);
                    if (outFile.is_open()) {
                        outFile << text;
                        outFile.close();
                        printf("Saved file: %s\n", mCodeEditorFilePath.c_str());
                    } else {
                        printf("Error: Could not save file: %s\n", mCodeEditorFilePath.c_str());
                    }
                }

                if (ImGui::MenuItem("Open in External Editor")) {
                    // Use external editor setting from config
                    std::string editorCmd = mConfig->GetExternalEditor();
                    std::string command;

#ifdef _WIN32
                    if (editorCmd == "start") {
                        command = "start \"\" \"" + mCodeEditorFilePath + "\"";
                    } else {
                        command = editorCmd + " \"" + mCodeEditorFilePath + "\"";
                    }
#else
                    command = editorCmd + " \"" + mCodeEditorFilePath + "\" &";
#endif

                    printf("Opening in external editor: %s\n", command.c_str());
                    int result = system(command.c_str());
                    if (result != 0) {
                        printf("Warning: External editor command may have failed\n");
                    }
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Close")) {
                    open = false;
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit"))
            {
                bool ro = mCodeEditor.IsReadOnly();
                if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
                    mCodeEditor.SetReadOnly(ro);
                ImGui::Separator();

                if (ImGui::MenuItem("Undo", "Ctrl+Z", nullptr, !ro && mCodeEditor.CanUndo()))
                    mCodeEditor.Undo();
                if (ImGui::MenuItem("Redo", "Ctrl+Y", nullptr, !ro && mCodeEditor.CanRedo()))
                    mCodeEditor.Redo();

                ImGui::Separator();

                if (ImGui::MenuItem("Copy", "Ctrl+C", nullptr, mCodeEditor.HasSelection()))
                    mCodeEditor.Copy();
                if (ImGui::MenuItem("Cut", "Ctrl+X", nullptr, !ro && mCodeEditor.HasSelection()))
                    mCodeEditor.Cut();
                if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && mCodeEditor.HasSelection()))
                    mCodeEditor.Delete();
                if (ImGui::MenuItem("Paste", "Ctrl+V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
                    mCodeEditor.Paste();

                ImGui::Separator();

                if (ImGui::MenuItem("Select all", nullptr, nullptr))
                    mCodeEditor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(mCodeEditor.GetTotalLines(), 0));

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Dark palette"))
                    mCodeEditor.SetPalette(TextEditor::GetDarkPalette());
                if (ImGui::MenuItem("Light palette"))
                    mCodeEditor.SetPalette(TextEditor::GetLightPalette());
                if (ImGui::MenuItem("Retro blue palette"))
                    mCodeEditor.SetPalette(TextEditor::GetRetroBluePalette());
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // Show filename and stats
        ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), "File:");
        ImGui::SameLine();
        ImGui::Text("%s", mCodeEditorFilePath.c_str());

        ImGui::SameLine(ImGui::GetWindowWidth() - 300);
        auto cpos = mCodeEditor.GetCursorPosition();
        ImGui::Text("%d lines | Ln %d, Col %d", mCodeEditor.GetTotalLines(), cpos.mLine + 1, cpos.mColumn + 1);

        // Render the text editor
        mCodeEditor.Render("##TextEditor");

        ImGui::End();
    }

    if (!open) {
        mShowCodeEditor = false;
    }
}

void LauncherUI::ShowTranslationEditorDialog()
{
    ImGui::OpenPopup("Translation Editor");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(1000, 600), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Translation Editor", &mTranslationEditor.show, ImGuiWindowFlags_NoScrollbar))
    {
        // Build file paths — scale mode uses the scale directory directly;
        // test mode uses the test's translations/ subdirectory.
        std::string baseName;
        std::string translationsDir;
        bool isScale = mTranslationEditor.scaleMode;

        if (mTranslationEditor.scaleMode) {
            // Scale Builder mode: translations live in the scale directory itself
            if (mTranslationEditor.scaleCode[0] == '\0' || mTranslationEditor.scaleDir[0] == '\0') {
                ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Error: No scale selected");
                if (ImGui::Button("Close", ImVec2(120, 0))) {
                    mTranslationEditor.show = false;
                    mTranslationEditor.Clear();
                    mTranslationEditor.ClearScaleMode();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
                return;
            }
            baseName = mTranslationEditor.scaleCode;
            translationsDir = mTranslationEditor.scaleDir;

            // Header
            ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Scale:");
            ImGui::SameLine();
            ImGui::Text("%s", mTranslationEditor.scaleCode);
        } else {
            // Test mode (regular battery test)
            if (!mCurrentStudy || mTranslationEditor.testIndex < 0) {
                ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Error: No test selected");
                if (ImGui::Button("Close", ImVec2(120, 0))) {
                    mTranslationEditor.show = false;
                    mTranslationEditor.Clear();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
                return;
            }

            const auto& tests = mCurrentStudy->GetTests();
            if (mTranslationEditor.testIndex >= (int)tests.size()) {
                ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Error: Invalid test index");
                if (ImGui::Button("Close", ImVec2(120, 0))) {
                    mTranslationEditor.show = false;
                    mTranslationEditor.Clear();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
                return;
            }

            const Test& test = tests[mTranslationEditor.testIndex];
            baseName = fs::path(test.testName).filename().string();
            translationsDir = (fs::path(mTranslationEditor.testPath) / "translations").string();
            isScale = fs::exists(fs::path(mTranslationEditor.testPath) / "definitions");

            // Header
            ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Test:");
            ImGui::SameLine();
            ImGui::Text("%s", test.testName.c_str());
        }

        // English file: scale builder and OSD-scale tests use {name}.en.json directly;
        // traditional tests use {name}.pbl-en.json.
        std::string englishFile;
        if (isScale) {
            std::string newPath = (fs::path(translationsDir) / (baseName + ".en.json")).string();
            std::string oldPath = (fs::path(translationsDir) / (baseName + ".pbl-en.json")).string();
            englishFile = fs::exists(newPath) ? newPath : (fs::exists(oldPath) ? oldPath : newPath);
        } else {
            englishFile = (fs::path(translationsDir) / (baseName + ".pbl-en.json")).string();
        }

        // Scan for available language files
        std::vector<std::string> availableLanguages;
        availableLanguages.push_back("en");  // English is always available as base

        if (fs::exists(translationsDir) && fs::is_directory(translationsDir)) {
            std::set<std::string> langSet;
            // Scale-code prefix filter: in scale mode only list files for this scale
            std::string scalePrefix = mTranslationEditor.scaleMode ?
                (std::string(mTranslationEditor.scaleCode) + ".") : "";
            for (const auto& entry : fs::directory_iterator(translationsDir)) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();
                    // In scale mode, skip files that don't belong to this scale
                    if (!scalePrefix.empty() && filename.find(scalePrefix) != 0) {
                        continue;
                    }
                    std::string lang;
                    // Try dash-based format: "name.pbl-lang.json"
                    size_t dashPos = filename.rfind('-');
                    size_t dotPos = filename.rfind(".json");
                    if (dashPos != std::string::npos && dotPos != std::string::npos && dotPos > dashPos) {
                        lang = filename.substr(dashPos + 1, dotPos - dashPos - 1);
                    } else if (dotPos != std::string::npos) {
                        // Try dot-based format: "name.lang.json"
                        size_t lastDot = filename.rfind('.', dotPos - 1);
                        if (lastDot != std::string::npos) {
                            lang = filename.substr(lastDot + 1, dotPos - lastDot - 1);
                        }
                    }
                    if (!lang.empty() && lang != "en") {
                        langSet.insert(lang);
                    }
                }
            }
            for (const auto& l : langSet) {
                availableLanguages.push_back(l);
            }
        }

        // Language selector
        ImGui::SameLine(0, 20);
        ImGui::Text("Language:");
        ImGui::SameLine();
        ImGui::PushItemWidth(100);

        static char prevLanguage[16] = "";
        if (ImGui::BeginCombo("##Language", mTranslationEditor.language[0] ? mTranslationEditor.language : "Select...")) {
            for (const auto& lang : availableLanguages) {
                bool isSelected = (std::string(mTranslationEditor.language) == lang);
                if (ImGui::Selectable(lang.c_str(), isSelected)) {
                    std::strncpy(mTranslationEditor.language, lang.c_str(), sizeof(mTranslationEditor.language) - 1);
                    mTranslationEditor.language[sizeof(mTranslationEditor.language) - 1] = '\0';
                }
            }
            // New language option
            ImGui::Separator();
            ImGui::TextDisabled("New code (2 chars):");
            static char newLang[16] = "";
            ImGui::PushItemWidth(60);
            if (ImGui::InputText("##NewLang", newLang, 4, ImGuiInputTextFlags_EnterReturnsTrue)) {
                if (strlen(newLang) > 0) {
                    std::strncpy(mTranslationEditor.language, newLang, sizeof(mTranslationEditor.language) - 1);
                    mTranslationEditor.language[sizeof(mTranslationEditor.language) - 1] = '\0';
                    newLang[0] = '\0';
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::PopItemWidth();
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();

        // Detect language change and reload
        if (strcmp(prevLanguage, mTranslationEditor.language) != 0) {
            mTranslationEditor.dataLoaded = false;
            mTranslationEditor.selectedKeyIndex = -1;
            std::strncpy(prevLanguage, mTranslationEditor.language, sizeof(prevLanguage) - 1);
            prevLanguage[sizeof(prevLanguage) - 1] = '\0';
        }

        // Load translation data if not loaded
        if (!mTranslationEditor.dataLoaded && mTranslationEditor.language[0] != '\0') {
            mTranslationEditor.Clear();

            // First load English as the base
            if (fs::exists(englishFile)) {
                try {
                    std::ifstream f(englishFile);
                    nlohmann::json j = nlohmann::json::parse(f);
                    for (auto& [key, value] : j.items()) {
                        mTranslationEditor.keys.push_back(key);
                        mTranslationEditor.englishValues[key] = value.get<std::string>();
                        mTranslationEditor.targetValues[key] = "";  // Initialize empty
                    }
                } catch (const std::exception& e) {
                    printf("Error loading English translation file: %s\n", e.what());
                }
            }

            // Then load target language if it exists and is not English
            if (std::string(mTranslationEditor.language) != "en") {
                std::string targetFile;
                if (isScale) {
                    std::string newPath = (fs::path(translationsDir) / (baseName + "." + mTranslationEditor.language + ".json")).string();
                    std::string oldPath = (fs::path(translationsDir) / (baseName + ".pbl-" + mTranslationEditor.language + ".json")).string();
                    targetFile = fs::exists(newPath) ? newPath : oldPath;
                } else {
                    targetFile = (fs::path(translationsDir) / (baseName + ".pbl-" + mTranslationEditor.language + ".json")).string();
                }
                if (fs::exists(targetFile)) {
                    try {
                        std::ifstream f(targetFile);
                        nlohmann::json j = nlohmann::json::parse(f);
                        for (auto& [key, value] : j.items()) {
                            mTranslationEditor.targetValues[key] = value.get<std::string>();
                            // Add any keys that weren't in English file
                            if (mTranslationEditor.englishValues.find(key) == mTranslationEditor.englishValues.end()) {
                                mTranslationEditor.keys.push_back(key);
                                mTranslationEditor.englishValues[key] = "";
                            }
                        }
                    } catch (const std::exception& e) {
                        printf("Error loading target translation file: %s\n", e.what());
                    }
                }
            } else {
                // For English, target == english
                mTranslationEditor.targetValues = mTranslationEditor.englishValues;
            }

            mTranslationEditor.dataLoaded = true;
            mTranslationEditor.dirty = false;
            if (!mTranslationEditor.keys.empty()) {
                mTranslationEditor.selectedKeyIndex = 0;
            }
        }

        // Show dirty indicator
        if (mTranslationEditor.dirty) {
            ImGui::SameLine(0, 20);
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "(unsaved changes)");
        }

        ImGui::Spacing();
        ImGui::Separator();

        // Main content area
        if (!mTranslationEditor.language[0]) {
            ImGui::Spacing();
            ImGui::TextWrapped("Select a language to edit translations.");
        } else if (mTranslationEditor.keys.empty()) {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "No English translation file found.");
            ImGui::TextWrapped("Create translations/");
            ImGui::SameLine(0, 0);
            if (isScale) {
                ImGui::Text("%s.en.json", baseName.c_str());
            } else {
                ImGui::Text("%s.pbl-en.json", baseName.c_str());
            }
            ImGui::SameLine(0, 0);
            ImGui::TextWrapped(" first.");
        } else {
            // Two-panel layout: key list on left, edit boxes on right
            float contentHeight = ImGui::GetContentRegionAvail().y - 40;  // Leave room for buttons

            // Left panel - key list
            ImGui::BeginChild("KeyList", ImVec2(168, contentHeight), true);
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Keys");
            ImGui::Separator();
            for (size_t i = 0; i < mTranslationEditor.keys.size(); i++) {
                const std::string& key = mTranslationEditor.keys[i];
                bool isSelected = (mTranslationEditor.selectedKeyIndex == (int)i);

                if (ImGui::Selectable(key.c_str(), isSelected)) {
                    mTranslationEditor.selectedKeyIndex = (int)i;
                }
            }
            ImGui::EndChild();

            ImGui::SameLine();

            // Right panel - edit boxes
            ImGui::BeginChild("EditPanel", ImVec2(0, contentHeight), true);

            if (mTranslationEditor.selectedKeyIndex >= 0 && mTranslationEditor.selectedKeyIndex < (int)mTranslationEditor.keys.size()) {
                const std::string& selectedKey = mTranslationEditor.keys[mTranslationEditor.selectedKeyIndex];
                std::string& englishVal = mTranslationEditor.englishValues[selectedKey];
                std::string& targetVal = mTranslationEditor.targetValues[selectedKey];

                // Show key name
                ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Key: %s", selectedKey.c_str());
                ImGui::Spacing();

                // Calculate available height for text boxes
                float availHeight = ImGui::GetContentRegionAvail().y;
                float boxHeight = (availHeight - 60) / 2;

                // Original value on top (read-only reference with word wrap)
                ImGui::Text("Original (reference):");

                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
                char displayBuf[8192];
                std::strncpy(displayBuf, englishVal.c_str(), sizeof(displayBuf) - 1);
                displayBuf[sizeof(displayBuf) - 1] = '\0';
                ImGui::InputTextMultiline("##original_ro", displayBuf, sizeof(displayBuf), ImVec2(-1, boxHeight),
                    ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_WordWrap);
                ImGui::PopStyleColor();

                ImGui::Spacing();

                // Editable value on bottom
                ImGui::Text("%s (editing):", mTranslationEditor.language);

                static char editBuf[8192];
                std::strncpy(editBuf, targetVal.c_str(), sizeof(editBuf) - 1);
                editBuf[sizeof(editBuf) - 1] = '\0';

                if (ImGui::InputTextMultiline("##target", editBuf, sizeof(editBuf), ImVec2(-1, boxHeight), ImGuiInputTextFlags_WordWrap)) {
                    targetVal = editBuf;
                    mTranslationEditor.dirty = true;
                }
            } else {
                ImGui::TextDisabled("Select a key from the list to edit");
            }

            ImGui::EndChild();
        }

        ImGui::Spacing();

        // Buttons
        bool canSave = mTranslationEditor.dirty && !mTranslationEditor.keys.empty();
        if (!canSave) {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button("Save", ImVec2(100, 0))) {
            // Create translations directory if needed
            try {
                fs::create_directories(translationsDir);
            } catch (...) {}

            // Build JSON and save
            nlohmann::json j;
            for (const auto& key : mTranslationEditor.keys) {
                j[key] = mTranslationEditor.targetValues[key];
            }

            std::string targetFile;
            if (std::string(mTranslationEditor.language) == "en") {
                targetFile = englishFile;
            } else if (isScale) {
                targetFile = (fs::path(translationsDir) / (baseName + "." + mTranslationEditor.language + ".json")).string();
            } else {
                targetFile = (fs::path(translationsDir) / (baseName + ".pbl-" + mTranslationEditor.language + ".json")).string();
            }

            try {
                std::ofstream f(targetFile);
                f << j.dump(4);  // Pretty print with 4-space indent
                f.close();
                mTranslationEditor.dirty = false;
                printf("Saved translations to: %s\n", targetFile.c_str());
            } catch (const std::exception& e) {
                printf("Error saving translation file: %s\n", e.what());
            }
        }

        if (!canSave) {
            ImGui::EndDisabled();
        }

        ImGui::SameLine();

        if (ImGui::Button("Close", ImVec2(100, 0))) {
            if (mTranslationEditor.dirty) {
                // TODO: Could add a confirmation dialog here
            }
            mTranslationEditor.show = false;
            mTranslationEditor.Clear();
            prevLanguage[0] = '\0';  // Reset for next time
            ImGui::CloseCurrentPopup();
        }

        // Add key button
        ImGui::SameLine(0, 20);
        if (ImGui::Button("+ Add Key", ImVec2(100, 0))) {
            ImGui::OpenPopup("Add Key");
        }

        if (ImGui::BeginPopup("Add Key")) {
            static char newKey[64] = "";
            ImGui::Text("Key name:");
            if (ImGui::IsWindowAppearing()) {
                ImGui::SetKeyboardFocusHere();
            }
            if (ImGui::InputText("##newkey", newKey, sizeof(newKey), ImGuiInputTextFlags_EnterReturnsTrue)) {
                if (strlen(newKey) > 0) {
                    std::string key(newKey);
                    // Convert to uppercase
                    for (char& c : key) c = toupper(c);
                    // Check if key already exists
                    if (mTranslationEditor.englishValues.find(key) == mTranslationEditor.englishValues.end()) {
                        mTranslationEditor.keys.push_back(key);
                        mTranslationEditor.englishValues[key] = "";
                        mTranslationEditor.targetValues[key] = "";
                        mTranslationEditor.dirty = true;
                        // Select the new key
                        mTranslationEditor.selectedKeyIndex = (int)mTranslationEditor.keys.size() - 1;
                    }
                    newKey[0] = '\0';
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndPopup();
        }

        ImGui::EndPopup();
    }
}

std::vector<std::string> LauncherUI::CheckExistingSubjectCodes()
{
    std::vector<std::string> existingCodes;

    if (!mCurrentStudy || !mCurrentChain) {
        return existingCodes;
    }

    std::string studyPath = mCurrentStudy->GetPath();
    const auto& chainItems = mCurrentChain->GetItems();

    // Collect unique list of tests in the chain
    std::vector<std::string> testsInChain;
    for (const auto& item : chainItems) {
        if (item.type == ItemType::Test) {
            // Check if not already in list
            bool found = false;
            for (const auto& testName : testsInChain) {
                if (testName == item.testName) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                testsInChain.push_back(item.testName);
            }
        }
    }

    // For each test, scan its data directory for subdirectories (subject codes)
    for (const auto& testName : testsInChain) {
        std::string dataDir = studyPath + "/tests/" + testName + "/data";

        try {
            if (!fs::exists(dataDir) || !fs::is_directory(dataDir)) {
                continue; // data directory doesn't exist yet
            }

            for (const auto& entry : fs::directory_iterator(dataDir)) {
                if (!entry.is_directory()) continue;

                std::string name = entry.path().filename().string();

                // Check if we've already added this code
                bool found = false;
                for (const auto& code : existingCodes) {
                    if (code == name) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    existingCodes.push_back(name);
                }
            }
        } catch (const fs::filesystem_error&) {
            // Directory doesn't exist or can't be read
        }
    }

    return existingCodes;
}

std::vector<std::string> LauncherUI::BuildAdditionalArguments()
{
    std::vector<std::string> args;

    // Screen resolution
    if (mScreenResolution > 0) {
        const char* resolutionStrings[] = {
            "",  // Auto
            "1920x1080",
            "1680x1050",
            "1440x900",
            "1366x768",
            "1280x1024",
            "1280x800",
            "1280x720",
            "1024x768",
            "800x600"
        };
        args.push_back("--display");
        args.push_back(resolutionStrings[mScreenResolution]);
    }

    // VSync
    if (mVSync) {
        args.push_back("--vsyncon");
    }

    // Graphics driver
    if (strlen(mGraphicsDriver) > 0) {
        args.push_back("--driver");
        args.push_back(mGraphicsDriver);
    }

    // Custom arguments - parse space-separated
    if (strlen(mCustomArguments) > 0) {
        std::string custom(mCustomArguments);
        std::istringstream iss(custom);
        std::string arg;
        while (iss >> arg) {
            args.push_back(arg);
        }
    }

    return args;
}

// ============================================================================
// Scale Builder Implementation
// ============================================================================

void LauncherUI::ShowScaleBuilder()
{
    if (!mScaleManager) {
        return;
    }

    // Main layout: left panel (scale list) + right panel (editor)
    ImGui::BeginChild("ScaleLeftPanel", ImVec2(250, 0), true);
    RenderScaleList();
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("ScaleRightPanel", ImVec2(0, 0), true);
    if (mCurrentScale) {
        // Tab bar for different editor sections
        if (ImGui::BeginTabBar("ScaleEditorTabs"))
        {
            if (ImGui::BeginTabItem("Scale Info"))
            {
                RenderScaleInfoEditor();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Questions"))
            {
                RenderQuestionsEditor();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Dimensions & Scoring"))
            {
                RenderScoringEditor();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Translations"))
            {
                RenderTranslationsEditor();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Sections"))
            {
                RenderSectionsTab();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Parameters"))
            {
                RenderParametersEditor();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    } else {
        ImGui::TextWrapped("Select a scale from the list or create a new one to begin editing.");
        ImGui::Spacing();
        if (ImGui::Button("Create New Scale")) {
            mCurrentScale = ScaleDefinition::CreateNew("newscale");
            mScaleTransLanguage[0] = '\0';
            mScaleTransSelectedKey = -1;
        }
    }
    ImGui::EndChild();
}

void LauncherUI::RenderScaleList()
{
    ImGui::Text("Available Scales");
    ImGui::Separator();

    // Row 1: New Scale | Save Scale
    float buttonWidth = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) / 2.0f;

    // New Scale button (default style)
    if (ImGui::Button("New Scale", ImVec2(buttonWidth, 0))) {
        mCurrentScale = ScaleDefinition::CreateNew("newscale");
        mSelectedScaleIndex = -1;
        mScaleTransLanguage[0] = '\0';
        mScaleTransSelectedKey = -1;
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Create a new scale definition (Ctrl+N)");
    }

    ImGui::SameLine();

    // Save Scale button (blue)
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.8f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 0.9f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.3f, 0.7f, 1.0f));

    bool canSaveScale = (mCurrentScale != nullptr);
    if (!canSaveScale) {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
    }

    if (ImGui::Button("Save Scale", ImVec2(buttonWidth, 0))) {
        if (canSaveScale) {
            if (mScaleManager->SaveScale(mCurrentScale)) {
                printf("Scale saved successfully\n");
                mScaleList = mScaleManager->GetAvailableScales();
                mLooseOSDEntries = mScaleManager->GetLooseOSDEntries();
            } else {
                printf("Error: Failed to save scale\n");
            }
        }
    }

    if (!canSaveScale) {
        ImGui::PopStyleVar();
    }
    if (ImGui::IsItemHovered() && !canSaveScale) {
        ImGui::SetTooltip("Select a scale first");
    } else if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Save the current scale definition (Ctrl+S)");
    }

    ImGui::PopStyleColor(3);

    // Row 2: Test Scale | Create Study
    // Test Scale button (orange)
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.5f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.6f, 0.1f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.4f, 0.0f, 1.0f));

    bool canTestScale = (mCurrentScale != nullptr);
    if (!canTestScale) {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
    }

    if (ImGui::Button("Test Scale", ImVec2(buttonWidth, 0))) {
        if (canTestScale) {
            TestCurrentScale();
        }
    }

    if (!canTestScale) {
        ImGui::PopStyleVar();
    }
    if (ImGui::IsItemHovered() && !canTestScale) {
        ImGui::SetTooltip("Select a scale first");
    } else if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Preview this scale in ScaleRunner (Ctrl+T)");
    }

    ImGui::PopStyleColor(3);

    ImGui::SameLine();

    // Add to Study button (green)
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.6f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.7f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.5f, 0.0f, 1.0f));

    bool canAddToStudy = (mCurrentScale != nullptr && mWorkspace != nullptr);
    if (!canAddToStudy) {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
    }

    if (ImGui::Button("Add to Study", ImVec2(buttonWidth, 0))) {
        if (canAddToStudy) {
            mCreateStudyDialog.show = true;
            mCreateStudyDialog.needScaleSelection = false;
            mCreateStudyDialog.addToExisting = false;
            mCreateStudyDialog.selectedStudyIndex = -1;
            std::strncpy(mCreateStudyDialog.studyName, mCurrentScale->GetScaleInfo().code.c_str(),
                        sizeof(mCreateStudyDialog.studyName) - 1);
            mCreateStudyDialog.studyName[sizeof(mCreateStudyDialog.studyName) - 1] = '\0';
            mCreateStudyDialog.errorMessage[0] = '\0';
            mCreateStudyDialog.confirmOverwrite = false;
            // Refresh study list for the dropdown
            mStudyList = mWorkspace->GetStudyDirectories();
        }
    }

    if (!canAddToStudy) {
        ImGui::PopStyleVar();
    }
    if (ImGui::IsItemHovered() && !canAddToStudy) {
        ImGui::SetTooltip("Select a scale first");
    } else if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Add this scale to a new or existing study");
    }

    ImGui::PopStyleColor(3);

    // Row 3: Open Test Data (always reserve space to prevent scrollbox bounce)
    {
        std::string testDataDir;
        bool hasTestDir = false;
        if (mCurrentScale && mWorkspace) {
            testDataDir = mWorkspace->GetWorkspacePath() + "/temp/scale-test-"
                          + mCurrentScale->GetScaleInfo().code + "/data";
            // Show button if the parent temp directory exists (test was run at least once)
            std::string tempDir = mWorkspace->GetWorkspacePath() + "/temp/scale-test-"
                                  + mCurrentScale->GetScaleInfo().code;
            hasTestDir = fs::exists(tempDir);
        }

        if (hasTestDir) {
            if (ImGui::Button("Open Test Data", ImVec2(-1, 0))) {
                OpenDirectoryInFileBrowser(testDataDir);
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Open test output directory: %s", testDataDir.c_str());
            }
        } else {
            // Reserve the same height as a button to prevent layout shift
            ImGui::Dummy(ImVec2(-1, ImGui::GetFrameHeight()));
        }
    }

    ImGui::Spacing();

    // Load scale list if empty
    if (mScaleList.empty()) {
        mScaleList = mScaleManager->GetAvailableScales();
    }

    // Scan for loose .osd files on first display (and after installs)
    if (!mLooseOSDEntriesLoaded) {
        mLooseOSDEntries = mScaleManager->GetLooseOSDEntries();
        mLooseOSDEntriesLoaded = true;
    }

    // Scrollable list in its own child so the buttons above stay fixed
    ImGui::BeginChild("ScaleListScroll", ImVec2(0, 0), false);

    for (size_t i = 0; i < mScaleList.size(); i++) {
        bool isSelected = (i == (size_t)mSelectedScaleIndex);
        if (ImGui::Selectable(mScaleList[i].c_str(), isSelected)) {
            mSelectedScaleIndex = (int)i;
            // Load the selected scale
            mCurrentScale = mScaleManager->LoadScale(mScaleList[i]);
            mScaleTransLanguage[0] = '\0';
            mScaleTransSelectedKey = -1;
            if (mCurrentScale) {
                printf("Loaded scale: %s\n", mScaleList[i].c_str());
            } else {
                printf("Error: Failed to load scale: %s\n", mScaleList[i].c_str());
            }
        }
    }

    // Show loose .osd files that need to be installed
    if (!mLooseOSDEntries.empty()) {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextDisabled("Uninstalled OSD files:");
        for (const auto& looseEntry : mLooseOSDEntries) {
            std::string label = "[+] " + looseEntry.name + " (" + looseEntry.code + ")";
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 0.4f, 1.0f));
            if (ImGui::Selectable(label.c_str(), false)) {
                mInstallOSDDialog.show = true;
                mInstallOSDDialog.entry = looseEntry;
                mInstallOSDDialog.errorMessage[0] = '\0';
            }
            ImGui::PopStyleColor();
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Click to install into scales/%s/", looseEntry.code.c_str());
            }
        }
    }

    ImGui::EndChild();

    // Install OSD dialog
    if (mInstallOSDDialog.show) {
        ImGui::OpenPopup("Install OSD Scale");
        mInstallOSDDialog.show = false;
    }
    if (ImGui::BeginPopupModal("Install OSD Scale", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextWrapped("Install \"%s\"?", mInstallOSDDialog.entry.name.c_str());
        ImGui::Spacing();
        ImGui::TextDisabled("Code: %s", mInstallOSDDialog.entry.code.c_str());
        ImGui::TextWrapped("The file will be moved to:");
        ImGui::TextDisabled("  scales/%s/%s.osd", mInstallOSDDialog.entry.code.c_str(),
                            mInstallOSDDialog.entry.code.c_str());
        ImGui::Spacing();
        if (mInstallOSDDialog.errorMessage[0] != '\0') {
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", mInstallOSDDialog.errorMessage);
            ImGui::Spacing();
        }
        if (ImGui::Button("Install", ImVec2(120, 0))) {
            auto scale = mScaleManager->InstallLooseOSD(mInstallOSDDialog.entry.path);
            if (scale) {
                // Refresh lists and auto-select the newly installed scale
                mScaleList = mScaleManager->GetAvailableScales();
                mLooseOSDEntries = mScaleManager->GetLooseOSDEntries();
                std::string installedCode = scale->GetScaleInfo().code;
                mCurrentScale = scale;
                mScaleTransLanguage[0] = '\0';
                mScaleTransSelectedKey = -1;
                mSelectedScaleIndex = -1;
                for (size_t i = 0; i < mScaleList.size(); i++) {
                    if (mScaleList[i] == installedCode) {
                        mSelectedScaleIndex = (int)i;
                        break;
                    }
                }
                ImGui::CloseCurrentPopup();
            } else {
                std::snprintf(mInstallOSDDialog.errorMessage,
                              sizeof(mInstallOSDDialog.errorMessage),
                              "Installation failed. Check that the file is a valid .osd bundle.");
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void LauncherUI::RenderScaleInfoEditor()
{
    if (!mCurrentScale) return;

    ImGui::Text("Basic Information");
    ImGui::Separator();
    ImGui::Spacing();

    auto& info = mCurrentScale->GetScaleInfo();

    // Scale name
    char name[256];
    std::strncpy(name, info.name.c_str(), sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';
    ImGui::TextUnformatted("Name:");
    ImGui::SetNextItemWidth(-1);
    if (ImGui::InputText("##Name", name, sizeof(name))) {
        info.name = name;
        mCurrentScale->SetDirty(true);
    }

    // Scale code
    char code[64];
    std::strncpy(code, info.code.c_str(), sizeof(code) - 1);
    code[sizeof(code) - 1] = '\0';
    ImGui::TextUnformatted("Code:");
    ImGui::SetNextItemWidth(-1);
    if (ImGui::InputText("##Code", code, sizeof(code))) {
        info.code = code;
        mCurrentScale->SetDirty(true);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Short identifier (e.g., 'grit', 'crt')");
    }

    // Abbreviation
    char abbrev[64];
    std::strncpy(abbrev, info.abbreviation.c_str(), sizeof(abbrev) - 1);
    abbrev[sizeof(abbrev) - 1] = '\0';
    ImGui::TextUnformatted("Abbreviation:");
    ImGui::SetNextItemWidth(-1);
    if (ImGui::InputText("##Abbreviation", abbrev, sizeof(abbrev))) {
        info.abbreviation = abbrev;
        mCurrentScale->SetDirty(true);
    }

    // Description
    char desc[1024];
    std::strncpy(desc, info.description.c_str(), sizeof(desc) - 1);
    desc[sizeof(desc) - 1] = '\0';
    ImGui::TextUnformatted("Description:");
    if (ImGui::InputTextMultiline("##Description", desc, sizeof(desc), ImVec2(-1, 80),
                                  ImGuiInputTextFlags_WordWrap)) {
        info.description = desc;
        mCurrentScale->SetDirty(true);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Publication Info");
    ImGui::Spacing();

    // Citation
    char citation[1024];
    std::strncpy(citation, info.citation.c_str(), sizeof(citation) - 1);
    citation[sizeof(citation) - 1] = '\0';
    ImGui::TextUnformatted("Citation:");
    if (ImGui::InputTextMultiline("##Citation", citation, sizeof(citation), ImVec2(-1, 100),
                                  ImGuiInputTextFlags_WordWrap)) {
        info.citation = citation;
        mCurrentScale->SetDirty(true);
    }

    // License
    char license[256];
    std::strncpy(license, info.license.c_str(), sizeof(license) - 1);
    license[sizeof(license) - 1] = '\0';
    ImGui::TextUnformatted("License:");
    ImGui::SetNextItemWidth(-1);
    if (ImGui::InputText("##License", license, sizeof(license))) {
        info.license = license;
        mCurrentScale->SetDirty(true);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Short label: CC BY 4.0, Public Domain, free to use, etc.");
    }

    // License explanation
    char licExpl[1024];
    std::strncpy(licExpl, info.license_explanation.c_str(), sizeof(licExpl) - 1);
    licExpl[sizeof(licExpl) - 1] = '\0';
    ImGui::TextUnformatted("License Details:");
    if (ImGui::InputTextMultiline("##LicenseExplanation", licExpl, sizeof(licExpl), ImVec2(-1, 60),
                                  ImGuiInputTextFlags_WordWrap)) {
        info.license_explanation = licExpl;
        mCurrentScale->SetDirty(true);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Full license terms or permissions grant.\nCapture the substance of the license so the record\nis self-contained even if external URLs go dead.");
    }

    // License URL
    char licUrl[512];
    std::strncpy(licUrl, info.license_url.c_str(), sizeof(licUrl) - 1);
    licUrl[sizeof(licUrl) - 1] = '\0';
    ImGui::TextUnformatted("License URL:");
    ImGui::SetNextItemWidth(-1);
    if (ImGui::InputText("##LicenseURL", licUrl, sizeof(licUrl))) {
        info.license_url = licUrl;
        mCurrentScale->SetDirty(true);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("URL documenting the license terms (e.g., CC deed, author's download page).");
    }

    // Version
    char version[32];
    std::strncpy(version, info.version.c_str(), sizeof(version) - 1);
    version[sizeof(version) - 1] = '\0';
    ImGui::TextUnformatted("Version:");
    ImGui::SetNextItemWidth(-1);
    if (ImGui::InputText("##Version", version, sizeof(version))) {
        info.version = version;
        mCurrentScale->SetDirty(true);
    }

    // URL
    char url[512];
    std::strncpy(url, info.url.c_str(), sizeof(url) - 1);
    url[sizeof(url) - 1] = '\0';
    ImGui::TextUnformatted("URL:");
    ImGui::SetNextItemWidth(-1);
    if (ImGui::InputText("##URL", url, sizeof(url))) {
        info.url = url;
        mCurrentScale->SetDirty(true);
    }

    // Domain
    char domain[128];
    std::strncpy(domain, info.domain.c_str(), sizeof(domain) - 1);
    domain[sizeof(domain) - 1] = '\0';
    ImGui::TextUnformatted("Domain:");
    ImGui::SetNextItemWidth(-1);
    if (ImGui::InputText("##Domain", domain, sizeof(domain))) {
        info.domain = domain;
        mCurrentScale->SetDirty(true);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Subject domain for classification (e.g., Mood, Substance Use, Personality, Work, Education).");
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Participant Display (English)");
    ImGui::Spacing();

    // Display Title (what participants see - optional override of scale name)
    std::string displayTitle = mCurrentScale->GetTranslation("en", "display_title");
    if (displayTitle.empty()) {
        displayTitle = info.name;  // Default to scale name
    }
    char dispTitle[256];
    std::strncpy(dispTitle, displayTitle.c_str(), sizeof(dispTitle) - 1);
    dispTitle[sizeof(dispTitle) - 1] = '\0';
    ImGui::TextUnformatted("Display Title:");
    ImGui::SetNextItemWidth(-1);
    if (ImGui::InputText("##DisplayTitle", dispTitle, sizeof(dispTitle))) {
        mCurrentScale->AddTranslation("en", "display_title", dispTitle);
        mCurrentScale->SetDirty(true);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Title shown to participants (leave blank to use scale name).\nUse this to avoid revealing the scale's purpose.");
    }

    // Instructions (question_head)
    std::string instructions = mCurrentScale->GetTranslation("en", "question_head");
    char instr[512];
    std::strncpy(instr, instructions.c_str(), sizeof(instr) - 1);
    instr[sizeof(instr) - 1] = '\0';
    ImGui::TextUnformatted("Instructions:");
    if (ImGui::InputTextMultiline("##Instructions", instr, sizeof(instr), ImVec2(-1, 60),
                                  ImGuiInputTextFlags_WordWrap)) {
        mCurrentScale->AddTranslation("en", "question_head", instr);
        mCurrentScale->SetDirty(true);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Instructions shown before each question");
    }

    // Debrief
    std::string debrief = mCurrentScale->GetTranslation("en", "debrief");
    char debr[512];
    std::strncpy(debr, debrief.c_str(), sizeof(debr) - 1);
    debr[sizeof(debr) - 1] = '\0';
    ImGui::TextUnformatted("Debrief Message:");
    if (ImGui::InputTextMultiline("##DebriefMessage", debr, sizeof(debr), ImVec2(-1, 60),
                                  ImGuiInputTextFlags_WordWrap)) {
        mCurrentScale->AddTranslation("en", "debrief", debr);
        mCurrentScale->SetDirty(true);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Message shown after completing the scale");
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Likert Scale Defaults");
    ImGui::Spacing();

    auto& likert = mCurrentScale->GetLikertOptions();

    // Default number of points
    int points = likert.points;
    if (ImGui::InputInt("Default Points", &points)) {
        if (points >= 2 && points <= 10) {
            likert.points = points;

            // Auto-populate labels if points increased beyond current label count
            size_t currentLabelCount = likert.labels.size();
            if (static_cast<size_t>(points) > currentLabelCount) {
                std::string scaleCode = mCurrentScale->GetScaleInfo().code;
                for (size_t i = currentLabelCount; i < static_cast<size_t>(points); i++) {
                    int labelNum = static_cast<int>(i) + 1;
                    std::string newKey = scaleCode + "_response_" + std::to_string(labelNum);
                    likert.labels.push_back(newKey);
                    mCurrentScale->AddTranslation("en", newKey, "Response " + std::to_string(labelNum));
                }
            }

            mCurrentScale->SetDirty(true);
        }
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Default number of response options for Likert questions");
    }

    // Default min/max values
    int minVal = likert.min;
    if (ImGui::InputInt("Default Min Value", &minVal)) {
        likert.min = minVal;
        mCurrentScale->SetDirty(true);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Default minimum value (-1 = auto: binary scales use 0, regular scales use 1)");
    }

    int maxVal = likert.max;
    if (ImGui::InputInt("Default Max Value", &maxVal)) {
        likert.max = maxVal;
        mCurrentScale->SetDirty(true);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Default maximum value (-1 = auto: points-1 for binary, points for regular)");
    }

    ImGui::Spacing();
    ImGui::Text("Response Options (available for all questions)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Define response options that questions can use.\nValues are computed from Points, Min, and Max settings above.");
    }

    // Display response options as a table
    std::vector<std::string>& labelKeys = likert.labels;

    // Calculate what the values would be for these labels
    // Binary scales (points=2): reverse order (max to min)
    // Regular scales: natural order (min to max)
    std::vector<int> values;
    int actualMin = (likert.min == -1) ? ((points == 2) ? 0 : 1) : likert.min;
    int actualMax = (likert.max == -1) ? (actualMin + points - 1) : likert.max;

    for (int i = 1; i <= points; i++) {
        int value;
        if (points == 2) {
            // Binary: reverse order
            value = actualMax - (i - 1);
        } else {
            // Regular: natural order
            value = actualMin + (i - 1);
        }
        values.push_back(value);
    }

    // Table header
    if (ImGui::BeginTable("ResponseOptionsTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Label Key", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupColumn("Label Text", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableHeadersRow();

        // Track if we need to remove any labels
        int removeIndex = -1;

        // Display rows
        for (size_t i = 0; i < labelKeys.size() && i < values.size(); i++) {
            ImGui::TableNextRow();
            ImGui::PushID(static_cast<int>(i));

            // Column 1: Value (computed, read-only)
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%d", values[i]);

            // Column 2: Label Key (editable)
            ImGui::TableSetColumnIndex(1);
            char keyBuffer[64];
            std::strncpy(keyBuffer, labelKeys[i].c_str(), sizeof(keyBuffer) - 1);
            keyBuffer[sizeof(keyBuffer) - 1] = '\0';
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::InputText("##key", keyBuffer, sizeof(keyBuffer))) {
                // Update the key (need to update both the vector and translations)
                std::string oldKey = labelKeys[i];
                std::string newKey = keyBuffer;
                std::string labelText = mCurrentScale->GetTranslation("en", oldKey);
                labelKeys[i] = newKey;
                mCurrentScale->AddTranslation("en", newKey, labelText);
                mCurrentScale->SetDirty(true);
            }

            // Column 3: Label Text (editable)
            ImGui::TableSetColumnIndex(2);
            std::string labelText = mCurrentScale->GetTranslation("en", labelKeys[i]);
            char textBuffer[256];
            std::strncpy(textBuffer, labelText.c_str(), sizeof(textBuffer) - 1);
            textBuffer[sizeof(textBuffer) - 1] = '\0';
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::InputText("##text", textBuffer, sizeof(textBuffer))) {
                mCurrentScale->AddTranslation("en", labelKeys[i], textBuffer);
                mCurrentScale->SetDirty(true);
            }

            // Column 4: Actions
            ImGui::TableSetColumnIndex(3);
            if (ImGui::SmallButton("Remove")) {
                removeIndex = static_cast<int>(i);
            }

            ImGui::PopID();
        }

        ImGui::EndTable();

        // Remove label if requested
        if (removeIndex >= 0) {
            labelKeys.erase(labelKeys.begin() + removeIndex);
            // Update points to match (since we removed an option)
            likert.points = static_cast<int>(labelKeys.size());
            mCurrentScale->SetDirty(true);
        }
    }

    // Add new option button
    if (ImGui::Button("Add Response Option")) {
        // Generate new label key using scale code
        std::string scaleCode = mCurrentScale->GetScaleInfo().code;
        int nextNum = static_cast<int>(labelKeys.size()) + 1;
        std::string newKey = scaleCode + "_response_" + std::to_string(nextNum);
        labelKeys.push_back(newKey);
        mCurrentScale->AddTranslation("en", newKey, "Response " + std::to_string(nextNum));
        // Update points to match
        likert.points = static_cast<int>(labelKeys.size());
        mCurrentScale->SetDirty(true);
    }

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Add a new response option (will update Points automatically)");
    }

    // Default Required setting
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Required Questions");
    ImGui::Spacing();

    {
        int defReq = mCurrentScale->GetDefaultRequired();
        const char* defReqItems[] = { "Per-type defaults", "All required", "All optional" };
        int defReqIdx = (defReq == -1) ? 0 : (defReq == 1 ? 1 : 2);
        if (ImGui::Combo("Default Required", &defReqIdx, defReqItems, IM_ARRAYSIZE(defReqItems))) {
            int newVal = (defReqIdx == 0) ? -1 : (defReqIdx == 1 ? 1 : 0);
            mCurrentScale->SetDefaultRequired(newVal);
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Scale-level default for whether questions must be answered.\n"
                              "Per-type defaults: scored types (likert, vas, etc.) are required,\n"
                              "text entry (short, long) is optional.\n"
                              "Individual questions can override this in their settings.");
        }
    }
}

void LauncherUI::RenderQuestionsEditor()
{
    if (!mCurrentScale) return;

    ImGui::Text("Questions");
    ImGui::SameLine();
    if (ImGui::Button("Add Question")) {
        mQuestionEditor.show = true;
        mQuestionEditor.editingIndex = -1;
        mQuestionEditor.isSection = false;
        // Auto-generate first unused q{N} ID
        {
            const auto& qs = mCurrentScale->GetQuestions();
            std::set<std::string> used;
            for (const auto& q : qs) used.insert(q.id);
            int nextNum = 1;
            while (used.count("q" + std::to_string(nextNum))) nextNum++;
            snprintf(mQuestionEditor.id, sizeof(mQuestionEditor.id), "q%d", nextNum);
        }
        mQuestionEditor.textKey[0] = '\0';
        mQuestionEditor.questionText[0] = '\0';
        mQuestionEditor.questionType = 0;  // Default to likert
        mQuestionEditor.randomGroup = 1;   // Default: all in same shuffle pool
        mQuestionEditor.requiredState = -1;  // Default: use type/scale default
        mQuestionEditor.hasVisibleWhen = false;
        mQuestionEditor.visibleWhenLogic = 0;
        mQuestionEditor.visibleWhenIsComplex = false;
        mQuestionEditor.visibleWhenConditions.clear();
        mQuestionEditor.hasGate = false;
        mQuestionEditor.gateRequiredValue[0] = '\0';
        mQuestionEditor.gateOperator = 0;
        mQuestionEditor.gateValue = 0.0;
        mQuestionEditor.gateTerminateMessageKey[0] = '\0';
        mQuestionEditor.gateTerminateMessageText[0] = '\0';
        mQuestionEditor.answerAlias[0] = '\0';
    }
    ImGui::SameLine();
    if (ImGui::Button("Batch Import...")) {
        mBatchImport.show = true;
        mBatchImport.questionText[0] = '\0';
        // Pre-fill with scale code and advance startNumber past existing IDs
        if (mCurrentScale) {
            std::strncpy(mBatchImport.idPrefix, mCurrentScale->GetScaleInfo().code.c_str(), sizeof(mBatchImport.idPrefix) - 1);
            mBatchImport.idPrefix[sizeof(mBatchImport.idPrefix) - 1] = '\0';
            std::string prefix = mBatchImport.idPrefix;
            int maxNum = 0;
            for (const auto& q : mCurrentScale->GetQuestions()) {
                if (q.id.size() > prefix.size() && q.id.substr(0, prefix.size()) == prefix) {
                    try {
                        int n = std::stoi(q.id.substr(prefix.size()));
                        if (n > maxNum) maxNum = n;
                    } catch (...) {}
                }
            }
            mBatchImport.startNumber = maxNum + 1;
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Add Section")) {
        mQuestionEditor = QuestionEditorState{};
        mQuestionEditor.show = true;
        mQuestionEditor.editingIndex = -1;
        mQuestionEditor.isSection = true;
        // Find first unused sec_{N} ID
        {
            const auto& qs = mCurrentScale->GetQuestions();
            std::set<std::string> used;
            for (const auto& q : qs) used.insert(q.id);
            int nextNum = 1;
            while (used.count("sec_" + std::to_string(nextNum))) nextNum++;
            std::snprintf(mQuestionEditor.id, sizeof(mQuestionEditor.id), "sec_%d", nextNum);
        }
    }

    ImGui::Separator();
    ImGui::Spacing();

    auto& questions = mCurrentScale->GetQuestions();
    ImGui::Text("Total questions: %zu", questions.size());
    ImGui::Spacing();

    // Question list
    if (ImGui::BeginTable("QuestionsTable", 9, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupColumn("##drag", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 18);
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 80);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 70);
        ImGui::TableSetupColumn("Req", ImGuiTableColumnFlags_WidthFixed, 25);
        ImGui::TableSetupColumn("Rand", ImGuiTableColumnFlags_WidthFixed, 40);
        ImGui::TableSetupColumn("Cond", ImGuiTableColumnFlags_WidthFixed, 30);
        ImGui::TableSetupColumn("Question Text", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Order", ImGuiTableColumnFlags_WidthFixed, 0);
        ImGui::TableSetupColumn("Edit", ImGuiTableColumnFlags_WidthFixed, 80);
        ImGui::TableHeadersRow();

        // ── Virtual "Start" row (implicit section 0, always first, non-moveable) ──
        {
            // Determine whether the first real item is a section marker.
            // If not, the implicit section covers everything before the first marker.
            bool hasLeadingSection = (!questions.empty() && questions[0].type == "section");
            ImGui::TableNextRow();
            ImGui::PushID(-999);
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,
                ImGui::GetColorU32(ImVec4(0.08f, 0.20f, 0.08f, 0.85f)));

            // Drag handle — disabled (not moveable)
            ImGui::TableNextColumn();
            ImGui::TextDisabled("  ");  // no drag handle

            // ID
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0.6f, 0.9f, 0.6f, 1.0f), "(start)");

            // Type
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0.6f, 0.9f, 0.6f, 1.0f), "[section]");

            // Req, Rand, Cond — blank
            ImGui::TableNextColumn();
            ImGui::TableNextColumn();
            ImGui::TableNextColumn();

            // Text — show note about implicit section
            ImGui::TableNextColumn();
            if (hasLeadingSection) {
                ImGui::TextDisabled("(implicit start — first section marker overrides)");
            } else {
                ImGui::TextDisabled("Implicit start section (revisable by default)");
            }

            // Order — empty (cannot be moved)
            ImGui::TableNextColumn();

            // Edit column — Edit button opens section editor for the implicit start
            ImGui::TableNextColumn();
            if (!hasLeadingSection) {
                if (ImGui::SmallButton("Edit##start")) {
                    auto& e = mQuestionEditor;
                    e = QuestionEditorState{};
                    e.show = true;
                    e.isSection = true;
                    e.isVirtualStart = true;
                    e.editingIndex = -1;
                    std::strncpy(e.id, "sec_start", sizeof(e.id) - 1);
                    e.id[sizeof(e.id) - 1] = '\0';
                    e.sectionRevisable = true;
                }
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Add an explicit section marker at the start\n"
                                      "to control Back button behavior, etc.");
            } else {
                ImGui::TextDisabled("(see row 1)");
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("The first item is already a section marker.\n"
                                      "Edit it directly in the row below.");
            }

            ImGui::PopID();
        }

        for (size_t i = 0; i < questions.size(); i++) {
            auto& q = questions[i];
            ImGui::TableNextRow();
            ImGui::PushID((int)i);

            // Section marker — render as a distinct blue-tinted row
            if (q.type == "section") {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,
                    ImGui::GetColorU32(ImVec4(0.15f, 0.25f, 0.45f, 0.85f)));

                // Drag handle column
                ImGui::TableNextColumn();
                ImGui::Selectable("::##sdh", false, ImGuiSelectableFlags_AllowOverlap);
                if (ImGui::IsItemHovered())
                    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
                if (ImGui::BeginDragDropSource()) {
                    int dragIdx = (int)i;
                    ImGui::SetDragDropPayload("QUESTION_ROW", &dragIdx, sizeof(int));
                    ImGui::Text("Move: %s", q.id.c_str());
                    ImGui::EndDragDropSource();
                }
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* pl = ImGui::AcceptDragDropPayload("QUESTION_ROW")) {
                        int srcIdx = *(const int*)pl->Data;
                        if (srcIdx != (int)i) {
                            mCurrentScale->MoveQuestion(srcIdx, (int)i);
                            mCurrentScale->SetDirty(true);
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                // ID column
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(q.id.c_str());

                // Type column
                ImGui::TableNextColumn();
                ImGui::TextColored(ImVec4(0.7f, 0.85f, 1.0f, 1.0f), "[section]");

                // Req column — blank
                ImGui::TableNextColumn();

                // Rand column — blank
                ImGui::TableNextColumn();

                // Cond column — show "S" if has visible_when; "NR" if not revisable
                ImGui::TableNextColumn();
                if (q.has_visible_when) {
                    ImGui::TextUnformatted("S");
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Section has a visible_when condition");
                }
                if (!q.revisable) {
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "NR");
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Not revisable — Back button disabled in this section");
                }

                // Question Text column — show translated title if set
                ImGui::TableNextColumn();
                if (!q.text_key.empty() && mCurrentScale) {
                    std::string title = mCurrentScale->GetTranslation("en", q.text_key);
                    if (!title.empty())
                        ImGui::TextUnformatted(title.c_str());
                }

                // Order column — move up/down
                ImGui::TableNextColumn();
                {
                    bool canMoveUp   = (i > 0);
                    bool canMoveDown = (i < questions.size() - 1);
                    if (!canMoveUp) ImGui::BeginDisabled();
                    if (ImGui::SmallButton("^##sup")) {
                        mCurrentScale->MoveQuestion((int)i, (int)i - 1);
                        mCurrentScale->SetDirty(true);
                    }
                    if (!canMoveUp) ImGui::EndDisabled();
                    ImGui::SameLine();
                    if (!canMoveDown) ImGui::BeginDisabled();
                    if (ImGui::SmallButton("v##sdn")) {
                        mCurrentScale->MoveQuestion((int)i, (int)i + 1);
                        mCurrentScale->SetDirty(true);
                    }
                    if (!canMoveDown) ImGui::EndDisabled();
                }

                // Edit / Delete column
                ImGui::TableNextColumn();
                if (ImGui::SmallButton("Edit##s")) {
                    auto& e = mQuestionEditor;
                    e = QuestionEditorState{};
                    e.show = true;
                    e.editingIndex = (int)i;
                    e.isSection = true;
                    std::strncpy(e.id, q.id.c_str(), sizeof(e.id) - 1);
                    e.id[sizeof(e.id) - 1] = '\0';
                    if (!q.text_key.empty() && mCurrentScale) {
                        std::string titleText = mCurrentScale->GetTranslation("en", q.text_key);
                        std::strncpy(e.questionText, titleText.c_str(), sizeof(e.questionText) - 1);
                        e.questionText[sizeof(e.questionText) - 1] = '\0';
                    }
                    e.hasVisibleWhen = q.has_visible_when;
                    e.visibleWhenLogic = (q.visible_when_logic == "any") ? 1 : 0;
                    e.visibleWhenIsComplex = q.visible_when_is_complex;
                    e.visibleWhenConditions.clear();
                    for (const auto& c : q.visible_when_simple) {
                        EditorCondition ec;
                        ec.sourceType = (c.source_type == "item") ? 1 : 0;
                        std::strncpy(ec.sourceName, c.source_name.c_str(), sizeof(ec.sourceName) - 1);
                        ec.sourceName[sizeof(ec.sourceName) - 1] = '\0';
                        if (c.op == "not_equals") ec.op = 1;
                        else if (c.op == "greater_than") ec.op = 2;
                        else if (c.op == "less_than") ec.op = 3;
                        else if (c.op == "in") ec.op = 4;
                        else if (c.op == "not_in") ec.op = 5;
                        else ec.op = 0;
                        if (c.is_list) {
                            std::string joined;
                            for (size_t vi = 0; vi < c.values.size(); vi++) {
                                if (vi) joined += ",";
                                joined += c.values[vi];
                            }
                            std::strncpy(ec.value, joined.c_str(), sizeof(ec.value) - 1);
                        } else {
                            std::strncpy(ec.value, c.value.c_str(), sizeof(ec.value) - 1);
                        }
                        ec.value[sizeof(ec.value) - 1] = '\0';
                        e.visibleWhenConditions.push_back(ec);
                    }
                    e.sectionRevisable = q.revisable;
                    e.sectionRandomize = q.section_randomize;
                    std::string fixedStr;
                    for (size_t fi = 0; fi < q.section_randomize_fixed.size(); fi++) {
                        if (fi) fixedStr += ",";
                        fixedStr += q.section_randomize_fixed[fi];
                    }
                    std::strncpy(e.sectionRandomizeFixed, fixedStr.c_str(), sizeof(e.sectionRandomizeFixed) - 1);
                    e.sectionRandomizeFixed[sizeof(e.sectionRandomizeFixed) - 1] = '\0';
                }
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.9f, 0.1f, 0.1f, 1.0f));
                if (ImGui::SmallButton("Del##s")) {
                    mDeleteConfirmIndex = (int)i;
                    ImGui::OpenPopup("Confirm Delete");
                }
                ImGui::PopStyleColor(3);

                ImGui::PopID();
                continue;
            }

            // Drag handle column
            ImGui::TableNextColumn();
            ImGui::Selectable("::##qdh", false, ImGuiSelectableFlags_AllowOverlap);
            if (ImGui::IsItemHovered())
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
            if (ImGui::BeginDragDropSource()) {
                int dragIdx = (int)i;
                ImGui::SetDragDropPayload("QUESTION_ROW", &dragIdx, sizeof(int));
                ImGui::Text("Move: %s", q.id.c_str());
                ImGui::EndDragDropSource();
            }
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* pl = ImGui::AcceptDragDropPayload("QUESTION_ROW")) {
                    int srcIdx = *(const int*)pl->Data;
                    if (srcIdx != (int)i) {
                        mCurrentScale->MoveQuestion(srcIdx, (int)i);
                        mCurrentScale->SetDirty(true);
                    }
                }
                ImGui::EndDragDropTarget();
            }

            // ID column
            ImGui::TableNextColumn();
            ImGui::Text("%s", q.id.c_str());

            // Type column
            ImGui::TableNextColumn();
            ImGui::Text("%s", q.type.c_str());

            // Req (required) column - clickable toggle: default -> required -> optional -> default
            ImGui::TableNextColumn();
            {
                bool isDisplayOnly = (q.type == "inst" || q.type == "image");
                if (!isDisplayOnly) {
                    // Resolve display symbol and tooltip
                    std::string symbol;
                    std::string tooltipText;
                    if (q.required_state == 1) {
                        symbol = "+";
                        tooltipText = "Required (explicit)\nClick to toggle";
                    } else if (q.required_state == 0) {
                        symbol = "-";
                        tooltipText = "Optional (explicit)\nClick to toggle";
                    } else {
                        // Resolve effective from scale/type default
                        int scaleDef = mCurrentScale->GetDefaultRequired();
                        bool effectiveRequired;
                        if (scaleDef == 1) {
                            effectiveRequired = true;
                            tooltipText = "Required (scale default)\nClick to toggle";
                        } else if (scaleDef == 0) {
                            effectiveRequired = false;
                            tooltipText = "Optional (scale default)\nClick to toggle";
                        } else {
                            effectiveRequired = (q.type != "short" && q.type != "long");
                            tooltipText = effectiveRequired ? "Required (type default)\nClick to toggle" : "Optional (type default)\nClick to toggle";
                        }
                        symbol = effectiveRequired ? "(+)" : "(-)";
                    }
                    ImGui::PushID(static_cast<int>(i * 100 + 99));
                    float colWidth = ImGui::GetColumnWidth();
                    float textWidth = ImGui::CalcTextSize(symbol.c_str()).x;
                    float indent = (colWidth - textWidth) * 0.5f;
                    if (indent > 0.0f) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
                    if (ImGui::Selectable(symbol.c_str(), false, 0, ImVec2(colWidth, 0))) {
                        // Cycle: -1 (default) -> 1 (required) -> 0 (optional) -> -1 (default)
                        if (q.required_state == -1) {
                            q.required_state = 1;
                        } else if (q.required_state == 1) {
                            q.required_state = 0;
                        } else {
                            q.required_state = -1;
                        }
                        mCurrentScale->SetDirty(true);
                    }
                    ImGui::PopID();
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("%s", tooltipText.c_str());
                    }
                }
            }

            // Rand (randomization group) column
            ImGui::TableNextColumn();
            ImGui::PushItemWidth(40);
            int rg = q.random_group;
            if (ImGui::InputInt("##rg", &rg, 0, 0)) {
                if (rg < 0) rg = 0;
                q.random_group = rg;
                mCurrentScale->SetDirty(true);
            }
            ImGui::PopItemWidth();
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Randomization group\n0 = fixed position\n1+ = shuffle within group");
            }

            // Cond (condition indicator) column
            ImGui::TableNextColumn();
            {
                std::string condLabel;
                std::string condTooltip;
                bool hasP = false, hasQ = false, hasD = false;
                for (const auto& c : q.visible_when_simple) {
                    if (c.source_type == "parameter") hasP = true;
                    else hasQ = true;
                }
                // Check dimension-level visible_when
                if (!q.dimension.empty()) {
                    for (const auto& dim : mCurrentScale->GetDimensions()) {
                        if (dim.id == q.dimension && dim.has_visible_when) {
                            hasD = true;
                            break;
                        }
                    }
                }
                if (q.has_visible_when && q.visible_when_is_complex) {
                    condLabel = "*";
                    condTooltip = "Complex nested condition (edit in code)";
                } else {
                    if (hasP) condLabel += "P";
                    if (hasQ) condLabel += "Q";
                    if (hasD) condLabel += "D";
                }
                if (!condLabel.empty()) {
                    ImGui::TextUnformatted(condLabel.c_str());
                    if (ImGui::IsItemHovered()) {
                        if (condTooltip.empty()) {
                            condTooltip = "";
                            if (hasP) condTooltip += "P = parameter condition\n";
                            if (hasQ) condTooltip += "Q = question condition\n";
                            if (hasD) condTooltip += "D = dimension condition";
                        }
                        ImGui::SetTooltip("%s", condTooltip.c_str());
                    }
                }
            }

            // Question Text column (truncated with tooltip)
            ImGui::TableNextColumn();
            std::string questionText = mCurrentScale->GetTranslation("en", q.text_key);
            if (questionText.empty()) {
                questionText = "[" + q.text_key + "]";
            }

            // Truncate to 60 characters
            std::string displayText = questionText;
            if (displayText.length() > 60) {
                displayText = displayText.substr(0, 57) + "...";
            }

            ImGui::TextWrapped("%s", displayText.c_str());
            if (ImGui::IsItemHovered() && questionText.length() > 60) {
                ImGui::BeginTooltip();
                ImGui::PushTextWrapPos(400.0f);
                ImGui::TextWrapped("%s", questionText.c_str());
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }

            // Order (move up/down) column
            ImGui::TableNextColumn();
            {
                bool canMoveUp = (i > 0);
                bool canMoveDown = (i < questions.size() - 1);
                if (!canMoveUp) ImGui::BeginDisabled();
                if (ImGui::SmallButton("^##up")) {
                    mCurrentScale->MoveQuestion((int)i, (int)i - 1);
                    mCurrentScale->SetDirty(true);
                }
                if (!canMoveUp) ImGui::EndDisabled();
                ImGui::SameLine();
                if (!canMoveDown) ImGui::BeginDisabled();
                if (ImGui::SmallButton("v##dn")) {
                    mCurrentScale->MoveQuestion((int)i, (int)i + 1);
                    mCurrentScale->SetDirty(true);
                }
                if (!canMoveDown) ImGui::EndDisabled();
            }

            // Edit / Delete column
            ImGui::TableNextColumn();
            if (ImGui::SmallButton("Edit##q")) {
                printf("Edit question %s\n", q.id.c_str());
                // Open question editor with current values
                mQuestionEditor.show = true;
                mQuestionEditor.editingIndex = (int)i;
                mQuestionEditor.isSection = false;
                std::strncpy(mQuestionEditor.id, q.id.c_str(), sizeof(mQuestionEditor.id) - 1);
                mQuestionEditor.id[sizeof(mQuestionEditor.id) - 1] = '\0';
                std::strncpy(mQuestionEditor.textKey, q.text_key.c_str(), sizeof(mQuestionEditor.textKey) - 1);
                mQuestionEditor.textKey[sizeof(mQuestionEditor.textKey) - 1] = '\0';

                // Load question text from translation (English)
                std::string qText = mCurrentScale->GetTranslation("en", q.text_key);
                std::strncpy(mQuestionEditor.questionText, qText.c_str(), sizeof(mQuestionEditor.questionText) - 1);
                mQuestionEditor.questionText[sizeof(mQuestionEditor.questionText) - 1] = '\0';

                // Set question type index
                const char* questionTypes[] = { "likert", "multi", "short", "long", "vas", "inst", "multicheck", "grid", "image", "imageresponse" };
                for (int ti = 0; ti < 10; ti++) {
                    if (q.type == questionTypes[ti]) {
                        mQuestionEditor.questionType = ti;
                        break;
                    }
                }

                // Load randomization group and required state
                mQuestionEditor.randomGroup = q.random_group;
                mQuestionEditor.requiredState = q.required_state;

                // Load validation — per-constraint fields
                {
                    auto loadErr = [&](const std::string& key, char* buf, size_t sz) {
                        std::string txt = key.empty() ? "" : (mCurrentScale ? mCurrentScale->GetTranslation("en", key) : "");
                        strncpy(buf, txt.c_str(), sz - 1); buf[sz - 1] = '\0';
                    };
                    const auto& val = q.validation;
                    auto& e = mQuestionEditor;
                    e.valMinLengthEnabled  = val.min_length  >= 0; e.valMinLength  = val.min_length  >= 0 ? val.min_length  : 0;
                    e.valMaxLengthEnabled  = val.max_length  >= 0; e.valMaxLength  = val.max_length  >= 0 ? val.max_length  : 0;
                    e.valMinWordsEnabled   = val.min_words   >= 0; e.valMinWords   = val.min_words   >= 0 ? val.min_words   : 0;
                    e.valMaxWordsEnabled   = val.max_words   >= 0; e.valMaxWords   = val.max_words   >= 0 ? val.max_words   : 0;
                    e.valNumberMinEnabled  = val.number_min_set; e.valNumberMin = val.number_min;
                    e.valNumberMaxEnabled  = val.number_max_set; e.valNumberMax = val.number_max;
                    e.valPatternEnabled    = !val.pattern.empty();
                    strncpy(e.valPattern, val.pattern.c_str(), sizeof(e.valPattern) - 1); e.valPattern[sizeof(e.valPattern)-1] = '\0';
                    e.valMinSelectedEnabled = val.min_selected >= 0; e.valMinSelected = val.min_selected >= 0 ? val.min_selected : 0;
                    e.valMaxSelectedEnabled = val.max_selected >= 0; e.valMaxSelected = val.max_selected >= 0 ? val.max_selected : 0;
                    loadErr(val.min_length_error,   e.valMinLengthError,   sizeof(e.valMinLengthError));
                    loadErr(val.max_length_error,   e.valMaxLengthError,   sizeof(e.valMaxLengthError));
                    loadErr(val.min_words_error,    e.valMinWordsError,    sizeof(e.valMinWordsError));
                    loadErr(val.max_words_error,    e.valMaxWordsError,    sizeof(e.valMaxWordsError));
                    loadErr(val.number_min_error,   e.valNumberMinError,   sizeof(e.valNumberMinError));
                    loadErr(val.number_max_error,   e.valNumberMaxError,   sizeof(e.valNumberMaxError));
                    loadErr(val.pattern_error,      e.valPatternError,     sizeof(e.valPatternError));
                    loadErr(val.min_selected_error, e.valMinSelectedError, sizeof(e.valMinSelectedError));
                    loadErr(val.max_selected_error, e.valMaxSelectedError, sizeof(e.valMaxSelectedError));
                }

                // Load answer alias (S3 answer piping)
                strncpy(mQuestionEditor.answerAlias, q.answer_alias.c_str(), sizeof(mQuestionEditor.answerAlias) - 1);
                mQuestionEditor.answerAlias[sizeof(mQuestionEditor.answerAlias) - 1] = '\0';

                // Load gate (blocking)
                mQuestionEditor.hasGate = q.has_gate;
                strncpy(mQuestionEditor.gateRequiredValue, q.gate_required_value.c_str(), sizeof(mQuestionEditor.gateRequiredValue) - 1);
                mQuestionEditor.gateRequiredValue[sizeof(mQuestionEditor.gateRequiredValue) - 1] = '\0';
                // Load operator form (short questions)
                {
                    const char* opNames[] = { "greater_than", "less_than", "equals", "not_equals" };
                    mQuestionEditor.gateOperator = 0;
                    for (int oi = 0; oi < 4; ++oi) {
                        if (q.gate_operator == opNames[oi]) { mQuestionEditor.gateOperator = oi; break; }
                    }
                }
                mQuestionEditor.gateValue = q.gate_value;
                strncpy(mQuestionEditor.gateTerminateMessageKey, q.gate_terminate_message_key.c_str(), sizeof(mQuestionEditor.gateTerminateMessageKey) - 1);
                mQuestionEditor.gateTerminateMessageKey[sizeof(mQuestionEditor.gateTerminateMessageKey) - 1] = '\0';
                {
                    std::string msgText = (mCurrentScale && !q.gate_terminate_message_key.empty())
                        ? mCurrentScale->GetTranslation("en", q.gate_terminate_message_key) : "";
                    strncpy(mQuestionEditor.gateTerminateMessageText, msgText.c_str(), sizeof(mQuestionEditor.gateTerminateMessageText) - 1);
                    mQuestionEditor.gateTerminateMessageText[sizeof(mQuestionEditor.gateTerminateMessageText) - 1] = '\0';
                }

                // Load conditional display
                mQuestionEditor.hasVisibleWhen = q.has_visible_when;
                mQuestionEditor.visibleWhenLogic = (q.visible_when_logic == "any") ? 1 : 0;
                mQuestionEditor.visibleWhenIsComplex = q.visible_when_is_complex;
                mQuestionEditor.visibleWhenConditions.clear();
                for (const auto& c : q.visible_when_simple) {
                    EditorCondition ec;
                    ec.sourceType = (c.source_type == "item") ? 1 : 0;
                    strncpy(ec.sourceName, c.source_name.c_str(), sizeof(ec.sourceName) - 1);
                    ec.sourceName[sizeof(ec.sourceName) - 1] = '\0';
                    // Map operator string to index
                    if (c.op == "not_equals") ec.op = 1;
                    else if (c.op == "greater_than") ec.op = 2;
                    else if (c.op == "less_than") ec.op = 3;
                    else if (c.op == "in") ec.op = 4;
                    else if (c.op == "not_in") ec.op = 5;
                    else ec.op = 0;  // equals
                    if (c.is_list) {
                        std::string joined;
                        for (size_t vi = 0; vi < c.values.size(); vi++) {
                            if (vi) joined += ",";
                            joined += c.values[vi];
                        }
                        strncpy(ec.value, joined.c_str(), sizeof(ec.value) - 1);
                    } else {
                        strncpy(ec.value, c.value.c_str(), sizeof(ec.value) - 1);
                    }
                    ec.value[sizeof(ec.value) - 1] = '\0';
                    mQuestionEditor.visibleWhenConditions.push_back(ec);
                }

                // Load Likert-specific fields
                mQuestionEditor.likertPoints = q.likert_points;
                mQuestionEditor.likertMin = q.likert_min;
                mQuestionEditor.likertMax = q.likert_max;
                mQuestionEditor.likertReverse = q.likert_reverse;

                // Load VAS-specific fields
                mQuestionEditor.vasMinValue = q.min_value;
                mQuestionEditor.vasMaxValue = q.max_value;
                std::strncpy(mQuestionEditor.vasLeftLabel, q.left_label.c_str(), sizeof(mQuestionEditor.vasLeftLabel) - 1);
                mQuestionEditor.vasLeftLabel[sizeof(mQuestionEditor.vasLeftLabel) - 1] = '\0';
                std::strncpy(mQuestionEditor.vasRightLabel, q.right_label.c_str(), sizeof(mQuestionEditor.vasRightLabel) - 1);
                mQuestionEditor.vasRightLabel[sizeof(mQuestionEditor.vasRightLabel) - 1] = '\0';

                // Load Multi/multicheck-specific fields (join options with newlines)
                std::string optionsText;
                for (size_t oi = 0; oi < q.options.size(); oi++) {
                    if (oi > 0) optionsText += "\n";
                    optionsText += q.options[oi];
                }
                std::strncpy(mQuestionEditor.multiOptions, optionsText.c_str(), sizeof(mQuestionEditor.multiOptions) - 1);
                mQuestionEditor.multiOptions[sizeof(mQuestionEditor.multiOptions) - 1] = '\0';

                // Load Grid-specific fields
                std::string columnsText;
                for (size_t ci = 0; ci < q.columns.size(); ci++) {
                    if (ci > 0) columnsText += "\n";
                    columnsText += q.columns[ci];
                }
                std::strncpy(mQuestionEditor.gridColumns, columnsText.c_str(), sizeof(mQuestionEditor.gridColumns) - 1);
                mQuestionEditor.gridColumns[sizeof(mQuestionEditor.gridColumns) - 1] = '\0';

                std::string rowsText;
                for (size_t ri = 0; ri < q.rows.size(); ri++) {
                    if (ri > 0) rowsText += "\n";
                    rowsText += q.rows[ri];
                }
                std::strncpy(mQuestionEditor.gridRows, rowsText.c_str(), sizeof(mQuestionEditor.gridRows) - 1);
                mQuestionEditor.gridRows[sizeof(mQuestionEditor.gridRows) - 1] = '\0';

                // Load Image-specific fields
                std::strncpy(mQuestionEditor.imagePath, q.image.c_str(), sizeof(mQuestionEditor.imagePath) - 1);
                mQuestionEditor.imagePath[sizeof(mQuestionEditor.imagePath) - 1] = '\0';
            }
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.9f, 0.1f, 0.1f, 1.0f));
            if (ImGui::SmallButton("Del##q")) {
                mDeleteConfirmIndex = (int)i;
                ImGui::OpenPopup("Confirm Delete");
            }
            ImGui::PopStyleColor(3);

            ImGui::PopID();
        }

        // Deletion confirmation modal — rendered outside the loop so index is stable
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("Confirm Delete", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            if (mDeleteConfirmIndex >= 0 && mDeleteConfirmIndex < (int)questions.size()) {
                const auto& dq = questions[mDeleteConfirmIndex];
                ImGui::Text("Delete '%s' (%s)?", dq.id.c_str(), dq.type.c_str());
                ImGui::Spacing();
                ImGui::TextDisabled("Translation text is kept in the language file.");
                ImGui::TextDisabled("It can be reused if an item with the same ID is added later.");
                ImGui::Spacing();
                ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.7f, 0.1f, 0.1f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(1.0f, 0.1f, 0.1f, 1.0f));
                if (ImGui::Button("Delete", ImVec2(100, 0))) {
                    questions.erase(questions.begin() + mDeleteConfirmIndex);
                    mCurrentScale->SetDirty(true);
                    mDeleteConfirmIndex = -1;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::PopStyleColor(3);
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(100, 0))) {
                    mDeleteConfirmIndex = -1;
                    ImGui::CloseCurrentPopup();
                }
            } else {
                mDeleteConfirmIndex = -1;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::EndTable();
    }
}

void LauncherUI::RenderScoringEditor()
{
    if (!mCurrentScale) return;

    auto& dimensions = mCurrentScale->GetDimensions();
    auto& scoring = mCurrentScale->GetScoring();
    auto& questions = mCurrentScale->GetQuestions();

    if (dimensions.empty()) {
        ImGui::TextWrapped("No dimensions defined yet. Add a dimension to configure scoring.");
        ImGui::Spacing();
        if (ImGui::Button("Add Dimension")) {
            mDimensionEditor.show = true;
            mDimensionEditor.editingIndex = -1;
            mDimensionEditor.id[0] = '\0';
            mDimensionEditor.name[0] = '\0';
            mDimensionEditor.abbreviation[0] = '\0';
            mDimensionEditor.description[0] = '\0';
            mDimensionEditor.selectable = false;
            mDimensionEditor.defaultEnabled = true;
            mDimensionEditor.enabledParam[0] = '\0';
            mDimensionEditor.hasVisibleWhen = false;
            mDimensionEditor.visibleWhenLogic = 0;
            mDimensionEditor.visibleWhenConditions.clear();
        }
        return;
    }

    // Two-column layout: Dimensions list on left, scoring on right
    ImGui::BeginChild("DimensionList", ImVec2(200, 0), true);

    if (ImGui::Button("Add", ImVec2(-1, 0))) {
        mDimensionEditor.show = true;
        mDimensionEditor.editingIndex = -1;
        mDimensionEditor.id[0] = '\0';
        mDimensionEditor.name[0] = '\0';
        mDimensionEditor.abbreviation[0] = '\0';
        mDimensionEditor.description[0] = '\0';
        mDimensionEditor.selectable = false;
        mDimensionEditor.defaultEnabled = true;
        mDimensionEditor.enabledParam[0] = '\0';
        mDimensionEditor.hasVisibleWhen = false;
        mDimensionEditor.visibleWhenLogic = 0;
        mDimensionEditor.visibleWhenConditions.clear();
    }
    ImGui::Separator();

    // List all dimensions
    for (size_t i = 0; i < dimensions.size(); i++) {
        bool isSelected = (i == static_cast<size_t>(mSelectedDimensionIndex));
        if (ImGui::Selectable(dimensions[i].name.c_str(), isSelected)) {
            mSelectedDimensionIndex = static_cast<int>(i);
        }
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // Right panel: dimension info + scoring for selected dimension
    ImGui::BeginChild("ItemSelection", ImVec2(0, 0), true);

    if (mSelectedDimensionIndex >= 0 && mSelectedDimensionIndex < static_cast<int>(dimensions.size())) {
        const auto& selectedDim = dimensions[mSelectedDimensionIndex];

        // Dimension info header
        ImGui::Text("%s", selectedDim.name.c_str());
        ImGui::SameLine();
        ImGui::TextDisabled("(%s)", selectedDim.id.c_str());
        ImGui::SameLine();

        // Edit button
        if (ImGui::SmallButton("Edit##dim")) {
            mDimensionEditor.show = true;
            mDimensionEditor.editingIndex = mSelectedDimensionIndex;
            strncpy(mDimensionEditor.id, selectedDim.id.c_str(), sizeof(mDimensionEditor.id) - 1);
            strncpy(mDimensionEditor.name, selectedDim.name.c_str(), sizeof(mDimensionEditor.name) - 1);
            strncpy(mDimensionEditor.abbreviation, selectedDim.abbreviation.c_str(), sizeof(mDimensionEditor.abbreviation) - 1);
            strncpy(mDimensionEditor.description, selectedDim.description.c_str(), sizeof(mDimensionEditor.description) - 1);

            // Load selectable/enable param
            mDimensionEditor.selectable = selectedDim.selectable;
            mDimensionEditor.defaultEnabled = selectedDim.default_enabled;
            strncpy(mDimensionEditor.enabledParam, selectedDim.enabled_param.c_str(), sizeof(mDimensionEditor.enabledParam) - 1);
            mDimensionEditor.enabledParam[sizeof(mDimensionEditor.enabledParam) - 1] = '\0';

            // Load conditional display
            mDimensionEditor.hasVisibleWhen = selectedDim.has_visible_when;
            mDimensionEditor.visibleWhenLogic = (selectedDim.visible_when_logic == "any") ? 1 : 0;
            mDimensionEditor.visibleWhenConditions.clear();
            for (const auto& c : selectedDim.visible_when) {
                EditorCondition ec;
                ec.sourceType = (c.source_type == "item") ? 1 : 0;
                strncpy(ec.sourceName, c.source_name.c_str(), sizeof(ec.sourceName) - 1);
                ec.sourceName[sizeof(ec.sourceName) - 1] = '\0';
                if (c.op == "not_equals") ec.op = 1;
                else if (c.op == "greater_than") ec.op = 2;
                else if (c.op == "less_than") ec.op = 3;
                else if (c.op == "in") ec.op = 4;
                else if (c.op == "not_in") ec.op = 5;
                else ec.op = 0;
                if (c.is_list) {
                    std::string joined;
                    for (size_t vi = 0; vi < c.values.size(); vi++) {
                        if (vi) joined += ",";
                        joined += c.values[vi];
                    }
                    strncpy(ec.value, joined.c_str(), sizeof(ec.value) - 1);
                } else {
                    strncpy(ec.value, c.value.c_str(), sizeof(ec.value) - 1);
                }
                ec.value[sizeof(ec.value) - 1] = '\0';
                mDimensionEditor.visibleWhenConditions.push_back(ec);
            }
        }
        ImGui::SameLine();

        // Delete button
        if (ImGui::SmallButton("Delete##dim")) {
            // Remove scoring for this dimension
            scoring.erase(selectedDim.id);
            // Remove the dimension itself
            mCurrentScale->GetDimensions().erase(mCurrentScale->GetDimensions().begin() + mSelectedDimensionIndex);
            mCurrentScale->SetDirty(true);
            if (mSelectedDimensionIndex >= static_cast<int>(mCurrentScale->GetDimensions().size())) {
                mSelectedDimensionIndex = static_cast<int>(mCurrentScale->GetDimensions().size()) - 1;
            }
            ImGui::EndChild();
            return;
        }

        // Show abbreviation and description if present
        if (!selectedDim.abbreviation.empty() || !selectedDim.description.empty()) {
            if (!selectedDim.abbreviation.empty()) {
                ImGui::TextDisabled("Abbrev: %s", selectedDim.abbreviation.c_str());
                if (!selectedDim.description.empty()) {
                    ImGui::SameLine();
                    ImGui::TextDisabled("  |  %s", selectedDim.description.c_str());
                }
            } else {
                ImGui::TextDisabled("%s", selectedDim.description.c_str());
            }
        }

        ImGui::Separator();
        ImGui::Spacing();

        // Get or create scoring for this dimension
        if (scoring.find(selectedDim.id) == scoring.end()) {
            DimensionScoring newScoring;
            newScoring.method = "mean_coded";
            newScoring.description = "";
            scoring[selectedDim.id] = newScoring;
        }

        auto& dimScoring = scoring[selectedDim.id];

        // Scoring method dropdown
        const char* methodOptions[] = { "mean_coded", "sum_coded", "weighted_sum", "weighted_mean", "sum_correct" };
        int methodCount = 5;
        int currentMethod = 0;
        for (int i = 0; i < methodCount; i++) {
            if (dimScoring.method == methodOptions[i]) {
                currentMethod = i;
                break;
            }
        }

        if (ImGui::Combo("Scoring Method", &currentMethod, methodOptions, methodCount)) {
            dimScoring.method = methodOptions[currentMethod];
            mCurrentScale->SetDirty(true);
        }

        bool isSumCorrect = (dimScoring.method == "sum_correct");
        bool isWeighted   = (dimScoring.method == "weighted_sum" || dimScoring.method == "weighted_mean");

        ImGui::Spacing();
        if (isSumCorrect) {
            ImGui::Text("Select items and set correct answers:");
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::PushTextWrapPos(400.0f);
                ImGui::TextWrapped("Enter acceptable answers separated by | (pipe).\n"
                                   "Matching is case-insensitive with trimmed whitespace.\n"
                                   "Wildcards: * = any characters, ? = any single character.\n"
                                   "Example: 5|five|.05|0.05|$0.05|5 cents|*5*cent*");
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }
        } else if (isWeighted) {
            ImGui::Text("Select items, set coding and weights:");
        } else {
            ImGui::Text("Select items and set coding:");
        }

        // Norms and Transform buttons — compact, on the same visual line
        {
            // Transform button
            std::string transformBtn = dimScoring.transform.empty()
                ? "Transform..."
                : ("Transform (" + std::to_string(dimScoring.transform.size()) + ")");

            // Norms button
            std::string normsBtn = dimScoring.norms.empty()
                ? "Norms..."
                : ("Norms (" + std::to_string(dimScoring.norms.size()) + ")");

            float btnWidth1 = ImGui::CalcTextSize(normsBtn.c_str()).x + ImGui::GetStyle().FramePadding.x * 2;
            float btnWidth2 = ImGui::CalcTextSize(transformBtn.c_str()).x + ImGui::GetStyle().FramePadding.x * 2;
            float spacing = ImGui::GetStyle().ItemSpacing.x;
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - btnWidth1 - btnWidth2 - spacing * 2);

            if (ImGui::SmallButton(transformBtn.c_str())) {
                ImGui::OpenPopup("TransformEditor");
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Add arithmetic transform steps to rescale the raw score.\n"
                                  "Steps are applied in order (e.g., subtract min, divide by range, multiply by 100).");
            }
            ImGui::SameLine();
            if (ImGui::SmallButton(normsBtn.c_str())) {
                mNormsEditor.show = true;
                mNormsEditor.dimensionId = selectedDim.id;
                mNormsEditor.dimensionName = selectedDim.name;
                mNormsEditor.rows.clear();
                for (const auto& t : dimScoring.norms) {
                    NormsEditorState::ThresholdEdit te;
                    te.minVal = (float)t.min;
                    te.maxVal = (float)t.max;
                    strncpy(te.label, t.label.c_str(), sizeof(te.label) - 1);
                    te.label[sizeof(te.label) - 1] = '\0';
                    mNormsEditor.rows.push_back(te);
                }
            }

            // Transform editor popup
            if (ImGui::BeginPopup("TransformEditor")) {
                ImGui::Text("Score Transform Steps");
                ImGui::SameLine();
                ImGui::TextDisabled("(?)");
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Each step applies an arithmetic operation to the running score.\n"
                                      "Example: to rescale a sum (range 10-50) to 0-100:\n"
                                      "  1. subtract 10\n"
                                      "  2. divide   40\n"
                                      "  3. multiply 100");
                }
                ImGui::Separator();

                int removeIdx = -1;
                const char* ops[] = { "add", "subtract", "multiply", "divide" };
                for (int ti = 0; ti < (int)dimScoring.transform.size(); ti++) {
                    auto& step = dimScoring.transform[ti];
                    ImGui::PushID(ti);

                    // Step number
                    ImGui::Text("%d.", ti + 1);
                    ImGui::SameLine();

                    // Op dropdown
                    int opIdx = 0;
                    for (int oi = 0; oi < 4; oi++) {
                        if (step.op == ops[oi]) { opIdx = oi; break; }
                    }
                    ImGui::PushItemWidth(100);
                    if (ImGui::Combo("##op", &opIdx, ops, 4)) {
                        step.op = ops[opIdx];
                        mCurrentScale->SetDirty(true);
                    }
                    ImGui::PopItemWidth();
                    ImGui::SameLine();

                    // Value
                    float val = (float)step.value;
                    ImGui::PushItemWidth(100);
                    if (ImGui::InputFloat("##val", &val, 0, 0, "%.4g")) {
                        step.value = (double)val;
                        mCurrentScale->SetDirty(true);
                    }
                    ImGui::PopItemWidth();
                    ImGui::SameLine();

                    // Remove button
                    if (ImGui::SmallButton("X##rm")) {
                        removeIdx = ti;
                    }

                    ImGui::PopID();
                }

                if (removeIdx >= 0) {
                    dimScoring.transform.erase(dimScoring.transform.begin() + removeIdx);
                    mCurrentScale->SetDirty(true);
                }

                if (ImGui::Button("+ Add Step")) {
                    dimScoring.transform.push_back(TransformStep("add", 0.0));
                    mCurrentScale->SetDirty(true);
                }

                if (!dimScoring.transform.empty()) {
                    ImGui::SameLine();
                    if (ImGui::Button("Clear All")) {
                        dimScoring.transform.clear();
                        mCurrentScale->SetDirty(true);
                    }
                }

                ImGui::EndPopup();
            }
        }

        ImGui::Separator();

        // Items table
        int numCols = isWeighted ? 5 : 4;
        if (ImGui::BeginTable("ItemScoringTable", numCols, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable)) {
            ImGui::TableSetupColumn("Include", ImGuiTableColumnFlags_WidthFixed, 60);
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 120);
            if (isSumCorrect) {
                ImGui::TableSetupColumn("Question Text", ImGuiTableColumnFlags_WidthFixed, 200);
                ImGui::TableSetupColumn("Correct Answers", ImGuiTableColumnFlags_WidthStretch);
            } else if (isWeighted) {
                ImGui::TableSetupColumn("Question Text", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Coding", ImGuiTableColumnFlags_WidthFixed, 80);
                ImGui::TableSetupColumn("Weight", ImGuiTableColumnFlags_WidthFixed, 80);
            } else {
                ImGui::TableSetupColumn("Question Text", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Coding", ImGuiTableColumnFlags_WidthFixed, 120);
            }
            ImGui::TableHeadersRow();

            for (const auto& question : questions) {
                ImGui::TableNextRow();
                ImGui::PushID(question.id.c_str());

                auto itemIt = std::find(dimScoring.items.begin(), dimScoring.items.end(), question.id);
                bool isIncluded = (itemIt != dimScoring.items.end());

                // Checkbox column
                ImGui::TableSetColumnIndex(0);
                if (ImGui::Checkbox("##include", &isIncluded)) {
                    if (isIncluded) {
                        dimScoring.items.push_back(question.id);
                        if (!isSumCorrect && dimScoring.item_coding.find(question.id) == dimScoring.item_coding.end()) {
                            dimScoring.item_coding[question.id] = 1;
                        }
                        if (isWeighted && dimScoring.weights.find(question.id) == dimScoring.weights.end()) {
                            dimScoring.weights[question.id] = 1.0;
                        }
                    } else {
                        dimScoring.items.erase(itemIt);
                        dimScoring.item_coding.erase(question.id);
                        dimScoring.correct_answers.erase(question.id);
                    }
                    mCurrentScale->SetDirty(true);
                }

                // ID column
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", question.id.c_str());

                // Question text column
                ImGui::TableSetColumnIndex(2);
                std::string questionText = mCurrentScale->GetTranslation("en", question.text_key);
                if (questionText.empty()) {
                    questionText = "[" + question.text_key + "]";
                }
                std::string displayText = questionText;
                int truncLen = isSumCorrect ? 40 : 60;
                if (static_cast<int>(displayText.length()) > truncLen) {
                    displayText = displayText.substr(0, truncLen - 3) + "...";
                }
                ImGui::TextWrapped("%s", displayText.c_str());
                if (ImGui::IsItemHovered() && static_cast<int>(questionText.length()) > truncLen) {
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(400.0f);
                    ImGui::TextWrapped("%s", questionText.c_str());
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }

                // Last column: Coding or Correct Answers
                ImGui::TableSetColumnIndex(3);
                if (isSumCorrect) {
                    if (isIncluded) {
                        auto caIt = dimScoring.correct_answers.find(question.id);
                        int answerCount = (caIt != dimScoring.correct_answers.end()) ? (int)caIt->second.size() : 0;

                        std::string btnLabel;
                        if (answerCount == 0) {
                            btnLabel = "[click to add]##ca";
                        } else {
                            btnLabel = std::to_string(answerCount) + " answer" + (answerCount != 1 ? "s" : "") + "##ca";
                        }

                        if (ImGui::SmallButton(btnLabel.c_str())) {
                            mCorrectAnswersEditor.show = true;
                            mCorrectAnswersEditor.questionId = question.id;
                            mCorrectAnswersEditor.dimensionId = selectedDim.id;
                            mCorrectAnswersEditor.questionType = question.type;

                            std::string qText = mCurrentScale->GetTranslation("en", question.text_key);
                            if (qText.empty()) qText = "[" + question.text_key + "]";
                            mCorrectAnswersEditor.questionText = qText;

                            mCorrectAnswersEditor.answers.clear();
                            mCorrectAnswersEditor.caseSensitive.clear();
                            if (caIt != dimScoring.correct_answers.end()) {
                                for (const auto& raw : caIt->second) {
                                    if (raw.size() >= 4 && raw.substr(0, 4) == "(?c)") {
                                        mCorrectAnswersEditor.answers.push_back(raw.substr(4));
                                        mCorrectAnswersEditor.caseSensitive.push_back(true);
                                    } else {
                                        mCorrectAnswersEditor.answers.push_back(raw);
                                        mCorrectAnswersEditor.caseSensitive.push_back(false);
                                    }
                                }
                            }
                        }

                        if (ImGui::IsItemHovered() && answerCount > 0) {
                            ImGui::BeginTooltip();
                            ImGui::PushTextWrapPos(300.0f);
                            for (const auto& ans : caIt->second) {
                                ImGui::BulletText("%s", ans.c_str());
                            }
                            ImGui::PopTextWrapPos();
                            ImGui::EndTooltip();
                        }
                    } else {
                        ImGui::TextDisabled("--");
                    }
                } else {
                    if (isIncluded) {
                        int currentCoding = 1;
                        auto codingIt = dimScoring.item_coding.find(question.id);
                        if (codingIt != dimScoring.item_coding.end()) {
                            currentCoding = codingIt->second;
                        }

                        const char* codingOptions[] = { "Normal (1)", "Reverse (-1)", "Not Scored (0)" };
                        int codingIndex = (currentCoding == 1) ? 0 : (currentCoding == -1) ? 1 : 2;

                        if (ImGui::Combo("##coding", &codingIndex, codingOptions, 3)) {
                            switch (codingIndex) {
                                case 0: dimScoring.item_coding[question.id] = 1; break;
                                case 1: dimScoring.item_coding[question.id] = -1; break;
                                case 2: dimScoring.item_coding[question.id] = 0; break;
                            }
                            mCurrentScale->SetDirty(true);
                        }
                    } else {
                        ImGui::TextDisabled("--");
                    }
                }

                // Weight column (only for weighted methods)
                if (isWeighted) {
                    ImGui::TableSetColumnIndex(4);
                    if (isIncluded) {
                        auto wIt = dimScoring.weights.find(question.id);
                        float w = (wIt != dimScoring.weights.end()) ? (float)wIt->second : 1.0f;
                        ImGui::SetNextItemWidth(70.0f);
                        if (ImGui::InputFloat("##weight", &w, 0.0f, 0.0f, "%.3f")) {
                            dimScoring.weights[question.id] = (double)w;
                            mCurrentScale->SetDirty(true);
                        }
                    } else {
                        ImGui::TextDisabled("--");
                    }
                }

                ImGui::PopID();
            }

            ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::Text("Items in dimension: %zu", dimScoring.items.size());

        if (isWeighted && !dimScoring.items.empty()) {
            double weightSum = 0.0;
            bool hasZeroOrNeg = false;
            for (const auto& itemId : dimScoring.items) {
                auto wIt = dimScoring.weights.find(itemId);
                double w = (wIt != dimScoring.weights.end()) ? wIt->second : 1.0;
                weightSum += w;
                if (w <= 0.0) hasZeroOrNeg = true;
            }
            ImGui::SameLine();
            ImGui::Text("  Weight sum: %.4f", weightSum);
            if (dimScoring.method == "weighted_mean") {
                ImGui::SameLine();
                ImGui::TextDisabled("(denominator)");
            }
            if (hasZeroOrNeg) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
                ImGui::Text("Warning: one or more weights are zero or negative.");
                ImGui::PopStyleColor();
            }
        }

    } else {
        ImGui::Text("Select a dimension from the list");
    }

    ImGui::EndChild();

    // ── Computed Variables section ────────────────────────────────
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    auto& computed = mCurrentScale->GetComputed();

    bool showComputed = ImGui::CollapsingHeader(
        computed.empty() ? "Computed Variables" : ("Computed Variables (" + std::to_string(computed.size()) + ")").c_str(),
        ImGuiTreeNodeFlags_DefaultOpen * 0);  // collapsed by default

    if (showComputed) {
        ImGui::TextDisabled("Derived values from expressions referencing score.*, answer.*, computed.*");
        ImGui::Spacing();

        std::string removeKey;
        for (auto& [key, cv] : computed) {
            ImGui::PushID(key.c_str());

            // Name + type on one line
            ImGui::Text("%s", key.c_str());
            ImGui::SameLine();
            ImGui::TextDisabled("(%s)", cv.type.c_str());
            ImGui::SameLine();
            if (ImGui::SmallButton("X##rm")) {
                removeKey = key;
            }

            // Expression
            char expr[512];
            std::strncpy(expr, cv.expression.c_str(), sizeof(expr) - 1);
            expr[sizeof(expr) - 1] = '\0';
            ImGui::SetNextItemWidth(-1);
            if (ImGui::InputText("##expr", expr, sizeof(expr))) {
                cv.expression = expr;
                mCurrentScale->SetDirty(true);
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Expression using score.*, answer.*, computed.* references.\n"
                                  "Examples:\n"
                                  "  score.PHQ_total >= 10\n"
                                  "  answer.weight / (answer.height * answer.height)\n"
                                  "  computed.met_vigorous + computed.met_moderate");
            }

            ImGui::Spacing();
            ImGui::PopID();
        }

        if (!removeKey.empty()) {
            computed.erase(removeKey);
            mCurrentScale->SetDirty(true);
        }

        // Add new computed variable
        static char newComputedName[128] = "";
        static int newComputedType = 0;
        ImGui::PushItemWidth(150);
        ImGui::InputText("##newCVName", newComputedName, sizeof(newComputedName));
        ImGui::PopItemWidth();
        ImGui::SameLine();
        const char* cvTypes[] = { "number", "boolean" };
        ImGui::PushItemWidth(80);
        ImGui::Combo("##newCVType", &newComputedType, cvTypes, 2);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::Button("+ Add##cv")) {
            if (strlen(newComputedName) > 0 && computed.find(newComputedName) == computed.end()) {
                ComputedVariable cv;
                cv.type = cvTypes[newComputedType];
                computed[newComputedName] = cv;
                mCurrentScale->SetDirty(true);
                newComputedName[0] = '\0';
            }
        }
    }
}

void LauncherUI::RenderTranslationsEditor()
{
    if (!mCurrentScale) return;

    auto& translations = mCurrentScale->GetTranslations();

    // Collect available languages from the translations map
    std::vector<std::string> availableLanguages;
    availableLanguages.push_back("en");  // English always first
    for (const auto& [lang, _] : translations) {
        if (lang != "en") {
            availableLanguages.push_back(lang);
        }
    }

    // Ensure English translations exist
    if (translations.find("en") == translations.end()) {
        translations["en"] = {};
    }

    // Collect all keys from English translations
    std::vector<std::string> allKeys;
    for (const auto& [key, _] : translations["en"]) {
        allKeys.push_back(key);
    }
    // Sort keys for consistent display
    std::sort(allKeys.begin(), allKeys.end());

    // Header: language selector
    ImGui::Text("Language:");
    ImGui::SameLine();
    ImGui::PushItemWidth(100);

    if (ImGui::BeginCombo("##ScaleTransLang", mScaleTransLanguage[0] ? mScaleTransLanguage : "Select...")) {
        for (const auto& lang : availableLanguages) {
            bool isSelected = (std::string(mScaleTransLanguage) == lang);
            if (ImGui::Selectable(lang.c_str(), isSelected)) {
                std::strncpy(mScaleTransLanguage, lang.c_str(), sizeof(mScaleTransLanguage) - 1);
                mScaleTransLanguage[sizeof(mScaleTransLanguage) - 1] = '\0';
                mScaleTransSelectedKey = allKeys.empty() ? -1 : 0;
            }
        }
        // New language option
        ImGui::Separator();
        ImGui::TextDisabled("Add language (2-3 chars):");
        static char newScaleLang[16] = "";
        ImGui::PushItemWidth(60);
        if (ImGui::InputText("##NewScaleLang", newScaleLang, 4, ImGuiInputTextFlags_EnterReturnsTrue)) {
            if (strlen(newScaleLang) > 0) {
                // Create the new language entry with empty values copied from English keys
                std::string langCode(newScaleLang);
                for (char& c : langCode) c = tolower(c);
                if (translations.find(langCode) == translations.end()) {
                    translations[langCode] = {};
                    for (const auto& [key, _] : translations["en"]) {
                        translations[langCode][key] = "";
                    }
                    mCurrentScale->SetDirty(true);
                }
                std::strncpy(mScaleTransLanguage, langCode.c_str(), sizeof(mScaleTransLanguage) - 1);
                mScaleTransLanguage[sizeof(mScaleTransLanguage) - 1] = '\0';
                newScaleLang[0] = '\0';
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::PopItemWidth();
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();

    ImGui::SameLine(0, 20);
    ImGui::TextDisabled("%zu keys, %zu languages", allKeys.size(), availableLanguages.size());

    // "Launch Translation Editor" — saves scale to disk, then opens the translation editor dialog
    ImGui::SameLine(0, 20);
    bool canLaunch = (mCurrentScale != nullptr) && (mScaleManager != nullptr);
    if (!canLaunch) ImGui::BeginDisabled();
    if (ImGui::Button("Launch Translation Editor")) {
        // Save scale so translation files exist on disk
        mScaleManager->SaveScale(mCurrentScale);

        // Open the translation editor dialog in scale mode
        mTranslationEditor.scaleMode = true;
        std::string code = mCurrentScale->GetScaleInfo().code;
        std::strncpy(mTranslationEditor.scaleCode, code.c_str(), sizeof(mTranslationEditor.scaleCode) - 1);
        mTranslationEditor.scaleCode[sizeof(mTranslationEditor.scaleCode) - 1] = '\0';
        std::string scaleDir = mScaleManager->GetScalesPath() + "/" + code;
        std::strncpy(mTranslationEditor.scaleDir, scaleDir.c_str(), sizeof(mTranslationEditor.scaleDir) - 1);
        mTranslationEditor.scaleDir[sizeof(mTranslationEditor.scaleDir) - 1] = '\0';
        mTranslationEditor.testIndex = -1;  // Not used in scale mode

        // Pre-fill language from current inline selector
        if (mScaleTransLanguage[0]) {
            std::strncpy(mTranslationEditor.language, mScaleTransLanguage, sizeof(mTranslationEditor.language) - 1);
            mTranslationEditor.language[sizeof(mTranslationEditor.language) - 1] = '\0';
        } else {
            std::strncpy(mTranslationEditor.language, "en", sizeof(mTranslationEditor.language) - 1);
        }
        mTranslationEditor.dataLoaded = false;
        mTranslationEditor.show = true;
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("Save scale and open the full translation editor dialog");
    }
    if (!canLaunch) ImGui::EndDisabled();

    ImGui::Separator();

    if (!mScaleTransLanguage[0]) {
        ImGui::Spacing();
        ImGui::TextWrapped("Select a language to view or edit translations. English (en) is the base language.");
        return;
    }

    std::string currentLang(mScaleTransLanguage);
    bool isEnglish = (currentLang == "en");

    // Ensure target language map exists
    if (translations.find(currentLang) == translations.end()) {
        translations[currentLang] = {};
    }
    auto& targetMap = translations[currentLang];
    auto& englishMap = translations["en"];

    if (allKeys.empty()) {
        ImGui::Spacing();
        ImGui::TextWrapped("No translation keys defined yet. Keys are created automatically when you add questions in the Questions tab.");
        return;
    }

    // Two-panel layout
    float contentHeight = ImGui::GetContentRegionAvail().y;

    // Left panel - key list
    ImGui::BeginChild("ScaleTransKeyList", ImVec2(180, contentHeight), true);
    ImGui::TextDisabled("Keys");
    ImGui::Separator();

    for (size_t i = 0; i < allKeys.size(); i++) {
        const std::string& key = allKeys[i];
        bool isSelected = (mScaleTransSelectedKey == (int)i);

        // Show indicator if target value is empty (untranslated)
        bool untranslated = false;
        if (!isEnglish) {
            auto it = targetMap.find(key);
            untranslated = (it == targetMap.end() || it->second.empty());
        }

        if (untranslated) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
        }

        if (ImGui::Selectable(key.c_str(), isSelected)) {
            mScaleTransSelectedKey = (int)i;
        }

        if (untranslated) {
            ImGui::PopStyleColor();
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Right panel - edit area
    ImGui::BeginChild("ScaleTransEditPanel", ImVec2(0, contentHeight), true);

    if (mScaleTransSelectedKey >= 0 && mScaleTransSelectedKey < (int)allKeys.size()) {
        const std::string& selectedKey = allKeys[mScaleTransSelectedKey];

        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Key: %s", selectedKey.c_str());
        ImGui::Spacing();

        float availHeight = ImGui::GetContentRegionAvail().y;

        if (isEnglish) {
            // Editing English: single editable box
            ImGui::Text("English (editing):");

            std::string& val = englishMap[selectedKey];
            char editBuf[8192];
            std::strncpy(editBuf, val.c_str(), sizeof(editBuf) - 1);
            editBuf[sizeof(editBuf) - 1] = '\0';

            if (ImGui::InputTextMultiline("##scaletrans_edit", editBuf, sizeof(editBuf),
                    ImVec2(-1, availHeight - 30), ImGuiInputTextFlags_WordWrap)) {
                val = editBuf;
                mCurrentScale->SetDirty(true);
            }
        } else {
            // Editing another language: English reference on top, target below
            float boxHeight = (availHeight - 60) / 2;

            ImGui::Text("English (reference):");
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
            std::string englishVal = englishMap.count(selectedKey) ? englishMap[selectedKey] : "";
            char refBuf[8192];
            std::strncpy(refBuf, englishVal.c_str(), sizeof(refBuf) - 1);
            refBuf[sizeof(refBuf) - 1] = '\0';
            ImGui::InputTextMultiline("##scaletrans_ref", refBuf, sizeof(refBuf),
                ImVec2(-1, boxHeight), ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_WordWrap);
            ImGui::PopStyleColor();

            ImGui::Spacing();

            ImGui::Text("%s (editing):", currentLang.c_str());
            std::string& targetVal = targetMap[selectedKey];
            char editBuf[8192];
            std::strncpy(editBuf, targetVal.c_str(), sizeof(editBuf) - 1);
            editBuf[sizeof(editBuf) - 1] = '\0';

            if (ImGui::InputTextMultiline("##scaletrans_target", editBuf, sizeof(editBuf),
                    ImVec2(-1, boxHeight), ImGuiInputTextFlags_WordWrap)) {
                targetVal = editBuf;
                mCurrentScale->SetDirty(true);
            }
        }
    } else {
        ImGui::TextDisabled("Select a key from the list to edit");
    }

    ImGui::EndChild();
}

void LauncherUI::RenderSectionsTab()
{
    if (!mCurrentScale) return;

    auto& raw = mCurrentScale->GetRawDefinition();
    const auto& questions = mCurrentScale->GetQuestions();

    // Collect section IDs from question list (in order)
    std::vector<std::string> sectionIds;
    for (const auto& q : questions)
        if (q.type == "section")
            sectionIds.push_back(q.id);

    // ── Left panel: randomization ────────────────────────────────────────
    ImGui::BeginChild("SectionsRandomPanel", ImVec2(320, 0), true);
    ImGui::Text("Section Order Randomization (S4)");
    ImGui::Separator();
    ImGui::Spacing();

    // Top-level toggle
    bool randomizeEnabled = raw.contains("randomize_sections");
    if (ImGui::Checkbox("Randomize section order", &randomizeEnabled)) {
        if (randomizeEnabled) {
            raw["randomize_sections"] = {{"method", "shuffle"}, {"fixed", nlohmann::json::array()}};
        } else {
            raw.erase("randomize_sections");
        }
        mCurrentScale->SetDirty(true);
    }

    if (randomizeEnabled && raw.contains("randomize_sections")) {
        auto& rs = raw["randomize_sections"];

        // Build fixed set for fast lookup
        std::set<std::string> fixedSet;
        if (rs.contains("fixed") && rs["fixed"].is_array())
            for (const auto& f : rs["fixed"])
                if (f.is_string()) fixedSet.insert(f.get<std::string>());

        ImGui::Spacing();
        ImGui::TextDisabled("Fixed sections are not shuffled:");
        ImGui::Spacing();

        if (sectionIds.empty()) {
            ImGui::TextDisabled("(No sections defined — add sections in the Questions tab)");
        } else {
            for (const auto& sid : sectionIds) {
                bool isFixed = fixedSet.count(sid) > 0;
                std::string label = "Fixed##fix_" + sid;
                if (ImGui::Checkbox(label.c_str(), &isFixed)) {
                    if (isFixed)
                        fixedSet.insert(sid);
                    else
                        fixedSet.erase(sid);
                    // Rebuild fixed array
                    rs["fixed"] = nlohmann::json::array();
                    for (const auto& f : fixedSet)
                        rs["fixed"].push_back(f);
                    mCurrentScale->SetDirty(true);
                }
                ImGui::SameLine();
                ImGui::TextUnformatted(sid.c_str());
            }
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // ── Right panel: branch groups ───────────────────────────────────────
    ImGui::BeginChild("SectionsBranchPanel", ImVec2(0, 0), true);
    ImGui::Text("Branch Groups (A1)");
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::TextWrapped(
        "Branch groups let you randomly assign participants to different "
        "sequences of sections. Each group has named arms; the runner picks "
        "one arm per participant.");
    ImGui::Spacing();

    // Ensure branches key exists as array if missing
    bool hasBranches = raw.contains("branches") && raw["branches"].is_array();

    if (ImGui::Button("Add Branch Group")) {
        if (!hasBranches) {
            raw["branches"] = nlohmann::json::array();
            hasBranches = true;
        }
        nlohmann::json newGroup;
        newGroup["id"] = "branch_" + std::to_string(raw["branches"].size() + 1);
        newGroup["method"] = "random";
        newGroup["arms"] = nlohmann::json::array();
        raw["branches"].push_back(newGroup);
        mSelectedBranchGroupIndex = (int)raw["branches"].size() - 1;
        mCurrentScale->SetDirty(true);
    }

    if (hasBranches) {
        auto& branches = raw["branches"];
        int numGroups = (int)branches.size();

        // Left sub-column: group list
        ImGui::BeginChild("BranchGroupList", ImVec2(160, 0), true);
        for (int gi = 0; gi < numGroups; ++gi) {
            std::string groupId = branches[gi].value("id", "branch_" + std::to_string(gi+1));
            bool selected = (mSelectedBranchGroupIndex == gi);
            if (ImGui::Selectable(groupId.c_str(), selected))
                mSelectedBranchGroupIndex = gi;
        }
        ImGui::EndChild();
        ImGui::SameLine();

        // Right sub-column: group editor
        ImGui::BeginChild("BranchGroupEditor", ImVec2(0, 0), true);
        if (mSelectedBranchGroupIndex >= 0 && mSelectedBranchGroupIndex < numGroups) {
            auto& grp = branches[mSelectedBranchGroupIndex];

            // Group ID
            char gidBuf[64];
            std::string gidStr = grp.value("id", "");
            std::strncpy(gidBuf, gidStr.c_str(), sizeof(gidBuf) - 1);
            gidBuf[sizeof(gidBuf)-1] = '\0';
            ImGui::Text("Group ID:"); ImGui::SameLine();
            ImGui::SetNextItemWidth(150);
            if (ImGui::InputText("##gid", gidBuf, sizeof(gidBuf)))
            { grp["id"] = gidBuf; mCurrentScale->SetDirty(true); }

            // Method
            ImGui::Text("Method:"); ImGui::SameLine();
            const char* methods[] = {"random", "balanced", "parameter"};
            int methodIdx = 0;
            std::string curMethod = grp.value("method", "random");
            for (int m = 0; m < 3; ++m)
                if (curMethod == methods[m]) { methodIdx = m; break; }
            ImGui::SetNextItemWidth(130);
            if (ImGui::Combo("##gmethod", &methodIdx, methods, 3))
            { grp["method"] = methods[methodIdx]; mCurrentScale->SetDirty(true); }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("Arms:");

            // Arms
            if (!grp.contains("arms") || !grp["arms"].is_array())
                grp["arms"] = nlohmann::json::array();
            auto& arms = grp["arms"];

            if (ImGui::Button("+ Add Arm")) {
                nlohmann::json arm;
                arm["id"] = "arm_" + std::to_string(arms.size() + 1);
                arm["sections"] = nlohmann::json::array();
                arms.push_back(arm);
                mCurrentScale->SetDirty(true);
            }

            int armToDelete = -1;
            for (int ai = 0; ai < (int)arms.size(); ++ai) {
                auto& arm = arms[ai];
                ImGui::PushID(ai);

                // Arm ID input
                char armIdBuf[64];
                std::string armIdStr = arm.value("id", "");
                std::strncpy(armIdBuf, armIdStr.c_str(), sizeof(armIdBuf) - 1);
                armIdBuf[sizeof(armIdBuf)-1] = '\0';
                ImGui::SetNextItemWidth(100);
                if (ImGui::InputText("##armid", armIdBuf, sizeof(armIdBuf)))
                { arm["id"] = armIdBuf; mCurrentScale->SetDirty(true); }

                ImGui::SameLine();

                // Section checklist popup
                std::string popupId = "ArmSections##" + std::to_string(ai);
                if (!arm.contains("sections") || !arm["sections"].is_array())
                    arm["sections"] = nlohmann::json::array();
                std::set<std::string> armSecs;
                for (const auto& s : arm["sections"])
                    if (s.is_string()) armSecs.insert(s.get<std::string>());
                std::string secLabel = std::to_string(armSecs.size()) + " section(s)";
                if (ImGui::Button(secLabel.c_str()))
                    ImGui::OpenPopup(popupId.c_str());
                if (ImGui::BeginPopup(popupId.c_str())) {
                    ImGui::Text("Sections in this arm:");
                    ImGui::Separator();
                    if (sectionIds.empty()) {
                        ImGui::TextDisabled("(No sections — add in Questions tab)");
                    } else {
                        for (const auto& sid : sectionIds) {
                            bool inArm = armSecs.count(sid) > 0;
                            if (ImGui::Checkbox(sid.c_str(), &inArm)) {
                                if (inArm) armSecs.insert(sid);
                                else armSecs.erase(sid);
                                // Rebuild sections array preserving order from sectionIds
                                arm["sections"] = nlohmann::json::array();
                                for (const auto& s : sectionIds)
                                    if (armSecs.count(s)) arm["sections"].push_back(s);
                                mCurrentScale->SetDirty(true);
                            }
                        }
                    }
                    ImGui::EndPopup();
                }

                ImGui::SameLine();
                if (ImGui::SmallButton("Del")) armToDelete = ai;

                ImGui::PopID();
            }
            if (armToDelete >= 0) {
                arms.erase(arms.begin() + armToDelete);
                mCurrentScale->SetDirty(true);
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f,0.15f,0.15f,1.0f));
            if (ImGui::Button("Delete Branch Group")) {
                branches.erase(branches.begin() + mSelectedBranchGroupIndex);
                mSelectedBranchGroupIndex = -1;
                mCurrentScale->SetDirty(true);
            }
            ImGui::PopStyleColor();
        } else {
            ImGui::TextDisabled("Select a branch group from the list.");
        }
        ImGui::EndChild();
    }

    ImGui::EndChild(); // SectionsBranchPanel
}

void LauncherUI::RenderParametersEditor()
{
    if (!mCurrentScale) return;

    auto& params = mCurrentScale->GetParameters();

    // Base parameters are managed automatically by the runner — don't expose them here.
    // shuffle_questions and show_header appear in Scale Info; scale is internal.
    static const std::set<std::string> baseNames = {"scale", "shuffle_questions", "show_header"};

    ImGui::Text("Scale Parameters");
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::TextWrapped(
        "Parameters are researcher-configurable values set at study deployment time. They have three uses:\n"
        "  1. Text substitution: use {param_name} in any question or instruction string.\n"
        "  2. Question/section visibility: visible_when conditions can test parameter values.\n"
        "  3. Branch selection: branches with method=\"parameter\" route participants based on a parameter value.\n"
        "Common examples: tool name, population (child/adult), condition (pre/post), age threshold.");
    ImGui::Spacing();

    // ── Parameter table ──────────────────────────────────────────────────────
    static const char* kTypes[] = {"string", "integer", "float", "boolean", "choice"};

    // Collect custom parameters (exclude base names), preserving map order
    std::vector<std::string> customKeys;
    for (const auto& [k, _] : params)
        if (!baseNames.count(k)) customKeys.push_back(k);

    if (customKeys.empty()) {
        ImGui::TextDisabled("No custom parameters defined. Use 'Add Parameter' below.");
        ImGui::Spacing();
    } else {
        if (ImGui::BeginTable("ParamsTable", 5,
                              ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                              ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchProp,
                              ImVec2(0, 200)))
        {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Name",        ImGuiTableColumnFlags_WidthFixed,   140.0f);
            ImGui::TableSetupColumn("Type",        ImGuiTableColumnFlags_WidthFixed,    80.0f);
            ImGui::TableSetupColumn("Default",     ImGuiTableColumnFlags_WidthFixed,   120.0f);
            ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("##del",       ImGuiTableColumnFlags_WidthFixed,    50.0f);
            ImGui::TableHeadersRow();

            std::string toDelete;

            for (const auto& key : customKeys) {
                auto& p = params[key];
                ImGui::TableNextRow();
                ImGui::PushID(key.c_str());

                // Name (read-only — renaming would invalidate any {param_name} references)
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(key.c_str());
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Use {%s} in translation strings to substitute this value.", key.c_str());

                // Type dropdown
                ImGui::TableSetColumnIndex(1);
                int typeIdx = 0;
                for (int i = 0; i < 5; i++)
                    if (p.type == kTypes[i]) { typeIdx = i; break; }
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::Combo("##type", &typeIdx, kTypes, 5)) {
                    p.type = kTypes[typeIdx];
                    // Clear options only when switching away from boolean/choice
                    if (p.type != "boolean" && p.type != "choice") p.options.clear();
                    if (p.type == "boolean") p.options = {"0", "1"};
                    mCurrentScale->SetDirty(true);
                }

                // Default value
                ImGui::TableSetColumnIndex(2);
                if (p.type == "boolean") {
                    // Render as checkbox
                    bool bval = (p.defaultValue == "1" || p.defaultValue == "true");
                    if (ImGui::Checkbox("##booldef", &bval)) {
                        p.defaultValue = bval ? "1" : "0";
                        mCurrentScale->SetDirty(true);
                    }
                } else {
                    char defBuf[256];
                    std::strncpy(defBuf, p.defaultValue.c_str(), sizeof(defBuf) - 1);
                    defBuf[sizeof(defBuf) - 1] = '\0';
                    ImGui::SetNextItemWidth(-FLT_MIN);
                    if (ImGui::InputText("##def", defBuf, sizeof(defBuf))) {
                        p.defaultValue = defBuf;
                        mCurrentScale->SetDirty(true);
                    }
                }

                // Description
                ImGui::TableSetColumnIndex(3);
                char descBuf[512];
                std::strncpy(descBuf, p.description.c_str(), sizeof(descBuf) - 1);
                descBuf[sizeof(descBuf) - 1] = '\0';
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::InputText("##desc", descBuf, sizeof(descBuf))) {
                    p.description = descBuf;
                    mCurrentScale->SetDirty(true);
                }

                // Delete button
                ImGui::TableSetColumnIndex(4);
                if (ImGui::SmallButton("Del")) toDelete = key;

                ImGui::PopID();
            }

            ImGui::EndTable();

            if (!toDelete.empty()) {
                params.erase(toDelete);
                mCurrentScale->SetDirty(true);
            }
        }
    }

    // ── Base parameter overrides ─────────────────────────────────────────────
    ImGui::Spacing();
    ImGui::Text("Standard Parameter Defaults");
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::TextWrapped(
        "Override the default values for the built-in ScaleRunner parameters. "
        "Leave at system default if no special behaviour is needed.");
    ImGui::Spacing();

    // shuffle_questions
    {
        auto it = params.find("shuffle_questions");
        bool inOsd = (it != params.end());
        bool shuffleOn = inOsd && (it->second.defaultValue == "1");

        // Three-state: "use system default (0)" or "override to 1"
        // Checkbox means "override to 1"; unchecked+no OSD entry = use system default
        bool overrideOn = inOsd;
        if (ImGui::Checkbox("Randomize questions by default (shuffle_questions = 1)", &overrideOn)) {
            if (overrideOn) {
                ScaleParameter sp("boolean", "1",
                    "Randomize item order within randomization groups (recommended for this scale)");
                sp.options = {"0", "1"};
                params["shuffle_questions"] = sp;
            } else {
                params.erase("shuffle_questions");
            }
            mCurrentScale->SetDirty(true);
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip(
                "Check this to make shuffle_questions default to ON for this scale.\n"
                "Recommended for scales where item order effects matter (e.g., BSR).");
        (void)shuffleOn;
    }

    ImGui::Spacing();

    // show_header
    {
        auto it = params.find("show_header");
        bool inOsd = (it != params.end());

        // Checkbox means "override to 0 (hide header)"
        bool hideHeader = inOsd && (it->second.defaultValue == "0");
        if (ImGui::Checkbox("Hide scale title header by default (show_header = 0)", &hideHeader)) {
            if (hideHeader) {
                ScaleParameter sp("boolean", "0",
                    "Hide scale title — recommended when the title would reveal the scale's purpose");
                sp.options = {"0", "1"};
                params["show_header"] = sp;
            } else {
                params.erase("show_header");
            }
            mCurrentScale->SetDirty(true);
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip(
                "Check this to hide the scale title header by default.\n"
                "Useful when showing the scale name would reveal its purpose to participants\n"
                "(e.g., Bullshit Receptivity Scale).");
    }

    // ── Choice parameter options editor ─────────────────────────────────────
    {
        bool hasChoiceParams = false;
        for (const auto& key : customKeys)
            if (params[key].type == "choice") { hasChoiceParams = true; break; }

        if (hasChoiceParams) {
            ImGui::Spacing();
            ImGui::Text("Choice Parameter Options");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::TextWrapped(
                "For each 'choice' parameter, enter the allowed values as a comma-separated list. "
                "The default value must be one of these options. Branch arm IDs and visible_when "
                "values should match these exactly.");
            ImGui::Spacing();

            for (const auto& key : customKeys) {
                auto& p = params[key];
                if (p.type != "choice") continue;

                ImGui::PushID(key.c_str());
                ImGui::Text("%s options:", key.c_str());
                ImGui::SameLine();

                // Build comma-sep string from options vector for editing
                static std::map<std::string, std::string> sOptBufs;
                if (!sOptBufs.count(key)) {
                    std::string joined;
                    for (size_t i = 0; i < p.options.size(); i++) {
                        if (i > 0) joined += ",";
                        joined += p.options[i];
                    }
                    sOptBufs[key] = joined;
                }
                auto& buf = sOptBufs[key];
                char cbuf[512];
                std::strncpy(cbuf, buf.c_str(), sizeof(cbuf) - 1);
                cbuf[sizeof(cbuf) - 1] = '\0';
                ImGui::SetNextItemWidth(300.0f);
                if (ImGui::InputText("##opts", cbuf, sizeof(cbuf))) {
                    buf = cbuf;
                    // Parse comma-sep back to vector
                    p.options.clear();
                    std::string s = cbuf;
                    std::stringstream ss(s);
                    std::string token;
                    while (std::getline(ss, token, ',')) {
                        // Trim leading/trailing spaces
                        size_t start = token.find_first_not_of(" \t");
                        size_t end   = token.find_last_not_of(" \t");
                        if (start != std::string::npos)
                            p.options.push_back(token.substr(start, end - start + 1));
                    }
                    mCurrentScale->SetDirty(true);
                }
                ImGui::SameLine();
                ImGui::TextDisabled("(comma-separated)");
                ImGui::PopID();
            }
        }
    }

    // ── Add new parameter ────────────────────────────────────────────────────
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Add Parameter");
    ImGui::Spacing();

    static char  sNewName[64]     = "";
    static char  sNewDefault[256] = "";
    static char  sNewDesc[512]    = "";
    static int   sNewTypeIdx      = 0;  // index into kTypes

    // Labels above their fields using BeginGroup/EndGroup
    ImGui::BeginGroup();
    ImGui::Text("Name");
    ImGui::SetNextItemWidth(130.0f);
    ImGui::InputText("##add_name", sNewName, sizeof(sNewName));
    ImGui::EndGroup();

    ImGui::SameLine();

    ImGui::BeginGroup();
    ImGui::Text("Type");
    ImGui::SetNextItemWidth(90.0f);
    ImGui::Combo("##add_type", &sNewTypeIdx, kTypes, 5);
    ImGui::EndGroup();

    ImGui::SameLine();

    ImGui::BeginGroup();
    ImGui::Text("Default");
    ImGui::SetNextItemWidth(120.0f);
    ImGui::InputText("##add_default", sNewDefault, sizeof(sNewDefault));
    ImGui::EndGroup();

    ImGui::SameLine();

    ImGui::BeginGroup();
    ImGui::Text("Description");
    ImGui::SetNextItemWidth(220.0f);
    ImGui::InputText("##add_desc", sNewDesc, sizeof(sNewDesc));
    ImGui::EndGroup();

    ImGui::SameLine();

    ImGui::BeginGroup();
    ImGui::Text(" ");  // spacer to align button baseline with fields
    bool nameOk = sNewName[0] != '\0' && !baseNames.count(sNewName) && !params.count(sNewName);
    if (!nameOk) ImGui::BeginDisabled();
    if (ImGui::Button("Add")) {
        ScaleParameter sp;
        sp.type         = kTypes[sNewTypeIdx];
        sp.defaultValue = sNewDefault;
        sp.description  = sNewDesc;
        if (sp.type == "boolean") sp.options = {"0", "1"};
        params[sNewName] = sp;
        mCurrentScale->SetDirty(true);
        sNewName[0]    = '\0';
        sNewDefault[0] = '\0';
        sNewDesc[0]    = '\0';
        sNewTypeIdx    = 0;
    }
    if (!nameOk) ImGui::EndDisabled();
    ImGui::EndGroup();

    if (sNewName[0] != '\0') {
        if (baseNames.count(sNewName))
            ImGui::TextColored(ImVec4(1, 0.4f, 0.4f, 1),
                "'%s' is a reserved parameter name — edit it in Standard Parameter Defaults above.", sNewName);
        else if (params.count(sNewName))
            ImGui::TextColored(ImVec4(1, 0.4f, 0.4f, 1),
                "A parameter named '%s' already exists.", sNewName);
    }
}

void LauncherUI::ShowQuestionEditor()
{
    if (!mCurrentScale) {
        mQuestionEditor.show = false;
        return;
    }

    // Branch for section editor — uses its own simplified form
    if (mQuestionEditor.isSection) {
        RenderSectionEditorForm();
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(600, 450), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));

    const char* title = (mQuestionEditor.editingIndex >= 0) ? "Edit Question" : "Add Question";
    if (!ImGui::Begin(title, &mQuestionEditor.show, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("Question Details");
    ImGui::Separator();
    ImGui::Spacing();

    // Question ID (also used as translation key — lowercase letters, digits, underscores only)
    ImGui::InputText("ID", mQuestionEditor.id, sizeof(mQuestionEditor.id));
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Unique question identifier (e.g., q1, age, grit2).\n"
                          "Use lowercase letters, digits, and underscores only.\n"
                          "Also used as the translation key for the question text.");
    }

    // Question Text (multiline input)
    ImGui::Text("Question Text:");
    ImGui::InputTextMultiline("##QuestionText", mQuestionEditor.questionText, sizeof(mQuestionEditor.questionText),
                              ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 4));
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("The actual question text (English). Quotes are allowed.");
    }

    // Question Type
    const char* questionTypes[] = { "likert", "multi", "short", "long", "vas", "inst", "multicheck", "grid", "image", "imageresponse" };
    int prevType = mQuestionEditor.questionType;
    ImGui::Combo("Type", &mQuestionEditor.questionType, questionTypes, IM_ARRAYSIZE(questionTypes));
    // Auto-set randomization group to 0 when type changes to inst
    if (mQuestionEditor.questionType != prevType && mQuestionEditor.questionType == 5) {
        mQuestionEditor.randomGroup = 0;
    }

    // Randomization Group
    ImGui::InputInt("Randomization Group", &mQuestionEditor.randomGroup, 1, 1);
    if (mQuestionEditor.randomGroup < 0) mQuestionEditor.randomGroup = 0;
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("0 = fixed position\n1+ = shuffle within group when randomization is enabled");
    }

    // Required
    {
        // Build label for default option based on type
        const char* qTypes[] = { "likert", "multi", "short", "long", "vas", "inst", "multicheck", "grid", "image", "imageresponse" };
        std::string currentType = qTypes[mQuestionEditor.questionType];
        bool isDisplayOnly = (currentType == "inst" || currentType == "image");
        std::string defaultLabel = "Default (";
        if (isDisplayOnly) {
            defaultLabel += "n/a - display only)";
        } else {
            int scaleDef = mCurrentScale ? mCurrentScale->GetDefaultRequired() : -1;
            if (scaleDef == 1) {
                defaultLabel += "required, scale setting)";
            } else if (scaleDef == 0) {
                defaultLabel += "optional, scale setting)";
            } else {
                bool typeRequired = (currentType != "short" && currentType != "long");
                defaultLabel += typeRequired ? "required, type default)" : "optional, type default)";
            }
        }
        const char* requiredItems[] = { defaultLabel.c_str(), "Required", "Optional" };
        int reqComboIdx = (mQuestionEditor.requiredState == -1) ? 0 : (mQuestionEditor.requiredState == 1 ? 1 : 2);
        if (ImGui::Combo("Required", &reqComboIdx, requiredItems, IM_ARRAYSIZE(requiredItems))) {
            mQuestionEditor.requiredState = (reqComboIdx == 0) ? -1 : (reqComboIdx == 1 ? 1 : 0);
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Whether the participant must answer this question.\n"
                              "Default: scored types are required, text entry is optional.\n"
                              "Can also be overridden at scale level in the Info tab.");
        }
    }

    // Input Validation section — per-constraint, only for short, long, multicheck
    {
        std::string currentType = questionTypes[mQuestionEditor.questionType];
        bool isText     = (currentType == "short" || currentType == "long");
        bool isShort    = (currentType == "short");
        bool isMulti    = (currentType == "multicheck");
        if (isText || isMulti) {
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("Input Validation");
            ImGui::Spacing();
            ImGui::TextDisabled("Enable individual constraints below. Each can have its own error message.");
            ImGui::Spacing();

            // Helper macro-like lambda for one constraint row
            auto constraintRow = [&](const char* label, bool& enabled, int& val,
                                     char* errBuf, size_t errBufSz, const char* tooltip) {
                ImGui::Checkbox(label, &enabled);
                if (ImGui::IsItemHovered() && tooltip[0]) ImGui::SetTooltip("%s", tooltip);
                if (enabled) {
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(80);
                    std::string intId = std::string("##v") + label;
                    ImGui::InputInt(intId.c_str(), &val);
                    ImGui::SameLine();
                    std::string errId = std::string("##e") + label;
                    ImGui::SetNextItemWidth(-FLT_MIN);
                    ImGui::InputText(errId.c_str(), errBuf, errBufSz);
                    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Error message shown when this constraint fails (English)");
                }
            };
            auto constraintRowDbl = [&](const char* label, bool& enabled, double& val,
                                        char* errBuf, size_t errBufSz, const char* tooltip) {
                ImGui::Checkbox(label, &enabled);
                if (ImGui::IsItemHovered() && tooltip[0]) ImGui::SetTooltip("%s", tooltip);
                if (enabled) {
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(100);
                    std::string dblId = std::string("##v") + label;
                    ImGui::InputDouble(dblId.c_str(), &val, 1.0, 10.0, "%.2f");
                    ImGui::SameLine();
                    std::string errId = std::string("##e") + label;
                    ImGui::SetNextItemWidth(-FLT_MIN);
                    ImGui::InputText(errId.c_str(), errBuf, errBufSz);
                    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Error message shown when this constraint fails (English)");
                }
            };

            auto& e = mQuestionEditor;
            if (isText) {
                ImGui::TextDisabled("Characters:"); ImGui::SameLine(); ImGui::TextDisabled("value"); ImGui::SameLine(); ImGui::TextDisabled("  error message");
                constraintRow("Min characters", e.valMinLengthEnabled, e.valMinLength, e.valMinLengthError, sizeof(e.valMinLengthError), "Minimum number of characters required");
                constraintRow("Max characters", e.valMaxLengthEnabled, e.valMaxLength, e.valMaxLengthError, sizeof(e.valMaxLengthError), "Maximum number of characters allowed");
                ImGui::Spacing();
                ImGui::TextDisabled("Words:"); ImGui::SameLine(); ImGui::TextDisabled("value"); ImGui::SameLine(); ImGui::TextDisabled("  error message");
                constraintRow("Min words",      e.valMinWordsEnabled,  e.valMinWords,  e.valMinWordsError,  sizeof(e.valMinWordsError),  "Minimum number of words required");
                constraintRow("Max words",      e.valMaxWordsEnabled,  e.valMaxWords,  e.valMaxWordsError,  sizeof(e.valMaxWordsError),  "Maximum number of words allowed");
            }
            if (isShort) {
                ImGui::Spacing();
                ImGui::TextDisabled("Numeric range (also restricts input to digits):");
                constraintRowDbl("Min value", e.valNumberMinEnabled, e.valNumberMin, e.valNumberMinError, sizeof(e.valNumberMinError), "Minimum numeric value");
                constraintRowDbl("Max value", e.valNumberMaxEnabled, e.valNumberMax, e.valNumberMaxError, sizeof(e.valNumberMaxError), "Maximum numeric value");
                ImGui::Spacing();
                ImGui::Checkbox("Pattern (regex)", &e.valPatternEnabled);
                if (e.valPatternEnabled) {
                    ImGui::SetNextItemWidth(200);
                    ImGui::InputText("##vpat", e.valPattern, sizeof(e.valPattern));
                    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Regular expression. Supports: . ^ $ * + ? [] \\s \\w \\d | ()");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(-FLT_MIN);
                    ImGui::InputText("##epat", e.valPatternError, sizeof(e.valPatternError));
                    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Error message shown when pattern does not match (English)");
                }
            }
            if (isMulti) {
                constraintRow("Min selected", e.valMinSelectedEnabled, e.valMinSelected, e.valMinSelectedError, sizeof(e.valMinSelectedError), "Minimum number of options to select");
                constraintRow("Max selected", e.valMaxSelectedEnabled, e.valMaxSelected, e.valMaxSelectedError, sizeof(e.valMaxSelectedError), "Maximum number of options to select");
            }
        }
    }

    // Conditional Display section
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Conditional Display");
    ImGui::Spacing();

    RenderVisibleWhenEditor(mQuestionEditor);

    // Likert-specific fields (only show if type is likert)
    if (mQuestionEditor.questionType == 0) {  // likert
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Likert Options");
        ImGui::Spacing();

        ImGui::InputInt("Number of Points", &mQuestionEditor.likertPoints);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Number of response options (-1 = use scale default)");
        }
        if (mQuestionEditor.likertPoints < -1) mQuestionEditor.likertPoints = -1;
        if (mQuestionEditor.likertPoints == 0 || mQuestionEditor.likertPoints == 1) mQuestionEditor.likertPoints = -1;
        if (mQuestionEditor.likertPoints > 10) mQuestionEditor.likertPoints = 10;

        ImGui::InputInt("Min Value", &mQuestionEditor.likertMin);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Minimum value (-1 = use scale default)");
        }

        ImGui::InputInt("Max Value", &mQuestionEditor.likertMax);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Maximum value (-1 = use scale default)");
        }

        ImGui::Checkbox("Reverse display order", &mQuestionEditor.likertReverse);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Display buttons right-to-left (highest value on the left).\n"
                              "The stored value is unchanged — this only affects\n"
                              "the visual layout. Useful for descending scales\n"
                              "(e.g., quality-of-life ladders: 10 on left, 0 on right).");
        }

        // Response Options Selection
        ImGui::Spacing();
        ImGui::Text("Response Options for this Question");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Select which response options to use (leave all unchecked to use scale defaults)");
        }

        // Get scale-level response options
        auto& scaleLikert = mCurrentScale->GetLikertOptions();
        auto& scaleLabels = scaleLikert.labels;

        // Get current question's labels (for editing mode)
        std::vector<std::string> currentLabels;
        if (mQuestionEditor.editingIndex >= 0) {
            auto& questions = mCurrentScale->GetQuestions();
            if (mQuestionEditor.editingIndex < (int)questions.size()) {
                currentLabels = questions[mQuestionEditor.editingIndex].likert_labels;
            }
        }

        // Display checkboxes for each scale-level option
        if (scaleLabels.empty()) {
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "No response options defined at scale level.");
            ImGui::TextWrapped("Go to the Info tab to add response options.");
        } else {
            // Initialize selected flags if needed
            if (mQuestionEditor.selectedResponseOptions.size() != scaleLabels.size()) {
                mQuestionEditor.selectedResponseOptions.resize(scaleLabels.size(), false);
                // Initialize from current question's labels
                for (size_t i = 0; i < scaleLabels.size(); i++) {
                    mQuestionEditor.selectedResponseOptions[i] = (std::find(currentLabels.begin(), currentLabels.end(), scaleLabels[i]) != currentLabels.end());
                }
            }

            for (size_t i = 0; i < scaleLabels.size(); i++) {
                std::string labelText = mCurrentScale->GetTranslation("en", scaleLabels[i]);
                std::string displayText = scaleLabels[i] + ": " + labelText;
                // std::vector<bool> uses proxy references, need a temp variable
                bool selected = mQuestionEditor.selectedResponseOptions[i];
                if (ImGui::Checkbox(displayText.c_str(), &selected)) {
                    mQuestionEditor.selectedResponseOptions[i] = selected;
                }
            }

            ImGui::Text("(Leave all unchecked to use all scale-level options)");
        }
    }

    // VAS-specific fields (only show if type is vas)
    if (mQuestionEditor.questionType == 4) {  // vas
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("VAS (Visual Analog Scale) Options");
        ImGui::Spacing();

        ImGui::InputInt("Min Value", &mQuestionEditor.vasMinValue);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Minimum value for the scale (e.g., 0)");
        }

        ImGui::InputInt("Max Value", &mQuestionEditor.vasMaxValue);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Maximum value for the scale (e.g., 100)");
        }

        ImGui::InputText("Top Label", mQuestionEditor.vasLeftLabel, sizeof(mQuestionEditor.vasLeftLabel));
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Text for top of vertical scale (e.g., 'Extremely')");
        }

        ImGui::InputText("Bottom Label", mQuestionEditor.vasRightLabel, sizeof(mQuestionEditor.vasRightLabel));
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Text for bottom of vertical scale (e.g., 'Not at all')");
        }
    }

    // Multi/multicheck-specific fields (only show if type is multi or multicheck)
    if (mQuestionEditor.questionType == 1 || mQuestionEditor.questionType == 6) {  // multi or multicheck
        ImGui::Spacing();
        ImGui::Separator();
        const char* typeLabel = (mQuestionEditor.questionType == 1) ? "Multiple Choice" : "Multiple Check";
        ImGui::Text("%s Options", typeLabel);
        ImGui::Spacing();

        ImGui::Text("Options (one per line):");
        ImGui::InputTextMultiline("##MultiOptions", mQuestionEditor.multiOptions, sizeof(mQuestionEditor.multiOptions),
                                  ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 6));
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Enter one option per line. These will become answer choices.");
        }
    }

    // Grid-specific fields (only show if type is grid)
    if (mQuestionEditor.questionType == 7) {  // grid
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Grid Question Options");
        ImGui::Spacing();

        ImGui::Text("Column Headers (one per line):");
        ImGui::InputTextMultiline("##GridColumns", mQuestionEditor.gridColumns, sizeof(mQuestionEditor.gridColumns),
                                  ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 3));
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Rating scale labels (e.g., 'Never', 'Sometimes', 'Always')");
        }

        ImGui::Text("Row Labels/Sub-questions (one per line):");
        ImGui::InputTextMultiline("##GridRows", mQuestionEditor.gridRows, sizeof(mQuestionEditor.gridRows),
                                  ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 6));
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Sub-questions that will be rated using the column headers");
        }
    }

    // Image-specific fields (only show if type is image or imageresponse)
    if (mQuestionEditor.questionType == 8 || mQuestionEditor.questionType == 9) {  // image or imageresponse
        ImGui::Spacing();
        ImGui::Separator();
        const char* typeLabel = (mQuestionEditor.questionType == 8) ? "Image Display" : "Image Response";
        ImGui::Text("%s Options", typeLabel);
        ImGui::Spacing();

        ImGui::InputText("Image Path", mQuestionEditor.imagePath, sizeof(mQuestionEditor.imagePath));
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Path to image file (relative to test directory or absolute)");
        }
    }

    // ── Answer Alias (S3 answer piping) ──────────────────────────────────────
    {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Text("Answer Alias");
        ImGui::TextDisabled("Optional: give this answer a semantic name for use in {answer.alias} piping.");
        ImGui::InputText("Alias##answerAlias", mQuestionEditor.answerAlias, sizeof(mQuestionEditor.answerAlias));
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("e.g. 'favorite_activity' lets later questions use {answer.favorite_activity}");
    }

    // ── Gate (blocking) ──────────────────────────────────────────────────────
    // Only relevant for multi/binary questions
    {
        int qt = mQuestionEditor.questionType;
        bool isGateable = (qt == 1 || qt == 2);  // multi (exact match) or short (numeric threshold)
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Text("Gate (Blocking)");
        if (!isGateable) {
            ImGui::TextDisabled("Available for multi and short questions only.");
        } else {
            ImGui::Checkbox("This question is a gate", &mQuestionEditor.hasGate);
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("If the participant does not give the required response,\n"
                                  "the scale terminates early, saves data, and shows a message.");
            }
            if (mQuestionEditor.hasGate) {
                if (qt == 1) {
                    // multi: exact-match required value
                    ImGui::InputText("Required value", mQuestionEditor.gateRequiredValue, sizeof(mQuestionEditor.gateRequiredValue));
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("The option value that allows the participant to continue.\n"
                                          "Any other selection terminates the scale.");
                    }
                } else {
                    // short: numeric operator + threshold
                    const char* opLabels[] = { "> greater than", "< less than", "= equals", "\xe2\x89\xa0 not equals" };
                    ImGui::Combo("Operator##gateOp", &mQuestionEditor.gateOperator, opLabels, 4);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Continue if response is: response <op> threshold\nE.g., age > 17 passes participants who are 18+.");
                    ImGui::InputDouble("Threshold##gateVal", &mQuestionEditor.gateValue, 0.0, 0.0, "%.4g");
                }
                ImGui::Text("Termination message (English):");
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Message shown to ineligible participants.\nTranslation key is auto-generated as {id}_gate_msg.");
                ImGui::InputTextMultiline("##GateMsg", mQuestionEditor.gateTerminateMessageText,
                                          sizeof(mQuestionEditor.gateTerminateMessageText),
                                          ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 3));
            }
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Buttons
    const char* buttonLabel = (mQuestionEditor.editingIndex >= 0) ? "Save" : "Add Question";
    if (ImGui::Button(buttonLabel, ImVec2(120, 0))) {
        // Validate
        if (strlen(mQuestionEditor.id) == 0) {
            printf("Error: Question ID cannot be empty\n");
        } else if (mQuestionEditor.editingIndex >= 0) {
            // Update existing question
            auto& questions = mCurrentScale->GetQuestions();
            if (mQuestionEditor.editingIndex < (int)questions.size()) {
                questions[mQuestionEditor.editingIndex].id = mQuestionEditor.id;
                questions[mQuestionEditor.editingIndex].text_key = mQuestionEditor.id;
                questions[mQuestionEditor.editingIndex].type = questionTypes[mQuestionEditor.questionType];
                questions[mQuestionEditor.editingIndex].random_group = mQuestionEditor.randomGroup;
                questions[mQuestionEditor.editingIndex].required_state = mQuestionEditor.requiredState;

                // Update validation (C9) — per-constraint fields
                {
                    auto& val = questions[mQuestionEditor.editingIndex].validation;
                    std::string qid = mQuestionEditor.id;
                    auto& e = mQuestionEditor;

                    auto storeErr = [&](const char* buf, const std::string& suffix) -> std::string {
                        if (buf[0] && mCurrentScale) {
                            std::string key = qid + "_" + suffix;
                            mCurrentScale->AddTranslation("en", key, buf);
                            return key;
                        }
                        return "";
                    };

                    val.min_length     = e.valMinLengthEnabled   ? e.valMinLength    : -1;
                    val.max_length     = e.valMaxLengthEnabled   ? e.valMaxLength    : -1;
                    val.min_words      = e.valMinWordsEnabled    ? e.valMinWords     : -1;
                    val.max_words      = e.valMaxWordsEnabled    ? e.valMaxWords     : -1;
                    val.number_min_set = e.valNumberMinEnabled;
                    val.number_min     = e.valNumberMinEnabled   ? e.valNumberMin    : 0.0;
                    val.number_max_set = e.valNumberMaxEnabled;
                    val.number_max     = e.valNumberMaxEnabled   ? e.valNumberMax    : 0.0;
                    val.pattern        = e.valPatternEnabled     ? e.valPattern      : "";
                    val.min_selected   = e.valMinSelectedEnabled ? e.valMinSelected  : -1;
                    val.max_selected   = e.valMaxSelectedEnabled ? e.valMaxSelected  : -1;

                    val.min_length_error    = e.valMinLengthEnabled   ? storeErr(e.valMinLengthError,    "min_length_err")    : "";
                    val.max_length_error    = e.valMaxLengthEnabled   ? storeErr(e.valMaxLengthError,    "max_length_err")    : "";
                    val.min_words_error     = e.valMinWordsEnabled    ? storeErr(e.valMinWordsError,     "min_words_err")     : "";
                    val.max_words_error     = e.valMaxWordsEnabled    ? storeErr(e.valMaxWordsError,     "max_words_err")     : "";
                    val.number_min_error    = e.valNumberMinEnabled   ? storeErr(e.valNumberMinError,    "number_min_err")    : "";
                    val.number_max_error    = e.valNumberMaxEnabled   ? storeErr(e.valNumberMaxError,    "number_max_err")    : "";
                    val.pattern_error       = e.valPatternEnabled     ? storeErr(e.valPatternError,      "pattern_err")       : "";
                    val.min_selected_error  = e.valMinSelectedEnabled ? storeErr(e.valMinSelectedError,  "min_selected_err")  : "";
                    val.max_selected_error  = e.valMaxSelectedEnabled ? storeErr(e.valMaxSelectedError,  "max_selected_err")  : "";
                }

                // Update answer alias (S3)
                questions[mQuestionEditor.editingIndex].answer_alias = mQuestionEditor.answerAlias;

                // Update gate (blocking) — multi (exact match) or short (numeric operator)
                {
                    auto& q2 = questions[mQuestionEditor.editingIndex];
                    bool gateAllowed = (mQuestionEditor.questionType == 1 || mQuestionEditor.questionType == 2);
                    q2.has_gate = mQuestionEditor.hasGate && gateAllowed;
                    const char* opNames[] = { "greater_than", "less_than", "equals", "not_equals" };
                    if (q2.has_gate && mQuestionEditor.questionType == 2) {
                        // short: numeric operator form
                        q2.gate_required_value = "";
                        q2.gate_operator = opNames[mQuestionEditor.gateOperator];
                        q2.gate_value = mQuestionEditor.gateValue;
                    } else {
                        q2.gate_required_value = q2.has_gate ? mQuestionEditor.gateRequiredValue : "";
                        q2.gate_operator = "";
                        q2.gate_value = 0.0;
                    }
                    if (q2.has_gate && mCurrentScale && mQuestionEditor.gateTerminateMessageText[0]) {
                        std::string autoQid = questions[mQuestionEditor.editingIndex].id;
                        std::string gateKey = mQuestionEditor.gateTerminateMessageKey[0]
                            ? mQuestionEditor.gateTerminateMessageKey : (autoQid + "_gate_msg");
                        mCurrentScale->AddTranslation("en", gateKey, mQuestionEditor.gateTerminateMessageText);
                        q2.gate_terminate_message_key = gateKey;
                    } else {
                        q2.gate_terminate_message_key = q2.has_gate
                            ? std::string(mQuestionEditor.gateTerminateMessageKey) : "";
                    }
                }

                // Update conditional display
                if (!mQuestionEditor.visibleWhenIsComplex) {
                    questions[mQuestionEditor.editingIndex].has_visible_when = mQuestionEditor.hasVisibleWhen;
                    questions[mQuestionEditor.editingIndex].visible_when_logic = (mQuestionEditor.visibleWhenLogic == 1) ? "any" : "all";
                    questions[mQuestionEditor.editingIndex].visible_when_simple.clear();
                    if (mQuestionEditor.hasVisibleWhen) {
                        const char* opNames[] = { "equals", "not_equals", "greater_than", "less_than", "in", "not_in" };
                        for (const auto& ec : mQuestionEditor.visibleWhenConditions) {
                            VisibleWhenCondition c;
                            c.source_type = (ec.sourceType == 1) ? "item" : "parameter";
                            c.source_name = ec.sourceName;
                            c.op = opNames[ec.op < 6 ? ec.op : 0];
                            if (ec.op == 4 || ec.op == 5) {
                                c.is_list = true;
                                std::istringstream ss(ec.value);
                                std::string token;
                                while (std::getline(ss, token, ',')) {
                                    auto s = token.find_first_not_of(" \t");
                                    auto e2 = token.find_last_not_of(" \t");
                                    if (s != std::string::npos)
                                        c.values.push_back(token.substr(s, e2 - s + 1));
                                }
                            } else {
                                c.is_list = false;
                                c.value = ec.value;
                            }
                            questions[mQuestionEditor.editingIndex].visible_when_simple.push_back(c);
                        }
                    }
                    questions[mQuestionEditor.editingIndex].visible_when_is_complex = false;
                }

                // Update Likert-specific fields if type is likert
                if (mQuestionEditor.questionType == 0) {  // likert
                    questions[mQuestionEditor.editingIndex].likert_points = mQuestionEditor.likertPoints;
                    questions[mQuestionEditor.editingIndex].likert_min = mQuestionEditor.likertMin;
                    questions[mQuestionEditor.editingIndex].likert_max = mQuestionEditor.likertMax;
                    questions[mQuestionEditor.editingIndex].likert_reverse = mQuestionEditor.likertReverse;

                    // Save selected response options
                    auto& scaleLikert = mCurrentScale->GetLikertOptions();
                    auto& scaleLabels = scaleLikert.labels;
                    questions[mQuestionEditor.editingIndex].likert_labels.clear();
                    bool anySelected = false;
                    for (size_t i = 0; i < scaleLabels.size() && i < mQuestionEditor.selectedResponseOptions.size(); i++) {
                        if (mQuestionEditor.selectedResponseOptions[i]) {
                            questions[mQuestionEditor.editingIndex].likert_labels.push_back(scaleLabels[i]);
                            anySelected = true;
                        }
                    }
                    // If none selected, clear the labels array to use scale defaults
                    if (!anySelected) {
                        questions[mQuestionEditor.editingIndex].likert_labels.clear();
                    }
                }

                // Update VAS-specific fields if type is vas
                if (mQuestionEditor.questionType == 4) {  // vas
                    questions[mQuestionEditor.editingIndex].min_value = mQuestionEditor.vasMinValue;
                    questions[mQuestionEditor.editingIndex].max_value = mQuestionEditor.vasMaxValue;
                    questions[mQuestionEditor.editingIndex].left_label = mQuestionEditor.vasLeftLabel;
                    questions[mQuestionEditor.editingIndex].right_label = mQuestionEditor.vasRightLabel;
                }

                // Update Multi/multicheck-specific fields if type is multi or multicheck
                if (mQuestionEditor.questionType == 1 || mQuestionEditor.questionType == 6) {  // multi or multicheck
                    // Parse multiline options (split by newlines)
                    questions[mQuestionEditor.editingIndex].options.clear();
                    std::string optionsStr = mQuestionEditor.multiOptions;
                    std::istringstream iss(optionsStr);
                    std::string line;
                    while (std::getline(iss, line)) {
                        // Trim whitespace and skip empty lines
                        line.erase(0, line.find_first_not_of(" \t\r\n"));
                        line.erase(line.find_last_not_of(" \t\r\n") + 1);
                        if (!line.empty()) {
                            questions[mQuestionEditor.editingIndex].options.push_back(line);
                        }
                    }
                }

                // Update Grid-specific fields if type is grid
                if (mQuestionEditor.questionType == 7) {  // grid
                    // Parse column headers
                    questions[mQuestionEditor.editingIndex].columns.clear();
                    std::string columnsStr = mQuestionEditor.gridColumns;
                    std::istringstream colIss(columnsStr);
                    std::string line;
                    while (std::getline(colIss, line)) {
                        line.erase(0, line.find_first_not_of(" \t\r\n"));
                        line.erase(line.find_last_not_of(" \t\r\n") + 1);
                        if (!line.empty()) {
                            questions[mQuestionEditor.editingIndex].columns.push_back(line);
                        }
                    }

                    // Parse row labels
                    questions[mQuestionEditor.editingIndex].rows.clear();
                    std::string rowsStr = mQuestionEditor.gridRows;
                    std::istringstream rowIss(rowsStr);
                    while (std::getline(rowIss, line)) {
                        line.erase(0, line.find_first_not_of(" \t\r\n"));
                        line.erase(line.find_last_not_of(" \t\r\n") + 1);
                        if (!line.empty()) {
                            questions[mQuestionEditor.editingIndex].rows.push_back(line);
                        }
                    }
                }

                // Update Image-specific fields if type is image or imageresponse
                if (mQuestionEditor.questionType == 8 || mQuestionEditor.questionType == 9) {  // image or imageresponse
                    questions[mQuestionEditor.editingIndex].image = mQuestionEditor.imagePath;
                }

                // Update question text in translation (English)
                std::string textKey = questions[mQuestionEditor.editingIndex].text_key;
                std::string questionText = mQuestionEditor.questionText;
                mCurrentScale->AddTranslation("en", textKey, questionText);

                printf("Updated question: %s (type: %s)\n", questions[mQuestionEditor.editingIndex].id.c_str(), questions[mQuestionEditor.editingIndex].type.c_str());
                mCurrentScale->SetDirty(true);
            }
            mQuestionEditor.show = false;
        } else {
            // Check for duplicate ID before creating
            auto& existingQs = mCurrentScale->GetQuestions();
            bool isDuplicate = false;
            for (const auto& eq : existingQs) {
                if (eq.id == mQuestionEditor.id) { isDuplicate = true; break; }
            }
            if (isDuplicate) {
                printf("Error: Question ID '%s' already exists\n", mQuestionEditor.id);
            } else {
            // Create new question
            ScaleQuestion newQuestion;
            newQuestion.id = mQuestionEditor.id;
            newQuestion.text_key = mQuestionEditor.id;
            newQuestion.type = questionTypes[mQuestionEditor.questionType];
            newQuestion.random_group = mQuestionEditor.randomGroup;
            newQuestion.required_state = mQuestionEditor.requiredState;

            // Set validation (C9) — per-constraint fields
            {
                auto& val = newQuestion.validation;
                std::string qid = mQuestionEditor.id;
                auto& e = mQuestionEditor;

                auto storeErr = [&](const char* buf, const std::string& suffix) -> std::string {
                    if (buf[0] && mCurrentScale) {
                        std::string key = qid + "_" + suffix;
                        mCurrentScale->AddTranslation("en", key, buf);
                        return key;
                    }
                    return "";
                };

                val.min_length     = e.valMinLengthEnabled   ? e.valMinLength    : -1;
                val.max_length     = e.valMaxLengthEnabled   ? e.valMaxLength    : -1;
                val.min_words      = e.valMinWordsEnabled    ? e.valMinWords     : -1;
                val.max_words      = e.valMaxWordsEnabled    ? e.valMaxWords     : -1;
                val.number_min_set = e.valNumberMinEnabled;
                val.number_min     = e.valNumberMinEnabled   ? e.valNumberMin    : 0.0;
                val.number_max_set = e.valNumberMaxEnabled;
                val.number_max     = e.valNumberMaxEnabled   ? e.valNumberMax    : 0.0;
                val.pattern        = e.valPatternEnabled     ? e.valPattern      : "";
                val.min_selected   = e.valMinSelectedEnabled ? e.valMinSelected  : -1;
                val.max_selected   = e.valMaxSelectedEnabled ? e.valMaxSelected  : -1;

                val.min_length_error    = e.valMinLengthEnabled   ? storeErr(e.valMinLengthError,    "min_length_err")    : "";
                val.max_length_error    = e.valMaxLengthEnabled   ? storeErr(e.valMaxLengthError,    "max_length_err")    : "";
                val.min_words_error     = e.valMinWordsEnabled    ? storeErr(e.valMinWordsError,     "min_words_err")     : "";
                val.max_words_error     = e.valMaxWordsEnabled    ? storeErr(e.valMaxWordsError,     "max_words_err")     : "";
                val.number_min_error    = e.valNumberMinEnabled   ? storeErr(e.valNumberMinError,    "number_min_err")    : "";
                val.number_max_error    = e.valNumberMaxEnabled   ? storeErr(e.valNumberMaxError,    "number_max_err")    : "";
                val.pattern_error       = e.valPatternEnabled     ? storeErr(e.valPatternError,      "pattern_err")       : "";
                val.min_selected_error  = e.valMinSelectedEnabled ? storeErr(e.valMinSelectedError,  "min_selected_err")  : "";
                val.max_selected_error  = e.valMaxSelectedEnabled ? storeErr(e.valMaxSelectedError,  "max_selected_err")  : "";
            }

            // Set answer alias (S3)
            newQuestion.answer_alias = mQuestionEditor.answerAlias;

            // Set gate (blocking) — multi (exact match) or short (numeric operator)
            {
                bool gateAllowed = (mQuestionEditor.questionType == 1 || mQuestionEditor.questionType == 2);
                newQuestion.has_gate = mQuestionEditor.hasGate && gateAllowed;
                const char* opNames[] = { "greater_than", "less_than", "equals", "not_equals" };
                if (newQuestion.has_gate && mQuestionEditor.questionType == 2) {
                    newQuestion.gate_required_value = "";
                    newQuestion.gate_operator = opNames[mQuestionEditor.gateOperator];
                    newQuestion.gate_value = mQuestionEditor.gateValue;
                } else {
                    newQuestion.gate_required_value = newQuestion.has_gate ? mQuestionEditor.gateRequiredValue : "";
                    newQuestion.gate_operator = "";
                    newQuestion.gate_value = 0.0;
                }
                if (newQuestion.has_gate && mCurrentScale && mQuestionEditor.gateTerminateMessageText[0]) {
                    std::string autoQid = mQuestionEditor.id;
                    std::string gateKey = mQuestionEditor.gateTerminateMessageKey[0]
                        ? mQuestionEditor.gateTerminateMessageKey : (autoQid + "_gate_msg");
                    mCurrentScale->AddTranslation("en", gateKey, mQuestionEditor.gateTerminateMessageText);
                    newQuestion.gate_terminate_message_key = gateKey;
                } else {
                    newQuestion.gate_terminate_message_key = "";
                }
            }

            // Set conditional display
            newQuestion.has_visible_when = mQuestionEditor.hasVisibleWhen;
            newQuestion.visible_when_logic = (mQuestionEditor.visibleWhenLogic == 1) ? "any" : "all";
            if (mQuestionEditor.hasVisibleWhen) {
                const char* opNames[] = { "equals", "not_equals", "greater_than", "less_than", "in", "not_in" };
                for (const auto& ec : mQuestionEditor.visibleWhenConditions) {
                    VisibleWhenCondition c;
                    c.source_type = (ec.sourceType == 1) ? "item" : "parameter";
                    c.source_name = ec.sourceName;
                    c.op = opNames[ec.op < 6 ? ec.op : 0];
                    if (ec.op == 4 || ec.op == 5) {
                        c.is_list = true;
                        std::istringstream ss(ec.value);
                        std::string token;
                        while (std::getline(ss, token, ',')) {
                            auto s = token.find_first_not_of(" \t");
                            auto e2 = token.find_last_not_of(" \t");
                            if (s != std::string::npos)
                                c.values.push_back(token.substr(s, e2 - s + 1));
                        }
                    } else {
                        c.is_list = false;
                        c.value = ec.value;
                    }
                    newQuestion.visible_when_simple.push_back(c);
                }
            }

            // Set Likert-specific fields if type is likert
            if (mQuestionEditor.questionType == 0) {  // likert
                newQuestion.likert_points = mQuestionEditor.likertPoints;
                newQuestion.likert_min = mQuestionEditor.likertMin;
                newQuestion.likert_max = mQuestionEditor.likertMax;
                newQuestion.likert_reverse = mQuestionEditor.likertReverse;

                // Save selected response options
                auto& scaleLikert = mCurrentScale->GetLikertOptions();
                auto& scaleLabels = scaleLikert.labels;
                bool anySelected = false;
                for (size_t i = 0; i < scaleLabels.size() && i < mQuestionEditor.selectedResponseOptions.size(); i++) {
                    if (mQuestionEditor.selectedResponseOptions[i]) {
                        newQuestion.likert_labels.push_back(scaleLabels[i]);
                        anySelected = true;
                    }
                }
                // If none selected, leave labels empty to use scale defaults
                if (!anySelected) {
                    newQuestion.likert_labels.clear();
                }
            }

            // Set VAS-specific fields if type is vas
            if (mQuestionEditor.questionType == 4) {  // vas
                newQuestion.min_value = mQuestionEditor.vasMinValue;
                newQuestion.max_value = mQuestionEditor.vasMaxValue;
                newQuestion.left_label = mQuestionEditor.vasLeftLabel;
                newQuestion.right_label = mQuestionEditor.vasRightLabel;
            }

            // Set Multi/multicheck-specific fields if type is multi or multicheck
            if (mQuestionEditor.questionType == 1 || mQuestionEditor.questionType == 6) {  // multi or multicheck
                // Parse multiline options (split by newlines)
                std::string optionsStr = mQuestionEditor.multiOptions;
                std::istringstream iss(optionsStr);
                std::string line;
                while (std::getline(iss, line)) {
                    // Trim whitespace and skip empty lines
                    line.erase(0, line.find_first_not_of(" \t\r\n"));
                    line.erase(line.find_last_not_of(" \t\r\n") + 1);
                    if (!line.empty()) {
                        newQuestion.options.push_back(line);
                    }
                }
            }

            // Set Grid-specific fields if type is grid
            if (mQuestionEditor.questionType == 7) {  // grid
                // Parse column headers
                std::string columnsStr = mQuestionEditor.gridColumns;
                std::istringstream colIss(columnsStr);
                std::string line;
                while (std::getline(colIss, line)) {
                    line.erase(0, line.find_first_not_of(" \t\r\n"));
                    line.erase(line.find_last_not_of(" \t\r\n") + 1);
                    if (!line.empty()) {
                        newQuestion.columns.push_back(line);
                    }
                }

                // Parse row labels
                std::string rowsStr = mQuestionEditor.gridRows;
                std::istringstream rowIss(rowsStr);
                while (std::getline(rowIss, line)) {
                    line.erase(0, line.find_first_not_of(" \t\r\n"));
                    line.erase(line.find_last_not_of(" \t\r\n") + 1);
                    if (!line.empty()) {
                        newQuestion.rows.push_back(line);
                    }
                }
            }

            // Set Image-specific fields if type is image or imageresponse
            if (mQuestionEditor.questionType == 8 || mQuestionEditor.questionType == 9) {  // image or imageresponse
                newQuestion.image = mQuestionEditor.imagePath;
            }

            // Add to scale
            mCurrentScale->GetQuestions().push_back(newQuestion);

            // Add question text to translation (English)
            std::string textKey = newQuestion.text_key;
            std::string questionText = mQuestionEditor.questionText;
            mCurrentScale->AddTranslation("en", textKey, questionText);

            printf("Added question: %s (type: %s)\n", newQuestion.id.c_str(), newQuestion.type.c_str());
            mCurrentScale->SetDirty(true);

            mQuestionEditor.show = false;
            } // end duplicate-ID check else
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120, 0))) {
        mQuestionEditor.show = false;
    }

    ImGui::End();
}

void LauncherUI::RenderVisibleWhenEditor(QuestionEditorState& e)
{
    if (e.visibleWhenIsComplex) {
        ImGui::TextWrapped("This item has nested conditional logic.");
        ImGui::TextWrapped("Conditions are preserved — use code editor to modify.");
        return;
    }

    ImGui::Checkbox("Show conditionally", &e.hasVisibleWhen);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("When checked, this item is only shown when conditions are met");

    if (e.hasVisibleWhen) {
        const char* logicItems[] = { "AND (all must match)", "OR (any must match)" };
        ImGui::Combo("Combine with", &e.visibleWhenLogic, logicItems, IM_ARRAYSIZE(logicItems));

        int removeIndex = -1;
        for (int ci = 0; ci < (int)e.visibleWhenConditions.size(); ci++) {
            auto& cond = e.visibleWhenConditions[ci];
            ImGui::PushID(ci);

            const char* sourceTypes[] = { "Parameter", "Item" };
            ImGui::PushItemWidth(90);
            ImGui::Combo("##src", &cond.sourceType, sourceTypes, IM_ARRAYSIZE(sourceTypes));
            ImGui::PopItemWidth();
            ImGui::SameLine();

            ImGui::PushItemWidth(100);
            ImGui::InputText("##name", cond.sourceName, sizeof(cond.sourceName));
            ImGui::PopItemWidth();
            ImGui::SameLine();

            const char* operators[] = { "equals", "not_equals", "greater_than", "less_than", "in", "not_in" };
            ImGui::PushItemWidth(100);
            ImGui::Combo("##op", &cond.op, operators, IM_ARRAYSIZE(operators));
            ImGui::PopItemWidth();
            ImGui::SameLine();

            ImGui::PushItemWidth(cond.op >= 4 ? 200 : 100);
            ImGui::InputText("##val", cond.value, sizeof(cond.value));
            if (cond.op >= 4 && ImGui::IsItemHovered())
                ImGui::SetTooltip("Comma-separated list of values, e.g. edu_grad,edu_phd");
            ImGui::PopItemWidth();
            ImGui::SameLine();

            if (ImGui::SmallButton("X"))
                removeIndex = ci;

            ImGui::PopID();
        }
        if (removeIndex >= 0)
            e.visibleWhenConditions.erase(e.visibleWhenConditions.begin() + removeIndex);

        if (ImGui::SmallButton("+ Add Condition")) {
            EditorCondition ec;
            e.visibleWhenConditions.push_back(ec);
            e.randomGroup = 0;
        }
    }
}

void LauncherUI::RenderSectionEditorForm()
{
    auto& e = mQuestionEditor;
    if (!e.show) return;

    const char* title = (e.editingIndex >= 0) ? "Edit Section"
                      : e.isVirtualStart       ? "Edit Start Section"
                      :                          "Add Section";
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(
        ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
        ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));

    if (!ImGui::Begin(title, &e.show, ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
        return;
    }

    ImGui::Text("Section Marker");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::InputText("Section ID", e.id, sizeof(e.id));
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Unique section identifier (e.g., sec_1, demographics).\n"
                          "Use lowercase letters, digits, and underscores only.");

    ImGui::Text("Title (optional):");
    ImGui::SetNextItemWidth(-FLT_MIN);
    ImGui::InputText("##sec_title", e.questionText, sizeof(e.questionText));
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Displayed as a section heading by runners that support it.\n"
                          "Stored in translation file under the section ID as key.");

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Conditional Display");
    ImGui::Spacing();

    RenderVisibleWhenEditor(e);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Navigation");
    ImGui::Spacing();

    ImGui::Checkbox("Revisable (allow Back button within section)", &e.sectionRevisable);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("When checked (default), runners show a Back button allowing\n"
                          "participants to revise answers within this section.\n"
                          "When unchecked, responses are final once submitted.");

    ImGui::Spacing();

    ImGui::Checkbox("Randomize questions within section", &e.sectionRandomize);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("When checked, questions in this section are presented in random order.\n"
                          "Questions with random_group=0 are always fixed.\n"
                          "Additional questions can be pinned using the Fixed IDs field below.");
    if (e.sectionRandomize) {
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputText("##fixedIds", e.sectionRandomizeFixed, sizeof(e.sectionRandomizeFixed));
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Comma-separated question IDs to keep in their original position\n"
                              "(e.g. inst1,sec_break). Leave blank to shuffle all questions.");
        ImGui::TextDisabled("Fixed IDs (comma-separated, optional)");
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    bool canSave = (e.id[0] != '\0');
    if (!canSave) ImGui::BeginDisabled();
    const char* saveLabel = (e.editingIndex < 0) ? "Add Section" : "Save Section";
    if (ImGui::Button(saveLabel, ImVec2(120, 0))) {
        ScaleQuestion sec;
        sec.id = e.id;
        sec.type = "section";
        sec.text_key = e.id;  // convention: key == id
        sec.has_visible_when = e.hasVisibleWhen;
        sec.visible_when_logic = (e.visibleWhenLogic == 1) ? "any" : "all";
        sec.visible_when_is_complex = false;
        sec.revisable = e.sectionRevisable;
        sec.section_randomize = e.sectionRandomize;
        sec.section_randomize_fixed.clear();
        if (e.sectionRandomize && e.sectionRandomizeFixed[0]) {
            std::string fixedStr = e.sectionRandomizeFixed;
            std::istringstream ss(fixedStr);
            std::string token;
            while (std::getline(ss, token, ',')) {
                auto start = token.find_first_not_of(" \t");
                auto end   = token.find_last_not_of(" \t");
                if (start != std::string::npos)
                    sec.section_randomize_fixed.push_back(token.substr(start, end - start + 1));
            }
        }
        if (e.hasVisibleWhen) {
            const char* opNames[] = { "equals", "not_equals", "greater_than", "less_than", "in", "not_in" };
            for (const auto& ec : e.visibleWhenConditions) {
                VisibleWhenCondition c;
                c.source_type = (ec.sourceType == 1) ? "item" : "parameter";
                c.source_name = ec.sourceName;
                c.op = opNames[ec.op < 6 ? ec.op : 0];
                if (ec.op == 4 || ec.op == 5) {
                    c.is_list = true;
                    std::istringstream ss(ec.value);
                    std::string token;
                    while (std::getline(ss, token, ',')) {
                        auto s = token.find_first_not_of(" \t");
                        auto e2 = token.find_last_not_of(" \t");
                        if (s != std::string::npos)
                            c.values.push_back(token.substr(s, e2 - s + 1));
                    }
                } else {
                    c.is_list = false;
                    c.value = ec.value;
                }
                sec.visible_when_simple.push_back(c);
            }
        }
        if (e.questionText[0] && mCurrentScale)
            mCurrentScale->AddTranslation("en", sec.id, e.questionText);

        if (e.editingIndex < 0) {
            if (e.isVirtualStart)
                mCurrentScale->InsertQuestion(0, sec);
            else
                mCurrentScale->AddQuestion(sec);
        } else {
            auto& questions = mCurrentScale->GetQuestions();
            if (e.editingIndex < (int)questions.size()) {
                questions[e.editingIndex] = sec;
                mCurrentScale->SetDirty(true);
            }
        }
        e.show = false;
    }
    if (!canSave) ImGui::EndDisabled();
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(80, 0)))
        e.show = false;

    ImGui::End();
}

void LauncherUI::ShowBatchImportDialog()
{
    if (!mCurrentScale) {
        mBatchImport.show = false;
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));

    if (!ImGui::Begin("Batch Import Questions", &mBatchImport.show, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::End();
        return;
    }

    ImGui::TextWrapped("Paste your questions below, one per line. Each line will become a separate question.");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Common settings
    ImGui::Text("Common Settings for All Questions");
    ImGui::Spacing();

    // ID Prefix
    ImGui::InputText("ID Prefix", mBatchImport.idPrefix, sizeof(mBatchImport.idPrefix));
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Will be combined with zero-padded numbers (e.g., 'MOCI' becomes MOCI001, MOCI002, ...)");
    }

    // Start Number
    ImGui::InputInt("Start Number", &mBatchImport.startNumber);

    // Question Type
    const char* questionTypes[] = { "likert", "multi", "short", "long", "vas", "inst", "multicheck", "grid", "image", "imageresponse" };
    if (ImGui::Combo("Type", &mBatchImport.questionType, questionTypes, IM_ARRAYSIZE(questionTypes))) {
        // Reset likert config when type changes
        if (mBatchImport.questionType == 0) {  // likert
            mBatchImport.likertPreset = 0;
            mBatchImport.likertPoints = 5;
        }
    }

    // Likert-specific configuration (only show if type is likert)
    if (mBatchImport.questionType == 0) {  // likert
        ImGui::Spacing();
        ImGui::Text("Likert Response Options:");
        ImGui::Indent();

        // Preset dropdown
        const char* likertPresets[] = {
            "Custom",
            "TRUE / FALSE (2-point)",
            "Disagree / Agree (5-point)",
            "Strongly Disagree / Strongly Agree (7-point)",
            "Never / Always (5-point)",
            "Not at all / Extremely (5-point)"
        };

        if (ImGui::Combo("Preset", &mBatchImport.likertPreset, likertPresets, IM_ARRAYSIZE(likertPresets))) {
            // Apply preset values
            switch (mBatchImport.likertPreset) {
                case 1:  // TRUE/FALSE
                    mBatchImport.likertPoints = 2;
                    std::strncpy(mBatchImport.likertLabels, "TRUE|FALSE", sizeof(mBatchImport.likertLabels) - 1);
                    break;
                case 2:  // Agree 5-point
                    mBatchImport.likertPoints = 5;
                    std::strncpy(mBatchImport.likertLabels, "Strongly Disagree|Disagree|Neutral|Agree|Strongly Agree", sizeof(mBatchImport.likertLabels) - 1);
                    break;
                case 3:  // Agree 7-point
                    mBatchImport.likertPoints = 7;
                    std::strncpy(mBatchImport.likertLabels, "Strongly Disagree|Disagree|Somewhat Disagree|Neutral|Somewhat Agree|Agree|Strongly Agree", sizeof(mBatchImport.likertLabels) - 1);
                    break;
                case 4:  // Never/Always 5-point
                    mBatchImport.likertPoints = 5;
                    std::strncpy(mBatchImport.likertLabels, "Never|Rarely|Sometimes|Often|Always", sizeof(mBatchImport.likertLabels) - 1);
                    break;
                case 5:  // Not at all/Extremely 5-point
                    mBatchImport.likertPoints = 5;
                    std::strncpy(mBatchImport.likertLabels, "Not at all|A little|Moderately|Quite a bit|Extremely", sizeof(mBatchImport.likertLabels) - 1);
                    break;
                default:  // Custom
                    mBatchImport.likertPoints = 5;
                    mBatchImport.likertLabels[0] = '\0';
                    break;
            }
            mBatchImport.likertLabels[sizeof(mBatchImport.likertLabels) - 1] = '\0';
        }

        // Manual configuration
        ImGui::InputInt("Number of Points", &mBatchImport.likertPoints);
        if (mBatchImport.likertPoints < 2) mBatchImport.likertPoints = 2;
        if (mBatchImport.likertPoints > 10) mBatchImport.likertPoints = 10;

        ImGui::InputInt("Min Value", &mBatchImport.likertMin);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Minimum value for responses (-1 = use default based on points)");
        }

        ImGui::InputInt("Max Value", &mBatchImport.likertMax);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Maximum value for responses (-1 = use default based on points)");
        }

        ImGui::InputText("Response Labels", mBatchImport.likertLabels, sizeof(mBatchImport.likertLabels));
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Pipe-separated labels (e.g., 'True|False' or 'Strongly Disagree|...|Strongly Agree')");
        }

        ImGui::Unindent();
        ImGui::Spacing();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Question text input
    ImGui::Text("Question Text (one per line):");
    ImGui::InputTextMultiline("##QuestionText", mBatchImport.questionText, sizeof(mBatchImport.questionText), ImVec2(-1, 250));

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Preview count
    int lineCount = 0;
    for (int i = 0; mBatchImport.questionText[i] != '\0'; i++) {
        if (mBatchImport.questionText[i] == '\n') lineCount++;
    }
    if (strlen(mBatchImport.questionText) > 0 && mBatchImport.questionText[strlen(mBatchImport.questionText)-1] != '\n') {
        lineCount++;  // Count last line if it doesn't end with newline
    }
    ImGui::Text("Questions to import: %d", lineCount);

    ImGui::Spacing();

    // Buttons
    if (ImGui::Button("Import Questions", ImVec2(150, 0))) {
        if (strlen(mBatchImport.idPrefix) == 0) {
            printf("Error: ID Prefix cannot be empty\n");
        } else {
            // Parse questions line by line
            std::vector<std::string> lines;
            std::string text(mBatchImport.questionText);
            size_t start = 0;
            size_t end = text.find('\n');

            while (end != std::string::npos) {
                std::string line = text.substr(start, end - start);
                // Trim whitespace
                while (!line.empty() && isspace(line.front())) line.erase(0, 1);
                while (!line.empty() && isspace(line.back())) line.pop_back();
                if (!line.empty()) {
                    lines.push_back(line);
                }
                start = end + 1;
                end = text.find('\n', start);
            }

            // Handle last line
            if (start < text.length()) {
                std::string line = text.substr(start);
                while (!line.empty() && isspace(line.front())) line.erase(0, 1);
                while (!line.empty() && isspace(line.back())) line.pop_back();
                if (!line.empty()) {
                    lines.push_back(line);
                }
            }

            // Parse likert labels if this is a likert type
            std::vector<std::string> likertLabelsList;
            if (mBatchImport.questionType == 0 && strlen(mBatchImport.likertLabels) > 0) {
                std::string labelsStr(mBatchImport.likertLabels);
                size_t start = 0;
                size_t end = labelsStr.find('|');
                while (end != std::string::npos) {
                    likertLabelsList.push_back(labelsStr.substr(start, end - start));
                    start = end + 1;
                    end = labelsStr.find('|', start);
                }
                // Handle last label
                if (start < labelsStr.length()) {
                    likertLabelsList.push_back(labelsStr.substr(start));
                }
            }

            // Create questions
            int questionNumber = mBatchImport.startNumber;

            // Prepare label keys if labels are provided (will be set per-question)
            std::vector<std::string> labelKeys;
            if (mBatchImport.questionType == 0 && !likertLabelsList.empty()) {
                // Create translation keys for response labels
                for (size_t i = 0; i < likertLabelsList.size(); i++) {
                    std::string labelKey = std::string(mBatchImport.idPrefix) + "_response_" + std::to_string(i + 1);
                    labelKeys.push_back(labelKey);
                    // Add label to translations
                    mCurrentScale->GetTranslations()["en"][labelKey] = likertLabelsList[i];
                }

                // Also update scale-level defaults (for questions that don't override)
                mCurrentScale->GetLikertOptions().points = mBatchImport.likertPoints;
                mCurrentScale->GetLikertOptions().labels = labelKeys;
                mCurrentScale->GetLikertOptions().min = mBatchImport.likertMin;
                mCurrentScale->GetLikertOptions().max = mBatchImport.likertMax;
            }

            // Pre-build set of existing IDs to detect duplicates
            std::set<std::string> existingIds;
            for (const auto& q : mCurrentScale->GetQuestions()) existingIds.insert(q.id);

            for (const auto& line : lines) {
                ScaleQuestion newQuestion;
                // Format question number with 3-digit zero padding (e.g., 001, 002, ..., 010, 011)
                char numStr[16];
                snprintf(numStr, sizeof(numStr), "%03d", questionNumber);
                newQuestion.id = std::string(mBatchImport.idPrefix) + numStr;
                newQuestion.text_key = newQuestion.id;
                newQuestion.type = questionTypes[mBatchImport.questionType];

                // Skip if ID already exists
                if (existingIds.count(newQuestion.id)) {
                    printf("Warning: Skipping duplicate question ID '%s'\n", newQuestion.id.c_str());
                    questionNumber++;
                    continue;
                }
                existingIds.insert(newQuestion.id);  // Track newly added ID

                // Apply likert-specific settings
                if (mBatchImport.questionType == 0) {  // likert
                    newQuestion.likert_points = mBatchImport.likertPoints;
                    newQuestion.likert_min = mBatchImport.likertMin;
                    newQuestion.likert_max = mBatchImport.likertMax;
                    // Set labels at question level
                    newQuestion.likert_labels = labelKeys;
                }

                mCurrentScale->GetQuestions().push_back(newQuestion);

                // Also add to translations (English)
                mCurrentScale->GetTranslations()["en"][newQuestion.text_key] = line;

                questionNumber++;
            }

            printf("Batch imported %zu questions\n", lines.size());
            mBatchImport.show = false;
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(150, 0))) {
        mBatchImport.show = false;
    }

    ImGui::End();
}

void LauncherUI::ShowDimensionEditor()
{
    if (!mCurrentScale) {
        mDimensionEditor.show = false;
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));

    bool isEditing = (mDimensionEditor.editingIndex >= 0);
    const char* windowTitle = isEditing ? "Edit Dimension" : "Add Dimension";

    if (!ImGui::Begin(windowTitle, &mDimensionEditor.show, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("Dimension Details");
    ImGui::Separator();
    ImGui::Spacing();

    // Dimension ID (read-only when editing to avoid breaking scoring references)
    if (isEditing) {
        ImGui::InputText("ID", mDimensionEditor.id, sizeof(mDimensionEditor.id), ImGuiInputTextFlags_ReadOnly);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("ID cannot be changed after creation (used in scoring references)");
        }
    } else {
        ImGui::InputText("ID", mDimensionEditor.id, sizeof(mDimensionEditor.id));
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Short identifier used in scoring (e.g., 'checking', 'cleaning')");
        }
    }

    // Name
    ImGui::InputText("Name", mDimensionEditor.name, sizeof(mDimensionEditor.name));
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Full name (e.g., 'Checking Behaviors')");
    }

    // Abbreviation
    ImGui::InputText("Abbreviation", mDimensionEditor.abbreviation, sizeof(mDimensionEditor.abbreviation));
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Short abbreviation (e.g., 'CHK')");
    }

    // Description
    ImGui::Spacing();
    ImGui::Text("Description:");
    ImGui::InputTextMultiline("##DimDescription", mDimensionEditor.description, sizeof(mDimensionEditor.description), ImVec2(-1, 100));

    // Parameter-driven enable/disable
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Dimension Selection");
    ImGui::Spacing();

    ImGui::Checkbox("Allow researcher to enable/disable this dimension##selectable", &mDimensionEditor.selectable);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("When checked, a boolean parameter is auto-generated in the study\nschema so researchers can turn this dimension on or off.");
    }

    if (mDimensionEditor.selectable) {
        ImGui::Checkbox("Enabled by default##defEnabled", &mDimensionEditor.defaultEnabled);
        ImGui::SetNextItemWidth(200);
        ImGui::InputText("Parameter name (optional)##enabledParam", mDimensionEditor.enabledParam, sizeof(mDimensionEditor.enabledParam));
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Leave blank to auto-name as do_{id}. Must match enabled_param\nin the OSD if you want to reference it elsewhere.");
        }
    }

    // Conditional Display section
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Conditional Display");
    ImGui::Spacing();

    ImGui::Checkbox("Show conditionally##dim", &mDimensionEditor.hasVisibleWhen);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("When set, all questions in this dimension are shown/hidden\nbased on these conditions. Evaluated dynamically —\ncan reference previous answers.");
    }

    if (mDimensionEditor.hasVisibleWhen) {
        const char* logicItems[] = { "AND (all must match)", "OR (any must match)" };
        ImGui::Combo("Combine with##dim", &mDimensionEditor.visibleWhenLogic, logicItems, IM_ARRAYSIZE(logicItems));

        int removeIndex = -1;
        for (int ci = 0; ci < (int)mDimensionEditor.visibleWhenConditions.size(); ci++) {
            auto& cond = mDimensionEditor.visibleWhenConditions[ci];
            ImGui::PushID(ci);

            const char* sourceTypes[] = { "Parameter", "Item" };
            ImGui::PushItemWidth(90);
            ImGui::Combo("##src", &cond.sourceType, sourceTypes, IM_ARRAYSIZE(sourceTypes));
            ImGui::PopItemWidth();
            ImGui::SameLine();

            ImGui::PushItemWidth(100);
            ImGui::InputText("##name", cond.sourceName, sizeof(cond.sourceName));
            ImGui::PopItemWidth();
            ImGui::SameLine();

            const char* operators[] = { "equals", "not_equals", "greater_than", "less_than" };
            ImGui::PushItemWidth(100);
            ImGui::Combo("##op", &cond.op, operators, IM_ARRAYSIZE(operators));
            ImGui::PopItemWidth();
            ImGui::SameLine();

            ImGui::PushItemWidth(100);
            ImGui::InputText("##val", cond.value, sizeof(cond.value));
            ImGui::PopItemWidth();
            ImGui::SameLine();

            if (ImGui::SmallButton("X")) {
                removeIndex = ci;
            }

            ImGui::PopID();
        }
        if (removeIndex >= 0) {
            mDimensionEditor.visibleWhenConditions.erase(
                mDimensionEditor.visibleWhenConditions.begin() + removeIndex);
        }

        if (ImGui::SmallButton("+ Add Condition##dim")) {
            EditorCondition ec;
            mDimensionEditor.visibleWhenConditions.push_back(ec);
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Helper lambda to save visible_when from editor to dimension
    auto saveDimVisibleWhen = [this](ScaleDimension& dim) {
        dim.has_visible_when = mDimensionEditor.hasVisibleWhen;
        dim.visible_when_logic = (mDimensionEditor.visibleWhenLogic == 1) ? "any" : "all";
        dim.visible_when.clear();
        if (mDimensionEditor.hasVisibleWhen) {
            const char* opNames[] = { "equals", "not_equals", "greater_than", "less_than", "in", "not_in" };
            for (const auto& ec : mDimensionEditor.visibleWhenConditions) {
                VisibleWhenCondition c;
                c.source_type = (ec.sourceType == 1) ? "item" : "parameter";
                c.source_name = ec.sourceName;
                c.op = opNames[ec.op < 6 ? ec.op : 0];
                if (ec.op == 4 || ec.op == 5) {
                    c.is_list = true;
                    std::istringstream ss(ec.value);
                    std::string token;
                    while (std::getline(ss, token, ',')) {
                        auto s = token.find_first_not_of(" \t");
                        auto e2 = token.find_last_not_of(" \t");
                        if (s != std::string::npos)
                            c.values.push_back(token.substr(s, e2 - s + 1));
                    }
                } else {
                    c.is_list = false;
                    c.value = ec.value;
                }
                dim.visible_when.push_back(c);
            }
        }
    };

    // Buttons
    const char* okLabel = isEditing ? "Save" : "Create";
    if (ImGui::Button(okLabel, ImVec2(120, 0))) {
        // Validate
        if (strlen(mDimensionEditor.id) == 0) {
            printf("Error: Dimension ID cannot be empty\n");
        } else if (strlen(mDimensionEditor.name) == 0) {
            printf("Error: Dimension name cannot be empty\n");
        } else if (isEditing) {
            // Update existing dimension
            auto& dim = mCurrentScale->GetDimensions()[mDimensionEditor.editingIndex];
            dim.name = mDimensionEditor.name;
            dim.abbreviation = mDimensionEditor.abbreviation;
            dim.description = mDimensionEditor.description;
            dim.selectable = mDimensionEditor.selectable;
            dim.default_enabled = mDimensionEditor.defaultEnabled;
            dim.enabled_param = mDimensionEditor.selectable ? mDimensionEditor.enabledParam : "";
            saveDimVisibleWhen(dim);
            mCurrentScale->SetDirty(true);
            mDimensionEditor.show = false;
        } else {
            // Create new dimension
            ScaleDimension newDimension;
            newDimension.id = mDimensionEditor.id;
            newDimension.name = mDimensionEditor.name;
            newDimension.abbreviation = mDimensionEditor.abbreviation;
            newDimension.description = mDimensionEditor.description;
            newDimension.selectable = mDimensionEditor.selectable;
            newDimension.default_enabled = mDimensionEditor.defaultEnabled;
            newDimension.enabled_param = mDimensionEditor.selectable ? mDimensionEditor.enabledParam : "";
            saveDimVisibleWhen(newDimension);

            mCurrentScale->GetDimensions().push_back(newDimension);
            mCurrentScale->SetDirty(true);
            printf("Added dimension: %s (%s)\n", newDimension.name.c_str(), newDimension.id.c_str());

            mDimensionEditor.show = false;
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120, 0))) {
        mDimensionEditor.show = false;
    }

    ImGui::End();
}

void LauncherUI::ShowCorrectAnswersEditor()
{
    if (!mCurrentScale) {
        mCorrectAnswersEditor.show = false;
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(800, 500), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
                            ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));

    std::string title = "Correct Answers: " + mCorrectAnswersEditor.questionId;
    if (!ImGui::Begin(title.c_str(), &mCorrectAnswersEditor.show, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::End();
        return;
    }

    // Compact header: question context + instructions side by side
    ImGui::TextWrapped("Q: %s", mCorrectAnswersEditor.questionText.c_str());
    ImGui::SameLine();
    ImGui::TextDisabled("(%s)", mCorrectAnswersEditor.questionType.c_str());

    if (mCorrectAnswersEditor.questionType == "multicheck") {
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.3f, 1.0f),
            "Multicheck: enter the set of options that should all be selected.");
    } else if (mCorrectAnswersEditor.questionType == "grid") {
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.3f, 1.0f),
            "Grid: enter one correct value per row (column number, 1-based).");
    } else {
        ImGui::TextDisabled("Response is correct if it matches ANY entry below.  "
                            "Use * for any chars, ? for single char.  Case-insensitive by default.");
    }

    ImGui::Separator();

    // Answer list header
    float matchCaseWidth = 80.0f;
    float removeWidth = 60.0f;
    float spacing = ImGui::GetStyle().ItemSpacing.x;

    // Column labels
    ImGui::Text("Answer / Pattern");
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - matchCaseWidth - removeWidth - spacing);
    ImGui::Text("Aa");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Match case: when checked, comparison is case-sensitive");
    }

    // Scrollable answer list
    int removeIndex = -1;

    ImGui::BeginChild("AnswerList", ImVec2(0, -35), true);
    for (size_t i = 0; i < mCorrectAnswersEditor.answers.size(); i++) {
        ImGui::PushID((int)i);

        // Input field
        char buf[512];
        std::strncpy(buf, mCorrectAnswersEditor.answers[i].c_str(), sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';

        float inputWidth = ImGui::GetContentRegionAvail().x - matchCaseWidth - removeWidth - spacing * 2;
        ImGui::SetNextItemWidth(inputWidth);
        if (ImGui::InputText("##ans", buf, sizeof(buf))) {
            mCorrectAnswersEditor.answers[i] = buf;
        }

        // Match case checkbox
        ImGui::SameLine();
        // Ensure caseSensitive vector is in sync
        while (mCorrectAnswersEditor.caseSensitive.size() <= i) {
            mCorrectAnswersEditor.caseSensitive.push_back(false);
        }
        bool cs = mCorrectAnswersEditor.caseSensitive[i];
        if (ImGui::Checkbox("Match##cs", &cs)) {
            mCorrectAnswersEditor.caseSensitive[i] = cs;
        }

        // Remove button
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.2f, 0.2f, 1.0f));
        if (ImGui::Button("X", ImVec2(25, 0))) {
            removeIndex = (int)i;
        }
        ImGui::PopStyleColor();

        ImGui::PopID();
    }
    ImGui::EndChild();

    // Remove the marked item (outside the loop)
    if (removeIndex >= 0) {
        mCorrectAnswersEditor.answers.erase(mCorrectAnswersEditor.answers.begin() + removeIndex);
        if (removeIndex < static_cast<int>(mCorrectAnswersEditor.caseSensitive.size())) {
            mCorrectAnswersEditor.caseSensitive.erase(mCorrectAnswersEditor.caseSensitive.begin() + removeIndex);
        }
    }

    // Bottom buttons
    if (ImGui::Button("+ Add")) {
        mCorrectAnswersEditor.answers.push_back("");
        mCorrectAnswersEditor.caseSensitive.push_back(false);
    }

    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 160);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
    if (ImGui::Button("OK", ImVec2(70, 0))) {
        // Save answers back to scoring data, reconstructing (?c) prefix
        auto& scoring = mCurrentScale->GetScoring();
        auto it = scoring.find(mCorrectAnswersEditor.dimensionId);
        if (it != scoring.end()) {
            std::vector<std::string> cleaned;
            for (size_t i = 0; i < mCorrectAnswersEditor.answers.size(); i++) {
                const auto& ans = mCorrectAnswersEditor.answers[i];
                if (!ans.empty()) {
                    bool cs = (i < mCorrectAnswersEditor.caseSensitive.size()) && mCorrectAnswersEditor.caseSensitive[i];
                    if (cs) {
                        cleaned.push_back("(?c)" + ans);
                    } else {
                        cleaned.push_back(ans);
                    }
                }
            }
            if (cleaned.empty()) {
                it->second.correct_answers.erase(mCorrectAnswersEditor.questionId);
            } else {
                it->second.correct_answers[mCorrectAnswersEditor.questionId] = cleaned;
            }
            mCurrentScale->SetDirty(true);
        }
        mCorrectAnswersEditor.show = false;
    }
    ImGui::PopStyleColor();

    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(70, 0))) {
        mCorrectAnswersEditor.show = false;
    }

    ImGui::End();
}

void LauncherUI::ShowNormsEditor()
{
    if (!mCurrentScale) {
        mNormsEditor.show = false;
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
                            ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));

    std::string title = "Norms Editor — " + mNormsEditor.dimensionName;
    if (!ImGui::Begin(title.c_str(), &mNormsEditor.show, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::End();
        return;
    }

    ImGui::TextDisabled("Set score ranges and interpretation labels for the report.");
    ImGui::Separator();

    int removeIndex = -1;

    ImGui::BeginChild("ThresholdList", ImVec2(0, -40), true);
    if (ImGui::BeginTable("NormsTable", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp))
    {
        ImGui::TableSetupColumn("Min",   ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Max",   ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("##del", ImGuiTableColumnFlags_WidthFixed, 30.0f);
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < mNormsEditor.rows.size(); i++) {
            ImGui::PushID((int)i);
            auto& row = mNormsEditor.rows[i];

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::SetNextItemWidth(-1);
            ImGui::InputFloat("##min", &row.minVal, 0, 0, "%.1f");

            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-1);
            ImGui::InputFloat("##max", &row.maxVal, 0, 0, "%.1f");

            ImGui::TableSetColumnIndex(2);
            ImGui::SetNextItemWidth(-1);
            ImGui::InputText("##label", row.label, sizeof(row.label));

            ImGui::TableSetColumnIndex(3);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.2f, 0.2f, 1.0f));
            if (ImGui::SmallButton("x")) {
                removeIndex = (int)i;
            }
            ImGui::PopStyleColor();

            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    ImGui::EndChild();

    if (removeIndex >= 0) {
        mNormsEditor.rows.erase(mNormsEditor.rows.begin() + removeIndex);
    }

    if (ImGui::Button("+ Add Threshold")) {
        NormsEditorState::ThresholdEdit te;
        if (!mNormsEditor.rows.empty()) {
            float prevMax = mNormsEditor.rows.back().maxVal;
            te.minVal = prevMax + 1.0f;
            te.maxVal = te.minVal + 5.0f;
        }
        mNormsEditor.rows.push_back(te);
    }

    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 160);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
    if (ImGui::Button("Save", ImVec2(70, 0))) {
        auto& scoring = mCurrentScale->GetScoring();
        auto it = scoring.find(mNormsEditor.dimensionId);
        if (it != scoring.end()) {
            it->second.norms.clear();
            for (const auto& row : mNormsEditor.rows) {
                NormThreshold nt;
                nt.min = (double)row.minVal;
                nt.max = (double)row.maxVal;
                nt.label = row.label;
                it->second.norms.push_back(nt);
            }
            mCurrentScale->SetDirty(true);
        }
        mNormsEditor.show = false;
    }
    ImGui::PopStyleColor();

    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(70, 0))) {
        mNormsEditor.show = false;
    }

    ImGui::End();
}

void LauncherUI::ShowCreateStudyFromScaleDialog()
{
    // If in scale-selection mode, we need mWorkspace but not mCurrentScale
    // If not in scale-selection mode, we need both
    if (!mWorkspace) {
        mCreateStudyDialog.show = false;
        return;
    }

    if (!mCreateStudyDialog.needScaleSelection && !mCurrentScale) {
        mCreateStudyDialog.show = false;
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(500, 350), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));

    if (!ImGui::Begin("Add Scale to Study", &mCreateStudyDialog.show, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("Add this scale as a test in a study.");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Scale selection (if needed - when opened from Study Bar)
    if (mCreateStudyDialog.needScaleSelection) {
        ImGui::Text("Select Scale:");
        const char* currentScaleName = (mCreateStudyDialog.selectedScaleIndex >= 0 &&
                                        mCreateStudyDialog.selectedScaleIndex < (int)mScaleList.size())
                                        ? mScaleList[mCreateStudyDialog.selectedScaleIndex].c_str()
                                        : "Select a scale...";

        if (ImGui::BeginCombo("##ScaleSelect", currentScaleName)) {
            for (size_t i = 0; i < mScaleList.size(); i++) {
                bool is_selected = (mCreateStudyDialog.selectedScaleIndex == (int)i);
                if (ImGui::Selectable(mScaleList[i].c_str(), is_selected)) {
                    mCreateStudyDialog.selectedScaleIndex = i;
                    std::strncpy(mCreateStudyDialog.studyName, mScaleList[i].c_str(),
                                sizeof(mCreateStudyDialog.studyName) - 1);
                    mCreateStudyDialog.studyName[sizeof(mCreateStudyDialog.studyName) - 1] = '\0';
                    mCreateStudyDialog.confirmOverwrite = false;
                    mCreateStudyDialog.errorMessage[0] = '\0';
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::Spacing();
    }

    // Mode selector: Create New Study vs Add to Existing Study
    if (ImGui::RadioButton("Create new study", !mCreateStudyDialog.addToExisting)) {
        mCreateStudyDialog.addToExisting = false;
        mCreateStudyDialog.errorMessage[0] = '\0';
        mCreateStudyDialog.confirmOverwrite = false;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Add to existing study", mCreateStudyDialog.addToExisting)) {
        mCreateStudyDialog.addToExisting = true;
        mCreateStudyDialog.errorMessage[0] = '\0';
        mCreateStudyDialog.confirmOverwrite = false;
    }

    ImGui::Spacing();

    if (mCreateStudyDialog.addToExisting) {
        // Existing study dropdown
        ImGui::Text("Select Study:");
        const char* currentStudyName = (mCreateStudyDialog.selectedStudyIndex >= 0 &&
                                        mCreateStudyDialog.selectedStudyIndex < (int)mStudyList.size())
                                        ? mStudyList[mCreateStudyDialog.selectedStudyIndex].c_str()
                                        : "Select a study...";

        if (ImGui::BeginCombo("##ExistingStudySelect", currentStudyName)) {
            for (size_t i = 0; i < mStudyList.size(); i++) {
                bool is_selected = (mCreateStudyDialog.selectedStudyIndex == (int)i);
                if (ImGui::Selectable(mStudyList[i].c_str(), is_selected)) {
                    mCreateStudyDialog.selectedStudyIndex = (int)i;
                    mCreateStudyDialog.errorMessage[0] = '\0';
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    } else {
        // New study name input
        ImGui::Text("Study Name:");
        if (ImGui::InputText("##StudyName", mCreateStudyDialog.studyName, sizeof(mCreateStudyDialog.studyName))) {
            mCreateStudyDialog.confirmOverwrite = false;
            mCreateStudyDialog.errorMessage[0] = '\0';
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("The name of the study directory to create in my_studies/");
        }
    }

    ImGui::Spacing();

    // Show error message if any
    if (strlen(mCreateStudyDialog.errorMessage) > 0) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
        ImGui::TextWrapped("%s", mCreateStudyDialog.errorMessage);
        ImGui::PopStyleColor();
        ImGui::Spacing();
    }

    ImGui::Separator();
    ImGui::Spacing();

    // Action button
    const char* buttonText = mCreateStudyDialog.addToExisting ? "Add" :
                             (mCreateStudyDialog.confirmOverwrite ? "Update" : "Create");
    if (ImGui::Button(buttonText, ImVec2(120, 0))) {
        // Load the scale if needed
        std::shared_ptr<ScaleDefinition> scaleToUse = mCurrentScale;
        if (mCreateStudyDialog.needScaleSelection) {
            if (mCreateStudyDialog.selectedScaleIndex < 0) {
                std::strncpy(mCreateStudyDialog.errorMessage, "Please select a scale first.",
                            sizeof(mCreateStudyDialog.errorMessage) - 1);
                scaleToUse = nullptr;
            } else {
                std::string scaleCode = mScaleList[mCreateStudyDialog.selectedScaleIndex];
                scaleToUse = mScaleManager->LoadScale(scaleCode);
                if (!scaleToUse) {
                    std::strncpy(mCreateStudyDialog.errorMessage, "Failed to load selected scale.",
                                sizeof(mCreateStudyDialog.errorMessage) - 1);
                }
            }
        }

        if (scaleToUse) {
            if (mCreateStudyDialog.addToExisting) {
                // Add to existing study
                if (mCreateStudyDialog.selectedStudyIndex < 0 ||
                    mCreateStudyDialog.selectedStudyIndex >= (int)mStudyList.size()) {
                    std::strncpy(mCreateStudyDialog.errorMessage, "Please select a study.",
                                sizeof(mCreateStudyDialog.errorMessage) - 1);
                } else {
                    std::string studyDir = mStudyList[mCreateStudyDialog.selectedStudyIndex];
                    std::string studyPath = mWorkspace->GetStudiesPath() + "/" + studyDir;

                    if (mScaleManager->AddScaleToStudy(scaleToUse, studyPath)) {
                        printf("Scale '%s' added to study '%s'\n",
                               scaleToUse->GetScaleInfo().code.c_str(), studyDir.c_str());

                        // Also add to "Main" chain if it exists
                        std::string mainChainPath = studyPath + "/chains/Main.json";
                        if (fs::exists(mainChainPath)) {
                            ChainItem item(ItemType::Test);
                            item.testName = scaleToUse->GetScaleInfo().code;
                            item.paramVariant = "default";
                            item.language = "en";
                            item.randomGroup = 0;

                            if (mCurrentChain && mCurrentChain->GetFilePath() == mainChainPath) {
                                mCurrentChain->AddItem(item);
                                mCurrentChain->Save();
                                printf("Added scale to Main chain (current chain)\n");
                            } else {
                                auto mainChain = Chain::LoadFromFile(mainChainPath);
                                if (mainChain) {
                                    mainChain->AddItem(item);
                                    mainChain->Save();
                                    printf("Added scale to Main chain\n");
                                }
                            }
                        }

                        mCreateStudyDialog.show = false;
                        mStudyList = mWorkspace->GetStudyDirectories();

                        // Reload current study so the new test appears in the listing
                        if (mCurrentStudy && mCurrentStudy->GetPath() == studyPath) {
                            LoadStudy(studyPath);
                        }
                    } else {
                        std::strncpy(mCreateStudyDialog.errorMessage,
                                    "Failed to add scale to study. Check console for details.",
                                    sizeof(mCreateStudyDialog.errorMessage) - 1);
                    }
                }
            } else {
                // Create new study
                if (strlen(mCreateStudyDialog.studyName) == 0) {
                    std::strncpy(mCreateStudyDialog.errorMessage, "Study name cannot be empty.",
                                sizeof(mCreateStudyDialog.errorMessage) - 1);
                } else {
                    std::string studyPath = mWorkspace->GetStudiesPath() + "/" + std::string(mCreateStudyDialog.studyName);
                    if (fs::exists(studyPath) && !mCreateStudyDialog.confirmOverwrite) {
                        std::string warnMsg = "Study '" + std::string(mCreateStudyDialog.studyName) + "' already exists. Click Update to overwrite it.";
                        std::strncpy(mCreateStudyDialog.errorMessage, warnMsg.c_str(),
                                    sizeof(mCreateStudyDialog.errorMessage) - 1);
                        mCreateStudyDialog.errorMessage[sizeof(mCreateStudyDialog.errorMessage) - 1] = '\0';
                        mCreateStudyDialog.confirmOverwrite = true;
                    } else {
                        std::string customName = std::string(mCreateStudyDialog.studyName);
                        if (mScaleManager->CreateStudyFromScale(scaleToUse, mWorkspace->GetStudiesPath(), customName)) {
                            printf("Scale study '%s' created successfully in my_studies/\n", mCreateStudyDialog.studyName);
                            mCreateStudyDialog.show = false;
                            mCreateStudyDialog.confirmOverwrite = false;
                            mStudyList = mWorkspace->GetStudyDirectories();
                        } else {
                            std::strncpy(mCreateStudyDialog.errorMessage,
                                        "Failed to create study. Check console for details.",
                                        sizeof(mCreateStudyDialog.errorMessage) - 1);
                        }
                    }
                }
            }
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120, 0))) {
        mCreateStudyDialog.show = false;
        mCreateStudyDialog.confirmOverwrite = false;
    }

    ImGui::End();
}

void LauncherUI::TestCurrentScale()
{
    if (!mCurrentScale) {
        printf("Error: No scale loaded to test\n");
        return;
    }

    if (!mWorkspace) {
        printf("Error: No workspace loaded\n");
        return;
    }

    printf("Testing scale: %s\n", mCurrentScale->GetScaleInfo().code.c_str());

    try {
        std::string scaleCode = mCurrentScale->GetScaleInfo().code;

        // Create temp directory in workspace: workspace/temp/scale-test-{code}/
        std::string tempDir = mWorkspace->GetWorkspacePath() + "/temp/scale-test-" + scaleCode;

        // Remove old temp directory if it exists
        if (fs::exists(tempDir)) {
            fs::remove_all(tempDir);
        }

        // Create temp directory structure matching ScaleRunner's expected layout:
        //   {tempDir}/
        //     {scaleCode}.pbl       <- ScaleRunner.pbl (renamed)
        //     definitions/          <- {scaleCode}.json
        //     translations/         <- {scaleCode}.{lang}.json
        //     params/               <- parameter file
        //     data/                 <- output
        fs::create_directories(tempDir);
        fs::create_directories(tempDir + "/definitions");
        fs::create_directories(tempDir + "/translations");
        fs::create_directories(tempDir + "/params");
        fs::create_directories(tempDir + "/data");

        printf("Created temp test directory: %s\n", tempDir.c_str());

        // Copy ScaleRunner.pbl to temp directory
        std::string scaleRunnerSource = mBatteryPath + "/../media/apps/scales/ScaleRunner.pbl";
        std::string scaleRunnerDest = tempDir + "/" + scaleCode + ".pbl";

        if (!fs::exists(scaleRunnerSource)) {
            printf("Error: ScaleRunner.pbl not found at: %s\n", scaleRunnerSource.c_str());
            return;
        }

        fs::copy_file(scaleRunnerSource, scaleRunnerDest, fs::copy_options::overwrite_existing);
        printf("Copied ScaleRunner.pbl\n");

        // Export scale definition to definitions/ and translations to translations/
        if (!mCurrentScale->ExportToJSON(tempDir + "/definitions", tempDir + "/translations")) {
            printf("Error: Failed to export scale files\n");
            return;
        }

        printf("Exported scale definition and translations\n");

        // Generate schema and default params from scale definition
        SyncScaleSchema(tempDir, scaleCode);

        printf("Preparing to test scale...\n");

        // Create experiment runner
        if (mRunningExperiment) {
            if (mRunningExperiment->IsRunning()) {
                printf("Warning: Previous test still running\n");
                return;
            }
            delete mRunningExperiment;
            mRunningExperiment = nullptr;
        }

        // Run the test using --pfile to pass the parameter file
        // (PEBL prepends "params/" to the --pfile argument)
        mRunningExperiment = new ExperimentRunner(mConfig);
        std::vector<std::string> args = {
            "--pfile", scaleCode + ".pbl.par.json",
            "--windowed"
        };

        bool success = mRunningExperiment->RunExperiment(scaleRunnerDest, args,
                                                          ("TEST_" + scaleCode).c_str(),
                                                          "en", false);

        if (success) {
            printf("Scale test started successfully\n");
            printf("Working directory: %s\n", tempDir.c_str());
            printf("Data will be saved to: %s/data/\n", tempDir.c_str());
            mShowStderr = false;  // Start showing stdout
        } else {
            printf("Failed to run scale test\n");
            delete mRunningExperiment;
            mRunningExperiment = nullptr;
        }

    } catch (const std::exception& e) {
        printf("Exception while testing scale: %s\n", e.what());
    }
}

void LauncherUI::ShowSnapshotCreatedDialog()
{
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(650, 300), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Snapshot Created", &mShowSnapshotCreated, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::End();
        return;
    }

    ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "✓ Snapshot created successfully!");
    ImGui::Separator();
    ImGui::Spacing();

    // Show snapshot info
    ImGui::Text("Snapshot Name:");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "%s", mLastSnapshotName);

    ImGui::Spacing();

    ImGui::Text("Location:");
    ImGui::TextWrapped("%s", mLastSnapshotPath);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextWrapped("This snapshot excludes data/ directories and is ready to upload to PEBLHub or share with others.");
    ImGui::Spacing();

    // Buttons
    if (ImGui::Button("Open in File Manager", ImVec2(200, 0))) {
        OpenDirectoryInFileBrowser(std::string(mLastSnapshotPath));
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Open the snapshot directory in your file manager");
    }

    ImGui::SameLine();

    // Create ZIP button
    if (ImGui::Button("Create ZIP", ImVec2(150, 0))) {
        // Create ZIP file from snapshot
        std::string zipPath = std::string(mLastSnapshotPath) + ".zip";
        printf("Creating ZIP file: %s\n", zipPath.c_str());

        // Use zip command (cross-platform via shell)
#ifdef _WIN32
        // Windows: Use PowerShell Compress-Archive
        std::string command = "powershell -Command \"Compress-Archive -Path '" +
                             std::string(mLastSnapshotPath) + "' -DestinationPath '" +
                             zipPath + "' -Force\"";
#else
        // Linux/Mac: Use zip command
        std::string command = "cd \"" + fs::path(mLastSnapshotPath).parent_path().string() + "\" && " +
                             "zip -r \"" + fs::path(zipPath).filename().string() + "\" \"" +
                             fs::path(mLastSnapshotPath).filename().string() + "\"";
#endif

        printf("Running: %s\n", command.c_str());
        int result = system(command.c_str());

        if (result == 0) {
            printf("ZIP file created: %s\n", zipPath.c_str());
            // Show success message
            ImGui::OpenPopup("ZIP Created");
        } else {
            printf("Failed to create ZIP file (exit code: %d)\n", result);
            ImGui::OpenPopup("ZIP Failed");
        }
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Create a ZIP file from this snapshot for easy sharing/upload");
    }

    ImGui::SameLine();

    if (ImGui::Button("Close", ImVec2(100, 0))) {
        mShowSnapshotCreated = false;
    }

    // Success/failure popups for ZIP creation
    if (ImGui::BeginPopupModal("ZIP Created", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "ZIP file created successfully!");
        ImGui::Text("Location: %s.zip", mLastSnapshotPath);
        ImGui::Spacing();
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("ZIP Failed", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Failed to create ZIP file");
        ImGui::Text("You can manually ZIP the snapshot directory:");
        ImGui::TextWrapped("%s", mLastSnapshotPath);
        ImGui::Spacing();
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}
