# PEBL Scale Builder - C++ + Dear ImGui Implementation Plan

## Overview

Integrate a visual scale builder/editor into the PEBL Launcher using Dear ImGui. This will allow researchers to create and edit psychological scales without manually writing JSON files.

## Architecture

### 1. C++ Class Structure

Following the existing `Study.h` / `Study.cpp` pattern, create data model classes:

#### `ScaleDefinition.h` / `ScaleDefinition.cpp`
```cpp
class ScaleDefinition {
public:
    struct ScaleInfo {
        std::string code;           // e.g., "grit"
        std::string name;           // e.g., "Grit Scale"
        std::string citation;
        std::string description;
        std::string author;
        std::string version;
    };

    struct Dimension {
        std::string name;           // e.g., "GRIT_PERSEVERANCE"
        std::string description;    // e.g., "Perseverance of Effort"
        std::vector<std::string> questionIDs;  // e.g., ["grit2", "grit3"]
    };

    struct Question {
        std::string id;             // e.g., "grit1"
        std::string type;           // inst, likert, vas, grid, short, long, multi, multicheck, image, imageresponse
        std::string prompt;         // Question text
        int coding;                 // 1 or -1 (for reverse coding)

        // Type-specific fields
        std::vector<std::string> options;      // For likert/multi/multicheck
        std::vector<std::string> correctAnswers; // For multi (correct responses)
        std::string imagePath;                 // For image/imageresponse
        int minValue, maxValue;                // For vas
        std::string leftLabel, rightLabel;     // For vas
    };

    struct Scoring {
        std::string type;           // mean_coded, sum_coded, weighted_sum, sum_correct
        std::map<std::string, double> weights;  // For weighted_sum
    };

    // Factory methods
    static std::shared_ptr<ScaleDefinition> CreateNew(const std::string& code);
    static std::shared_ptr<ScaleDefinition> LoadFromFile(const std::string& jsonPath);

    // Serialization
    bool SaveToFile(const std::string& jsonPath);
    bool ExportToJSON(const std::string& definitionsPath, const std::string& translationsPath);

    // Validation (calls scale-validator.pbl)
    bool Validate(std::string& errorOutput);

    // Getters/Setters
    ScaleInfo& GetScaleInfo() { return mScaleInfo; }
    std::vector<Question>& GetQuestions() { return mQuestions; }
    std::vector<Dimension>& GetDimensions() { return mDimensions; }
    Scoring& GetScoring() { return mScoring; }

private:
    ScaleInfo mScaleInfo;
    std::vector<Question> mQuestions;
    std::vector<Dimension> mDimensions;
    Scoring mScoring;
    std::map<std::string, std::map<std::string, std::string>> mTranslations; // lang -> key -> value
};
```

#### `ScaleManager.h` / `ScaleManager.cpp`
```cpp
class ScaleManager {
public:
    ScaleManager(const std::string& batteryPath);

    // Scan battery/scales/ for existing scales
    std::vector<std::string> GetAvailableScales();

    // Create new scale
    std::shared_ptr<ScaleDefinition> CreateScale(const std::string& code);

    // Load existing scale
    std::shared_ptr<ScaleDefinition> LoadScale(const std::string& code);

    // Delete scale
    bool DeleteScale(const std::string& code);

    // Get paths
    std::string GetDefinitionPath(const std::string& code);
    std::string GetTranslationPath(const std::string& code, const std::string& lang);

private:
    std::string mBatteryPath;
    std::string mScalesPath;      // battery/scales/
    std::string mDefinitionsPath; // battery/scales/definitions/
    std::string mTranslationsPath; // battery/scales/translations/
};
```

### 2. User Interface Design

#### Integration Point: Menu Bar
Add "Scale Builder" to the launcher menu bar (in `LauncherUI::RenderMenuBar()`):

```cpp
if (ImGui::BeginMenu("Tools")) {
    if (ImGui::MenuItem("Scale Builder")) {
        mShowScaleBuilder = true;
    }
    if (ImGui::MenuItem("Translation Editor")) {
        LaunchTranslationEditor();
    }
    if (ImGui::MenuItem("Data Combiner")) {
        LaunchDataCombiner();
    }
    ImGui::EndMenu();
}
```

#### Main Scale Builder Window
Modal window similar to `ShowParameterEditor()` but larger, following the pattern:

