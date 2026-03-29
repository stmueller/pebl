// ScaleManager.cpp - PEBL Scale file management implementation
// Copyright (c) 2026 Shane T. Mueller
// Licensed under GPL

#include "ScaleManager.h"
#include "Study.h"
#include "Chain.h"
#include <json.hpp>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <set>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace fs = std::filesystem;
using json = nlohmann::json;

ScaleManager::ScaleManager(const std::string& batteryPath, const std::string& workspacePath)
    : mBatteryPath(batteryPath)
    , mWorkspacePath(workspacePath)
{
    // Library paths: media/apps/scales/ (relative to PEBL root, which is parent of battery/)
    mBatteryScalesPath = batteryPath + "/../media/apps/scales";
    mBatteryDefinitionsPath = mBatteryScalesPath + "/definitions";

    // Workspace paths (writable, user scales)
    if (!workspacePath.empty()) {
        mWorkspaceScalesPath = workspacePath + "/scales";
        mWorkspaceDefinitionsPath = mWorkspaceScalesPath + "/definitions";
    }

    EnsureDirectoriesExist();
}

ScaleManager::~ScaleManager()
{
}

void ScaleManager::EnsureDirectoriesExist()
{
    // Library directories are read-only, don't try to create them

    // Workspace directories (should be writable)
    if (!mWorkspacePath.empty()) {
        try {
            fs::create_directories(mWorkspaceScalesPath);
        } catch (const std::exception& e) {
            // Continue even if directory creation fails
        }
    }
}

std::vector<std::string> ScaleManager::GetAvailableScales()
{
    std::set<std::string> scalesSet;  // Use set to avoid duplicates

    // Scan a directory for per-scale subdirectories containing <code>/<code>.json or <code>.osd
    auto scanForScales = [&](const std::string& basePath) {
        try {
            if (!fs::exists(basePath)) return;
            for (const auto& entry : fs::directory_iterator(basePath)) {
                if (entry.is_directory()) {
                    std::string dirName = entry.path().filename().string();
                    std::string defFile = entry.path().string() + "/" + dirName + ".json";
                    std::string osdFile = entry.path().string() + "/" + dirName + ".osd";
                    if (fs::exists(defFile) || fs::exists(osdFile)) {
                        scalesSet.insert(dirName);
                    }
                }
            }
        } catch (const std::exception& e) {
            // Continue on error
        }
    };

    // Scan library: media/apps/scales/definitions/<code>/<code>.json
    scanForScales(mBatteryDefinitionsPath);

    // Scan workspace: workspace/scales/<code>/<code>.json
    if (!mWorkspacePath.empty()) {
        scanForScales(mWorkspaceScalesPath);
    }

    // Convert set to sorted vector
    std::vector<std::string> scales(scalesSet.begin(), scalesSet.end());
    std::sort(scales.begin(), scales.end());

    return scales;
}

std::shared_ptr<ScaleDefinition> ScaleManager::CreateScale(const std::string& code)
{
    return ScaleDefinition::CreateNew(code);
}

std::shared_ptr<ScaleDefinition> ScaleManager::LoadScale(const std::string& code)
{
    auto scale = std::make_shared<ScaleDefinition>();

    // Try workspace first: workspace/scales/<code>/<code>.json
    if (!mWorkspaceScalesPath.empty()) {
        if (scale->LoadFromScalesDir(mWorkspaceScalesPath, code)) {
            return scale;
        }
    }

    // Fall back to library: media/apps/scales/definitions/<code>/<code>.json
    if (scale->LoadFromScalesDir(mBatteryDefinitionsPath, code)) {
        return scale;
    }

    return nullptr;
}

bool ScaleManager::SaveScale(std::shared_ptr<ScaleDefinition> scale)
{
    if (!scale) {
        return false;
    }

    // Save to per-scale directory in workspace: workspace/scales/<code>/
    // Primary format is .osd (single-file bundle); no separate .json or translation files needed.
    std::string scaleDir = GetScalesPath() + "/" + scale->GetScaleInfo().code;
    try {
        fs::create_directories(scaleDir);
    } catch (const std::exception& e) {
        printf("Error creating scale directory: %s\n", e.what());
        return false;
    }

    return scale->ExportToOSD(scaleDir);
}

