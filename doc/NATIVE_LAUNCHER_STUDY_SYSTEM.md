# Native Launcher Study Management System

## Overview

The PEBL native launcher (C++ compiled version) implements a study-based organizational structure that aligns with the online platform. This enables bidirectional workflows: create studies natively and upload them, or download snapshots from online and use them directly.

## Key Design Principles

1. **Study-level organization** - Studies are the primary organizational unit containing tests and chains
2. **Self-contained studies** - Each study directory contains everything needed to run it
3. **Snapshot format = Native format** - Same JSON structure for local and online
4. **Read-only battery** - Installed tests stay in Program Files, copied into studies as needed
5. **Clean exports** - Snapshots exclude participant data for sharing/uploading

## Directory Structure

### Installation Directories (Read-Only)

```
C:\Program Files (x86)\PEBL2\
├── bin\
│   ├── pebl2.exe              # PEBL interpreter
│   └── launcher.exe           # Study launcher/manager
├── battery\                   # Pre-built battery tests (read-only)
│   ├── BART\
│   ├── corsi\
│   ├── stroop\
│   ├── iowa\
│   └── ... (100+ tests)
├── pebl-lib\                  # Standard library
│   ├── ChainPage.pbl          # For instruction/consent/completion pages
│   ├── Utility.pbl
│   ├── UI.pbl
│   └── ...
├── demo\                      # Demo .pbl files (to be copied)
├── tutorial\                  # Tutorial files (to be copied)
├── documentation\             # Documentation (to be copied)
└── doc\
    └── manual.pdf             # User manual (to be copied)
```

### User Workspace

```
Documents\pebl-exp.2.3\
├── manual.pdf                 # Copied from installation
├── README.txt                 # First-run welcome/explanation
├── documentation\             # Copied from installation
├── demo\                      # Demo files copied from installation
├── tutorial\                  # Tutorial files copied from installation
├── my_studies\                # User's working studies
│   ├── my-cognitive-battery\  # Example study
│   │   ├── study-info.json    # Study metadata
│   │   ├── chains\
│   │   │   ├── main-chain.json
│   │   │   └── practice-chain.json
│   │   └── tests\             # Tests copied from battery
│   │       ├── BART\
│   │       │   ├── BART.pbl
│   │       │   ├── params\
│   │       │   │   └── BART.pbl.schema.json
│   │       │   ├── translations\
│   │       │   │   ├── BART.pbl-en.json
│   │       │   │   └── BART.pbl-es.json
│   │       │   └── data\      # Participant data (excluded from snapshots)
│   │       │       ├── 101\
│   │       │       │   └── BART-101.csv
│   │       │       └── 102\
│   │       └── corsi\
│   │           ├── corsi.pbl
│   │           └── data\
│   └── adhd-assessment\       # Another study
│       ├── study-info.json
│       ├── chains\
│       └── tests\
├── snapshots\                 # Clean study exports (no data)
│   ├── my-cognitive-battery_v1_2026-01-09\
│   │   ├── study-info.json
│   │   ├── chains\
│   │   │   ├── main-chain.json
│   │   │   └── practice-chain.json
│   │   └── tests\             # Tests WITHOUT data folders
│   │       ├── BART\
│   │       │   ├── BART.pbl
│   │       │   ├── params\
│   │       │   └── translations\
│   │       └── corsi\
│   ├── my-cognitive-battery_v1_2026-01-09.zip
│   └── adhd-assessment_v2_2026-01-05.zip
└── data\                      # Legacy: for backward compatibility
```

## File Formats

### study-info.json

Located at the root of each study directory.

```json
{
  "study_name": "My Cognitive Battery",
  "description": "Comprehensive cognitive assessment battery",
  "version": 1,
  "created_date": "2026-01-09T10:30:00Z",
  "modified_date": "2026-01-09T15:45:00Z",
  "author": "Lab Name",
  "study_token": "",
  "tests": [
    {
      "test_name": "BART",
      "test_path": "tests/BART",
      "included": true,
      "parameter_variants": {
        "default": {
          "description": "Standard BART parameters",
          "file": null
        },
        "large": {
          "description": "Large balloon version",
          "file": "params/BART-large.par.json"
        }
      }
    },
    {
      "test_name": "corsi",
      "test_path": "tests/corsi",
      "included": true,
      "parameter_variants": {
        "default": {
          "description": "Standard Corsi parameters",
          "file": null
        }
      }
    }
  ]
}
```