```cpp
void LauncherUI::ShowScaleBuilder()
{
    ImGui::SetNextWindowSize(ImVec2(1200, 800), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Scale Builder", &mShowScaleBuilder, ImGuiWindowFlags_MenuBar))
    {
        // Menu bar with File, Edit, Validate, Test
        RenderScaleBuilderMenuBar();

        // Left panel: Scale list
        ImGui::BeginChild("ScaleList", ImVec2(250, 0), true);
        RenderScaleList();
        ImGui::EndChild();

        ImGui::SameLine();

        // Right panel: Scale editor (tabbed)
        ImGui::BeginChild("ScaleEditor", ImVec2(0, 0), true);

        if (mCurrentScale) {
            if (ImGui::BeginTabBar("ScaleEditorTabs")) {
                if (ImGui::BeginTabItem("Scale Info")) {
                    RenderScaleInfoEditor();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Questions")) {
                    RenderQuestionsEditor();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Dimensions")) {
                    RenderDimensionsEditor();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Scoring")) {
                    RenderScoringEditor();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Translations")) {
                    RenderTranslationsEditor();
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
        } else {
            ImGui::Text("No scale selected. Create a new scale or select from the list.");
        }

        ImGui::EndChild();
    }
    ImGui::End();
}
```

### 3. Editor Components

#### Scale Info Tab
- Text input: Scale code (e.g., "grit")
- Text input: Full name
- Text input: Citation
- Multiline text: Description
- Text input: Author
- Text input: Version

#### Questions Tab
- List of questions (left panel)
- **Add buttons:**
  - "Add Single Question" - Add one question at a time
  - "Batch Import from Text..." - Import multiple questions from newline-separated list
  - "Import from CSV..." - Import from CSV/TSV file with full question metadata
- Remove/Reorder buttons
- Question editor (right panel):
  - Question ID (auto-generated or manual)
  - Question type dropdown (likert, vas, grid, etc.)
  - Prompt text (multiline)
  - Coding (1 or -1 radio buttons)
  - Type-specific fields (show/hide based on type)

**Question Type Templates:**
- **Likert**: Options list (editable), default: ["Not like me at all", "Not much like me", "Somewhat like me", "Mostly like me", "Very much like me"]
- **VAS**: Min/Max values, Left/Right labels
- **Multi/MultiCheck**: Options list, correct answers (for multi)
- **Grid**: Column headers, row labels
- **Image/ImageResponse**: Image path picker

**Batch Import from Text Dialog:**
When user clicks "Batch Import from Text...", show modal dialog:
1. **Question Text Input**: Large multiline text box
   - One question per line
   - Example: "I finish whatever I begin.\nSetbacks don't discourage me.\nI am a hard worker."
2. **Question Type**: Dropdown (likert, vas, multi, etc.)
3. **Common Options**: For likert/multi - enter the response options used by all questions
   - Example: "Not like me at all|Not much like me|Somewhat like me|Mostly like me|Very much like me"
4. **ID Prefix**: Auto-generate IDs (e.g., "grit" → grit1, grit2, grit3...)
5. **Coding Assignment**: Two modes:
   - **Simple**: All normal (1) or all reverse (-1)
   - **Individual**: Show list of questions with checkboxes to mark reverse-coded items
6. **Preview**: Show first 3 questions as they'll be created
7. **Create** button: Generate all questions at once

This allows creating 30 identical-format questions in seconds rather than minutes.

**CSV Import:**
When user clicks "Import from CSV...", show file picker and import dialog:

**Expected CSV Format:**
```csv
id,prompt,type,coding,options,dimension
grit1,I finish whatever I begin.,likert,1,"Not like me at all|Not much like me|Somewhat like me|Mostly like me|Very much like me",PERSEVERANCE
grit2,Setbacks don't discourage me.,likert,1,"Not like me at all|Not much like me|Somewhat like me|Mostly like me|Very much like me",PERSEVERANCE
grit3,I am a hard worker.,likert,-1,"Not like me at all|Not much like me|Somewhat like me|Mostly like me|Very much like me",PERSEVERANCE
```

**CSV Import Features:**
- Auto-detect delimiter (comma, tab, semicolon)
- Header row detection
- Column mapping (allow user to map CSV columns to question fields)
- Preview table showing first 5 rows
- Validation before import
- Option to append or replace existing questions

**CSV Import Dialog Flow:**
1. File picker → select CSV file
2. Parse and show preview table
3. Column mapping interface:
   - "Question Text" column: dropdown to select which CSV column
   - "Question ID" column: dropdown (or auto-generate)
   - "Coding" column: dropdown (or default to 1)
   - "Question Type" column: dropdown (or default to likert)
   - "Options" column: dropdown (for likert/multi types)
   - "Dimension" column: dropdown (optional)
4. Import options:
   - Append to existing questions
   - Replace all questions
   - Auto-number IDs if missing
