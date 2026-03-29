// ScaleDefinition.h - PEBL Scale data model (ScaleRunner format)
// Copyright (c) 2026 Shane T. Mueller
// Licensed under GPL

#ifndef SCALE_DEFINITION_H
#define SCALE_DEFINITION_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <json.hpp>

// Scale metadata (matches ScaleRunner scale_info)
struct ScaleInfo {
    std::string name;                // "Grit Scale"
    std::string code;                // "grit"
    std::string abbreviation;        // "Grit-O"
    std::string description;         // Brief description
    std::string citation;            // Full citation with DOI
    std::string license;             // Short license label (e.g., "CC BY 4.0", "Public Domain")
    std::string license_explanation; // Full license terms / permissions grant
    std::string license_url;         // URL documenting the license terms
    std::string version;             // "1.0"
    std::string url;                 // Website URL
    std::string domain;              // Subject domain (e.g., "Mood", "Substance Use", "Personality")

    ScaleInfo() : version("1.0") {}
};

// Runtime parameter definition
struct ScaleParameter {
    std::string type;           // "boolean", "integer", "string", "choice"
    std::string defaultValue;   // Default value as string
    std::string description;    // Description
    std::vector<std::string> options;  // Allowed values (for choice/boolean types)

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

// Visible-when condition (for conditional display)
struct VisibleWhenCondition {
    std::string source_type;  // "parameter" or "question"
    std::string source_name;  // parameter name or question ID
    std::string op;           // "equals", "not_equals", "greater_than", "less_than", "in", "not_in"
    std::string value;                   // scalar value (when is_list == false)
    std::vector<std::string> values;     // list value (when is_list == true, for "in"/"not_in")
    bool is_list;                        // true when value field is a JSON array
    VisibleWhenCondition() : source_type("parameter"), op("equals"), is_list(false) {}
};

// Dimension definition (for scoring and organization)
struct ScaleDimension {
    std::string id;             // "COI" (used in scoring)
    std::string name;           // "Consistency of Interest"
    std::string abbreviation;   // "COI"
    std::string description;    // Full description
    std::string enabled_param;  // Parameter name to enable/disable (or empty)
    bool selectable;            // If true, auto-generate a boolean enable param in schema
    bool default_enabled;       // Default value for the auto-generated enable param

    // Dynamic visibility conditions
    bool has_visible_when;
    std::string visible_when_logic;  // "all" or "any"
    std::vector<VisibleWhenCondition> visible_when;

    ScaleDimension() : selectable(false), default_enabled(true),
                       has_visible_when(false), visible_when_logic("all") {}
    ScaleDimension(const std::string& i, const std::string& n, const std::string& abbr)
        : id(i), name(n), abbreviation(abbr), has_visible_when(false), visible_when_logic("all") {}
};

// Per-question input validation — each constraint is independent with its own error key.
// Integer constraints use -1 to mean "not set". Boolean flags gate the number constraints.
struct QuestionValidation {
    // Character length (short / long)
    int min_length;                   // -1 = not set
    int max_length;                   // -1 = not set
    std::string min_length_error;     // translation key (auto-generated from question id)
    std::string max_length_error;

    // Word count (short / long)
    int min_words;
    int max_words;
    std::string min_words_error;
    std::string max_words_error;

    // Numeric range (short — implies number-only keyboard)
    bool number_min_set;
    bool number_max_set;
    double number_min;
    double number_max;
    std::string number_min_error;
    std::string number_max_error;

    // Regex pattern (short)
    std::string pattern;              // empty = not set
    std::string pattern_error;

    // Selection count (multicheck)
    int min_selected;
    int max_selected;
    std::string min_selected_error;
    std::string max_selected_error;

    bool HasAnyValidation() const {
        return min_length >= 0 || max_length >= 0 ||
               min_words >= 0 || max_words >= 0 ||
               number_min_set || number_max_set ||
               !pattern.empty() ||
               min_selected >= 0 || max_selected >= 0;
    }

    QuestionValidation()
        : min_length(-1), max_length(-1), min_words(-1), max_words(-1),
          number_min_set(false), number_max_set(false), number_min(0), number_max(0),
          min_selected(-1), max_selected(-1) {}
};

// Question definition (structure only - text comes from translations)
struct ScaleQuestion {
    std::string id;             // "grit2"
    std::string text_key;       // Translation key (usually same as id)
    std::string type;           // inst, likert, vas, grid, short, long, multi, multicheck, image, imageresponse, section
    std::string dimension;      // Dimension id (or empty/null)
    int coding;                 // 1 = normal, -1 = reverse, 0 = not scored
    int random_group;           // 0 = fixed position, >0 = shuffle group ID
    int required_state;         // -1 = use default, 0 = optional, 1 = required
    QuestionValidation validation;  // Input validation rules (C9)

