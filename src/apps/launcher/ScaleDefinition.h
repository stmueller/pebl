// ScaleDefinition.h - PEBL Scale data model (ScaleRunner format)
// Copyright (c) 2026 Shane T. Mueller
// Licensed under GPL

#ifndef SCALE_DEFINITION_H
#define SCALE_DEFINITION_H

#include <string>
#include <vector>
#include <map>
#include <memory>

// Scale metadata (matches ScaleRunner scale_info)
struct ScaleInfo {
    std::string name;           // "Grit Scale"
    std::string code;           // "grit"
    std::string abbreviation;   // "Grit-O"
    std::string description;    // Brief description
    std::string citation;       // Full citation with DOI
    std::string license;        // License information
    std::string version;        // "1.0"
    std::string url;            // Website URL

    ScaleInfo() : version("1.0") {}
};

// Runtime parameter definition
struct ScaleParameter {
    std::string type;           // "boolean", "integer", "string"
    std::string defaultValue;   // Default value as string
    std::string description;    // Description

    ScaleParameter() = default;
    ScaleParameter(const std::string& t, const std::string& d, const std::string& desc)
        : type(t), defaultValue(d), description(desc) {}
};

// Likert options (global for scale)
struct LikertOptions {
    int points;                 // Number of points (e.g., 5)
    int min;                    // Minimum value (optional, defaults based on points)
    int max;                    // Maximum value (optional, defaults based on points)
    std::string question_head;  // Translation key for instructions
    std::vector<std::string> labels;  // Translation keys for each option

    LikertOptions() : points(5), min(-1), max(-1), question_head("question_head") {}
    // Note: min=-1, max=-1 means "use defaults based on points"
};

// Dimension definition (for scoring and organization)
struct ScaleDimension {
    std::string id;             // "COI" (used in scoring)
    std::string name;           // "Consistency of Interest"
    std::string abbreviation;   // "COI"
    std::string description;    // Full description
    std::string enabled_param;  // Parameter name to enable/disable (or empty)

    ScaleDimension() = default;
    ScaleDimension(const std::string& i, const std::string& n, const std::string& abbr)
        : id(i), name(n), abbreviation(abbr) {}
};

// Question definition (structure only - text comes from translations)
struct ScaleQuestion {
    std::string id;             // "grit2"
    std::string text_key;       // Translation key (usually same as id)
    std::string type;           // inst, likert, vas, grid, short, long, multi, multicheck, image, imageresponse
    std::string dimension;      // Dimension id (or empty/null)
    int coding;                 // 1 = normal, -1 = reverse, 0 = not scored

    // Type-specific fields
    int likert_points;          // For likert questions
    int likert_min;             // For likert questions (optional, -1 = use scale default)
    int likert_max;             // For likert questions (optional, -1 = use scale default)
    std::vector<std::string> likert_labels;  // For likert questions (optional, empty = use scale default)
    int min_value;              // For vas
    int max_value;              // For vas
    std::string left_label;     // Translation key for VAS left
    std::string right_label;    // Translation key for VAS right
    std::vector<std::string> options;  // Translation keys for multi/multicheck
    std::vector<std::string> correct;  // Correct answer keys for multi
    std::string image;          // Image path for image/imageresponse
    std::vector<std::string> columns;  // Translation keys for grid columns
    std::vector<std::string> rows;     // Translation keys for grid rows

    ScaleQuestion() : coding(1), likert_points(5), likert_min(-1), likert_max(-1), min_value(0), max_value(100) {}
};

// Scoring definition for a dimension
struct DimensionScoring {
    std::string method;         // mean_coded, sum_coded, weighted_sum, sum_correct
    std::vector<std::string> items;  // Question IDs in this score
    std::string description;    // Description of the score
    std::map<std::string, double> weights;  // For weighted_sum (optional)
    std::map<std::string, int> item_coding;  // Per-item coding: item_id -> 1 (normal), -1 (reverse), 0 (not scored)
    std::map<std::string, std::vector<std::string>> correct_answers;  // For sum_correct: item_id -> list of acceptable answers/patterns

    DimensionScoring() : method("mean_coded") {}
};

// Report configuration
struct ReportConfig {
    std::string template_type;  // "standard", etc.
    std::vector<std::string> include;  // ["timestamp", "completion_time", "dimension_scores"]
    std::string header;         // Header text
    std::vector<std::string> footer_refs;  // Footer references/citations

    ReportConfig() : template_type("standard") {}
};