5. Preview imported questions
6. Import button

#### Dimensions Tab
- List of dimensions
- Add/Remove buttons
- Dimension editor:
  - Dimension name (e.g., "GRIT_PERSEVERANCE")
  - Description
  - Question selector (multi-select from available questions)

#### Scoring Tab
- Scoring type dropdown: mean_coded, sum_coded, weighted_sum, sum_correct
- For weighted_sum: Question ID -> Weight mapping

#### Translations Tab
- Language selector
- Key-Value editor (similar to existing translation editor)
- Import/Export buttons for translation files

### 4. Implementation Phases

#### Phase 1: Data Model (Week 1)
- [ ] Create `ScaleDefinition.h` / `.cpp`
- [ ] Create `ScaleManager.h` / `.cpp`
- [ ] Implement JSON serialization using nlohmann::json
- [ ] Write unit tests for data model
- [ ] Integration with existing `scale-validator.pbl`

**Files to create:**
- `src/apps/launcher/ScaleDefinition.h`
- `src/apps/launcher/ScaleDefinition.cpp`
- `src/apps/launcher/ScaleManager.h`
- `src/apps/launcher/ScaleManager.cpp`

**Dependencies:**
- `json.hpp` (already used in launcher)
- Standard C++ filesystem library

#### Phase 2: Basic UI Shell (Week 1-2)
- [ ] Add Scale Builder menu item
- [ ] Create main Scale Builder window
- [ ] Implement scale list panel
- [ ] Add New/Load/Delete scale operations
- [ ] Create tab structure

**Files to modify:**
- `src/apps/launcher/LauncherUI.h` - Add member variables and method declarations
- `src/apps/launcher/LauncherUI.cpp` - Add menu item and window rendering

**New state variables in `LauncherUI`:**
```cpp
private:
    bool mShowScaleBuilder;
    std::shared_ptr<ScaleManager> mScaleManager;
    std::shared_ptr<ScaleDefinition> mCurrentScale;
    int mSelectedScaleIndex;
    std::vector<std::string> mScaleList;

    // Scale Builder dialogs
    void ShowScaleBuilder();
    void RenderScaleBuilderMenuBar();
    void RenderScaleList();
    void RenderScaleInfoEditor();
    void RenderQuestionsEditor();
    void RenderDimensionsEditor();
    void RenderScoringEditor();
    void RenderTranslationsEditor();
```

#### Phase 3: Scale Info Editor (Week 2)
- [ ] Implement Scale Info tab with all fields
- [ ] Add validation for required fields
- [ ] Save/Load functionality
- [ ] Dirty state tracking (unsaved changes warning)

**ImGui widgets:**
- `ImGui::InputText()` for code, name, author, version
- `ImGui::InputTextMultiline()` for citation, description
- Color coding for validation errors

#### Phase 4: Questions Editor (Week 2-3)
- [ ] Question list with add/remove/reorder
- [ ] Question type selector
- [ ] Generic question fields (ID, prompt, coding)
- [ ] Type-specific field rendering
- [ ] Question templates for common types
- [ ] Drag-and-drop reordering
- [ ] **Batch Import from Text Dialog**
  - [ ] Multiline text input for question list
  - [ ] Common options configuration
  - [ ] ID prefix and auto-numbering
  - [ ] Coding assignment (individual or bulk)
  - [ ] Preview before creation
  - [ ] Parse and validate input
  - [ ] Create all questions at once
- [ ] **CSV Import Dialog**
  - [ ] CSV file picker
  - [ ] CSV parser (comma, tab, semicolon delimiters)
  - [ ] Preview table
  - [ ] Column mapping interface
  - [ ] Import options (append/replace)
  - [ ] Validation and error reporting
  - [ ] Create questions from CSV data

**Complex widgets:**
- Question type dropdown with descriptions
- Dynamic option list editor (for likert/multi)
- File picker for image questions
- Validation indicators
- **Batch import multi-line parser**

#### Phase 5: Dimensions Editor (Week 3)
- [ ] Dimension list
- [ ] Add/Remove dimensions
- [ ] Question multi-selector
- [ ] Visual indication of dimension coverage
- [ ] Drag questions between dimensions

**Widgets:**
- Multi-select list with checkboxes
- Visual coverage indicator (show which questions are in which dimensions)

#### Phase 6: Scoring Editor (Week 3)
- [ ] Scoring type selector
- [ ] Type-specific configuration
- [ ] Weight editor for weighted_sum
- [ ] Live preview of scoring formula

**Features:**
- Formula visualization
- Validation of weights (must match question IDs)

