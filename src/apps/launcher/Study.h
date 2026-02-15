// Study.h - PEBL Study data model
// Copyright (c) 2026 Shane T. Mueller
// Licensed under GPL

#ifndef STUDY_H
#define STUDY_H

#include <string>
#include <vector>
#include <map>
#include <memory>

// Parameter variant definition
struct ParameterVariant {
    std::string description;
    std::string file;  // Path to .par.json, empty for default

    ParameterVariant() = default;
    ParameterVariant(const std::string& desc, const std::string& f)
        : description(desc), file(f) {}
};

// Test definition within a study
struct Test {
    std::string testName;      // Test ID (e.g., "spatialgrid")
    std::string displayName;   // Display name (e.g., "Spatial Grid Memory")
    std::string testPath;
    bool included;
    std::map<std::string, ParameterVariant> parameterVariants;

    Test() : included(true) {}

    // Check if test directory exists
    bool Exists(const std::string& studyPath) const;

    // Get available languages (from translations/)
    std::vector<std::string> GetAvailableLanguages(const std::string& studyPath) const;

    // Get parameter variant or nullptr if not found
    const ParameterVariant* GetVariant(const std::string& variantName) const;
};

// Study data model
class Study {
public:
    Study();
    ~Study();

    // Load study from directory
    static std::shared_ptr<Study> LoadFromDirectory(const std::string& path);

    // Save study-info.json
    bool Save();

    // Create new empty study
    static std::shared_ptr<Study> CreateNew(const std::string& path,
                                            const std::string& name,
                                            const std::string& author = "");

    // Getters
    const std::string& GetName() const { return mName; }
    const std::string& GetDescription() const { return mDescription; }
    int GetVersion() const { return mVersion; }
    const std::string& GetAuthor() const { return mAuthor; }
    const std::string& GetStudyToken() const { return mStudyToken; }
    const std::string& GetUploadServerURL() const { return mUploadServerURL; }
    bool GetUploadEnabled() const { return mUploadEnabled; }
    const std::string& GetCreatedDate() const { return mCreatedDate; }
    const std::string& GetModifiedDate() const { return mModifiedDate; }
    const std::string& GetPath() const { return mPath; }
    const std::vector<Test>& GetTests() const { return mTests; }

    // Get 4-character study code from study name (alphanumeric only)
    std::string GetStudyCode() const;

    // Setters
    void SetName(const std::string& name) { mName = name; UpdateModifiedDate(); }
    void SetDescription(const std::string& desc) { mDescription = desc; UpdateModifiedDate(); }
    void SetAuthor(const std::string& author) { mAuthor = author; UpdateModifiedDate(); }
    void SetStudyToken(const std::string& token) { mStudyToken = token; UpdateModifiedDate(); }
    void SetUploadServerURL(const std::string& url) { mUploadServerURL = url; UpdateModifiedDate(); }
    void SetUploadEnabled(bool enabled) { mUploadEnabled = enabled; UpdateModifiedDate(); }
    void IncrementVersion() { mVersion++; UpdateModifiedDate(); }

    // Test management
    bool AddTest(const Test& test);
    bool RemoveTest(const std::string& testName);
    Test* GetTest(const std::string& testName);
    const Test* GetTest(const std::string& testName) const;

    // Chain management
    std::vector<std::string> GetChainFiles() const;
    int GetChainCount() const;

    // Upload configuration management
    // Create or update upload.json for a specific test
    bool CreateUploadConfigForTest(const std::string& testName);

    // Get path to upload.json for a test (tests/testname/upload.json)
    std::string GetUploadConfigPath(const std::string& testName) const;

    // Check if test has upload.json
    bool TestHasUploadConfig(const std::string& testName) const;

    // Validation
    struct ValidationResult {
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        bool IsValid() const { return errors.empty(); }
    };
    ValidationResult Validate() const;

private:
    bool LoadFromJSON(const std::string& jsonPath);
    bool SaveToJSON(const std::string& jsonPath);
    void UpdateModifiedDate();
    std::string GetCurrentISO8601Time() const;

    std::string mPath;           // Filesystem path to study directory
    std::string mName;
    std::string mDescription;
    int mVersion;
    std::string mAuthor;
    std::string mStudyToken;     // Study token from PEBLOnlinePlatform
    std::string mUploadServerURL; // Server URL for uploading (e.g., https://pebl.example.com)
    std::string mCreatedDate;    // ISO 8601
    std::string mModifiedDate;   // ISO 8601
    bool mUploadEnabled;
    std::vector<Test> mTests;
};

#endif // STUDY_H