// Data output configuration
struct DataOutput {
    std::string individual_file;   // "grit-{subnum}.csv"
    std::string pooled_file;       // "grit-pooled.csv"
    std::string report_file;       // "grit-report-{subnum}.html"
    std::string columns;           // CSV column specification
    std::string pooled_columns;    // Pooled file columns
};

// Translation data (language -> key -> value)
using ScaleTranslations = std::map<std::string, std::map<std::string, std::string>>;

// Main scale definition class
class ScaleDefinition {
public:
    ScaleDefinition();
    ~ScaleDefinition();

    // Factory methods
    static std::shared_ptr<ScaleDefinition> CreateNew(const std::string& code);
    static std::shared_ptr<ScaleDefinition> LoadFromFile(const std::string& jsonPath);

    // Serialization
    bool SaveToFile(const std::string& jsonPath);
    bool ExportToJSON(const std::string& definitionsPath, const std::string& translationsPath);

    // Load definition and translations from a scales directory (<basePath>/<code>/<code>.json)
    bool LoadFromScalesDir(const std::string& basePath, const std::string& scaleCode);

    // Validation
    bool Validate(std::string& errorOutput);

    // Getters (non-const for editing)
    ScaleInfo& GetScaleInfo() { return mScaleInfo; }
    std::map<std::string, ScaleParameter>& GetParameters() { return mParameters; }
    LikertOptions& GetLikertOptions() { return mLikertOptions; }
    std::vector<ScaleQuestion>& GetQuestions() { return mQuestions; }
    std::vector<ScaleDimension>& GetDimensions() { return mDimensions; }
    std::map<std::string, DimensionScoring>& GetScoring() { return mScoring; }
    ReportConfig& GetReportConfig() { return mReportConfig; }
    DataOutput& GetDataOutput() { return mDataOutput; }
    ScaleTranslations& GetTranslations() { return mTranslations; }

    // Const getters
    const ScaleInfo& GetScaleInfo() const { return mScaleInfo; }
    const std::map<std::string, ScaleParameter>& GetParameters() const { return mParameters; }
    const LikertOptions& GetLikertOptions() const { return mLikertOptions; }
    const std::vector<ScaleQuestion>& GetQuestions() const { return mQuestions; }
    const std::vector<ScaleDimension>& GetDimensions() const { return mDimensions; }
    const std::map<std::string, DimensionScoring>& GetScoring() const { return mScoring; }
    const ReportConfig& GetReportConfig() const { return mReportConfig; }
    const DataOutput& GetDataOutput() const { return mDataOutput; }
    const ScaleTranslations& GetTranslations() const { return mTranslations; }

    // Question management
    void AddQuestion(const ScaleQuestion& question);
    void RemoveQuestion(const std::string& questionID);
    void MoveQuestion(int fromIndex, int toIndex);
    ScaleQuestion* GetQuestion(const std::string& questionID);

    // Dimension management
    void AddDimension(const ScaleDimension& dimension);
    void RemoveDimension(const std::string& dimensionID);
    ScaleDimension* GetDimension(const std::string& dimensionID);

    // Translation management
    void AddTranslation(const std::string& language, const std::string& key, const std::string& value);
    void RemoveTranslation(const std::string& language);
    std::vector<std::string> GetAvailableLanguages() const;
    std::string GetTranslation(const std::string& language, const std::string& key) const;

    // Validation helpers
    struct ValidationResult {
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        bool IsValid() const { return errors.empty(); }
    };
    ValidationResult ValidateInternal() const;

    // Dirty flag for unsaved changes
    bool IsDirty() const { return mDirty; }
    void SetDirty(bool dirty) { mDirty = dirty; }

private:
    bool LoadDefinitionJSON(const std::string& jsonPath);
    bool LoadTranslationJSON(const std::string& jsonPath, const std::string& language);
    bool SaveDefinitionJSON(const std::string& jsonPath);
    bool SaveTranslationJSON(const std::string& jsonPath, const std::string& language);

    ScaleInfo mScaleInfo;
    std::map<std::string, ScaleParameter> mParameters;
    LikertOptions mLikertOptions;
    std::vector<ScaleQuestion> mQuestions;
    std::vector<ScaleDimension> mDimensions;
    std::map<std::string, DimensionScoring> mScoring;  // dimension_id -> scoring
    ReportConfig mReportConfig;
    DataOutput mDataOutput;
    ScaleTranslations mTranslations;
    bool mDirty;
};

#endif // SCALE_DEFINITION_H