#### Phase 7: Translations Editor (Week 4)
- [ ] Language selector
- [ ] Auto-extract translatable strings from questions
- [ ] Key-value editor (reuse existing translation editor pattern)
- [ ] Import/Export translation files
- [ ] Completion percentage indicator

**Integration:**
- Reuse `TranslationEditorState` pattern from existing code
- Generate translation keys automatically from question prompts

#### Phase 8: Testing & Validation (Week 4)
- [ ] Integrate with `scale-validator.pbl`
- [ ] Add "Validate" button to menu
- [ ] Show validation results in UI
- [ ] Add "Test Scale" button (launches ScaleRunner)
- [ ] Preview mode (render scale as it will appear)

**Menu actions:**
```cpp
if (ImGui::BeginMenu("File")) {
    if (ImGui::MenuItem("New Scale")) { CreateNewScale(); }
    if (ImGui::MenuItem("Save", "Ctrl+S")) { SaveCurrentScale(); }
    if (ImGui::MenuItem("Export to Battery")) { ExportScale(); }
    ImGui::EndMenu();
}

if (ImGui::BeginMenu("Validate")) {
    if (ImGui::MenuItem("Run Validator")) { RunValidator(); }
    if (ImGui::MenuItem("Test in ScaleRunner")) { TestScale(); }
    ImGui::EndMenu();
}
```

### 5. File Structure

```
src/apps/launcher/
├── ScaleDefinition.h           # Scale data model
├── ScaleDefinition.cpp
├── ScaleManager.h              # Scale file management
├── ScaleManager.cpp
├── LauncherUI.h                # Add scale builder methods
└── LauncherUI.cpp              # Implement scale builder UI
```

### 6. JSON Serialization Example

Using nlohmann::json (already included in launcher):

```cpp
// ScaleDefinition::SaveToFile()
nlohmann::json j;
j["scale_info"] = {
    {"code", mScaleInfo.code},
    {"name", mScaleInfo.name},
    {"citation", mScaleInfo.citation},
    {"description", mScaleInfo.description},
    {"author", mScaleInfo.author},
    {"version", mScaleInfo.version}
};

// Questions array
j["questions"] = nlohmann::json::array();
for (const auto& q : mQuestions) {
    nlohmann::json qj;
    qj["id"] = q.id;
    qj["type"] = q.type;
    qj["prompt"] = q.prompt;
    qj["coding"] = q.coding;

    if (!q.options.empty()) {
        qj["options"] = q.options;
    }

    j["questions"].push_back(qj);
}

// Write to file
std::ofstream file(jsonPath);
file << j.dump(2);  // Pretty print with 2-space indent
```

### 7. Batch Import Implementation Details

#### BatchImportDialog State Structure

```cpp
struct BatchImportState {
    bool show;
    char questionText[8192];      // Multiline input (large buffer)
    char idPrefix[64];            // e.g., "grit", "bfi"
    int questionType;             // Index into question type list
    char commonOptions[1024];     // Pipe-separated: "Option 1|Option 2|Option 3"
    int codingMode;               // 0=all normal, 1=all reverse, 2=individual
    std::vector<bool> individualCoding;  // Per-question coding (true = reverse)
    std::vector<std::string> parsedQuestions;  // Preview

    BatchImportState() : show(false), questionType(0), codingMode(0) {
        questionText[0] = '\0';
        idPrefix[0] = '\0';
        commonOptions[0] = '\0';
    }

    void Parse() {
        parsedQuestions.clear();
        individualCoding.clear();

        std::string text(questionText);
        std::stringstream ss(text);
        std::string line;

        while (std::getline(ss, line)) {
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);

            if (!line.empty()) {
                parsedQuestions.push_back(line);
                individualCoding.push_back(false);  // Default to normal coding
            }
        }
    }
};
```

#### Batch Import Dialog Rendering