**Field Descriptions:**
- `study_name` - Display name for the study
- `description` - Study description/purpose
- `version` - Integer version number (increment on significant changes)
- `created_date` - ISO 8601 timestamp
- `modified_date` - ISO 8601 timestamp
- `author` - Creator/lab name
- `study_token` - Empty for native studies; platform generates on upload
- `tests` - Array of included tests with parameter variants

### chains/*.json

Chain files define test sequences with instruction/consent/completion pages.

**File naming:** `chains/chain-name.json`

```json
{
  "chain_name": "main-chain",
  "description": "Main experimental chain with consent and instructions",
  "items": [
    {
      "item_type": "instruction",
      "title": "Welcome to the Study",
      "content": "In this study, you will complete several cognitive tasks.\n\nPlease read all instructions carefully.\n\nPress OK when ready to continue."
    },
    {
      "item_type": "consent",
      "title": "Informed Consent",
      "content": "By participating in this study, you agree to:\n\n- Complete all tasks to the best of your ability\n- Allow anonymous data collection\n- Follow all instructions\n\nPress OK to consent and continue."
    },
    {
      "item_type": "test",
      "test_name": "BART",
      "param_variant": "default",
      "language": "en",
      "title": "Balloon Task"
    },
    {
      "item_type": "test",
      "test_name": "corsi",
      "param_variant": "default",
      "language": "en",
      "title": "Memory Span Task"
    },
    {
      "item_type": "completion",
      "title": "Thank You",
      "content": "You have completed the study.\n\nThank you for your participation!\n\nYour responses have been recorded."
    }
  ]
}
```

**Item Types:**

1. **instruction** - Informational page before study/section
   - `title` - Page title
   - `content` - Text content (supports `\n` for line breaks)

2. **consent** - Consent form page
   - `title` - Page title
   - `content` - Consent text

3. **completion** - Thank you/completion page
   - `title` - Page title
   - `content` - Completion message

4. **test** - Execute a test
   - `test_name` - Name of test (must match test in study-info.json)
   - `param_variant` - Which parameter variant to use (from study-info.json)
   - `language` - Language code (e.g., "en", "es", "de")
   - `title` - Display name for this chain item

## First-Run Initialization

When launcher starts, check if `Documents/pebl-exp.2.3` exists. If not, run first-time setup.

### First-Time Setup Routine

```cpp
bool IsFirstRun() {
    std::string workspacePath = GetDocumentsPath() + "/pebl-exp.2.3";
    return !DirectoryExists(workspacePath);
}

void RunFirstTimeSetup() {
    std::string workspacePath = GetDocumentsPath() + "/pebl-exp.2.3";
    std::string installPath = GetInstallationPath();  // e.g., C:\Program Files (x86)\PEBL2

    // 1. Create directory structure
    CreateDirectory(workspacePath);
    CreateDirectory(workspacePath + "/my_studies");
    CreateDirectory(workspacePath + "/snapshots");
    CreateDirectory(workspacePath + "/documentation");
    CreateDirectory(workspacePath + "/demo");
    CreateDirectory(workspacePath + "/tutorial");
    CreateDirectory(workspacePath + "/data");  // Legacy compatibility

    // 2. Copy resources from installation
    CopyFile(installPath + "/doc/manual.pdf", workspacePath + "/manual.pdf");
    CopyDirectory(installPath + "/documentation", workspacePath + "/documentation");
    CopyDirectory(installPath + "/demo", workspacePath + "/demo");
    CopyDirectory(installPath + "/tutorial", workspacePath + "/tutorial");

    // 3. Create README.txt
    CreateWelcomeReadme(workspacePath + "/README.txt");

    // 4. Show welcome dialog (optional)
    ShowWelcomeDialog();
}

void CreateWelcomeReadme(const std::string& path) {
    std::ofstream readme(path);
    readme << "Welcome to PEBL 2.3!\n\n";
    readme << "Your workspace has been created at: " << GetDocumentsPath() << "/pebl-exp.2.3\n\n";
    readme << "Directory Structure:\n";
    readme << "  my_studies/    - Create and manage your research studies here\n";
    readme << "  snapshots/     - Export studies as shareable snapshots\n";
    readme << "  demo/          - Demo PEBL scripts to learn from\n";
    readme << "  tutorial/      - Tutorial files and examples\n";
    readme << "  documentation/ - Additional documentation\n";
    readme << "  manual.pdf     - Complete PEBL user manual\n\n";
    readme << "Getting Started:\n";
    readme << "  1. Launch the PEBL Launcher\n";
    readme << "  2. Create a new study: Studies > New Study\n";
    readme << "  3. Add tests from the battery\n";
    readme << "  4. Create test chains\n";
    readme << "  5. Run your study!\n\n";
    readme << "For help, see manual.pdf or visit pebl.sourceforge.net\n";
    readme.close();
}
```

