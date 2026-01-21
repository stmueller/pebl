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
#include "../../utility/BinReloc.h"
#include "imgui.h"
#include <SDL2/SDL_image.h>
#include <cstring>
#include <cctype>
#include <ctime>
#include <algorithm>
#include <random>
#include <map>
#include <filesystem>
#include <fstream>
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
    , mRunningChain(false)
    , mCurrentChainItemIndex(-1)
    , mShowParameterEditor(false)
    , mShowVariantNameDialog(false)
    , mEditingTestIndex(-1)
    , mShowDuplicateSubjectWarning(false)
    , mShowSettings(false)
    , mShowNewStudyDialog(false)
    , mShowNewChainDialog(false)
    , mShowStudySettingsDialog(false)
    , mShowFirstRunDialog(false)
    , mShowEditParticipantCodeDialog(false)
    , mAddTestSubTab(0)
    , mQuickLaunchSelectedFile(-1)
    , mShowCodeEditor(false)
{
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

    // Show translation editor dialog if requested
    if (mTranslationEditor.show) {
        ShowTranslationEditorDialog();
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

    // Show duplicate subject code warning if requested
    if (mShowDuplicateSubjectWarning) {
        ShowDuplicateSubjectWarning();
    }

    // Show edit participant code dialog if requested
    if (mShowEditParticipantCodeDialog) {
        ShowEditParticipantCodeDialog();
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
                        printf("Created snapshot: %s\n", snapshotName.c_str());
                        // Show success message
                        ImGui::OpenPopup("Snapshot Created");
                    } else {
                        printf("Failed to create snapshot\n");
                    }
                }
            }

            if (ImGui::MenuItem("Import Snapshot...")) {
                std::string snapshotPath = OpenDirectoryDialog("Select Snapshot Directory");
                if (!snapshotPath.empty() && mSnapshots) {
                    // Validate snapshot
                    auto validation = mSnapshots->ValidateSnapshot(snapshotPath);
                    if (validation.isValid) {
                        // Get snapshot info to suggest a name
                        auto info = mSnapshots->GetSnapshotInfo(snapshotPath);

                        // Import directly with suggested name
                        std::string studiesDir = mWorkspace->GetStudiesPath();
                        std::string newStudyName = info.studyName + "_imported";

                        if (mSnapshots->ImportSnapshot(snapshotPath, studiesDir, newStudyName)) {
                            printf("Imported snapshot as: %s\n", newStudyName.c_str());

                            // Load the newly imported study
                            std::string newStudyPath = studiesDir + "/" + newStudyName;
                            LoadStudy(newStudyPath);
                        } else {
                            printf("Failed to import snapshot\n");
                        }
                    } else {
                        printf("Invalid snapshot directory:\n");
                        for (const auto& error : validation.errors) {
                            printf("  - %s\n", error.c_str());
                        }
                    }
                }
            }

            if (ImGui::MenuItem("Import Snapshot ZIP...")) {
                std::string zipPath = OpenFileDialog("Select Snapshot ZIP", "*.zip");
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

                        if (snapshotPath.empty()) {
                            printf("Could not locate snapshot directory in extracted ZIP\n");
                            std::string cleanupCmd = "rm -rf " + tempDir;
                            system(cleanupCmd.c_str());
                            return;
                        }

                        // Convert platform format to launcher format (in place on extracted snapshot)
                        if (!mSnapshots->ConvertSnapshotFormat(snapshotPath)) {
                            printf("Warning: Failed to convert snapshot format, attempting to use as-is\n");
                        }

                        // Validate and import
                        auto validation = mSnapshots->ValidateSnapshot(snapshotPath);
                        if (validation.isValid) {
                            auto info = mSnapshots->GetSnapshotInfo(snapshotPath);
                            std::string studiesDir = mWorkspace->GetStudiesPath();
                            std::string newStudyName = info.studyName + "_imported";

                            if (mSnapshots->ImportSnapshot(snapshotPath, studiesDir, newStudyName)) {
                                printf("Imported snapshot ZIP as: %s\n", newStudyName.c_str());

                                // Load the newly imported study
                                std::string newStudyPath = studiesDir + "/" + newStudyName;
                                LoadStudy(newStudyPath);
                            } else {
                                printf("Failed to import snapshot\n");
                            }
                        } else {
                            printf("Invalid snapshot ZIP:\n");
                            for (const auto& error : validation.errors) {
                                printf("  - %s\n", error.c_str());
                            }
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

    // Output display
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Toggle button for stdout/stderr and expand button
    const char* outputLabel = mShowStderr ? "stderr" : "stdout";
    ImGui::Text("Output (%s):", outputLabel);

    // Expand/collapse button
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 150);
    if (ImGui::Button(mOutputExpanded ? "-" : "+", ImVec2(25, 0))) {
        mOutputExpanded = !mOutputExpanded;
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip(mOutputExpanded ? "Collapse output" : "Expand output");
    }

    // stdout/stderr toggle button
    ImGui::SameLine();
    if (ImGui::Button(mShowStderr ? "Show stdout" : "Show stderr", ImVec2(120, 0))) {
        mShowStderr = !mShowStderr;
    }

    // Scrollable output window - height depends on expanded state
    float outputHeight = mOutputExpanded ? ImGui::GetContentRegionAvail().y : 150.0f;
    ImGui::BeginChild("OutputWindow", ImVec2(0, outputHeight), true, ImGuiWindowFlags_HorizontalScrollbar);

    if (mRunningExperiment) {
        const std::string& output = mShowStderr ? mRunningExperiment->GetStderr() :
                                                   mRunningExperiment->GetStdout();

        if (!output.empty()) {
            // Use InputTextMultiline for selectable text
            ImGui::InputTextMultiline("##output",
                                       const_cast<char*>(output.c_str()),
                                       output.size() + 1,
                                       ImVec2(-1, -1),
                                       ImGuiInputTextFlags_ReadOnly);
        } else if (mRunningExperiment->IsRunning()) {
            ImGui::TextDisabled("Waiting for output...");
        } else {
            ImGui::TextDisabled("No output captured");
        }
    } else {
        ImGui::TextDisabled("Run an experiment to see output here");
    }

    ImGui::EndChild();
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
                    // Ensure all tests in chain have upload.json
                    int created = 0;
                    for (const auto& item : mCurrentChain->GetItems()) {
                        if (!item.IsPageItem()) {
                            if (!mCurrentStudy->TestHasUploadConfig(item.testName)) {
                                if (mCurrentStudy->CreateUploadConfigForTest(item.testName)) {
                                    created++;
                                }
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
                    ImGui::SetTooltip("Main workspace directory (typically Documents/pebl-exp.2.3/)");
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
        ImGui::TextWrapped("Edit experiment parameters. Changes will be saved to a .par.json file.");
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
                ImGui::TableSetupColumn("Description (default)", ImGuiTableColumnFlags_WidthStretch);
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

                    // Column 2: Editable value
                    ImGui::TableSetColumnIndex(1);
                    char buffer[256];
                    std::strncpy(buffer, param.value.c_str(), sizeof(buffer) - 1);
                    buffer[sizeof(buffer) - 1] = '\0';

                    ImGui::PushItemWidth(-1);
                    if (shouldFocusFirst && i == 0) {
                        ImGui::SetKeyboardFocusHere();
                    }
                    if (ImGui::InputText("##value", buffer, sizeof(buffer))) {
                        param.value = buffer;
                    }
                    ImGui::PopItemWidth();

                    // Column 3: Description with default value
                    ImGui::TableSetColumnIndex(2);
                    std::string descText = param.description;
                    if (!param.defaultValue.empty()) {
                        descText += " (default: [" + param.defaultValue + "])";
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

                    // Register the new variant with the study
                    // Extract variant name from file path (e.g., "variant-20260110-123456.par.json" -> "variant-20260110-123456")
                    size_t lastSlash = mParameterFile.find_last_of("/\\");
                    std::string filename = (lastSlash != std::string::npos)
                                         ? mParameterFile.substr(lastSlash + 1)
                                         : mParameterFile;
                    size_t dotPar = filename.find(".par.json");
                    if (dotPar != std::string::npos) {
                        std::string variantName = filename.substr(0, dotPar);

                        // Find which test this belongs to by checking the path
                        // Path format: studyPath/tests/testName/params/variant.par.json
                        size_t testsPos = mParameterFile.find("/tests/");
                        if (testsPos != std::string::npos && mCurrentStudy) {
                            size_t testNameStart = testsPos + 7;  // After "/tests/"
                            size_t testNameEnd = mParameterFile.find("/", testNameStart);
                            if (testNameEnd != std::string::npos) {
                                std::string testName = mParameterFile.substr(testNameStart, testNameEnd - testNameStart);

                                // Add variant to study
                                Test* test = mCurrentStudy->GetTest(testName);
                                if (test) {
                                    ParameterVariant variant;
                                    variant.file = variantName + ".par.json";
                                    variant.description = "Custom variant created " + variantName;
                                    test->parameterVariants[variantName] = variant;

                                    // Save study to persist the new variant
                                    mCurrentStudy->Save();
                                    printf("Registered variant '%s' for test '%s'\n", variantName.c_str(), testName.c_str());
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

        ImGui::TextWrapped("Parameter variants for test: %s", testName.c_str());
        ImGui::Separator();
        ImGui::Spacing();

        // Show existing variants
        if (variants && !variants->empty()) {
            ImGui::Text("Existing Parameter Variants:");
            ImGui::Spacing();

            ImGui::BeginChild("ExistingVariants", ImVec2(0, 150), true);
            for (const auto& pair : *variants) {
                ImGui::PushID(pair.first.c_str());

                if (ImGui::Selectable(pair.first.c_str(), false, 0, ImVec2(0, 30))) {
                    // Load this variant for editing
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
            ImGui::Separator();
            ImGui::Spacing();
        } else {
            ImGui::TextDisabled("No existing parameter variants found.");
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        }

        // Create new variant section
        ImGui::Text("Create New Variant:");
        ImGui::Spacing();

        ImGui::Text("Variant Name:");
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Examples: mousebutton, touchscreen, leftclick, arrowLR\n"
                            "The variant will be saved as %s-<name>.par.json", testName.c_str());
        }

        ImGui::PushItemWidth(-1);
        if (ImGui::IsWindowAppearing()) {
            ImGui::SetKeyboardFocusHere();
        }
        ImGui::InputText("##variantname", mVariantName, sizeof(mVariantName));
        ImGui::PopItemWidth();

        ImGui::Spacing();
        ImGui::TextDisabled("File will be saved as: %s-%s.par.json", testName.c_str(), mVariantName);
        ImGui::Spacing();

        // Buttons
        bool canCreate = strlen(mVariantName) > 0;

        if (!canCreate) {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button("Create New", ImVec2(120, 0))) {
            // Load parameter editor with this variant name
            LoadParameterEditorForVariant();
            mShowVariantNameDialog = false;
            ImGui::CloseCurrentPopup();
        }

        if (!canCreate) {
            ImGui::EndDisabled();
        }

        ImGui::SameLine();

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

            param.value = param.defaultValue;  // Initialize to default
            mParameters.push_back(param);
        }

        printf("Loaded %zu parameters from schema\n", mParameters.size());

        // Build parameter file path using testname-variantname.par.json pattern
        std::string variantFileName = test.testName + "-" + std::string(mVariantName) + ".par.json";
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
            for (const auto& entry : fs::directory_iterator(translationsDir)) {
                if (entry.is_regular_file() && entry.path().extension() == ".json") {
                    std::string filename = entry.path().stem().string();
                    // Extract language code from filename like "taskname.pbl-en.json"
                    size_t dashPos = filename.find_last_of('-');
                    if (dashPos != std::string::npos) {
                        std::string langCode = filename.substr(dashPos + 1);
                        mAvailableLanguages.push_back(langCode);
                    }
                }
            }

            // Sort language codes
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
    if (path.empty() || !fs::exists(path)) {
        printf("Cannot open directory: %s\n", path.c_str());
        return;
    }

#ifdef _WIN32
    // Windows: Use explorer - normalize path to use backslashes
    std::string winPath = path;
    for (char& c : winPath) {
        if (c == '/') c = '\\';
    }
    std::string command = "explorer \"" + winPath + "\"";
    printf("Opening directory: %s\n", winPath.c_str());
#elif __APPLE__
    // macOS: Use open
    std::string command = "open \"" + path + "\"";
#else
    // Linux: Use xdg-open
    std::string command = "xdg-open \"" + path + "\"";
#endif

    // Run command in background
    int result = system(command.c_str());
    if (result != 0) {
        printf("Failed to open directory in file browser: %s\n", path.c_str());
    }
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

        // Add language if specified
        if (!item.language.empty()) {
            args.push_back("-v");
            args.push_back("language=" + item.language);
        }

        // Add parameter variant if not default
        if (!item.paramVariant.empty() && item.paramVariant != "default") {
            args.push_back("-v");
            args.push_back("gParamFile=" + item.paramVariant);
        }

        // Add upload flag if chain has upload enabled
        if (mCurrentChain->GetUploadEnabled()) {
            std::string uploadPath = mCurrentStudy->GetUploadConfigPath(item.testName);

            // Ensure upload.json exists for this test
            if (!mCurrentStudy->TestHasUploadConfig(item.testName)) {
                printf("Creating upload.json for test: %s\n", item.testName.c_str());
                mCurrentStudy->CreateUploadConfigForTest(item.testName);
            }

            // Add --upload flag
            args.push_back("--upload");
            args.push_back(uploadPath);
            printf("Upload enabled: %s\n", uploadPath.c_str());
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

        // Add language if specified
        if (!item.language.empty()) {
            args.push_back("-v");
            args.push_back("language=" + item.language);
        }

        // Add parameter variant if not default
        if (!item.paramVariant.empty() && item.paramVariant != "default") {
            args.push_back("-v");
            args.push_back("gParamFile=" + item.paramVariant);
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

std::string LauncherUI::OpenFileDialog(const std::string& title, const std::string& filter)
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
        // "None" option
        if (ImGui::Selectable("None", !mCurrentStudy)) {
            mCurrentStudy.reset();
        }

        // List studies from workspace
        for (size_t i = 0; i < mStudyList.size(); i++) {
            bool is_selected = (mCurrentStudy && mCurrentStudy->GetName() == mStudyList[i]);
            if (ImGui::Selectable(mStudyList[i].c_str(), is_selected)) {
                mSelectedStudyIndex = i;
                auto studyDirs = mWorkspace->GetStudyDirectories();
                if (i < studyDirs.size()) {
                    LoadStudy(studyDirs[i]);
                }
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

    // Tests in study section
    ImGui::Text("Tests in Study:");
    ImGui::BeginChild("StudyTestsList", ImVec2(0, -40), true);

    if (!mCurrentStudy) {
        ImGui::TextDisabled("Load a study to view tests");
    } else if (mCurrentStudy->GetTests().empty()) {
        ImGui::TextDisabled("No tests in this study. Use 'Add to Study' button on Details tab to add tests.");
    } else {
        // Display tests in study
        const auto& tests = mCurrentStudy->GetTests();
        for (size_t i = 0; i < tests.size(); i++) {
            ImGui::PushID((int)i);

            ImGui::Text("%zu.", i + 1);
            ImGui::SameLine();
            ImGui::Text("%s", tests[i].testName.c_str());

            // Show parameter variants
            if (!tests[i].parameterVariants.empty()) {
                ImGui::SameLine();
                ImGui::TextDisabled("(%zu variants)", tests[i].parameterVariants.size());
            }

            ImGui::SameLine(ImGui::GetContentRegionAvail().x - 200);

            // Edit parameters button
            if (ImGui::SmallButton("Edit Params")) {
                EditTestParameters(i);
            }

            ImGui::SameLine();

            // Remove button
            if (ImGui::SmallButton("Remove")) {
                // Need to pass testName, not index
                const std::string testName = tests[i].testName;
                RemoveTestFromStudy(testName);
                ImGui::PopID();
                break;
            }

            ImGui::PopID();
        }
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

        // Save selected study to config
        mConfig->SetCurrentStudyPath(fullPath);
        mConfig->SaveConfig();

        // Auto-load Main chain if it exists
        std::string mainChainPath = fullPath + "/chains/Main.json";
        if (fs::exists(mainChainPath)) {
            LoadChain(mainChainPath);
            printf("Auto-loaded Main chain\n");
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

    // Clear variant name and show dialog
    mVariantName[0] = '\0';
    mShowVariantNameDialog = true;
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
        ImGui::TextWrapped("No study loaded. Create a new study or open an existing one to begin.");
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

    // Scrollable list of tests
    ImGui::BeginChild("TestList", ImVec2(0, -40), false);

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

        // Make test name clickable to select in Add Test panel (shows test info)
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.7f, 1.0f, 1.0f));  // Blue color
        if (ImGui::Selectable(displayName.c_str(), false, ImGuiSelectableFlags_None, ImVec2(availableWidth, 0))) {
            // Find this test in the battery browser and select it to show info
            std::string testNameToFind = tests[i].testName;

            // Search for this test in the mExperiments list by name
            // Match by comparing the experiment name with the test name
            bool found = false;
            for (int j = 0; j < (int)mExperiments.size(); j++) {
                // Check if experiment name matches testName
                // Experiment name could be "testname" or "parentdir/testname"
                if (mExperiments[j].name == testNameToFind ||
                    mExperiments[j].name.find("/" + testNameToFind) != std::string::npos ||
                    mExperiments[j].name.find(testNameToFind + "/") != std::string::npos) {
                    mSelectedExperiment = j;
                    LoadExperimentInfo(mExperiments[j].path);
                    mAddTestSubTab = 0;  // Switch to Battery tab
                    found = true;
                    break;
                }
            }

            if (!found) {
                printf("Note: Test '%s' not found in battery browser\n", testNameToFind.c_str());
            }
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

    // Note: Individual tests should be run as part of chains in the Run tab,
    // or previewed from the battery browser. No "Run" button here.
}

void LauncherUI::RenderAddTestPanel()
{
    ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Add Test to Study");
    ImGui::Separator();
    ImGui::Spacing();

    // Three sub-tabs: Battery, File, New
    if (ImGui::BeginTabBar("AddTestTabs")) {
        if (ImGui::BeginTabItem("Battery")) {
            mAddTestSubTab = 0;
            RenderBatteryBrowser();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("File")) {
            mAddTestSubTab = 1;
            RenderFileImport();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("New")) {
            mAddTestSubTab = 2;
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

        // Add to Study button (at top)
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));

        if (ImGui::Button("Add to Study", ImVec2(-1, 40))) {
            AddTestToStudy();
        }

        ImGui::PopStyleColor(3);

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

    // Output display
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Output header with stdout/stderr toggle
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 1.0f, 1.0f), "Output");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("stdout and stderr from the running chain");
    }

    ImGui::SameLine();
    if (ImGui::RadioButton("stdout", !mShowStderr)) {
        mShowStderr = false;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("stderr", mShowStderr)) {
        mShowStderr = true;
    }

    // "Open in Editor" button
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 135);
    if (ImGui::Button("Open in Editor", ImVec2(130, 0))) {
        // Get current output
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
            mCodeEditorFilePath = ""; // Empty path indicates unsaved output
            mShowCodeEditor = true;
        }
    }

    // Scrollable output window - always full size
    ImGui::BeginChild("OutputWindow", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

    if (mRunningExperiment) {
        std::string output;

        // If running a chain, show accumulated output from all items plus current item
        if (mRunningChain) {
            output = mShowStderr ? mChainAccumulatedStderr : mChainAccumulatedStdout;
            // Add current item's output
            const std::string& currentOutput = mShowStderr ? mRunningExperiment->GetStderr() :
                                                             mRunningExperiment->GetStdout();
            output += currentOutput;
        } else {
            // Single test run - just show current output
            output = mShowStderr ? mRunningExperiment->GetStderr() :
                                   mRunningExperiment->GetStdout();
        }

        if (!output.empty()) {
            // Use InputTextMultiline for selectable text
            ImGui::InputTextMultiline("##output",
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
        ImGui::InputTextMultiline("##output",
                                   const_cast<char*>(output.c_str()),
                                   output.size() + 1,
                                   ImVec2(-1, -1),
                                   ImGuiInputTextFlags_ReadOnly);
    } else {
        ImGui::TextDisabled("Run a chain to see output here");
    }

    ImGui::EndChild();
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
        // Build arguments
        std::vector<std::string> args;
        if (strlen(mSubjectCode) > 0) {
            args.push_back("-s");
            args.push_back(mSubjectCode);
        }
        if (strlen(mLanguageCode) > 0) {
            args.push_back("-v");
            args.push_back(mLanguageCode);
        }
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

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Output window
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 1.0f, 1.0f), "Output");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("stdout and stderr from the running script");
    }

    ImGui::SameLine();
    if (ImGui::RadioButton("stdout", !mShowStderr)) {
        mShowStderr = false;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("stderr", mShowStderr)) {
        mShowStderr = true;
    }

    // "Open in Editor" button
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 135);
    if (ImGui::Button("Open in Editor", ImVec2(130, 0))) {
        if (mRunningExperiment) {
            const std::string& output = mShowStderr ?
                mRunningExperiment->GetStderr() :
                mRunningExperiment->GetStdout();

            if (!output.empty()) {
                mCodeEditor.SetText(output);
                mCodeEditorFilePath = ""; // Empty path indicates unsaved output
                mShowCodeEditor = true;
            }
        }
    }

    ImGui::BeginChild("QuickLaunchOutput", ImVec2(0, 0), true);

    if (mRunningExperiment && mRunningExperiment->IsRunning()) {
        const std::string& output = mShowStderr ?
            mRunningExperiment->GetStderr() :
            mRunningExperiment->GetStdout();

        ImGui::InputTextMultiline("##qloutput",
                                   const_cast<char*>(output.c_str()),
                                   output.size() + 1,
                                   ImVec2(-1, -1),
                                   ImGuiInputTextFlags_ReadOnly);
    } else if (mRunningExperiment && !mRunningExperiment->IsRunning()) {
        const std::string& output = mShowStderr ?
            mRunningExperiment->GetStderr() :
            mRunningExperiment->GetStdout();

        ImGui::InputTextMultiline("##qloutput",
                                   const_cast<char*>(output.c_str()),
                                   output.size() + 1,
                                   ImVec2(-1, -1),
                                   ImGuiInputTextFlags_ReadOnly);
    } else {
        ImGui::TextDisabled("Run a script to see output here");
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
        ImGui::TextWrapped("Welcome! This appears to be your first time running PEBL 2.3.");
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

        // Build file paths
        std::string baseName = fs::path(test.testName).filename().string();
        std::string translationsDir = (fs::path(mTranslationEditor.testPath) / "translations").string();
        std::string englishFile = (fs::path(translationsDir) / (baseName + ".pbl-en.json")).string();

        // Header
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Test:");
        ImGui::SameLine();
        ImGui::Text("%s", test.testName.c_str());

        // Scan for available language files
        std::vector<std::string> availableLanguages;
        availableLanguages.push_back("en");  // English is always available as base

        if (fs::exists(translationsDir) && fs::is_directory(translationsDir)) {
            for (const auto& entry : fs::directory_iterator(translationsDir)) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();
                    size_t dashPos = filename.rfind('-');
                    size_t dotPos = filename.rfind(".json");
                    if (dashPos != std::string::npos && dotPos != std::string::npos && dotPos > dashPos) {
                        std::string lang = filename.substr(dashPos + 1, dotPos - dashPos - 1);
                        if (lang != "en") {
                            availableLanguages.push_back(lang);
                        }
                    }
                }
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
                std::string targetFile = (fs::path(translationsDir) / (baseName + ".pbl-" + mTranslationEditor.language + ".json")).string();
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
            ImGui::Text("%s.pbl-en.json", baseName.c_str());
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