```cpp
void LauncherUI::ShowBatchImportDialog()
{
    if (!mBatchImport.show) return;

    ImGui::SetNextWindowSize(ImVec2(900, 700), ImGuiCond_FirstUseEver);
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::Begin("Batch Import Questions", &mBatchImport.show))
    {
        ImGui::TextWrapped("Paste your questions below (one per line). All questions will use the same type and response options.");
        ImGui::Separator();
        ImGui::Spacing();

        // Step 1: Question text input
        ImGui::Text("Questions (one per line):");
        ImGui::InputTextMultiline("##QuestionText", mBatchImport.questionText,
                                  sizeof(mBatchImport.questionText),
                                  ImVec2(-1, 200));

        ImGui::Spacing();

        // Step 2: Question type
        ImGui::Text("Question Type:");
        const char* questionTypes[] = {"Likert", "VAS", "Multiple Choice", "Multiple Check", "Short Answer", "Long Answer"};
        ImGui::Combo("##QuestionType", &mBatchImport.questionType, questionTypes, IM_ARRAYSIZE(questionTypes));

        ImGui::Spacing();

        // Step 3: Common options (for likert/multi)
        if (mBatchImport.questionType <= 3) {  // Likert, VAS, Multi, MultiCheck
            ImGui::Text("Response Options (separate with |):");
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Example: Not at all|A little|Somewhat|Very much|Extremely");
            }
            ImGui::InputText("##CommonOptions", mBatchImport.commonOptions,
                           sizeof(mBatchImport.commonOptions));
        }

        ImGui::Spacing();

        // Step 4: ID prefix
        ImGui::Text("Question ID Prefix:");
        ImGui::InputText("##IDPrefix", mBatchImport.idPrefix, sizeof(mBatchImport.idPrefix));
        ImGui::SameLine();
        ImGui::TextDisabled("(IDs will be: %s1, %s2, %s3...)",
                           mBatchImport.idPrefix[0] ? mBatchImport.idPrefix : "q",
                           mBatchImport.idPrefix[0] ? mBatchImport.idPrefix : "q",
                           mBatchImport.idPrefix[0] ? mBatchImport.idPrefix : "q");

        ImGui::Spacing();

        // Step 5: Coding mode
        ImGui::Text("Coding:");
        ImGui::RadioButton("All Normal (1)", &mBatchImport.codingMode, 0);
        ImGui::SameLine();
        ImGui::RadioButton("All Reverse (-1)", &mBatchImport.codingMode, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Individual", &mBatchImport.codingMode, 2);

        // Individual coding checkboxes
        if (mBatchImport.codingMode == 2 && !mBatchImport.parsedQuestions.empty()) {
            ImGui::Spacing();
            ImGui::Text("Mark reverse-coded items:");
            ImGui::BeginChild("CodingList", ImVec2(0, 150), true);

            for (size_t i = 0; i < mBatchImport.parsedQuestions.size(); i++) {
                std::string label = std::to_string(i + 1) + ". " +
                                   mBatchImport.parsedQuestions[i].substr(0, 60);
                if (mBatchImport.parsedQuestions[i].length() > 60) label += "...";

                ImGui::Checkbox(label.c_str(), &mBatchImport.individualCoding[i]);
            }

            ImGui::EndChild();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Preview
        if (ImGui::Button("Parse & Preview")) {
            mBatchImport.Parse();
        }

        if (!mBatchImport.parsedQuestions.empty()) {
            ImGui::SameLine();
            ImGui::Text("Found %d questions", (int)mBatchImport.parsedQuestions.size());

            ImGui::Spacing();
            ImGui::Text("Preview (first 3):");
            ImGui::BeginChild("Preview", ImVec2(0, 100), true);

            for (size_t i = 0; i < std::min(size_t(3), mBatchImport.parsedQuestions.size()); i++) {
                std::string prefix = mBatchImport.idPrefix[0] ? mBatchImport.idPrefix : "q";
                int coding = (mBatchImport.codingMode == 1) ? -1 :
                           ((mBatchImport.codingMode == 2 && mBatchImport.individualCoding[i]) ? -1 : 1);

                ImGui::TextColored(ImVec4(0.7f, 0.9f, 1.0f, 1.0f),
                                 "%s%d", prefix.c_str(), (int)i + 1);
                ImGui::SameLine();
                ImGui::TextColored(coding == -1 ? ImVec4(1.0f, 0.7f, 0.5f, 1.0f) : ImVec4(0.7f, 1.0f, 0.7f, 1.0f),
                                 "[%s]", coding == -1 ? "REVERSE" : "NORMAL");
                ImGui::SameLine();
                ImGui::Text("%s", mBatchImport.parsedQuestions[i].c_str());
            }

            if (mBatchImport.parsedQuestions.size() > 3) {
                ImGui::Text("... and %d more", (int)mBatchImport.parsedQuestions.size() - 3);
            }

            ImGui::EndChild();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Action buttons
        if (ImGui::Button("Create Questions", ImVec2(150, 0))) {
            if (CreateQuestionsFromBatchImport()) {
                mBatchImport.show = false;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            mBatchImport.show = false;
        }
    }
    ImGui::End();
}

bool LauncherUI::CreateQuestionsFromBatchImport()
{
    if (!mCurrentScale) return false;
    if (mBatchImport.parsedQuestions.empty()) {
        // Show error: no questions parsed
        return false;
    }

    std::string prefix = mBatchImport.idPrefix[0] ? mBatchImport.idPrefix : "q";

    // Parse common options
    std::vector<std::string> options;
    if (mBatchImport.commonOptions[0]) {
        std::string optionsStr(mBatchImport.commonOptions);
        std::stringstream ss(optionsStr);
        std::string option;
        while (std::getline(ss, option, '|')) {
            // Trim whitespace
            option.erase(0, option.find_first_not_of(" \t"));
            option.erase(option.find_last_not_of(" \t") + 1);
            if (!option.empty()) {
                options.push_back(option);
            }
        }
    }

    // Create questions
    auto& questions = mCurrentScale->GetQuestions();
    int startNum = questions.size() + 1;  // Continue numbering from existing questions

    for (size_t i = 0; i < mBatchImport.parsedQuestions.size(); i++) {
        ScaleDefinition::Question q;
        q.id = prefix + std::to_string(startNum + i);
        q.prompt = mBatchImport.parsedQuestions[i];

        // Set type based on dropdown
        const char* typeNames[] = {"likert", "vas", "multi", "multicheck", "short", "long"};
        q.type = typeNames[mBatchImport.questionType];

        // Set coding
        if (mBatchImport.codingMode == 1) {
            q.coding = -1;  // All reverse
        } else if (mBatchImport.codingMode == 2) {
            q.coding = mBatchImport.individualCoding[i] ? -1 : 1;
        } else {
            q.coding = 1;  // All normal
        }

        // Set options for types that need them
        if (mBatchImport.questionType <= 3) {
            q.options = options;
        }

        questions.push_back(q);
    }

    // Clear batch import state
    mBatchImport.questionText[0] = '\0';
    mBatchImport.idPrefix[0] = '\0';
    mBatchImport.commonOptions[0] = '\0';
    mBatchImport.parsedQuestions.clear();
    mBatchImport.individualCoding.clear();

    return true;
}
```