    // Conditional display
    bool has_visible_when;
    std::string visible_when_logic;                        // "all" or "any"
    std::vector<VisibleWhenCondition> visible_when_simple;  // flat conditions (UI-editable)
    bool visible_when_is_complex;                           // nested beyond flat — raw JSON only

    // Type-specific fields
    int likert_points;          // For likert questions
    int likert_min;             // For likert questions (optional, -1 = use scale default)
    int likert_max;             // For likert questions (optional, -1 = use scale default)
    bool likert_reverse;        // Display buttons right-to-left (highest value on left)
    bool randomize_options;     // Shuffle option order for multi/multicheck (S5)
    std::vector<std::string> likert_labels;  // For likert questions (optional, empty = use scale default)
    int min_value;              // For vas
    int max_value;              // For vas
    std::string left_label;     // Translation key for VAS left (min_label)
    std::string right_label;    // Translation key for VAS right (max_label)
    std::string vas_orientation; // "horizontal" (default) or "vertical"
    struct VasAnchor {
        double value;
        std::string label;      // Translation key
    };
    std::vector<VasAnchor> vas_anchors;  // Named anchor points along the scale
    std::vector<std::string> options;  // Translation keys for multi/multicheck
    std::vector<std::string> correct;  // Correct answer keys for multi
    std::string image;          // Image path for image/imageresponse
    std::vector<std::string> columns;  // Translation keys for grid columns
    std::vector<std::string> rows;     // Translation keys for grid rows

    // Per-item question head — overrides scale-level likert_options.question_head
    std::string question_head;  // Translation key (empty = use scale default)

    // Answer alias — optional semantic name for use in {answer.alias} piping
    std::string answer_alias;

    // Gate (blocking) — if set, a wrong answer stops the scale, saves data, and shows a message
    bool has_gate;
    std::string gate_required_value;        // Exact-match form (multi): value that allows continuation
    std::string gate_operator;              // Numeric form (short): "greater_than","less_than","equals","not_equals"
    double gate_value;                      // Numeric form (short): threshold to compare against
    std::string gate_terminate_message_key; // Translation key for the termination message

    // Section control (type == "section" only)
    bool revisable;  // Default true: runners allow Back button within this section. false = responses are final.
    bool section_randomize;  // Default false: shuffle non-fixed questions within section
    std::vector<std::string> section_randomize_fixed;  // Question IDs pinned to original position

    ScaleQuestion() : coding(1), random_group(1), required_state(-1), has_visible_when(false), visible_when_logic("all"), visible_when_is_complex(false), likert_points(-1), likert_min(-1), likert_max(-1), likert_reverse(false), randomize_options(false), min_value(0), max_value(100), has_gate(false), gate_value(0.0), revisable(true), section_randomize(false) {}
};

struct NormThreshold {
    double min;
    double max;
    std::string label;
    NormThreshold() : min(0), max(0) {}
    NormThreshold(double mn, double mx, const std::string& lbl) : min(mn), max(mx), label(lbl) {}
};

// A single transform step: op (add/subtract/multiply/divide) + numeric value
struct TransformStep {
    std::string op;    // "add", "subtract", "multiply", "divide"
    double value;      // Numeric operand

    TransformStep() : op("add"), value(0.0) {}
    TransformStep(const std::string& o, double v) : op(o), value(v) {}
};

// Scoring definition for a dimension
struct DimensionScoring {
    std::string method;         // mean_coded, sum_coded, weighted_sum, weighted_mean, sum_correct
    std::vector<std::string> items;  // Question IDs in this score
    std::vector<std::string> scores; // Score IDs used as inputs (for composite scores)
    std::string description;    // Description of the score
    std::map<std::string, double> weights;  // For weighted_sum (optional)
    std::map<std::string, int> item_coding;  // Per-item coding: item_id -> 1 (normal), -1 (reverse), 0 (not scored)
    std::map<std::string, std::vector<std::string>> correct_answers;  // For sum_correct: item_id -> list of acceptable answers/patterns
    std::vector<NormThreshold> norms;  // Interpretation thresholds for report (optional)
    std::vector<TransformStep> transform;  // Post-scoring arithmetic transform steps (optional)

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

// Computed variable definition (S7)
struct ComputedVariable {
    std::string expression;     // e.g., "score.PHQ_total >= 10" or "answer.q1 * answer.q2 * 8.0"
    std::string type;           // "boolean", "number"
    std::vector<NormThreshold> norms;  // Optional interpretation thresholds

