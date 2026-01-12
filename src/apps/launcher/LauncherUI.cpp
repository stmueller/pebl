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
#include "../../utility/BinReloc.h"
#include "imgui.h"
#include <SDL2/SDL_image.h>
#include <cstring>
#include <cctype>
#include <ctime>
#include <algorithm>
#include <random>
#include <filesystem>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <json.hpp>

namespace fs = std::filesystem;

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
{
    // Initialize UI state from config
    std::strncpy(mSubjectCode, config->GetSubjectCode().c_str(), sizeof(mSubjectCode) - 1);
    mSubjectCode[sizeof(mSubjectCode) - 1] = '\0';
    mParticipantCode[0] = '\0';
    mStudyCode[0] = '\0';
    mQuickLaunchPath[0] = '\0';
    mQuickLaunchParamFile[0] = '\0';

    // Set Quick Launch to start in pebl-exp.<version>
    const char* home = getenv("HOME");
    if (home) {
        std::string peblExpPath = std::string(home) + "/Documents/pebl-exp." + PEBL_VERSION;
        struct stat info;
        if (stat(peblExpPath.c_str(), &info) == 0 && (info.st_mode & S_IFDIR)) {
            mQuickLaunchDirectory = peblExpPath;

            // Scan for .pbl files on startup
            DIR* dirp = opendir(peblExpPath.c_str());
            if (dirp) {
                struct dirent* entry;
                while ((entry = readdir(dirp)) != nullptr) {
                    std::string name = entry->d_name;
                    if (name.length() > 4 && name.substr(name.length() - 4) == ".pbl") {
                        mQuickLaunchFiles.push_back(name);
                    }
                }
                closedir(dirp);
                std::sort(mQuickLaunchFiles.begin(), mQuickLaunchFiles.end());
            }
        } else {
            // Fall back to config directory
            mQuickLaunchDirectory = config->GetExperimentDirectory();
        }
    } else {
        // Fall back to config directory
        mQuickLaunchDirectory = config->GetExperimentDirectory();
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

    // Initialize variant naming dialog
    mVariantName[0] = '\0';

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

    if (!batteryPath.empty() && batteryPath.length() < sizeof(mExperimentDir)) {
        std::strncpy(mExperimentDir, batteryPath.c_str(), sizeof(mExperimentDir) - 1);
        mExperimentDir[sizeof(mExperimentDir) - 1] = '\0';
        ScanExperimentDirectory(batteryPath);
        printf("Scanned battery directory: %s - found %zu tests\n",
               batteryPath.c_str(), mExperiments.size());
    }

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

            // Check exit code - only abort chain if it's a consent form with non-zero exit
            int exitCode = mRunningExperiment->GetExitCode();
            printf("Chain item exit code: %d\n", exitCode);

            // Get the current chain item to check its type
            bool shouldAbortChain = false;
            if (exitCode != 0 && mCurrentChain && mCurrentChainItemIndex >= 0 &&
                mCurrentChainItemIndex < (int)mCurrentChain->GetItems().size()) {
                const ChainItem& currentItem = mCurrentChain->GetItems()[mCurrentChainItemIndex];

                // Only abort chain for consent forms with non-zero exit
                // For regular tests, Ctrl-Shift-Alt-\ should just skip to next item
                if (currentItem.type == ItemType::Consent) {
                    shouldAbortChain = true;
                    printf("Consent form declined (exit code %d) - aborting chain\n", exitCode);
                } else {
                    printf("Non-consent item exited with code %d - continuing chain\n", exitCode);
                }
            }

            if (shouldAbortChain) {
                // Non-zero exit code on consent form - user declined consent
                printf("Chain terminated: User declined consent (exit code %d)\n", exitCode);

                // Accumulate output from this final item
                mChainAccumulatedStdout += mRunningExperiment->GetStdout();
                mChainAccumulatedStderr += mRunningExperiment->GetStderr();
                mChainAccumulatedStdout += "\n=== Chain terminated: User declined consent (exit code " +
                                          std::to_string(exitCode) + ") ===\n";

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
    RenderStudyBar();

    // Add significant vertical spacing after StudyBar to prevent overlap with tabs
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    // Main tabbed interface: Tests, Chains, Run
    // Make tab headers larger and more prominent (colors, padding, and larger font)
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(30, 8));   // Larger horizontal and vertical padding
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 8));    // More spacing around tabs
    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 8.0f);             // Rounded corners for pill effect
    ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.25f, 0.45f, 0.70f, 1.0f));           // Darker blue for inactive
    ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.35f, 0.60f, 0.85f, 1.0f));    // Brighter blue on hover
    ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.20f, 0.55f, 0.90f, 1.0f));     // Vivid blue for active

    if (ImGui::BeginTabBar("MainTabs", ImGuiTabBarFlags_None)) {
        // For each tab: set large font before BeginTabItem (for header), reset after (for content)

        ImGui::SetWindowFontScale(1.5f);  // Large font for tab header
        if (ImGui::BeginTabItem("Tests")) {
            // Reset font and styles for tab content
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar(3);

            RenderTestsTab();
            ImGui::EndTabItem();

            // Restore styles for next tab header
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(30, 8));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 8));
            ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 8.0f);
            ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.25f, 0.45f, 0.70f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.35f, 0.60f, 0.85f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.20f, 0.55f, 0.90f, 1.0f));
            ImGui::SetWindowFontScale(1.5f);
        }

        if (ImGui::BeginTabItem("Chains")) {
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar(3);

            RenderChainsTab();
            ImGui::EndTabItem();

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(30, 8));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 8));
            ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 8.0f);
            ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.25f, 0.45f, 0.70f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.35f, 0.60f, 0.85f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.20f, 0.55f, 0.90f, 1.0f));
            ImGui::SetWindowFontScale(1.5f);
        }

        if (ImGui::BeginTabItem("Run")) {
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar(3);

            RenderRunTab();
            ImGui::EndTabItem();

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(30, 8));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 8));
            ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 8.0f);
            ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.25f, 0.45f, 0.70f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.35f, 0.60f, 0.85f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.20f, 0.55f, 0.90f, 1.0f));
            ImGui::SetWindowFontScale(1.5f);
        }

        if (ImGui::BeginTabItem("Quick Launch")) {
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar(3);

            RenderQuickLaunchTab();
            ImGui::EndTabItem();

            // Restore styles for consistency (will be popped in final cleanup)
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(30, 8));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 8));
            ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 8.0f);
            ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.25f, 0.45f, 0.70f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.35f, 0.60f, 0.85f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.20f, 0.55f, 0.90f, 1.0f));
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

            if (ImGui::MenuItem("Study Settings...", nullptr, false, hasStudy)) {
                mShowStudySettingsDialog = true;
            }

            if (ImGui::MenuItem("Open Study Directory...", nullptr, false, hasStudy)) {
                if (mCurrentStudy) {
                    std::string studyPath = mCurrentStudy->GetPath();
                    OpenDirectoryInFileBrowser(studyPath);
                }
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Create Snapshot...", nullptr, false, hasStudy)) {
                // TODO: Show snapshot creation dialog
                if (mCurrentStudy) {
                    printf("TODO: Create snapshot of current study\n");
                }
            }

            if (ImGui::MenuItem("Import Snapshot...")) {
                std::string snapshotPath = OpenDirectoryDialog("Select Snapshot Directory");
                if (!snapshotPath.empty()) {
                    // TODO: Show import dialog with new name
                    printf("TODO: Import snapshot from: %s\n", snapshotPath.c_str());
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

            if (ImGui::MenuItem("Refresh Battery Tests")) {
                std::string batteryPath = mConfig->GetBatteryPath();
                if (!batteryPath.empty()) {
                    ScanExperimentDirectory(batteryPath);
                    printf("Refreshed battery: %zu tests found\n", mExperiments.size());
                }
            }

            ImGui::Separator();

            // Translation Editor - requires selected experiment
            bool hasSelectedExp = (mSelectedExperiment >= 0 && mSelectedExperiment < (int)mExperiments.size());
            if (ImGui::MenuItem("Translation Editor...", nullptr, false, hasSelectedExp)) {
                if (hasSelectedExp) {
                    LaunchTranslationEditor();
                }
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("Edit translations for the selected test (requires test selection)");
            }

            // Data Combiner - works in current directory
            if (ImGui::MenuItem("Data Combiner...")) {
                LaunchDataCombiner();
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Combine data files from multiple participants");
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
                std::string manualPath = mConfig->GetBatteryPath() + "/../doc/pman/PEBLManual2.2.pdf";
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

            ImGui::Separator();

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

        ImGui::Image((void*)(intptr_t)mScreenshotTexture,
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
                        printf("ERROR: Could not open schema file: %s\n", schemaPath.c_str());
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
            mParameterFile = fs::path(exp.directory) / (fs::path(exp.path).stem().string() + ".par.json");

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
    ImGui::Text("Chain Editor");
    ImGui::Separator();
    ImGui::Spacing();

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
    }

    ImGui::Spacing();

    // Chain info
    if (mCurrentChain) {
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "%s", mCurrentChain->GetName().c_str());
        ImGui::Text("Description: %s", mCurrentChain->GetDescription().c_str());
        ImGui::Text("Items in chain: %zu", mCurrentChain->GetItems().size());
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

                ImGui::PushItemWidth(60);
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
                float fontSize = mConfig->GetFontSize();
                ImGui::PushItemWidth(100);
                if (ImGui::SliderFloat("##FontSize", &fontSize, 10.0f, 24.0f, "%.1f")) {
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

                // Data output path
                ImGui::Text("Data Output Path:");
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Default location for test data files");
                }
                char dataPath[512];
                std::strncpy(dataPath, mConfig->GetDataOutputPath().c_str(), sizeof(dataPath) - 1);
                dataPath[sizeof(dataPath) - 1] = '\0';
                ImGui::PushItemWidth(-100);
                if (ImGui::InputText("##DataPath", dataPath, sizeof(dataPath))) {
                    mConfig->SetDataOutputPath(dataPath);
                }
                ImGui::PopItemWidth();
                ImGui::SameLine();
                if (ImGui::Button("Browse##Data")) {
                    std::string path = OpenDirectoryDialog("Select Data Output Directory");
                    if (!path.empty()) {
                        mConfig->SetDataOutputPath(path);
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
    mRunningExperiment = new ExperimentRunner();
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
    // Windows: Use explorer
    std::string command = "explorer \"" + path + "\"";
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

void LauncherUI::LaunchTranslationEditor()
{
    if (mSelectedExperiment < 0 || mSelectedExperiment >= (int)mExperiments.size()) {
        printf("ERROR: No experiment selected for translation editor\n");
        return;
    }

    const ExperimentInfo& exp = mExperiments[mSelectedExperiment];

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

    // Build command to launch translatetest.pbl
    // Format: pebl2 translatetest.pbl -v <scriptname> --language <lang>
    std::string scriptPath = exp.path;
    std::string lang = std::string(mLanguageCode);
    if (lang.empty()) {
        lang = "en";
    }

    std::string command = peblExec + " translatetest.pbl -v \"" + scriptPath + "\" --language " + lang;

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
    const auto& items = mCurrentChain->GetItems();
    mChainExecutionOrder.clear();
    mChainExecutionOrder.reserve(items.size());

    size_t i = 0;
    while (i < items.size()) {
        // Check if this starts a randomization group (test with randomGroup > 0)
        if (items[i].type == ItemType::Test && items[i].randomGroup > 0) {
            int groupId = items[i].randomGroup;
            std::vector<int> groupIndices;

            // Collect all consecutive items with the same randomGroup
            while (i < items.size() &&
                   items[i].type == ItemType::Test &&
                   items[i].randomGroup == groupId) {
                groupIndices.push_back(i);
                i++;
            }

            // Shuffle this group
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(groupIndices.begin(), groupIndices.end(), g);

            // Add shuffled indices to execution order
            for (int idx : groupIndices) {
                mChainExecutionOrder.push_back(idx);
            }

            printf("Randomized group %d (%zu tests)\n", groupId, groupIndices.size());
        } else {
            // Not a randomization group - add as-is
            mChainExecutionOrder.push_back(i);
            i++;
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
    mRunningExperiment = new ExperimentRunner();

    if (item.type == ItemType::Test) {
        // Execute test item
        std::string studyPath = mCurrentStudy->GetPath();
        std::string testPath = studyPath + "/tests/" + item.testName + "/" + item.testName + ".pbl";

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
            mRunningChain = false;
        }

    } else {
        // Execute page item (instruction/consent/completion)
        // Use ChainPage.pbl to display the page
        std::string tmpDir = "/tmp"; // TODO: Use proper temp directory
        std::string configFile = item.CreateChainPageConfig(tmpDir);

        if (configFile.empty()) {
            printf("Failed to create page config\n");
            delete mRunningExperiment;
            mRunningExperiment = nullptr;
            mRunningChain = false;
            return;
        }

        // Run ChainPage.pbl with the config
        std::string chainPagePath = "media/apps/ChainPage/ChainPage.pbl"; // Relative to PEBL install

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
    mRunningExperiment = new ExperimentRunner();

    if (item.type == ItemType::Test) {
        // Execute test item
        std::string studyPath = mCurrentStudy->GetPath();
        std::string testPath = studyPath + "/tests/" + item.testName + "/" + item.testName + ".pbl";

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
        std::string tmpDir = "/tmp";
        std::string configFile = item.CreateChainPageConfig(tmpDir);

        if (configFile.empty()) {
            printf("Failed to create page config\n");
            delete mRunningExperiment;
            mRunningExperiment = nullptr;
            return;
        }

        // Run ChainPage.pbl with the config
        std::string chainPagePath = "media/apps/ChainPage/ChainPage.pbl";

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

std::string LauncherUI::OpenDirectoryDialog(const std::string& title)
{
#ifdef _WIN32
    // Windows: Use folder browser dialog
    // This requires additional Windows API code
    printf("Directory dialog not yet implemented for Windows\n");
    return "";
#elif __APPLE__
    // macOS: Use osascript
    std::string command = "osascript -e 'POSIX path of (choose folder with prompt \"" + title + "\")'";
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
    std::string command = "zenity --file-selection --directory --title=\"" + title + "\" 2>/dev/null";
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
    printf("File dialog not yet implemented for Windows\n");
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
    printf("Save dialog not yet implemented for Windows\n");
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
    ImGui::OpenPopup("Test Editor");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);

    if (ImGui::BeginPopupModal("Test Editor", &mTestEditor.show, 0))
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

        ImGui::Text("Select Test:");
        ImGui::Spacing();

        // Test selection list
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

            // Language selection (optional)
            ImGui::Text("Language (optional):");
            ImGui::SameLine();
            ImGui::PushItemWidth(100);
            ImGui::InputText("##Language", mTestEditor.language, sizeof(mTestEditor.language));
            ImGui::PopItemWidth();
            ImGui::SameLine();
            ImGui::TextDisabled("(e.g., en, es, de)");

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
    std::string testDestDir = studyPath + "/tests/" + exp.name;

    try {
        // Create test directory
        fs::create_directories(testDestDir);

        // Copy the main .pbl file
        fs::path sourcePath(exp.path);
        fs::path destFile = fs::path(testDestDir) / sourcePath.filename();
        fs::copy_file(exp.path, destFile, fs::copy_options::overwrite_existing);
        printf("Copied %s to %s\n", exp.path.c_str(), destFile.string().c_str());

        // Copy params directory if it exists
        std::string paramsSource = exp.directory + "/params";
        if (fs::exists(paramsSource) && fs::is_directory(paramsSource)) {
            std::string paramsDest = testDestDir + "/params";
            fs::create_directories(paramsDest);
            fs::copy(paramsSource, paramsDest, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
            printf("Copied params directory\n");
        }

        // Copy translations directory if it exists
        std::string translationsSource = exp.directory + "/translations";
        if (fs::exists(translationsSource) && fs::is_directory(translationsSource)) {
            std::string translationsDest = testDestDir + "/translations";
            fs::create_directories(translationsDest);
            fs::copy(translationsSource, translationsDest, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
            printf("Copied translations directory\n");
        }

        // Add test entry to study
        Test test;
        test.testName = exp.name;
        test.testPath = exp.name;  // Relative path within study/tests/
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
            auto mainChain = Chain::LoadFromFile(mainChainPath);
            if (mainChain) {
                ChainItem item(ItemType::Test);
                item.testName = test.testName;
                item.paramVariant = "default";
                item.language = "en";
                item.randomGroup = 0;
                mainChain->AddItem(item);
                mainChain->Save();
                printf("Added test to Main chain\n");
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

        // Copy the main .pbl file
        fs::path destFile = fs::path(testDestDir) / path.filename();
        fs::copy_file(filePath, destFile, fs::copy_options::overwrite_existing);
        printf("Copied %s to %s\n", filePath.c_str(), destFile.string().c_str());

        // Copy params directory if it exists
        std::string paramsSource = sourceDir + "/params";
        if (fs::exists(paramsSource) && fs::is_directory(paramsSource)) {
            std::string paramsDest = testDestDir + "/params";
            fs::create_directories(paramsDest);
            fs::copy(paramsSource, paramsDest, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
            printf("Copied params directory\n");
        }

        // Copy translations directory if it exists
        std::string translationsSource = sourceDir + "/translations";
        if (fs::exists(translationsSource) && fs::is_directory(translationsSource)) {
            std::string translationsDest = testDestDir + "/translations";
            fs::create_directories(translationsDest);
            fs::copy(translationsSource, translationsDest, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
            printf("Copied translations directory\n");
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
            auto mainChain = Chain::LoadFromFile(mainChainPath);
            if (mainChain) {
                ChainItem item(ItemType::Test);
                item.testName = testName;
                item.paramVariant = "default";
                item.language = "en";
                item.randomGroup = 0;
                mainChain->AddItem(item);
                mainChain->Save();
                printf("Added test to Main chain\n");
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

        // Generate basic .pbl file based on template type
        std::string pblFile = testDir + "/" + testName + ".pbl";
        std::ofstream out(pblFile);

        if (!out.is_open()) {
            printf("Error: Could not create .pbl file\n");
            return;
        }

        // Write template code
        out << "## " << testName << " - PEBL Test\n";
        out << "## Generated from template\n\n";
        out << "define Start(p) {\n";
        out << "    ## Initialize\n";
        out << "    gWin <- MakeWindow(\"grey40\")\n";
        out << "    gSleepEasy <- 1\n\n";

        switch (templateType) {
            case 0:  // Simple Reaction Time
                out << "    ## Simple RT template\n";
                out << "    inst <- \"Press any key when you see the stimulus.\"\n";
                out << "    ShowText(inst, gWin)\n";
                out << "    WaitForAnyKeyPress()\n";
                out << "    ClearScreen(gWin)\n\n";
                out << "    ## Trial loop\n";
                out << "    loop(trial, Sequence(1, 10, 1)) {\n";
                out << "        Wait(RandomUniform(500, 1500))\n";
                out << "        stim <- EasyLabel(\"X\", gWin.centerX, gWin.centerY, gWin, 44)\n";
                out << "        Draw()\n";
                out << "        t0 <- GetTime()\n";
                out << "        WaitForAnyKeyPress()\n";
                out << "        rt <- GetTime() - t0\n";
                out << "        Hide(stim)\n";
                out << "        Draw()\n";
                out << "        FilePrint_(gFileOut, trial + \",\" + rt)\n";
                out << "    }\n";
                break;

            case 1:  // Choice Reaction Time
                out << "    ## Choice RT template\n";
                out << "    inst <- \"Press LEFT for 'X', RIGHT for 'O'.\"\n";
                out << "    ShowText(inst, gWin)\n";
                out << "    WaitForAnyKeyPress()\n";
                out << "    ClearScreen(gWin)\n\n";
                out << "    stimuli <- [\"X\", \"O\"]\n";
                out << "    loop(trial, Sequence(1, 10, 1)) {\n";
                out << "        stim <- Sample(stimuli)\n";
                out << "        label <- EasyLabel(stim, gWin.centerX, gWin.centerY, gWin, 44)\n";
                out << "        Draw()\n";
                out << "        t0 <- GetTime()\n";
                out << "        resp <- WaitForListKeyPress([\"<left>\", \"<right>\"])\n";
                out << "        rt <- GetTime() - t0\n";
                out << "        Hide(label)\n";
                out << "        Draw()\n";
                out << "        FilePrint_(gFileOut, trial + \",\" + stim + \",\" + resp + \",\" + rt)\n";
                out << "    }\n";
                break;

            case 2:  // Survey/Questionnaire
                out << "    ## Survey template\n";
                out << "    questions <- [\"Question 1?\", \"Question 2?\", \"Question 3?\"]\n";
                out << "    scale <- [\"1 - Strongly Disagree\", \"2\", \"3\", \"4\", \"5 - Strongly Agree\"]\n\n";
                out << "    loop(q, Sequence(1, Length(questions), 1)) {\n";
                out << "        resp <- GetEasyChoice(Nth(questions, q), scale, [1, 2, 3, 4, 5], gWin)\n";
                out << "        FilePrint_(gFileOut, q + \",\" + resp)\n";
                out << "    }\n";
                break;

            case 3:  // Memory Test
                out << "    ## Memory test template\n";
                out << "    stimuli <- [\"CAT\", \"DOG\", \"BIRD\", \"FISH\", \"HORSE\"]\n";
                out << "    ## Study phase\n";
                out << "    loop(stim, stimuli) {\n";
                out << "        ShowText(stim, gWin)\n";
                out << "        Wait(2000)\n";
                out << "        ClearScreen(gWin)\n";
                out << "        Wait(500)\n";
                out << "    }\n\n";
                out << "    ## Test phase\n";
                out << "    testItems <- Shuffle(Merge(stimuli, [\"TREE\", \"BOOK\"]))\n";
                out << "    loop(item, testItems) {\n";
                out << "        resp <- GetEasyChoice(item, [\"Old\", \"New\"], [1, 2], gWin)\n";
                out << "        FilePrint_(gFileOut, item + \",\" + resp)\n";
                out << "    }\n";
                break;

            case 4:  // Visual Search
                out << "    ## Visual search template\n";
                out << "    numDistractors <- 10\n";
                out << "    loop(trial, Sequence(1, 10, 1)) {\n";
                out << "        ## Create display\n";
                out << "        ## (Add your visual search implementation here)\n";
                out << "        Wait(500)\n";
                out << "    }\n";
                break;

            default:  // Blank Template
                out << "    ## Your code here\n\n";
                break;
        }

        out << "\n    ## Cleanup\n";
        out << "    ShowText(\"Experiment complete. Thank you!\", gWin)\n";
        out << "    WaitForAnyKeyPress()\n";
        out << "}\n";

        out.close();
        printf("Created test template: %s\n", pblFile.c_str());

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
    if (ImGui::Button("Load Study...")) {
        // Set initial directory to workspace studies folder
        std::string studiesPath = mWorkspace->GetStudiesPath();

        // Use zenity with --filename to start in studies directory
        #ifdef __linux__
        std::string command = "zenity --file-selection --directory --title=\"Select Study Directory\" --filename=\"" + studiesPath + "/\" 2>/dev/null";
        FILE* pipe = popen(command.c_str(), "r");
        std::string studyPath;
        if (pipe) {
            char buffer[1024];
            if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                studyPath = buffer;
                // Remove trailing newline
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
                           mCurrentStudy->GetDescription().c_str(),
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
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s", tests[i].testName.c_str());

        // Show parameter variants
        if (!tests[i].parameterVariants.empty()) {
            ImGui::SameLine();
            ImGui::TextDisabled("(%zu variants)", tests[i].parameterVariants.size());
        }

        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 235);

        if (ImGui::SmallButton("Edit")) {
            EditTestParameters(i);
        }

        ImGui::SameLine();

        if (ImGui::SmallButton("Data")) {
            // Launch data combiner in test's data directory
            std::string studyPath = mCurrentStudy->GetPath();
            std::string testPath = studyPath + "/tests/" + tests[i].testPath;
            std::string dataPath = testPath + "/data";

            // Create data directory if it doesn't exist
            if (!fs::exists(dataPath)) {
                fs::create_directories(dataPath);
            }

            LaunchDataCombiner(dataPath);
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Combine data files for this test");
        }

        ImGui::SameLine();

        if (ImGui::SmallButton("Remove")) {
            const std::string testName = tests[i].testName;
            RemoveTestFromStudy(testName);
            ImGui::PopID();
            break;
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

            ImGui::Image((void*)(intptr_t)mScreenshotTexture,
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

    static int selectedTemplate = 0;
    const char* templates[] = {
        "Simple Reaction Time",
        "Choice Reaction Time",
        "Survey/Questionnaire",
        "Memory Test",
        "Visual Search",
        "Blank Template"
    };

    ImGui::Text("Template:");
    ImGui::Combo("##Template", &selectedTemplate, templates, 6);

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

    ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Run Study");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextWrapped("Configure and run your study or individual chains.");
    ImGui::Spacing();
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

    // Check if subject code already exists and show warning
    if (mCurrentChain && strlen(mSubjectCode) > 0) {
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
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.0f, 1.0f), "⚠ Code already used!");
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("This subject code has already been used in this study.\nData may be overwritten!");
            }
        }

        // Show existing codes if any
        if (!existingCodes.empty()) {
            ImGui::Indent(20);
            ImGui::TextDisabled("Existing codes:");
            ImGui::SameLine();
            std::string codesList;
            for (size_t i = 0; i < existingCodes.size() && i < 10; i++) {
                if (i > 0) codesList += ", ";
                codesList += existingCodes[i];
            }
            if (existingCodes.size() > 10) {
                codesList += "... (" + std::to_string(existingCodes.size()) + " total)";
            }
            ImGui::TextDisabled("%s", codesList.c_str());
            ImGui::Unindent(20);
        }
    }

    ImGui::Spacing();

    // Language - allow custom language codes for inline translations
    ImGui::Text("Language:");
    ImGui::SameLine();
    ImGui::PushItemWidth(100);
    ImGui::InputText("##Language", mLanguageCode, sizeof(mLanguageCode));
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::TextDisabled("(e.g., en, es, de, fr - or custom for inline translations)");

    ImGui::Spacing();

    // Fullscreen
    ImGui::Checkbox("Fullscreen Mode", &mFullscreen);

    ImGui::Spacing();

    // Screen Resolution
    ImGui::Text("Screen Resolution:");
    ImGui::SameLine();
    ImGui::PushItemWidth(200);
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

    ImGui::Spacing();

    // VSync
    ImGui::Checkbox("Enable VSync", &mVSync);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Synchronize with monitor refresh rate");
    }

    ImGui::Spacing();

    // Graphics Driver (advanced, collapsible)
    if (ImGui::TreeNode("Advanced Options")) {
        ImGui::Text("Graphics Driver:");
        ImGui::SameLine();
        ImGui::PushItemWidth(200);
        ImGui::InputText("##Driver", mGraphicsDriver, sizeof(mGraphicsDriver));
        ImGui::PopItemWidth();
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Leave blank for auto-detect (e.g., opengl, directfb)");
        }

        ImGui::Spacing();

        ImGui::Text("Custom Arguments:");
        ImGui::PushItemWidth(-1);
        ImGui::InputText("##CustomArgs", mCustomArguments, sizeof(mCustomArguments));
        ImGui::PopItemWidth();
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Additional command-line arguments");
        }

        ImGui::TreePop();
    }

    ImGui::Spacing();
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

    ImGui::TextWrapped("Browse and run .pbl scripts for testing, demos, and quick experiments.");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Current directory display (read-only)
    ImGui::Text("Directory:");
    ImGui::SameLine();
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

    ImGui::Spacing();

    // File list (left) and configuration (right)
    float leftWidth = ImGui::GetContentRegionAvail().x * 0.5f;

    // Left: File browser
    ImGui::BeginChild("QuickLaunchFiles", ImVec2(leftWidth, 300), true);
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 1.0f, 1.0f), "PEBL Scripts");
    ImGui::Separator();

    // Scan current directory for directories and .pbl files
    std::vector<std::string> directories;
    std::vector<std::string> pblFiles;

    DIR* dirp = opendir(mQuickLaunchDirectory.c_str());
    if (dirp) {
        struct dirent* entry;
        while ((entry = readdir(dirp)) != nullptr) {
            std::string name = entry->d_name;

            // Skip current directory
            if (name == ".") {
                continue;
            }

            std::string fullPath = mQuickLaunchDirectory + "/" + name;
            struct stat info;
            if (stat(fullPath.c_str(), &info) == 0) {
                if (S_ISDIR(info.st_mode)) {
                    // Don't skip ".." - we want it for navigation
                    directories.push_back(name);
                } else if (name.length() > 4 && name.substr(name.length() - 4) == ".pbl") {
                    pblFiles.push_back(name);
                }
            }
        }
        closedir(dirp);

        std::sort(directories.begin(), directories.end());
        std::sort(pblFiles.begin(), pblFiles.end());
    }

    // Display directories first with folder icon
    for (const auto& dir : directories) {
        std::string displayName = (dir == "..") ? "[UP] .." : "[DIR] " + dir;
        if (ImGui::Selectable(displayName.c_str(), false)) {
            // Navigate into directory
            if (dir == "..") {
                // Go up one level
                size_t lastSlash = mQuickLaunchDirectory.find_last_of('/');
                if (lastSlash != std::string::npos && lastSlash > 0) {
                    mQuickLaunchDirectory = mQuickLaunchDirectory.substr(0, lastSlash);
                }
            } else {
                // Navigate into subdirectory
                mQuickLaunchDirectory = mQuickLaunchDirectory + "/" + dir;
            }
            mQuickLaunchSelectedFile = -1;
            mQuickLaunchPath[0] = '\0';
        }
    }

    // Display .pbl files
    int fileIndex = 0;
    for (const auto& file : pblFiles) {
        bool is_selected = (mQuickLaunchSelectedFile == fileIndex);
        if (ImGui::Selectable(file.c_str(), is_selected)) {
            mQuickLaunchSelectedFile = fileIndex;
            std::strncpy(mQuickLaunchPath,
                       (mQuickLaunchDirectory + "/" + file).c_str(),
                       sizeof(mQuickLaunchPath) - 1);
            mQuickLaunchPath[sizeof(mQuickLaunchPath) - 1] = '\0';
        }
        fileIndex++;
    }

    if (directories.empty() && pblFiles.empty()) {
        ImGui::TextDisabled("Empty directory");
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // Right: Configuration
    ImGui::BeginChild("QuickLaunchConfig", ImVec2(0, 300), true);
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
        mRunningExperiment = new ExperimentRunner();
        bool success = mRunningExperiment->RunExperiment(mQuickLaunchPath, args,
                                                          mSubjectCode, mLanguageCode,
                                                          mFullscreen);
        if (!success) {
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
        static bool initialized = false;

        if (!initialized) {
            std::strncpy(nameBuffer, mCurrentStudy->GetName().c_str(), sizeof(nameBuffer) - 1);
            std::strncpy(descBuffer, mCurrentStudy->GetDescription().c_str(), sizeof(descBuffer) - 1);
            std::strncpy(authorBuffer, mCurrentStudy->GetAuthor().c_str(), sizeof(authorBuffer) - 1);
            initialized = true;
        }

        ImGui::Text("Name:");
        ImGui::PushItemWidth(-1);
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

        if (ImGui::Button("Save", ImVec2(120, 0))) {
            mCurrentStudy->SetName(nameBuffer);
            mCurrentStudy->SetDescription(descBuffer);
            mCurrentStudy->SetAuthor(authorBuffer);
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
        ImGui::BulletText("battery/ - 100+ psychological test tasks");
        ImGui::BulletText("doc/ - Documentation");
        ImGui::BulletText("demo/ - Example experiments");
        ImGui::BulletText("tutorial/ - Tutorial materials");

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
                        installPath = std::string(prefix) + "/pebl2";
                        free(prefix);
                        printf("BinReloc found installation at: %s\n", installPath.c_str());
                    }
                }
                #endif

                // Fallback 1: Try to find pebl2 executable via /proc/self/exe
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

std::vector<std::string> LauncherUI::CheckExistingSubjectCodes()
{
    std::vector<std::string> existingCodes;

    if (!mCurrentStudy || !mCurrentChain) {
        printf("DEBUG CheckExistingSubjectCodes: No study or chain loaded\n");
        return existingCodes;
    }

    std::string studyPath = mCurrentStudy->GetPath();
    const auto& chainItems = mCurrentChain->GetItems();
    printf("DEBUG CheckExistingSubjectCodes: Study path: %s\n", studyPath.c_str());
    printf("DEBUG CheckExistingSubjectCodes: Chain has %zu items\n", chainItems.size());

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
                printf("DEBUG CheckExistingSubjectCodes: Found test in chain: %s\n", item.testName.c_str());
            }
        }
    }

    printf("DEBUG CheckExistingSubjectCodes: Scanning %zu unique tests\n", testsInChain.size());

    // For each test, scan its data directory for subdirectories (subject codes)
    for (const auto& testName : testsInChain) {
        std::string dataDir = studyPath + "/tests/" + testName + "/data";
        printf("DEBUG CheckExistingSubjectCodes: Scanning data directory: %s\n", dataDir.c_str());
        
        DIR* dir = opendir(dataDir.c_str());
        if (!dir) {
            printf("DEBUG CheckExistingSubjectCodes:   -> Directory does not exist or cannot be opened\n");
            continue; // data directory doesn't exist yet
        }

        printf("DEBUG CheckExistingSubjectCodes:   -> Directory opened successfully\n");
        int subdirCount = 0;
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string name = entry->d_name;

            // Skip . and ..
            if (name == "." || name == "..") {
                continue;
            }

            // Check if it's a directory
            std::string fullPath = dataDir + "/" + name;
            struct stat st;
            if (stat(fullPath.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
                subdirCount++;
                printf("DEBUG CheckExistingSubjectCodes:   -> Found subject code directory: %s\n", name.c_str());
                // This is a subject code directory
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
                } else {
                    printf("DEBUG CheckExistingSubjectCodes:      (already in list)\n");
                }
            }
        }
        closedir(dir);
        printf("DEBUG CheckExistingSubjectCodes:   -> Found %d subject code directories in this test\n", subdirCount);
    }

    printf("DEBUG CheckExistingSubjectCodes: TOTAL unique subject codes found: %zu\n", existingCodes.size());
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