#### CSV Import Implementation

```cpp
struct CSVImportState {
    bool show;
    std::string filePath;
    std::vector<std::vector<std::string>> csvData;  // Raw CSV data
    std::vector<std::string> headers;               // Column headers

    // Column mappings (index into headers, -1 = not mapped)
    int questionTextColumn;
    int questionIDColumn;
    int codingColumn;
    int typeColumn;
    int optionsColumn;
    int dimensionColumn;

    char delimiter;              // ',' '\t' or ';'
    bool hasHeaders;
    int importMode;              // 0=append, 1=replace
    char defaultOptions[1024];   // Default options if not in CSV

    CSVImportState() : show(false), delimiter(','), hasHeaders(true),
                      importMode(0), questionTextColumn(0),
                      questionIDColumn(-1), codingColumn(-1),
                      typeColumn(-1), optionsColumn(-1),
                      dimensionColumn(-1) {
        defaultOptions[0] = '\0';
    }

    void ParseCSV() {
        csvData.clear();
        headers.clear();

        std::ifstream file(filePath);
        if (!file.is_open()) return;

        std::string line;
        bool firstRow = true;

        while (std::getline(file, line)) {
            std::vector<std::string> row;
            std::stringstream ss(line);
            std::string cell;

            while (std::getline(ss, cell, delimiter)) {
                // Handle quoted cells (CSV standard)
                if (!cell.empty() && cell[0] == '"') {
                    cell = cell.substr(1);
                    while (cell.back() != '"' && std::getline(ss, cell, delimiter)) {
                        // Continue reading until closing quote
                    }
                    if (!cell.empty() && cell.back() == '"') {
                        cell.pop_back();
                    }
                }
                row.push_back(cell);
            }

            if (firstRow && hasHeaders) {
                headers = row;
                firstRow = false;
            } else {
                csvData.push_back(row);
            }
        }

        file.close();
    }
};

void LauncherUI::ShowCSVImportDialog()
{
    if (!mCSVImport.show) return;

    ImGui::SetNextWindowSize(ImVec2(1000, 750), ImGuiCond_FirstUseEver);
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::Begin("Import Questions from CSV", &mCSVImport.show))
    {
        // File picker
        if (mCSVImport.filePath.empty()) {
            ImGui::Text("Select CSV file:");
            if (ImGui::Button("Browse...")) {
                std::string path = OpenFileDialog("Select CSV File", "*.csv;*.tsv;*.txt");
                if (!path.empty()) {
                    mCSVImport.filePath = path;
                    mCSVImport.ParseCSV();
                }
            }
        } else {
            ImGui::Text("File: %s", mCSVImport.filePath.c_str());
            ImGui::SameLine();
            if (ImGui::Button("Change File")) {
                mCSVImport.filePath.clear();
            }

            ImGui::Separator();

            // CSV options
            ImGui::Checkbox("First row contains headers", &mCSVImport.hasHeaders);
            ImGui::SameLine();
            if (ImGui::Button("Re-parse")) {
                mCSVImport.ParseCSV();
            }

            ImGui::Spacing();

            // Column mapping
            ImGui::Text("Column Mapping:");
            ImGui::Separator();

            const char** headerNames = new const char*[mCSVImport.headers.size() + 1];
            headerNames[0] = "(none)";
            for (size_t i = 0; i < mCSVImport.headers.size(); i++) {
                headerNames[i + 1] = mCSVImport.headers[i].c_str();
            }

            ImGui::Combo("Question Text", &mCSVImport.questionTextColumn, headerNames, mCSVImport.headers.size() + 1);
            ImGui::Combo("Question ID", &mCSVImport.questionIDColumn, headerNames, mCSVImport.headers.size() + 1);
            ImGui::Combo("Coding (1/-1)", &mCSVImport.codingColumn, headerNames, mCSVImport.headers.size() + 1);
            ImGui::Combo("Question Type", &mCSVImport.typeColumn, headerNames, mCSVImport.headers.size() + 1);
            ImGui::Combo("Options", &mCSVImport.optionsColumn, headerNames, mCSVImport.headers.size() + 1);
            ImGui::Combo("Dimension", &mCSVImport.dimensionColumn, headerNames, mCSVImport.headers.size() + 1);

            delete[] headerNames;

            ImGui::Spacing();

            // Default options if not in CSV
            if (mCSVImport.optionsColumn == -1) {
                ImGui::Text("Default Options (for all questions):");
                ImGui::InputText("##DefaultOptions", mCSVImport.defaultOptions,
                               sizeof(mCSVImport.defaultOptions));
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Separate with | (pipe)");
                }
            }

            ImGui::Spacing();

            // Import mode
            ImGui::Text("Import Mode:");
            ImGui::RadioButton("Append to existing questions", &mCSVImport.importMode, 0);
            ImGui::RadioButton("Replace all questions", &mCSVImport.importMode, 1);

            ImGui::Spacing();
            ImGui::Separator();

            // Preview
            ImGui::Text("Preview (%d rows):", (int)mCSVImport.csvData.size());
            ImGui::BeginChild("CSVPreview", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar);

            if (ImGui::BeginTable("PreviewTable", mCSVImport.headers.size(),
                                 ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                // Headers
                for (const auto& header : mCSVImport.headers) {
                    ImGui::TableSetupColumn(header.c_str());
                }
                ImGui::TableHeadersRow();

                // Data (first 10 rows)
                for (size_t i = 0; i < std::min(size_t(10), mCSVImport.csvData.size()); i++) {
                    ImGui::TableNextRow();
                    for (size_t j = 0; j < mCSVImport.csvData[i].size(); j++) {
                        ImGui::TableNextColumn();
                        ImGui::Text("%s", mCSVImport.csvData[i][j].c_str());
                    }
                }

                if (mCSVImport.csvData.size() > 10) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("... and %d more rows", (int)mCSVImport.csvData.size() - 10);
                }

                ImGui::EndTable();
            }

            ImGui::EndChild();

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // Action buttons
            if (ImGui::Button("Import", ImVec2(120, 0))) {
                if (CreateQuestionsFromCSV()) {
                    mCSVImport.show = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                mCSVImport.show = false;
            }
        }
    }
    ImGui::End();
}
```