    ComputedVariable() : type("number") {}
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
    bool ExportToOSD(const std::string& outputDir);   // Bundle definition + all translations into {CODE}.osd

    // Load definition and translations from a scales directory (<basePath>/<code>/<code>.json)
    bool LoadFromScalesDir(const std::string& basePath, const std::string& scaleCode);

    // Load from a standalone .osd bundle file
    static std::shared_ptr<ScaleDefinition> LoadFromOSDFile(const std::string& osdPath);

    // True if this scale was loaded from a .osd bundle (vs. unpacked .json)
    bool IsSourceOSD() const { return mSourceIsOSD; }

    // Validation
    bool Validate(std::string& errorOutput);

    // Getters (non-const for editing)
    ScaleInfo& GetScaleInfo() { return mScaleInfo; }
    std::map<std::string, ScaleParameter>& GetParameters() { return mParameters; }
    LikertOptions& GetLikertOptions() { return mLikertOptions; }
    std::vector<ScaleQuestion>& GetQuestions() { return mQuestions; }
    std::vector<ScaleDimension>& GetDimensions() { return mDimensions; }
    std::map<std::string, DimensionScoring>& GetScoring() { return mScoring; }
    std::map<std::string, ComputedVariable>& GetComputed() { return mComputed; }
    ReportConfig& GetReportConfig() { return mReportConfig; }
    DataOutput& GetDataOutput() { return mDataOutput; }
    ScaleTranslations& GetTranslations() { return mTranslations; }
    nlohmann::json& GetRawDefinition() { return mRawDefinition; }

    // Const getters
    const ScaleInfo& GetScaleInfo() const { return mScaleInfo; }
    const std::map<std::string, ScaleParameter>& GetParameters() const { return mParameters; }
    const LikertOptions& GetLikertOptions() const { return mLikertOptions; }
    const std::vector<ScaleQuestion>& GetQuestions() const { return mQuestions; }
    const std::vector<ScaleDimension>& GetDimensions() const { return mDimensions; }
    const std::map<std::string, DimensionScoring>& GetScoring() const { return mScoring; }
    const std::map<std::string, ComputedVariable>& GetComputed() const { return mComputed; }
    const ReportConfig& GetReportConfig() const { return mReportConfig; }
    const DataOutput& GetDataOutput() const { return mDataOutput; }
    const ScaleTranslations& GetTranslations() const { return mTranslations; }
    const nlohmann::json& GetRawDefinition() const { return mRawDefinition; }

    // Question management
    void AddQuestion(const ScaleQuestion& question);
    void InsertQuestion(int index, const ScaleQuestion& question);
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

    // Default required setting (-1 = type defaults, 0 = all optional, 1 = all required)
    int GetDefaultRequired() const { return mDefaultRequired; }
    void SetDefaultRequired(int val) { mDefaultRequired = val; mDirty = true; }

    // Dirty flag for unsaved changes
    bool IsDirty() const { return mDirty; }
    void SetDirty(bool dirty) { mDirty = dirty; }

private:
    bool LoadDefinitionJSON(const std::string& jsonPath);
    bool ParseDefinitionFromJSON(const nlohmann::json& j);   // Parse already-loaded JSON object
    bool LoadFromOSDBundlePath(const std::string& osdPath);  // Load from .osd bundle file
    bool LoadTranslationJSON(const std::string& jsonPath, const std::string& language);
    bool SaveDefinitionJSON(const std::string& jsonPath);
    bool SaveTranslationJSON(const std::string& jsonPath, const std::string& language);
    bool BuildDefinitionJSONObject(nlohmann::json& outJSON) const;  // Build definition JSON from memory

    ScaleInfo mScaleInfo;
    std::map<std::string, ScaleParameter> mParameters;
    LikertOptions mLikertOptions;
    std::vector<ScaleQuestion> mQuestions;
    std::vector<ScaleDimension> mDimensions;
    std::map<std::string, DimensionScoring> mScoring;  // dimension_id -> scoring
    std::map<std::string, ComputedVariable> mComputed; // computed variable name -> definition
    ReportConfig mReportConfig;
    DataOutput mDataOutput;
    ScaleTranslations mTranslations;
    int mDefaultRequired;       // -1 = type defaults, 0 = all optional, 1 = all required
    bool mDirty;
    bool mSourceIsOSD;          // true if loaded from a .osd bundle file

    // Raw JSON from original file - preserves unknown fields (pages, response_footer, etc.)
    nlohmann::json mRawDefinition;
};

#endif // SCALE_DEFINITION_H