bool ScaleManager::DeleteScale(const std::string& code)
{
    try {
        // Delete definition file
        std::string defPath = GetDefinitionPath(code);
        if (fs::exists(defPath)) {
            fs::remove(defPath);
        }

        // Delete all translation files
        auto transFiles = GetTranslationFiles(code);
        for (const auto& transFile : transFiles) {
            if (fs::exists(transFile)) {
                fs::remove(transFile);
            }
        }

        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool ScaleManager::ExportToBattery(std::shared_ptr<ScaleDefinition> scale)
{
    return SaveScale(scale);
}

std::shared_ptr<ScaleDefinition> ScaleManager::ImportFromFile(const std::string& filePath)
{
    return ScaleDefinition::LoadFromFile(filePath);
}

std::vector<ScaleManager::LooseOSDEntry> ScaleManager::GetLooseOSDEntries() const
{
    std::vector<LooseOSDEntry> entries;
    if (mWorkspaceScalesPath.empty()) return entries;

    try {
        if (!fs::exists(mWorkspaceScalesPath)) return entries;
        for (const auto& entry : fs::directory_iterator(mWorkspaceScalesPath)) {
            if (!entry.is_regular_file()) continue;
            std::string filename = entry.path().filename().string();
            if (filename.size() < 5 || filename.substr(filename.size() - 4) != ".osd") continue;

            // Only treat as loose if there is no matching <code>/ subdirectory already
            std::string stemCode = filename.substr(0, filename.size() - 4);
            std::string subdirPath = mWorkspaceScalesPath + "/" + stemCode;
            if (fs::exists(subdirPath) && fs::is_directory(subdirPath)) continue;

            // Read code/name from the .osd bundle
            try {
                std::ifstream f(entry.path().string());
                if (!f.is_open()) continue;
                json bundle = json::parse(f);
                if (!bundle.contains("definition")) continue;

                LooseOSDEntry e;
                e.path = entry.path().string();
                if (bundle["definition"].contains("scale_info")) {
                    auto& si = bundle["definition"]["scale_info"];
                    if (si.contains("code")) e.code = si["code"].get<std::string>();
                    if (si.contains("name")) e.name = si["name"].get<std::string>();
                }
                if (e.code.empty()) e.code = stemCode;
                if (e.name.empty()) e.name = e.code;
                entries.push_back(e);
            } catch (...) {}
        }
    } catch (...) {}

    return entries;
}

std::shared_ptr<ScaleDefinition> ScaleManager::InstallLooseOSD(const std::string& osdPath)
{
    // Load the .osd to determine the scale code
    auto scale = ScaleDefinition::LoadFromOSDFile(osdPath);
    if (!scale) {
        printf("InstallLooseOSD: failed to load OSD from: %s\n", osdPath.c_str());
        return nullptr;
    }
    std::string code = scale->GetScaleInfo().code;
    if (code.empty()) {
        printf("InstallLooseOSD: scale has no code in: %s\n", osdPath.c_str());
        return nullptr;
    }

    // Create workspace/scales/<code>/ directory
    std::string targetDir = mWorkspaceScalesPath + "/" + code;
    std::string targetFile = targetDir + "/" + code + ".osd";
    try {
        fs::create_directories(targetDir);
        fs::copy_file(osdPath, targetFile, fs::copy_options::overwrite_existing);
        fs::remove(osdPath);  // Remove the loose file now that it is installed
        printf("Installed OSD '%s' to: %s\n", code.c_str(), targetFile.c_str());
    } catch (const std::exception& e) {
        printf("InstallLooseOSD error: %s\n", e.what());
        return nullptr;
    }

    return scale;
}

std::string ScaleManager::GetDefinitionPath(const std::string& code) const
{
    // Check workspace: workspace/scales/<code>/<code>.json
    if (!mWorkspaceScalesPath.empty()) {
        std::string path = mWorkspaceScalesPath + "/" + code + "/" + code + ".json";
        if (fs::exists(path)) return path;
    }
    // Check library: media/apps/scales/definitions/<code>/<code>.json
    {
        std::string path = mBatteryDefinitionsPath + "/" + code + "/" + code + ".json";
        if (fs::exists(path)) return path;
    }
    // Return expected library path even if not found
    return mBatteryDefinitionsPath + "/" + code + "/" + code + ".json";
}

std::string ScaleManager::GetOSDPath(const std::string& code) const
{
    // Check workspace: workspace/scales/<code>/<code>.osd
    if (!mWorkspaceScalesPath.empty()) {
        std::string path = mWorkspaceScalesPath + "/" + code + "/" + code + ".osd";
        if (fs::exists(path)) return path;
    }
    // Check library definitions: media/apps/scales/definitions/<code>/<code>.osd
    {
        std::string path = mBatteryDefinitionsPath + "/" + code + "/" + code + ".osd";
        if (fs::exists(path)) return path;
    }
    return "";
}

std::string ScaleManager::GetTranslationPath(const std::string& code, const std::string& lang) const
{
    // Check workspace: new format first, then old
    if (!mWorkspaceScalesPath.empty()) {
        std::string newPath = mWorkspaceScalesPath + "/" + code + "/" + code + "." + lang + ".json";
        if (fs::exists(newPath)) return newPath;
        std::string oldPath = mWorkspaceScalesPath + "/" + code + "/" + code + ".pbl-" + lang + ".json";
        if (fs::exists(oldPath)) return oldPath;
    }
    // Check library: new format first, then old
    {
        std::string newPath = mBatteryDefinitionsPath + "/" + code + "/" + code + "." + lang + ".json";
        if (fs::exists(newPath)) return newPath;
        std::string oldPath = mBatteryDefinitionsPath + "/" + code + "/" + code + ".pbl-" + lang + ".json";
        if (fs::exists(oldPath)) return oldPath;
    }
    // Return expected path in new format
    return mBatteryDefinitionsPath + "/" + code + "/" + code + "." + lang + ".json";
}

bool ScaleManager::ScaleExists(const std::string& code) const
{
    return fs::exists(GetDefinitionPath(code));
}

std::vector<std::string> ScaleManager::GetTranslationFiles(const std::string& code) const
{
    std::set<std::string> langsSeen;  // Track languages to avoid duplicates
    std::vector<std::string> files;

    std::string newPrefix = code + ".";
    std::string oldPrefix = code + ".pbl-";

    auto scanDir = [&](const std::string& dirPath) {
        try {
            if (!fs::exists(dirPath)) return;
            for (const auto& entry : fs::directory_iterator(dirPath)) {
                if (!entry.is_regular_file()) continue;
                std::string filename = entry.path().filename().string();
                if (filename.size() < 5 || filename.substr(filename.size() - 5) != ".json") continue;

                std::string lang;

                // Try new format: CODE.lang.json
                if (filename.find(newPrefix) == 0) {
                    lang = filename.substr(newPrefix.length(), filename.size() - newPrefix.length() - 5);
                    // Guard: lang must be non-empty and must not contain "pbl-" (that's old format)
                    if (lang.empty() || lang.find("pbl-") == 0) {
                        // Try old format instead
                        if (filename.find(oldPrefix) == 0) {
                            lang = filename.substr(oldPrefix.length(), filename.size() - oldPrefix.length() - 5);
                        } else {
                            continue;
                        }
                    }
                }
                // Try old format: CODE.pbl-lang.json
                else if (filename.find(oldPrefix) == 0) {
                    lang = filename.substr(oldPrefix.length(), filename.size() - oldPrefix.length() - 5);
                } else {
                    continue;
                }

                if (!lang.empty() && langsSeen.find(lang) == langsSeen.end()) {
                    langsSeen.insert(lang);
                    files.push_back(entry.path().string());
                }
            }
        } catch (const std::exception& e) {
            // Continue on error
        }
    };

    // Scan workspace first (higher priority), then library
    if (!mWorkspaceScalesPath.empty()) {
        scanDir(mWorkspaceScalesPath + "/" + code);
    }
    scanDir(mBatteryDefinitionsPath + "/" + code);

    return files;
}

ScaleManager::ScaleMetadata ScaleManager::GetScaleMetadata(const std::string& code) const
{
    ScaleMetadata meta;
    meta.code = code;
    meta.questionCount = 0;

    try {
        std::string defPath = GetDefinitionPath(code);
        if (fs::exists(defPath)) {
            // Load from .json definition file
            std::ifstream file(defPath);
            if (!file.is_open()) return meta;

            json j;
            file >> j;

            if (j.contains("scale_info")) {
                auto& info = j["scale_info"];
                if (info.contains("name")) meta.name = info["name"];
                if (info.contains("description")) meta.description = info["description"];
                if (info.contains("author")) meta.author = info["author"];
            }
            if (j.contains("questions") && j["questions"].is_array()) {
                meta.questionCount = j["questions"].size();
            } else if (j.contains("items") && j["items"].is_array()) {
                meta.questionCount = j["items"].size();
            }

            // Get available languages from separate files
            auto transFiles = GetTranslationFiles(code);
            for (const auto& transFile : transFiles) {
                fs::path p(transFile);
                std::string filename = p.filename().string();
                std::string prefix = code + ".pbl-";
                if (filename.find(prefix) == 0 && filename.size() > prefix.length() + 5) {
                    std::string lang = filename.substr(prefix.length());
                    lang = lang.substr(0, lang.length() - 5);
                    meta.availableLanguages.push_back(lang);
                }
            }
        } else {
            // Fall back to .osd bundle
            std::string osdPath = GetOSDPath(code);
            if (osdPath.empty()) return meta;

            std::ifstream file(osdPath);
            if (!file.is_open()) return meta;

            json bundle;
            file >> bundle;

            if (!bundle.contains("definition")) return meta;
            auto& j = bundle["definition"];

            if (j.contains("scale_info")) {
                auto& info = j["scale_info"];
                if (info.contains("name")) meta.name = info["name"];
                if (info.contains("description")) meta.description = info["description"];
                if (info.contains("author")) meta.author = info["author"];
            }
            if (j.contains("items") && j["items"].is_array()) {
                meta.questionCount = j["items"].size();
            } else if (j.contains("questions") && j["questions"].is_array()) {
                meta.questionCount = j["questions"].size();
            }

            // Languages come from the translations block inside the bundle
            if (bundle.contains("translations") && bundle["translations"].is_object()) {
                for (auto& [lang, _] : bundle["translations"].items()) {
                    meta.availableLanguages.push_back(lang);
                }
            }
        }

        std::sort(meta.availableLanguages.begin(), meta.availableLanguages.end());

    } catch (const std::exception& e) {
        // Return partial metadata on error
    }

    return meta;
}

bool ScaleManager::CreateStudyFromScale(std::shared_ptr<ScaleDefinition> scale,
                                         const std::string& workspaceStudiesPath,
                                         const std::string& studyName)
{
    if (!scale) {
        printf("Error: Scale is null\n");
        return false;
    }

    std::string scaleCode = scale->GetScaleInfo().code;

    // Use provided study name, or default to scale code
    std::string actualStudyName = studyName.empty() ? scaleCode : studyName;
    std::string studyPath = workspaceStudiesPath + "/" + actualStudyName;

    printf("Creating scale study at: %s\n", studyPath.c_str());

    try {
        // Check if study already exists - if so, remove it (overwrite mode)
        if (fs::exists(studyPath)) {
            printf("Removing existing study directory: %s\n", studyPath.c_str());
            fs::remove_all(studyPath);
        }

        // Create study using Study::CreateNew (use study name, not scale code)
        auto study = Study::CreateNew(studyPath, actualStudyName);
        if (!study) {
            printf("Error: Failed to create study structure\n");
            return false;
        }

        // Set study metadata from scale
        study->SetDescription("Scale: " + scale->GetScaleInfo().name);
        if (!scale->GetScaleInfo().citation.empty()) {
            study->SetAuthor(scale->GetScaleInfo().citation);
        }

        // Create test directory with standard PEBL structure matching ScaleRunner's expected layout:
        //   tests/{code}/
        //     {code}.pbl         <- ScaleRunner.pbl (renamed)
        //     definitions/       <- {code}.json
        //     translations/      <- {code}.{lang}.json
        //     params/            <- schema + par files
        std::string testPath = studyPath + "/tests/" + scaleCode;
        std::string paramsPath = testPath + "/params";
        fs::create_directories(testPath + "/definitions");
        fs::create_directories(testPath + "/translations");
        fs::create_directories(paramsPath);

        // Copy ScaleRunner.pbl and rename to match scale code
        std::string scaleRunnerSource = mBatteryScalesPath + "/ScaleRunner.pbl";
        std::string scaleRunnerDest = testPath + "/" + scaleCode + ".pbl";

        if (!fs::exists(scaleRunnerSource)) {
            printf("Error: ScaleRunner.pbl not found at: %s\n", scaleRunnerSource.c_str());
            return false;
        }

        fs::copy_file(scaleRunnerSource, scaleRunnerDest);
        printf("Copied ScaleRunner.pbl to %s\n", scaleRunnerDest.c_str());

        // Export definition to definitions/ and translations to translations/
        if (!scale->ExportToJSON(testPath + "/definitions", testPath + "/translations")) {
            printf("Error: Failed to export scale files\n");
            return false;
        }

        printf("Exported scale definition and translations\n");

        // Create .pbl.about.txt from README.md if available, else from metadata
        {
            std::string aboutPath = testPath + "/" + scaleCode + ".pbl.about.txt";
            std::string readmePath = mBatteryDefinitionsPath + "/" + scaleCode + "/README.md";
            bool copied = false;

            if (fs::exists(readmePath)) {
                try {
                    fs::copy_file(readmePath, aboutPath, fs::copy_options::overwrite_existing);
                    printf("Created about file from README.md: %s\n", aboutPath.c_str());
                    copied = true;
                } catch (const std::exception& e) {
                    printf("Warning: Failed to copy README.md: %s\n", e.what());
                }
            }

            // Also check workspace for README.md
            if (!copied && !mWorkspaceScalesPath.empty()) {
                std::string wsReadme = mWorkspaceScalesPath + "/" + scaleCode + "/README.md";
                if (fs::exists(wsReadme)) {
                    try {
                        fs::copy_file(wsReadme, aboutPath, fs::copy_options::overwrite_existing);
                        printf("Created about file from workspace README.md: %s\n", aboutPath.c_str());
                        copied = true;
                    } catch (const std::exception& e) {
                        printf("Warning: Failed to copy workspace README.md: %s\n", e.what());
                    }
                }
            }

            // Fallback: generate from scale metadata
            if (!copied) {
                std::ofstream aboutFile(aboutPath);
                if (aboutFile.is_open()) {
                    const auto& info = scale->GetScaleInfo();
                    aboutFile << info.name << std::endl;
                    aboutFile << std::endl;
                    if (!info.description.empty()) {
                        aboutFile << info.description << std::endl;
                        aboutFile << std::endl;
                    }
                    if (!info.citation.empty()) {
                        aboutFile << "Citation:" << std::endl;
                        aboutFile << info.citation << std::endl;
                        aboutFile << std::endl;
                    }
                    if (!info.license.empty()) {
                        aboutFile << "License: " << info.license << std::endl;
                    }
                    aboutFile.close();
                    printf("Created about file from metadata: %s\n", aboutPath.c_str());
                }
            }
        }

        // Generate schema and parameter files from the scale's OSD parameters block
        GenerateSchemaFiles(*scale, testPath);

        // Generate screenshot for the deployed test
        GenerateScreenshot(scaleCode, testPath);

        // Add test to study
        Test test;
        test.testName = scaleCode;  // The actual .pbl filename (e.g., "MOCI")
        test.displayName = scale->GetScaleInfo().name;  // Human-readable name
        test.testPath = scaleCode;  // Directory name (e.g., "MOCI")
        test.included = true;

        study->AddTest(test);

        // Save study
        if (!study->Save()) {
            printf("Error: Failed to save study-info.json\n");
            return false;
        }

        // Create default chain with the scale test
        std::string chainPath = studyPath + "/chains/Main.json";
        auto defaultChain = Chain::CreateNew(chainPath, "Main",
                                             "Default chain for " + scale->GetScaleInfo().name);
        if (defaultChain) {
            // Add the scale test to the chain
            ChainItem testItem(ItemType::Test);
            testItem.testName = scaleCode;
            testItem.paramVariant = "default";
            testItem.language = "en";
            testItem.randomGroup = 0;

            defaultChain->AddItem(testItem);

            if (defaultChain->Save()) {
                printf("Created default chain with test: Main.json\n");
            } else {
                printf("Warning: Failed to save default chain\n");
            }
        } else {
            printf("Warning: Failed to create default chain\n");
        }

        printf("Successfully created scale study: %s\n", scaleCode.c_str());
        return true;

    } catch (const std::exception& e) {
        printf("Exception while creating scale study: %s\n", e.what());
        return false;
    }
}

bool ScaleManager::AddScaleToStudy(std::shared_ptr<ScaleDefinition> scale,
                                    const std::string& studyPath)
{
    if (!scale) {
        printf("Error: Scale is null\n");
        return false;
    }

    std::string scaleCode = scale->GetScaleInfo().code;
    printf("Adding scale '%s' to study at: %s\n", scaleCode.c_str(), studyPath.c_str());

    try {
        // Load the existing study
        auto study = Study::LoadFromDirectory(studyPath);
        if (!study) {
            printf("Error: Failed to load study from: %s\n", studyPath.c_str());
            return false;
        }

        // Check if this scale/test already exists in the study
        std::string testPath = studyPath + "/tests/" + scaleCode;
        if (fs::exists(testPath)) {
            printf("Removing existing test directory: %s\n", testPath.c_str());
            fs::remove_all(testPath);
        }

        // Create test directory with standard PEBL structure matching ScaleRunner's expected layout:
        //   tests/{code}/
        //     {code}.pbl         <- ScaleRunner.pbl (renamed)
        //     definitions/       <- {code}.json
        //     translations/      <- {code}.{lang}.json
        //     params/            <- schema + par files
        std::string paramsPath = testPath + "/params";
        fs::create_directories(testPath + "/definitions");
        fs::create_directories(testPath + "/translations");
        fs::create_directories(paramsPath);

        // Copy ScaleRunner.pbl
        std::string scaleRunnerSource = mBatteryScalesPath + "/ScaleRunner.pbl";
        std::string scaleRunnerDest = testPath + "/" + scaleCode + ".pbl";

        if (!fs::exists(scaleRunnerSource)) {
            printf("Error: ScaleRunner.pbl not found at: %s\n", scaleRunnerSource.c_str());
            return false;
        }

        fs::copy_file(scaleRunnerSource, scaleRunnerDest);

        // Export definition to definitions/ and translations to translations/
        if (!scale->ExportToJSON(testPath + "/definitions", testPath + "/translations")) {
            printf("Error: Failed to export scale files\n");
            return false;
        }

        // Create .pbl.about.txt from README.md if available, else from metadata
        {
            std::string aboutPath = testPath + "/" + scaleCode + ".pbl.about.txt";
            std::string readmePath = mBatteryDefinitionsPath + "/" + scaleCode + "/README.md";
            bool copied = false;

            if (fs::exists(readmePath)) {
                try {
                    fs::copy_file(readmePath, aboutPath, fs::copy_options::overwrite_existing);
                    copied = true;
                } catch (const std::exception&) {}
            }

            if (!copied && !mWorkspaceScalesPath.empty()) {
                std::string wsReadme = mWorkspaceScalesPath + "/" + scaleCode + "/README.md";
                if (fs::exists(wsReadme)) {
                    try {
                        fs::copy_file(wsReadme, aboutPath, fs::copy_options::overwrite_existing);
                        copied = true;
                    } catch (const std::exception&) {}
                }
            }

            if (!copied) {
                std::ofstream aboutFile(aboutPath);
                if (aboutFile.is_open()) {
                    const auto& info = scale->GetScaleInfo();
                    aboutFile << info.name << std::endl;
                    aboutFile << std::endl;
                    if (!info.description.empty()) {
                        aboutFile << info.description << std::endl;
                        aboutFile << std::endl;
                    }
                    if (!info.citation.empty()) {
                        aboutFile << "Citation:" << std::endl;
                        aboutFile << info.citation << std::endl;
                    }
                    aboutFile.close();
                }
            }
        }

        // Generate schema and parameter files from the scale's OSD parameters block
        GenerateSchemaFiles(*scale, testPath);

        // Generate screenshot for the deployed test
        GenerateScreenshot(scaleCode, testPath);

        // Add test to study (if not already present)
        if (!study->GetTest(scaleCode)) {
            Test test;
            test.testName = scaleCode;
            test.displayName = scale->GetScaleInfo().name;
            test.testPath = scaleCode;
            test.included = true;
            study->AddTest(test);
        }

        // Save study
        if (!study->Save()) {
            printf("Error: Failed to save study-info.json\n");
            return false;
        }

        printf("Successfully added scale '%s' to study\n", scaleCode.c_str());
        return true;

    } catch (const std::exception& e) {
        printf("Exception while adding scale to study: %s\n", e.what());
        return false;
    }
}

bool ScaleManager::GenerateScreenshot(const std::string& scaleCode, const std::string& testPath)
{
    std::string screenshotDest = testPath + "/" + scaleCode + ".pbl.png";

    // If screenshot already exists at destination, skip generation
    if (fs::exists(screenshotDest)) {
        printf("Screenshot already exists: %s\n", screenshotDest.c_str());
        return true;
    }

    // First check for pre-existing screenshot in library definitions
    std::string libraryScreenshot = mBatteryDefinitionsPath + "/" + scaleCode + "/" + scaleCode + ".pbl.png";
    if (fs::exists(libraryScreenshot)) {
        try {
            fs::copy_file(libraryScreenshot, screenshotDest, fs::copy_options::overwrite_existing);
            printf("Copied existing screenshot from library: %s\n", screenshotDest.c_str());
            return true;
        } catch (const std::exception& e) {
            printf("Warning: Failed to copy library screenshot: %s\n", e.what());
        }
    }

    // Check workspace definitions
    if (!mWorkspaceDefinitionsPath.empty()) {
        std::string wsScreenshot = mWorkspaceDefinitionsPath + "/" + scaleCode + "/" + scaleCode + ".pbl.png";
        if (fs::exists(wsScreenshot)) {
            try {
                fs::copy_file(wsScreenshot, screenshotDest, fs::copy_options::overwrite_existing);
                printf("Copied existing screenshot from workspace: %s\n", screenshotDest.c_str());
                return true;
            } catch (const std::exception& e) {
                printf("Warning: Failed to copy workspace screenshot: %s\n", e.what());
            }
        }
    }

    // No pre-existing screenshot - generate one using scale-screenshot.pbl
    // Find the PEBL binary (same directory as the launcher executable)
    std::string peblPath;
#ifdef _WIN32
    char exeBuf[MAX_PATH];
    GetModuleFileNameA(NULL, exeBuf, MAX_PATH);
    std::string exeStr(exeBuf);
    size_t lastSlash = exeStr.find_last_of("\\/");
    if (lastSlash != std::string::npos) {
        peblPath = exeStr.substr(0, lastSlash + 1) + "pebl2.exe";
    } else {
        peblPath = "pebl2.exe";
    }
#else
    char exeBuf[1024];
    ssize_t len = readlink("/proc/self/exe", exeBuf, sizeof(exeBuf) - 1);
    if (len != -1) {
        exeBuf[len] = '\0';
        std::string exeStr(exeBuf);
        size_t lastSlash = exeStr.find_last_of('/');
        if (lastSlash != std::string::npos) {
            peblPath = exeStr.substr(0, lastSlash + 1) + "pebl2";
        } else {
            peblPath = "pebl2";
        }
    } else {
        peblPath = "pebl2";
    }
#endif

    if (!fs::exists(peblPath)) {
        printf("Warning: PEBL executable not found at: %s\n", peblPath.c_str());
        return false;
    }

    // Find scale-screenshot.pbl (one directory up from scales/)
    std::string screenshotScript = mBatteryScalesPath + "/../scale-screenshot.pbl";
    if (!fs::exists(screenshotScript)) {
        printf("Warning: scale-screenshot.pbl not found at: %s\n", screenshotScript.c_str());
        return false;
    }

    // Resolve to absolute paths for the command
    std::string absScript = fs::canonical(screenshotScript).string();
    std::string absTestPath = fs::canonical(testPath).string();

    // Determine working directory based on directory layout:
    // - Study deployment: testPath has definitions/<code>.json → run from testPath
    // - Workspace save: testPath has <code>.json directly → run from parent so script finds <code>/<code>.json
    std::string cwd = absTestPath;
    bool workspaceLayout = false;

    std::string directJson = absTestPath + "/" + scaleCode + ".json";
    std::string defsJson = absTestPath + "/definitions/" + scaleCode + ".json";

    if (fs::exists(directJson) && !fs::exists(defsJson)) {
        // Workspace layout: <code>/<code>.json — run from parent directory
        cwd = fs::path(absTestPath).parent_path().string();
        workspaceLayout = true;
    }

    // Run: cd <cwd> && pebl2 scale-screenshot.pbl -v <scaleCode>
#ifdef _WIN32
    std::string cmd = "cd /d \"" + cwd + "\" && \"" + peblPath + "\" \"" + absScript + "\" -v " + scaleCode + " >nul 2>&1";
#else
    std::string cmd = "cd \"" + cwd + "\" && \"" + peblPath + "\" \"" + absScript + "\" -v " + scaleCode + " >/dev/null 2>&1";
#endif

    printf("Generating screenshot: %s\n", cmd.c_str());
    int ret = system(cmd.c_str());

    // If workspace layout, output lands in cwd/<code>.pbl.png — move to testPath
    if (workspaceLayout) {
        std::string outputFile = cwd + "/" + scaleCode + ".pbl.png";
        if (fs::exists(outputFile)) {
            try {
                fs::rename(outputFile, screenshotDest);
            } catch (const std::exception&) {
                // rename may fail across filesystems, fall back to copy+remove
                try {
                    fs::copy_file(outputFile, screenshotDest, fs::copy_options::overwrite_existing);
                    fs::remove(outputFile);
                } catch (const std::exception& e) {
                    printf("Warning: Failed to move screenshot: %s\n", e.what());
                }
            }
        }
    }

    if (fs::exists(screenshotDest)) {
        printf("Screenshot generated: %s\n", screenshotDest.c_str());
        return true;
    } else {
        printf("Warning: Screenshot generation failed (exit code %d)\n", ret);
        return false;
    }
}

// Generate .pbl.schema.json and .pbl.par.json from the scale's OSD parameters block.
// Called at scale deployment time (CreateStudyFromScale / AddScaleToStudy) so all
// OSD-defined parameters are immediately visible in the parameter editor.
// The par.json is created fresh; the schema is always regenerated from the definition.
bool ScaleManager::GenerateSchemaFiles(const ScaleDefinition& scale, const std::string& testPath)
{
    const std::string& code   = scale.GetScaleInfo().code;
    const std::string& name   = scale.GetScaleInfo().name;
    const auto& osdParams     = scale.GetParameters();
    std::string paramsPath    = testPath + "/params";

    fs::create_directories(paramsPath);

    // Base parameter names handled specially — not iterated from OSD map
    static const std::set<std::string> baseNames = {"scale", "shuffle_questions", "show_header"};

    // Helper: convert a string default value to the right JSON type
    auto jsonDefault = [](const std::string& type, const std::string& value) -> nlohmann::json {
        if (type == "boolean" || type == "integer") {
            try { return std::stoi(value); } catch (...) {}
        } else if (type == "float") {
            try { return std::stod(value); } catch (...) {}
        }
        return value;
    };

    // Helper: look up a base param's default from OSD, falling back to hardcoded default
    auto osdDefault = [&](const std::string& pname, nlohmann::json fallback) -> nlohmann::json {
        auto it = osdParams.find(pname);
        if (it != osdParams.end() && !it->second.defaultValue.empty()) {
            return jsonDefault(it->second.type, it->second.defaultValue);
        }
        return fallback;
    };

    nlohmann::json schemaParams = nlohmann::json::array();
    nlohmann::json parDefaults  = nlohmann::json::object();

    // 1. scale — hidden, always locked to this scale's code
    schemaParams.push_back({
        {"name",        "scale"},
        {"type",        "string"},
        {"default",     code},
        {"description", "OSD scale code (reads definitions/{code}.json)"},
        {"hidden",      true}
    });
    parDefaults["scale"] = code;

    // 2. shuffle_questions — default from OSD if declared, else 0
    {
        auto def = osdDefault("shuffle_questions", 0);
        schemaParams.push_back({
            {"name",        "shuffle_questions"},
            {"type",        "boolean"},
            {"default",     def},
            {"description", "Randomize item order within randomization groups"},
            {"options",     nlohmann::json::array({0, 1})}
        });
        parDefaults["shuffle_questions"] = def;
    }

    // 3. show_header — default from OSD if declared, else 1
    {
        auto def = osdDefault("show_header", 1);
        schemaParams.push_back({
            {"name",        "show_header"},
            {"type",        "boolean"},
            {"default",     def},
            {"description", "Display the scale title header above the questionnaire"},
            {"options",     nlohmann::json::array({0, 1})}
        });
        parDefaults["show_header"] = def;
    }

    // 4. OSD-defined extra parameters (text substitution vars, feature flags, etc.)
    for (const auto& [pname, pdef] : osdParams) {
        if (baseNames.count(pname)) continue;  // already handled above

        nlohmann::json sp;
        sp["name"] = pname;
        sp["type"] = pdef.type.empty() ? "string" : pdef.type;

        if (!pdef.defaultValue.empty()) {
            auto def = jsonDefault(pdef.type, pdef.defaultValue);
            sp["default"]          = def;
            parDefaults[pname]     = def;
        }
        if (!pdef.description.empty()) {
            sp["description"] = pdef.description;
        }
        if (!pdef.options.empty()) {
            nlohmann::json opts = nlohmann::json::array();
            for (const auto& o : pdef.options) opts.push_back(o);
            sp["options"] = opts;
        } else if (pdef.type == "boolean") {
            sp["options"] = nlohmann::json::array({0, 1});
        }

        schemaParams.push_back(sp);
    }

    // 5. Selectable dimension enable/disable checkboxes
    // Build set of param names already covered (base + OSD explicit) to avoid duplicates
    std::set<std::string> coveredParams = baseNames;
    for (const auto& [pname, _] : osdParams) coveredParams.insert(pname);

    for (const auto& dim : scale.GetDimensions()) {
        if (!dim.selectable) continue;

        // Determine param name: use enabled_param if set, else auto-name "do_{id}"
        std::string pname = dim.enabled_param.empty() ? ("do_" + dim.id) : dim.enabled_param;
        if (coveredParams.count(pname)) continue;  // already defined explicitly
        coveredParams.insert(pname);

        int defVal = dim.default_enabled ? 1 : 0;
        schemaParams.push_back({
            {"name",        pname},
            {"type",        "boolean"},
            {"default",     defVal},
            {"description", "Include " + dim.name + " dimension"},
            {"options",     nlohmann::json::array({0, 1})}
        });
        parDefaults[pname] = defVal;
    }

    // Write schema
    nlohmann::json schema = {
        {"test",        code},
        {"version",     "1.0"},
        {"description", name + " — auto-generated from scale definition"},
        {"parameters",  schemaParams}
    };
    std::string schemaPath = paramsPath + "/" + code + ".pbl.schema.json";
    std::ofstream sf(schemaPath);
    if (!sf.is_open()) {
        printf("Warning: Failed to write schema file: %s\n", schemaPath.c_str());
        return false;
    }
    sf << schema.dump(2);
    sf.close();
    printf("Generated schema: %s\n", schemaPath.c_str());

    // Write par.json with all defaults (only if not already present)
    std::string parPath = paramsPath + "/" + code + ".pbl.par.json";
    if (!fs::exists(parPath)) {
        std::ofstream pf(parPath);
        if (pf.is_open()) {
            pf << parDefaults.dump(2);
            pf.close();
            printf("Generated params: %s\n", parPath.c_str());
        }
    }

    return true;
}