#### Example Usage Scenarios

**Scenario 1: Creating Grit Scale from Text (12 items):**

1. Click "Batch Import from Text..."
2. Paste all 12 questions:
   ```
   I finish whatever I begin.
   Setbacks don't discourage me.
   I am a hard worker.
   I am diligent.
   ...
   ```
3. Select "Likert" type
4. Enter options: `Not like me at all|Not much like me|Somewhat like me|Mostly like me|Very much like me`
5. Set ID prefix: `grit`
6. Select "Individual" coding
7. Check boxes for reverse-coded items (grit1, grit3, grit5, grit6, grit9, grit11)
8. Click "Create Questions"
9. Result: 12 questions created instantly with IDs grit1-grit12, correct coding

**Scenario 2: Importing Big Five Inventory from CSV (44 items):**

1. Click "Import from CSV..."
2. Select `bfi_items.csv` from file picker
3. Preview shows:
   ```
   | id   | text                                | valence | subscale     |
   |------|-------------------------------------|---------|--------------|
   | bfi1 | Is talkative                        | 1       | Extraversion |
   | bfi2 | Tends to find fault with others     | -1      | Agreeableness|
   | bfi3 | Does a thorough job                 | 1       | Conscientiousness|
   ```
4. Column mapping:
   - Question Text → "text"
   - Question ID → "id"
   - Coding → "valence"
   - Question Type → "likert" (default)
   - Dimension → "subscale"