### Version Migration (Future)

When `pebl-exp.2.2` exists but `pebl-exp.2.3` doesn't:

```cpp
void MigrateFromPreviousVersion() {
    std::string oldPath = GetDocumentsPath() + "/pebl-exp.2.2";
    std::string newPath = GetDocumentsPath() + "/pebl-exp.2.3";

    // Run normal first-time setup
    RunFirstTimeSetup();

    // Check for my_studies in old version
    if (DirectoryExists(oldPath + "/my_studies")) {
        // Ask user if they want to migrate studies
        if (ShowMigrationDialog()) {
            CopyDirectory(oldPath + "/my_studies", newPath + "/my_studies");
        }
    }
}
```

## Study Management Workflows

### 1. Creating a New Study

**User Action:** Studies → New Study

**Process:**
1. Show dialog: Enter study name, description, author
2. Create directory: `my_studies/study-name/`
3. Create empty `study-info.json`:
   ```json
   {
     "study_name": "User Entered Name",
     "description": "User entered description",
     "version": 1,
     "created_date": "2026-01-09T10:30:00Z",
     "modified_date": "2026-01-09T10:30:00Z",
     "author": "User entered author",
     "study_token": "",
     "tests": []
   }
   ```
4. Create empty directories:
   - `my_studies/study-name/chains/`
   - `my_studies/study-name/tests/`
5. Open study in Study Editor

### 2. Adding Tests to Study

**User Action:** In Study Editor → Add Test