5. Enter common options: `Disagree strongly|Disagree a little|Neither agree nor disagree|Agree a little|Agree strongly`
6. Select "Append to existing questions"
7. Click "Import"
8. Result: 44 questions imported with proper IDs, coding, and dimension assignments

### 8. Integration with Existing Tools

#### scale-validator.pbl
Call from C++ using `ExperimentRunner` pattern:

```cpp
bool ScaleDefinition::Validate(std::string& errorOutput)
{
    // Save to temp file first
    std::string tempPath = GetWorkspaceTempDirectory() + "/temp_scale.json";
    if (!SaveToFile(tempPath)) {
        errorOutput = "Failed to save scale for validation";
        return false;
    }

    // Run validator
    std::string peblPath = mConfig->GetPEBLPath();
    std::string validatorPath = GetBatteryPath() + "/scales/scale-validator.pbl";
    std::string command = peblPath + " " + validatorPath + " -v " + mScaleInfo.code;

    // Execute and capture output
    ExperimentRunner validator(command, GetWorkingDirectory());
    validator.WaitForCompletion();

    errorOutput = validator.GetStdout() + validator.GetStderr();
    return validator.GetExitCode() == 0;
}
```

#### ScaleRunner Testing
Add button to launch scale in ScaleRunner:

```cpp
void LauncherUI::TestCurrentScale()
{
    if (!mCurrentScale) return;

    // Save first
    if (!mCurrentScale->SaveToFile()) {
        // Show error dialog
        return;
    }

    // Build command: pebl2 ScaleRunner.pbl -v scale=SCALECODE
    std::string command = mConfig->GetPEBLPath() +
                         " " + GetBatteryPath() + "/scales/ScaleRunner.pbl" +
                         " -v scale=" + mCurrentScale->GetScaleInfo().code +
                         " --windowed";

    // Run in new window
    ExperimentRunner* runner = new ExperimentRunner(command, GetWorkingDirectory());
    runner->Start();
}
```

### 8. User Workflow

1. **Create New Scale**
   - Tools → Scale Builder
   - Click "New Scale"
   - Enter scale code (e.g., "bfi")
   - Fill in Scale Info tab

2. **Add Questions**
   - Switch to Questions tab
   - Click "Add Question"
   - Select question type from dropdown
   - Enter prompt and options
   - Set coding (normal or reverse)

3. **Define Dimensions**
   - Switch to Dimensions tab
   - Add dimension (e.g., "Extraversion")
   - Select questions that belong to this dimension

4. **Configure Scoring**
   - Switch to Scoring tab
   - Select scoring method
   - Configure weights if needed

5. **Add Translations**
   - Switch to Translations tab
   - Select target language
   - Translate all prompts and options

6. **Validate & Test**
   - Menu → Validate → Run Validator
   - Fix any errors shown
   - Menu → Validate → Test in ScaleRunner
   - Verify scale runs correctly

7. **Export to Battery**
   - Menu → File → Export to Battery
   - Creates files in `battery/scales/definitions/` and `battery/scales/translations/`

### 9. Benefits of This Approach

- **Integrated**: No need to switch between launcher and external tools
- **Visual**: See scale structure at a glance
- **Validated**: Real-time validation feedback
- **Testable**: One-click testing in ScaleRunner
- **Modern**: Uses Dear ImGui, same as rest of launcher
- **Maintainable**: Follows existing launcher code patterns
- **Cross-platform**: Works on Windows, Linux, macOS like rest of PEBL

### 10. Future Enhancements

- **Visual question preview**: Show how each question will look
- **Scale templates**: Pre-built templates for common scale types
- **Question bank**: Reusable question library
- **Import from PsyToolkit**: Parse PsyToolkit format
- **Import from Qualtrics**: Parse Qualtrics QSF files
- **Collaboration**: Export scale as shareable bundle
- **Version control**: Track changes to scale definitions
- **Scale library browser**: Browse and import from online repository

## Next Steps

1. Start with Phase 1 (Data Model) - get the C++ classes working first
2. Add basic JSON load/save functionality
3. Create simple UI shell to test data model
4. Gradually add editor tabs following the phases
5. Integrate validation and testing
6. Polish UI and add keyboard shortcuts