**Process:**
1. Show battery browser (read-only view of `C:\Program Files\PEBL2\battery\`)
2. User selects test (e.g., BART)
3. Copy entire test directory: `battery/BART/` → `my_studies/study-name/tests/BART/`
4. Add test entry to `study-info.json`:
   ```json
   {
     "test_name": "BART",
     "test_path": "tests/BART",
     "included": true,
     "parameter_variants": {
       "default": {
         "description": "Default parameters",
         "file": null
       }
     }
   }
   ```
5. Scan `tests/BART/params/` for `.schema.json` to populate available parameter variants
6. Refresh study view

### 3. Creating a Chain

**User Action:** In Study Editor → New Chain

**Process:**
1. Show dialog: Enter chain name, description
2. Create `chains/chain-name.json` with empty items array
3. Open Chain Editor
4. User adds items:
   - **Add Instruction Page:** Button → Dialog for title/content → Creates instruction item
   - **Add Consent Page:** Button → Dialog for title/content → Creates consent item
   - **Add Test:** Dropdown of study's tests → Select variant/language → Creates test item
   - **Add Completion Page:** Button → Dialog for title/content → Creates completion item
5. Save chain JSON
6. Chain appears in study's chain list

### 4. Running a Chain

**User Action:** In Study → Select Chain → Run

**Process:**
1. Load chain JSON
2. Prompt for subject number/ID
3. For each item in chain:
   - **If instruction/consent/completion:**
     - Create temporary JSON file: `temp-chainpage-{timestamp}.json`
       ```json
       {
         "title": "Page Title",
         "content": "Page content...",
         "page_type": "instruction"
       }
       ```
     - Execute: `pebl2 ChainPage.pbl -v temp-chainpage-{timestamp}.json`
     - Delete temp file after completion
   - **If test:**
     - Build command: `pebl2 tests/{test_name}/{test_name}.pbl -v subnum={id}`
     - If param_variant != "default": `-v pfile=params/{variant}.par.json`
     - If language specified: `-v language={lang}`
     - Execute test
     - Wait for completion
4. Show completion message
5. Data saved in `my_studies/study-name/tests/{test_name}/data/{subject_id}/`

### 5. Editing Study Metadata

**User Action:** In Study Editor → Edit Metadata

**Process:**
1. Load current `study-info.json`
2. Show dialog with editable fields:
   - Study name
   - Description
   - Author
3. Update JSON fields
4. Increment `modified_date`
5. Save `study-info.json`

### 6. Managing Parameter Variants

**User Action:** In Study Editor → Select Test → Manage Variants

**Process:**
1. Show current variants from `study-info.json`
2. User actions:
   - **Add Variant:** Name + Description → Creates entry, opens parameter editor
   - **Edit Variant:** Opens parameter editor (see Parameter Editor section)
   - **Delete Variant:** Removes from list (confirm if used in chains)
3. Save updated `study-info.json`

## Snapshot Creation and Import

### Creating a Snapshot

**User Action:** In Study → Export Snapshot

**Process:**
1. Show dialog:
   - Snapshot name (default: `{study-name}_v{version}_{date}`)
   - Destination: `snapshots/` (default)
   - Options:
     - [ ] Create ZIP file
     - [ ] Increment version number
2. Create snapshot directory: `snapshots/{snapshot-name}/`
3. Copy study structure, **excluding data directories**:
   ```cpp
   void CreateSnapshot(Study& study, std::string snapshotPath) {
       // Copy study-info.json
       CopyFile(study.path + "/study-info.json",
                snapshotPath + "/study-info.json");

       // Copy chains directory
       CopyDirectory(study.path + "/chains",
                     snapshotPath + "/chains");

       // Copy tests, excluding data folders
       CreateDirectory(snapshotPath + "/tests");
       for (auto& test : study.tests) {
           std::string testSrc = study.path + "/" + test.test_path;
           std::string testDst = snapshotPath + "/" + test.test_path;

           CopyDirectoryExcluding(testSrc, testDst, {"data"});
       }
   }
   ```
4. If "Create ZIP": Create `{snapshot-name}.zip` containing snapshot directory
5. If "Increment version": Update study's version number in `study-info.json`
6. Show success message: "Snapshot created at: {path}"

**Directory Copy Exclusion Logic:**
```cpp
void CopyDirectoryExcluding(std::string src, std::string dst,
                           std::vector<std::string> exclude) {
    CreateDirectory(dst);
    for (auto& entry : ListDirectory(src)) {
        std::string name = GetFileName(entry);

        // Skip excluded directories
        if (std::find(exclude.begin(), exclude.end(), name) != exclude.end()) {
            continue;
        }

        std::string srcPath = src + "/" + name;
        std::string dstPath = dst + "/" + name;

        if (IsDirectory(srcPath)) {
            CopyDirectoryExcluding(srcPath, dstPath, exclude);
        } else {
            CopyFile(srcPath, dstPath);
        }
    }
}
```

### Importing a Downloaded Snapshot

**User Action:** Studies → Import Snapshot

**Process:**
1. Show file/directory picker
2. User selects:
   - Snapshot directory (contains `study-info.json`)
   - Or `.zip` file (extract to temp, then process)
3. Validate snapshot:
   - Check for `study-info.json`
   - Check for `chains/` directory
   - Check for `tests/` directory
4. Show import dialog:
   - Study name (from `study-info.json`, editable)
   - Destination: `my_studies/{study-name}` (editable)
   - Conflict handling if study exists:
     - ( ) Overwrite existing study
     - ( ) Create new study with suffix (e.g., `study-name-2`)
     - ( ) Cancel
5. Copy snapshot to `my_studies/{study-name}/`
6. Create empty `data/` directories in each test:
   ```cpp
   for (auto& test : study.tests) {
       CreateDirectory(study.path + "/" + test.test_path + "/data");
   }
   ```
7. Add to study list
8. Show success message

### Uploading to Online Platform

**User Action:** In Study → Upload to Platform

**Process:**
1. Create temporary snapshot (in temp directory, not `snapshots/`)
2. Create ZIP of snapshot
3. Show upload dialog:
   - Platform URL (default: pebl-exp.2.2 server)
   - User credentials
   - Study visibility settings
4. HTTP POST ZIP file to platform upload endpoint
5. Platform response:
   - Success: Returns new `study_token`
   - Update local `study-info.json` with received `study_token`
6. Show success message with study URL

**Note:** Online platform needs to implement:
- `/api/upload-study` endpoint
- Parse uploaded ZIP
- Import `study-info.json` and `chains/*.json`
- Generate new study token
- Store in database

## UI Requirements

### Main Launcher Window

**Menu Bar:**
```
File
  └─ Open Test...              # File picker for standalone .pbl
  └─ Browse Documentation      # Opens pebl-exp.2.3/documentation/
  └─ Browse Demos              # Opens pebl-exp.2.3/demo/
  └─ Browse Tutorials          # Opens pebl-exp.2.3/tutorial/
  └─ Exit

Studies
  └─ My Studies                # Opens study manager
  └─ New Study                 # Create new study dialog
  └─ Import Snapshot           # Import downloaded snapshot
  └─ Browse Battery            # Read-only battery browser

Help
  └─ User Manual               # Opens pebl-exp.2.3/manual.pdf
  └─ About PEBL
```

### Study Manager View

**Layout:**
```
┌─ My Studies ────────────────────────────────────────────┐
│                                                          │
│  [New Study] [Import Snapshot] [Refresh]                │
│                                                          │
│  ┌────────────────────────────────────────────────────┐ │
│  │ Study Name              Modified      Version      │ │
│  ├────────────────────────────────────────────────────┤ │
│  │ My Cognitive Battery    2026-01-09    v1      [>] │ │
│  │ ADHD Assessment         2026-01-05    v2      [>] │ │
│  │ Memory Study            2025-12-20    v1      [>] │ │
│  └────────────────────────────────────────────────────┘ │
│                                                          │
│  [Open] [Delete] [Export Snapshot] [Upload]             │
│                                                          │
└──────────────────────────────────────────────────────────┘
```

### Study Editor View

**Layout:**
```
┌─ Study: My Cognitive Battery ───────────────────────────┐
│                                                          │
│  Name: My Cognitive Battery                             │
│  Author: Lab Name                                        │
│  Version: 1                                              │
│  [Edit Metadata]                                         │
│                                                          │
│  ┌─ Tests ──────────────────────────────────────────┐   │
│  │                                                   │   │
│  │  [Add Test from Battery]                         │   │
│  │                                                   │   │
│  │  ☑ BART                    [Variants] [Remove]   │   │
│  │  ☑ corsi                   [Variants] [Remove]   │   │
│  │  ☑ stroop                  [Variants] [Remove]   │   │
│  │                                                   │   │
│  └───────────────────────────────────────────────────┘   │
│                                                          │
│  ┌─ Chains ─────────────────────────────────────────┐   │
│  │                                                   │   │
│  │  [New Chain]                                     │   │
│  │                                                   │   │
│  │  main-chain                [Edit] [Run] [Delete] │   │
│  │  practice-chain            [Edit] [Run] [Delete] │   │
│  │                                                   │   │
│  └───────────────────────────────────────────────────┘   │
│                                                          │
│  [Export Snapshot] [Upload to Platform] [Close]         │
│                                                          │
└──────────────────────────────────────────────────────────┘
```

### Chain Editor View

**Layout:**
```
┌─ Chain Editor: main-chain ──────────────────────────────┐
│                                                          │
│  Chain Name: main-chain                                  │
│  Description: Main experimental chain                    │
│                                                          │
│  [Add Instruction] [Add Consent] [Add Test] [Add Completion] │
│                                                          │
│  ┌─ Chain Items ────────────────────────────────────┐   │
│  │                                                   │   │
│  │  1. [📄] Welcome Instructions    [Edit] [Delete] │   │
│  │  2. [📋] Informed Consent         [Edit] [Delete] │   │
│  │  3. [🧪] BART (default)           [Edit] [Delete] │   │
│  │  4. [🧪] corsi (default)          [Edit] [Delete] │   │
│  │  5. [✓] Thank You                 [Edit] [Delete] │   │
│  │                                                   │   │
│  │  [Move Up] [Move Down]                           │   │
│  │                                                   │   │
│  └───────────────────────────────────────────────────┘   │
│                                                          │
│  [Save] [Test Run] [Cancel]                             │
│                                                          │
└──────────────────────────────────────────────────────────┘
```

### Chain Page Dialog (Instruction/Consent/Completion)

**Layout:**
```
┌─ Add Instruction Page ──────────────────────────────────┐
│                                                          │
│  Title: ________________________________________________ │
│         Welcome to the Study                            │
│                                                          │
│  Content:                                                │
│  ┌────────────────────────────────────────────────────┐ │
│  │ In this study, you will complete several cognitive│ │
│  │ tasks.                                             │ │
│  │                                                    │ │
│  │ Please read all instructions carefully.           │ │
│  │                                                    │ │
│  │ Press OK when ready to continue.                  │ │
│  │                                                    │ │
│  │                                                    │ │
│  └────────────────────────────────────────────────────┘ │
│                                                          │
│  Note: Use \n for line breaks                           │
│                                                          │
│  [OK] [Cancel] [Preview]                                │
│                                                          │
└──────────────────────────────────────────────────────────┘
```

### Add Test to Chain Dialog

**Layout:**
```
┌─ Add Test to Chain ─────────────────────────────────────┐
│                                                          │
│  Test: [BART                          ▼]                │
│                                                          │
│  Display Name: _________________________________________ │
│                Balloon Risk Task                        │
│                                                          │
│  Parameter Variant: [default                        ▼]  │
│                                                          │
│  Language: [en ▼]                                        │
│                                                          │
│                                                          │
│  [OK] [Cancel]                                           │
│                                                          │
└──────────────────────────────────────────────────────────┘
```

## Parameter Editor

### Current Issue

The parameter editor currently allows editing the schema file, which is incorrect. The schema should be read-only and used to format the parameter editor UI.

### Correct Behavior

1. **Load schema:** `tests/{test}/params/{test}.pbl.schema.json`
2. **Schema format:**
   ```
   parameter_name|default_value|description
   trials|20|Number of trials
   timeout|5000|Response timeout in ms
   feedback|1|Show feedback (0=no, 1=yes)
   ```
3. **Create parameter editor UI** based on schema
4. **Load existing variant** (if editing): `tests/{test}/params/{variant}.par.json`
5. **User edits values** in generated form
6. **Save to:** `tests/{test}/params/{variant}.par.json`
   ```json
   {
     "trials": 30,
     "timeout": 3000,
     "feedback": 1
   }
   ```

### Parameter Editor UI

**Layout:**
```
┌─ Edit Parameters: BART (large variant) ─────────────────┐
│                                                          │
│  Variant Name: large                                     │
│  Description: Large balloon version                      │
│                                                          │
│  ┌─ Parameters ──────────────────────────────────────┐  │
│  │                                                    │  │
│  │  Trials                                            │  │
│  │  [30________]  (Default: 20)                       │  │
│  │  Number of trials                                  │  │
│  │                                                    │  │
│  │  Timeout (ms)                                      │  │
│  │  [5000______]  (Default: 5000)                     │  │
│  │  Response timeout in milliseconds                  │  │
│  │                                                    │  │
│  │  Show Feedback                                     │  │
│  │  [Yes ▼]       (Default: Yes)                      │  │
│  │  Display feedback after each trial                 │  │
│  │                                                    │  │
│  └────────────────────────────────────────────────────┘  │
│                                                          │
│  [Save] [Reset to Defaults] [Cancel]                    │
│                                                          │
└──────────────────────────────────────────────────────────┘
```

### Schema Parser

```cpp
struct ParameterDefinition {
    std::string name;
    std::string default_value;
    std::string description;
    std::string type;  // Inferred from default_value
};

std::vector<ParameterDefinition> ParseParameterSchema(std::string schemaPath) {
    std::vector<ParameterDefinition> params;
    std::ifstream schema(schemaPath);
    std::string line;

    while (std::getline(schema, line)) {
        if (line.empty() || line[0] == '#') continue;  // Skip empty/comment lines

        std::vector<std::string> parts = Split(line, '|');
        if (parts.size() >= 3) {
            ParameterDefinition param;
            param.name = parts[0];
            param.default_value = parts[1];
            param.description = parts[2];
            param.type = InferType(parts[1]);  // "int", "float", "bool", "string"

            params.push_back(param);
        }
    }

    return params;
}

std::string InferType(const std::string& value) {
    if (value == "0" || value == "1") return "bool";
    if (value.find('.') != std::string::npos) return "float";
    if (std::all_of(value.begin(), value.end(), ::isdigit)) return "int";
    return "string";
}
```

## Chain Execution Engine

### Chain Runner Implementation

```cpp
class ChainRunner {
public:
    ChainRunner(Study& study, Chain& chain, std::string subjectID);
    bool Run();

private:
    Study& study_;
    Chain& chain_;
    std::string subjectID_;
    std::string tempDir_;

    bool ExecuteInstruction(ChainItem& item);
    bool ExecuteConsent(ChainItem& item);
    bool ExecuteCompletion(ChainItem& item);
    bool ExecuteTest(ChainItem& item);

    std::string CreateTempChainPageJSON(ChainItem& item);
    std::string BuildTestCommand(ChainItem& item);
};

bool ChainRunner::Run() {
    // Create temp directory for chain page JSON files
    tempDir_ = CreateTempDirectory();

    for (auto& item : chain_.items) {
        bool success = false;

        switch (item.item_type) {
            case ItemType::Instruction:
                success = ExecuteInstruction(item);
                break;
            case ItemType::Consent:
                success = ExecuteConsent(item);
                break;
            case ItemType::Completion:
                success = ExecuteCompletion(item);
                break;
            case ItemType::Test:
                success = ExecuteTest(item);
                break;
        }

        if (!success) {
            CleanupTempDirectory(tempDir_);
            return false;
        }
    }

    CleanupTempDirectory(tempDir_);
    return true;
}

bool ChainRunner::ExecuteInstruction(ChainItem& item) {
    std::string jsonPath = CreateTempChainPageJSON(item);
    std::string command = BuildChainPageCommand(jsonPath);

    int exitCode = ExecuteCommand(command);

    DeleteFile(jsonPath);
    return (exitCode == 0);
}

std::string ChainRunner::CreateTempChainPageJSON(ChainItem& item) {
    std::string filename = tempDir_ + "/chainpage-" +
                          GenerateUUID() + ".json";

    json pageConfig;
    pageConfig["title"] = item.title;
    pageConfig["content"] = item.content;
    pageConfig["page_type"] = ItemTypeToString(item.item_type);

    std::ofstream out(filename);
    out << pageConfig.dump(2);
    out.close();

    return filename;
}

std::string ChainRunner::BuildTestCommand(ChainItem& item) {
    std::string testPath = study_.path + "/tests/" + item.test_name;
    std::string mainFile = testPath + "/" + item.test_name + ".pbl";

    std::string command = GetPEBLPath() + " " + mainFile;
    command += " -v subnum=" + subjectID_;

    // Add parameter file if not default variant
    if (item.param_variant != "default") {
        Test& test = study_.GetTest(item.test_name);
        auto variant = test.parameter_variants[item.param_variant];
        if (!variant.file.empty()) {
            command += " -v pfile=" + testPath + "/" + variant.file;
        }
    }

    // Add language
    if (!item.language.empty()) {
        command += " -v language=" + item.language;
    }

    // Add custom title if specified
    if (!item.title.empty()) {
        command += " -v title=" + EscapeQuotes(item.title);
    }

    return command;
}

bool ChainRunner::ExecuteTest(ChainItem& item) {
    std::string command = BuildTestCommand(item);
    int exitCode = ExecuteCommand(command);
    return (exitCode == 0);
}
```

## Data Management

### Data Organization

- **Data stays within test directories:** `my_studies/{study}/tests/{test}/data/`
- **Subject-based organization:** `data/{subject_id}/`
- **Test creates data files:** Tests handle their own file naming conventions

### Data Exclusion from Snapshots

When creating snapshots, **all `data/` directories are excluded**. This ensures:
- Clean, shareable study packages
- No accidental participant data leakage
- Smaller snapshot file sizes
- Upload-ready packages

### Data Backup Recommendations

Since data is stored in study directories, users should:
- Regularly backup `pebl-exp.2.3/my_studies/`
- Exclude `snapshots/` from backups (can be regenerated)
- Consider separate data archival strategy

## Implementation Checklist

### Phase 1: Core Infrastructure
- [ ] First-run initialization routine
- [ ] Study directory creation/management
- [ ] study-info.json parser/writer
- [ ] Chain JSON parser/writer
- [ ] Basic UI framework

### Phase 2: Study Management
- [ ] Study manager view
- [ ] Create new study dialog
- [ ] Edit study metadata
- [ ] Battery browser (read-only)
- [ ] Add test to study (copy from battery)
- [ ] Remove test from study

### Phase 3: Chain Management
- [ ] Chain editor UI
- [ ] Add/edit instruction pages
- [ ] Add/edit consent pages
- [ ] Add/edit completion pages
- [ ] Add test to chain
- [ ] Reorder chain items
- [ ] Save chain JSON

### Phase 4: Chain Execution
- [ ] Chain runner engine
- [ ] Temp ChainPage JSON creation
- [ ] ChainPage.pbl integration
- [ ] Test execution with parameters
- [ ] Subject ID management
- [ ] Error handling/recovery

### Phase 5: Snapshot Management
- [ ] Create snapshot (exclude data)
- [ ] ZIP snapshot creation
- [ ] Import snapshot
- [ ] Snapshot browser/manager
- [ ] Version increment logic

### Phase 6: Parameter Editor
- [ ] Schema file parser
- [ ] Dynamic parameter form generation
- [ ] Type inference (int/float/bool/string)
- [ ] Parameter variant management
- [ ] Save .par.json files

### Phase 7: Online Integration
- [ ] Upload snapshot to platform
- [ ] Platform authentication
- [ ] HTTP upload implementation
- [ ] Study token management
- [ ] Download from platform

### Phase 8: Polish
- [ ] Welcome dialog
- [ ] Help system integration
- [ ] Error messages/validation
- [ ] Confirmation dialogs
- [ ] Progress indicators
- [ ] Keyboard shortcuts

## Testing Strategy

### Unit Tests
- Study JSON parsing/writing
- Chain JSON parsing/writing
- Directory operations
- Snapshot creation (data exclusion)
- Parameter schema parsing

### Integration Tests
- First-run initialization
- Study creation workflow
- Chain creation/editing
- Chain execution
- Snapshot export/import

### User Acceptance Tests
- Create study from scratch
- Add battery tests
- Create complex chain
- Run chain with subject
- Export snapshot
- Import snapshot on different machine
- Upload to platform
- Download from platform

## Migration from Old Launcher

Users with existing `.config` files in the old launcher format:

### Option 1: Import Tool
Create one-time import utility:
- Read old `.config` files
- Parse EXPCHAIN entries
- Create new study structure
- Convert chains to JSON format

### Option 2: Legacy Support
Launcher can read both formats:
- Detect file type (`.config` vs `study-info.json`)
- Load accordingly
- Offer to convert old format to new

## Platform Requirements

For bidirectional workflow, online platform needs:

### Upload Endpoint
- Accept ZIP file containing snapshot
- Parse `study-info.json`
- Parse `chains/*.json`
- Import test files
- Generate new `study_token`
- Store in database
- Return study URL and token

### Download Enhancement
- Current snapshot generation already works
- Ensure consistent format with native

### Token Management
- Native studies have empty `study_token`
- Platform generates token on upload
- Platform sends token back to native launcher
- Native launcher updates local `study-info.json`

## Open Questions

1. **Study versioning:** How to handle version conflicts when uploading?
2. **Collaborative editing:** Multiple users working on same study?
3. **Test updates:** If battery test is updated, how to update studies using it?
4. **Data synchronization:** Should data be uploadable separately from study?
5. **Language management:** How to handle translations in native launcher?

## Appendix: Example Snapshot Structure

```
my-cognitive-battery_v1_2026-01-09/
├── study-info.json
├── README.md (auto-generated)
├── chains/
│   ├── main-chain.json
│   └── practice-chain.json
└── tests/
    ├── BART/
    │   ├── BART.pbl
    │   ├── BART.pbl.about.txt
    │   ├── params/
    │   │   ├── BART.pbl.schema.json
    │   │   └── BART-large.par.json
    │   └── translations/
    │       ├── BART.pbl-en.json
    │       └── BART.pbl-es.json
    └── corsi/
        ├── corsi.pbl
        ├── params/
        │   └── corsi.pbl.schema.json
        └── translations/
            └── corsi.pbl-en.json
```

**Note:** No `data/` directories in snapshot!
