# Native PEBL Launcher: upload.json Support Implementation Plan

## Executive Summary

**Goal**: Enable native PEBL launcher to upload data to PEBLOnlinePlatform using the same token-based system as the web version.

**Key Findings**:
1. ✅ **upload.json format already defined** - Used by Emscripten version
2. ✅ **UploadFile() already cross-platform** - Works on native Linux/Windows/macOS
3. ✅ **--upload flag already supported** - No PEBL core changes needed
4. ✅ **Snapshots already include upload.json** - One per test, pre-configured
5. ✅ **Participant code NOT an issue** - UploadFile() uses gSubNum parameter, ignores upload.json participant field

**What's Needed**: ZIP extraction and launcher UI to pass `--upload` flag when running tests from snapshots.

---

## Background: How Upload Works

### Current System (Emscripten)

**1. Launcher creates upload.json**:
```json
{
  "host": "pebl.example.com",
  "page": "/scripts/uploadPEBL_token.php",
  "port": 443,
  "token": "STUDY_ABC12345",
  "taskname": "stroop",
  "username": "STUDY_ABC12345",
  "uploadpassword": "STUDY_ABC12345"
}
```

**2. Launcher passes --upload flag**:
```bash
pebl2 test.pbl -s P001 --upload /upload.json
```

This sets:
- `gUpload = 1`
- `gUploadFile = "/upload.json"`

**3. Test calls UploadFile()**:
```pebl
UploadFile(gSubNum, dataFileName)
```

**4. UploadFile() implementation** (Transfer.pbl:194-260):
- Reads upload.json from disk
- Extracts host, page, port, taskname, token
- **Uses `subcode` parameter (gSubNum) for participant ID** - NOT upload.json
- POSTs data file to server via HTTP

**5. InitializeUpload()** (Utility.pbl:2117-2167):
- **Emscripten only**: Sets gSubNum from upload.json participant field (for multi-test chains)
- **Native**: No-op (returns 0)

### Participant Code Handling

**IMPORTANT**: Participant code is handled correctly:

✅ **UploadFile() uses gSubNum parameter**:
```pebl
define UploadFile(subcode, datafilename, settingsfile:"") {
    # ... reads upload.json for server settings
    out <- SyncDataFile(host, page, port, username, uploadpassword, taskname, subcode, datafilename, token)
                                                                              ^^^^^^^^
                                                                              Uses function parameter (gSubNum),
                                                                              NOT upload.json participant field
}
```

✅ **upload.json participant field is OPTIONAL**:
- Only used by InitializeUpload() on Emscripten
- Used to set gSubNum for multi-test chains (e.g., "P001-1", "P001-2")
- Native launcher doesn't use InitializeUpload(), so field is ignored

✅ **PEBLOnlinePlatform snapshots don't include participant field**:
```php
// PackageBuilder.php:287-296
$uploadConfig = [
    'host' => $host,
    'page' => '/scripts/uploadPEBL_token.php',
    'port' => $port,
    'token' => $snapshot['token_id'],
    'taskname' => $taskName,
    'username' => $snapshot['token_id'],
    'uploadpassword' => $snapshot['token_id']
    // NO 'participant' field!
];
```

**Conclusion**: No changes needed to UploadFile(). It already uses gSubNum correctly.

---

## Snapshot Package Structure

Based on PEBLOnlinePlatform's PackageBuilder.php, snapshot ZIPs contain:

```
StudyName_v1_SNAP_abc123.zip
├── study-info.json           # Study metadata (name, version, battery version, etc.)
├── README.md                 # Human-readable reproduction instructions
├── test-files-manifest.json  # Test file metadata with checksums
├── parameters/               # Parameter JSON files (if customized)
│   ├── stroop.json
│   ├── corsi.json
│   └── ...
├── chains/                   # Chain configurations (if any)
│   └── MainBattery.json
└── tests/                    # Complete test source files
    ├── stroop/
    │   ├── stroop.pbl        # Test script
    │   ├── upload.json       # 🔑 Pre-configured upload settings!
    │   ├── translations/     # Custom translations (if any)
    │   ├── sounds/           # Audio files (if any)
    │   └── params/           # Schema files
    ├── corsi/
    │   ├── corsi.pbl
    │   ├── upload.json       # 🔑 One per test!
    │   └── ...
    └── ...
```

**Key Features**:
- ✅ Each test directory includes pre-configured upload.json
- ✅ Server details, study token, and task name already set
- ✅ Participant code NOT included (added by launcher at runtime)
- ✅ Complete test source code and resources included
- ✅ Custom translations included (if any)

---

## Implementation Plan: Three-Tier Approach

### Tier 1: Snapshot Import with Upload Support (HIGHEST PRIORITY)

**Use Case**: Researcher downloads snapshot from PEBLOnlinePlatform and imports into native launcher.

**What Works Already**:
1. Snapshot ZIP includes pre-configured upload.json in each test directory
2. Native PEBL supports --upload flag
3. UploadFile() works cross-platform with token authentication

**What's Needed**:
1. ZIP extraction library in launcher
2. Snapshot import UI (or enhance existing study import)
3. Pass --upload flag when running tests from snapshot

**Implementation Steps**:

#### Step 1: Add ZIP Library

**Option A: libzip (Recommended)**
```bash
# Ubuntu/Debian
sudo apt-get install libzip-dev

# macOS
brew install libzip
```

**Option B: minizip (Lightweight)**
- Comes with zlib (already a dependency)
- No additional install needed
- Smaller API surface

**Recommendation**: Use libzip for better error handling and UTF-8 filename support.

**Makefile Changes**:
```makefile
LAUNCHER_LIBS += -lzip
```

#### Step 2: Implement ZIP Extraction

**New File**: `src/apps/launcher/ZipExtractor.h/cpp`

```cpp
class ZipExtractor {
public:
    // Extract ZIP file to destination directory
    // Returns true on success, false on failure
    static bool ExtractZip(const std::string& zipPath, const std::string& destPath);

    // List contents of ZIP file
    static std::vector<std::string> ListZipContents(const std::string& zipPath);

    // Validate ZIP file (check if it's a valid PEBL snapshot)
    static bool IsValidSnapshot(const std::string& zipPath);
};
```

**Key Functions**:
```cpp
bool ZipExtractor::IsValidSnapshot(const std::string& zipPath) {
    // Check for required files:
    // - study-info.json (required)
    // - tests/ directory (required)
    // - At least one upload.json in tests/ (indicates upload-enabled snapshot)
    std::vector<std::string> contents = ListZipContents(zipPath);

    bool hasStudyInfo = false;
    bool hasTests = false;
    bool hasUploadConfig = false;

    for (const auto& file : contents) {
        if (file == "study-info.json") hasStudyInfo = true;
        if (file.find("tests/") == 0) hasTests = true;
        if (file.find("/upload.json") != std::string::npos) hasUploadConfig = true;
    }

    return hasStudyInfo && hasTests;
    // Note: hasUploadConfig is optional (some snapshots may not have upload enabled)
}
```

#### Step 3: Snapshot Import UI

**Approach A: New Menu Item** (Recommended)
- File → Import Snapshot...
- Opens file picker for .zip files
- Extracts to workspace/studies/{study_name}/
- Loads study-info.json to populate study metadata
- Detects upload.json files automatically

**Approach B: Enhance Existing Import**
- Modify existing "Open Study..." to detect ZIP files
- If ZIP detected, extract first, then load

**UI Flow**:
```
User: File → Import Snapshot...
      ↓
Launcher: Open file picker (.zip filter)
      ↓
User: Selects StudyName_v1_SNAP_abc123.zip
      ↓
Launcher:
  1. Validate ZIP (IsValidSnapshot)
  2. Read study-info.json from ZIP (without extracting)
  3. Show confirmation dialog:
     "Import snapshot: [StudyName]
      Version: [Version 1]
      Tests: [stroop, corsi, ...]
      Snapshot ID: [SNAP_abc123]
      Upload enabled: [Yes/No]"
  4. Extract to workspace/studies/{study_name}/
  5. Create Study object from study-info.json
  6. Detect upload.json presence
  7. Add to studies list
      ↓
User: Study appears in "My Studies" list
      Click "Run" → Tests upload data automatically!
```

**LauncherUI.cpp Changes**:
```cpp
// In File menu (around line 750)
if (ImGui::MenuItem("Import Snapshot...", "Ctrl+I")) {
    std::string zipPath = OpenFileDialog("Select Snapshot ZIP", "*.zip", mWorkspace->GetWorkspacePath());
    if (!zipPath.empty()) {
        ImportSnapshot(zipPath);
    }
}

// New method
void LauncherUI::ImportSnapshot(const std::string& zipPath) {
    // 1. Validate
    if (!ZipExtractor::IsValidSnapshot(zipPath)) {
        ShowMessageDialog("Invalid Snapshot",
            "The selected file is not a valid PEBL snapshot package.");
        return;
    }

    // 2. Read study-info.json from ZIP (peek without extracting)
    std::string studyInfoJson = ZipExtractor::ReadFileFromZip(zipPath, "study-info.json");
    nlohmann::json info = nlohmann::json::parse(studyInfoJson);

    std::string studyName = info["study_name"];
    std::string version = info["version"];
    std::string snapshotId = info["snapshot_id"];

    // 3. Show confirmation dialog
    std::string message = "Import snapshot:\n\n"
                         "Study: " + studyName + "\n"
                         "Version: " + version + "\n"
                         "Snapshot ID: " + snapshotId + "\n\n"
                         "Tests: ";
    for (const auto& test : info["tests"]) {
        message += test["test_name"].get<std::string>() + ", ";
    }

    if (!ShowConfirmDialog("Import Snapshot", message)) {
        return;
    }

    // 4. Extract to workspace
    std::string destPath = mWorkspace->GetWorkspacePath() + "/studies/" + studyName;
    if (!ZipExtractor::ExtractZip(zipPath, destPath)) {
        ShowMessageDialog("Import Failed", "Failed to extract snapshot package.");
        return;
    }

    // 5. Create Study object from extracted files
    Study* study = Study::LoadFromSnapshot(destPath);
    if (study) {
        mWorkspace->AddStudy(study);
        mCurrentStudy = study;
        ShowMessageDialog("Import Successful",
            "Snapshot imported successfully!\n\n"
            "Tests with upload.json will automatically upload data.");
    }
}
```

#### Step 4: Pass --upload Flag When Running Tests

**ExperimentRunner.cpp Changes**:

```cpp
// In RunTest() or RunChain() method (around line 185)
void ExperimentRunner::RunTest(const std::string& scriptPath,
                               const std::string& participantCode,
                               const std::string& language,
                               /* ... other params ... */) {
    std::vector<std::string> args;
    args.push_back(scriptPath);
    args.push_back("-s");
    args.push_back(participantCode);
    args.push_back("--language");
    args.push_back(language);

    // 🔑 NEW: Check for upload.json in test directory
    std::string testDir = ExtractDirectory(scriptPath); // Get directory containing .pbl file
    std::string uploadJsonPath = testDir + "/upload.json";

    if (FileExists(uploadJsonPath)) {
        args.push_back("--upload");
        args.push_back(uploadJsonPath);

        // Log for debugging
        LogToFile("Upload enabled for test: " + scriptPath);
        LogToFile("  Using config: " + uploadJsonPath);
    }

    // ... rest of RunTest implementation
    ExperimentRunner runner;
    runner.RunExperiment(scriptPath, args, ...);
}
```

**Helper Function**:
```cpp
std::string ExperimentRunner::ExtractDirectory(const std::string& filePath) {
    size_t lastSlash = filePath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        return filePath.substr(0, lastSlash);
    }
    return ".";
}

bool ExperimentRunner::FileExists(const std::string& path) {
    struct stat st;
    return (stat(path.c_str(), &st) == 0);
}
```

**That's It!** With these changes, imported snapshots will automatically upload data.

**Testing**:
1. Import snapshot ZIP
2. Run a test from the snapshot
3. Check launcher-log.csv for upload.json detection
4. Verify data appears in PEBLOnlinePlatform data browser

---

### Tier 2: Manual Upload Configuration UI (MEDIUM PRIORITY)

**Use Case**: Researcher wants to add upload capability to a native-only study (not from snapshot).

**Implementation**:

#### Step 1: Add Upload Config to Study Settings

**Study.h Changes**:
```cpp
class Study {
public:
    // ... existing members

    struct UploadConfig {
        bool enabled;
        std::string serverURL;      // e.g., "https://pebl.example.com"
        std::string studyToken;     // e.g., "STUDY_ABC123"

        UploadConfig() : enabled(false) {}
    };

    UploadConfig uploadConfig;

    // Serialize/deserialize with study-info.json
    void LoadUploadConfig(const nlohmann::json& json);
    void SaveUploadConfig(nlohmann::json& json);
};
```

**study-info.json Format**:
```json
{
    "study_name": "My Study",
    "created_at": "2026-01-12",
    "upload_config": {
        "enabled": true,
        "server_url": "https://pebl.example.com",
        "study_token": "STUDY_ABC123"
    }
}
```

#### Step 2: Study Settings Dialog UI

**LauncherUI.cpp Changes** (in RenderStudySettings around line 2800):
```cpp
void LauncherUI::RenderStudySettings() {
    // ... existing settings

    ImGui::Separator();
    ImGui::TextWrapped("Upload Configuration");
    ImGui::Spacing();

    bool uploadEnabled = mCurrentStudy->uploadConfig.enabled;
    if (ImGui::Checkbox("Enable data upload to server", &uploadEnabled)) {
        mCurrentStudy->uploadConfig.enabled = uploadEnabled;
        mCurrentStudy->SaveToFile();
    }

    if (uploadEnabled) {
        ImGui::Indent();

        // Server URL input
        static char serverURL[512];
        strncpy(serverURL, mCurrentStudy->uploadConfig.serverURL.c_str(), sizeof(serverURL)-1);
        if (ImGui::InputText("Server URL", serverURL, sizeof(serverURL))) {
            mCurrentStudy->uploadConfig.serverURL = serverURL;
            mCurrentStudy->SaveToFile();
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Full URL to upload endpoint, e.g., https://pebl.example.com");
        }

        // Study token input
        static char studyToken[128];
        strncpy(studyToken, mCurrentStudy->uploadConfig.studyToken.c_str(), sizeof(studyToken)-1);
        if (ImGui::InputText("Study Token", studyToken, sizeof(studyToken))) {
            mCurrentStudy->uploadConfig.studyToken = studyToken;
            mCurrentStudy->SaveToFile();
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Study token from PEBLOnlinePlatform (e.g., STUDY_ABC123)");
        }

        // Test connection button
        if (ImGui::Button("Test Connection")) {
            TestUploadConnection(
                mCurrentStudy->uploadConfig.serverURL,
                mCurrentStudy->uploadConfig.studyToken
            );
        }

        ImGui::Unindent();
    }
}
```

#### Step 3: Generate upload.json for Each Test

**When running a test from manually-configured study**:

```cpp
void ExperimentRunner::RunTest(/* ... */) {
    // ... build args

    // Check if upload is enabled for this study
    if (mCurrentStudy && mCurrentStudy->uploadConfig.enabled) {
        // Generate upload.json on-the-fly in test directory
        std::string testDir = ExtractDirectory(scriptPath);
        std::string uploadJsonPath = testDir + "/upload.json";

        // Extract taskname from script filename
        std::string scriptName = ExtractFilename(scriptPath); // e.g., "stroop.pbl"
        std::string taskName = scriptName.substr(0, scriptName.find(".pbl")); // "stroop"

        // Create upload.json
        CreateUploadJson(
            uploadJsonPath,
            mCurrentStudy->uploadConfig.serverURL,
            mCurrentStudy->uploadConfig.studyToken,
            taskName
        );

        args.push_back("--upload");
        args.push_back(uploadJsonPath);
    }

    // ... run experiment
}

void ExperimentRunner::CreateUploadJson(
    const std::string& path,
    const std::string& serverURL,
    const std::string& token,
    const std::string& taskName) {

    // Parse serverURL to extract host, port, page
    URL parsed = ParseURL(serverURL);

    nlohmann::json config;
    config["host"] = parsed.host;
    config["page"] = parsed.path.empty() ? "/scripts/uploadPEBL_token.php" : parsed.path;
    config["port"] = parsed.port;
    config["token"] = token;
    config["taskname"] = taskName;
    config["username"] = token;
    config["uploadpassword"] = token;

    // Write to file
    std::ofstream out(path);
    out << config.dump(2); // Pretty-print with 2-space indent
    out.close();
}
```

#### Step 4: Test Connection Feature

```cpp
void LauncherUI::TestUploadConnection(const std::string& serverURL, const std::string& token) {
    // Send test request to server
    std::string testURL = serverURL + "?test=connection&token=" + token;

    // Use CURL or similar to test connection
    // For now, simple implementation:
    std::string command = "curl -s -o /dev/null -w '%{http_code}' '" + testURL + "'";
    FILE* pipe = popen(command.c_str(), "r");

    if (!pipe) {
        ShowMessageDialog("Connection Test Failed", "Could not test connection.");
        return;
    }

    char buffer[128];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe)) {
        result += buffer;
    }
    pclose(pipe);

    if (result == "200") {
        ShowMessageDialog("Connection Test Successful",
            "Successfully connected to upload server!");
    } else {
        ShowMessageDialog("Connection Test Failed",
            "HTTP Status: " + result + "\n\n"
            "Check server URL and token.");
    }
}
```

---

### Tier 3: Self-Hosted Upload Server (LOW PRIORITY)

**Use Case**: Researcher wants to run local PEBL studies with data upload to local server (not PEBLOnlinePlatform).

**Two Options**:

#### Option A: Simple PHP Server (Included in PEBL Distribution)

Create `bin/upload-server/server.php`:

```php
<?php
/**
 * Simple PEBL Upload Server
 * No authentication - suitable for lab-local use only
 * Saves to uploads/{taskname}/{participant}/ directory structure
 */

$dataDir = __DIR__ . '/uploads';

if (!is_dir($dataDir)) {
    mkdir($dataDir, 0755, true);
}

// Get POST parameters
$taskname = $_POST['taskname'] ?? 'unknown';
$subnum = $_POST['subnum'] ?? 'unknown';
$user_name = $_POST['user_name'] ?? '';

// Handle file upload
if (isset($_FILES['fileToUpload'])) {
    $file = $_FILES['fileToUpload'];

    // Create directory for this task and participant
    $uploadDir = $dataDir . '/' . $taskname . '/' . $subnum;
    if (!is_dir($uploadDir)) {
        mkdir($uploadDir, 0755, true);
    }

    // Save file with timestamp
    $filename = date('Y-m-d_H-i-s') . '_' . basename($file['name']);
    $targetPath = $uploadDir . '/' . $filename;

    if (move_uploaded_file($file['tmp_name'], $targetPath)) {
        http_response_code(200);
        echo json_encode(['success' => true, 'file' => $filename]);
    } else {
        http_response_code(500);
        echo json_encode(['success' => false, 'error' => 'Upload failed']);
    }
} else {
    http_response_code(400);
    echo json_encode(['success' => false, 'error' => 'No file provided']);
}
?>
```

**README for Simple Server**:

```markdown
# PEBL Simple Upload Server

## Quick Start

1. Ensure PHP is installed:
   ```bash
   php --version
   ```

2. Start server:
   ```bash
   cd bin/upload-server
   php -S localhost:8080 server.php
   ```

3. Configure PEBL launcher:
   - Server URL: http://localhost:8080/server.php
   - Study Token: (leave blank - no authentication)

4. Run tests - data will be saved to:
   ```
   bin/upload-server/uploads/
   ├── stroop/
   │   ├── P001/
   │   │   └── 2026-01-12_10-30-45_stroop-P001.csv
   │   └── P002/
   │       └── 2026-01-12_10-35-12_stroop-P002.csv
   └── corsi/
       └── P001/
           └── 2026-01-12_10-32-00_corsi-P001.csv
   ```

## Security Warning

This server has NO authentication. Only use on trusted local networks.
For production use, install full PEBLOnlinePlatform.
```

#### Option B: Self-Hosted PEBLOnlinePlatform

- Researcher installs full PEBLOnlinePlatform on their server
- Creates studies and tokens via web interface
- Native launcher connects to their server instead of public hub
- Full authentication and study management

**Launcher UI Changes**:
```cpp
// In Study Settings, add "Server Type" dropdown:
const char* serverTypes[] = {
    "PEBLOnlinePlatform Hub (requires token)",
    "Self-Hosted Platform (requires token + custom URL)",
    "Simple Local Server (no authentication)"
};

static int selectedServerType = 0;
ImGui::Combo("Server Type", &selectedServerType, serverTypes, 3);

if (selectedServerType == 0) {
    // Show only token field
    // serverURL defaults to "https://pebl.example.com"
} else if (selectedServerType == 1) {
    // Show token field AND custom URL field
} else {
    // Show only URL field, hide token
}
```

---

## Command-Line Integration Summary

**Current Chain Execution**:
```cpp
std::vector<std::string> args;
args.push_back(scriptPath);
args.push_back("-s");
args.push_back(participantCode);
args.push_back("--language");
args.push_back(language);

ExperimentRunner runner;
runner.RunExperiment(scriptPath, args, ...);
```

**With Upload Support**:
```cpp
std::vector<std::string> args;
args.push_back(scriptPath);
args.push_back("-s");
args.push_back(participantCode);
args.push_back("--language");
args.push_back(language);

// 🔑 Check for upload.json in test directory
std::string testDir = ExtractDirectory(scriptPath);
std::string uploadJsonPath = testDir + "/upload.json";

if (FileExists(uploadJsonPath)) {
    args.push_back("--upload");
    args.push_back(uploadJsonPath);
}

ExperimentRunner runner;
runner.RunExperiment(scriptPath, args, ...);
```

**That's all that's needed!** PEBL core already handles the rest.

---

## Testing Strategy

### Test 1: Snapshot Import (Tier 1)

**Prerequisites**:
- Access to PEBLOnlinePlatform instance
- At least one study with token created

**Steps**:
1. On PEBLOnlinePlatform:
   - Create study with one test (e.g., stroop)
   - Activate study (creates snapshot)
   - Download snapshot ZIP
2. In native launcher:
   - File → Import Snapshot...
   - Select downloaded ZIP
   - Confirm import
3. Run test:
   - Select imported study
   - Click "Run Test"
   - Enter participant code (e.g., "TEST001")
   - Complete test
4. Verify:
   - Check launcher-log.csv for upload.json detection
   - Check PEBLOnlinePlatform data browser for uploaded file
   - Confirm data is associated with correct study token

**Expected Result**: Data appears in PEBLOnlinePlatform within seconds of test completion.

### Test 2: Manual Configuration (Tier 2)

**Prerequisites**:
- Study token from PEBLOnlinePlatform

**Steps**:
1. In native launcher:
   - Create new study
   - Add test from battery (e.g., corsi)
   - Open Study Settings
   - Enable upload
   - Enter server URL: https://pebl.example.com
   - Enter study token: STUDY_ABC123
   - Click "Test Connection" → Should succeed
2. Run test:
   - Enter participant code
   - Complete test
3. Verify:
   - Check test directory for auto-generated upload.json
   - Check PEBLOnlinePlatform for uploaded data

**Expected Result**: upload.json created automatically, data uploads successfully.

### Test 3: Self-Hosted Simple Server (Tier 3)

**Prerequisites**:
- PHP installed

**Steps**:
1. Start simple server:
   ```bash
   cd bin/upload-server
   php -S localhost:8080 server.php
   ```
2. In native launcher:
   - Create study or use existing
   - Enable upload
   - Server URL: http://localhost:8080/server.php
   - Token: (leave blank)
3. Run test:
   - Enter participant code
   - Complete test
4. Verify:
   - Check `bin/upload-server/uploads/{taskname}/{participant}/` for CSV file
   - File should contain test data

**Expected Result**: Data saved to local uploads directory.

---

## Security Considerations

1. **Token Storage**: Tokens stored in study-info.json are accessible to anyone with file system access. This is acceptable for lab use but warn users in documentation.

2. **HTTPS Requirement**: Production deployments should use HTTPS. Add warning in UI if using HTTP with non-localhost server.

3. **Token Validation**: Server validates tokens, not client. Client just needs to pass token correctly.

4. **No Credential Storage**: Don't store usernames/passwords in native launcher. Use tokens only.

---

## ZIP Library Recommendation

**Use libzip** (preferred) because:
- ✅ Clean API
- ✅ Good error handling
- ✅ UTF-8 filename support
- ✅ Available on all platforms
- ✅ Actively maintained

**Installation**:
```bash
# Ubuntu/Debian
sudo apt-get install libzip-dev

# macOS
brew install libzip

# Windows (vcpkg)
vcpkg install libzip
```

**Makefile Changes**:
```makefile
# Add to launcher build
LAUNCHER_LIBS += -lzip

# Add to includes
INCLUDES += -I/usr/include
```

**Alternative: minizip** (if libzip not available):
- Comes with zlib
- Lighter weight
- Requires more manual work (encoding, error handling)

---

## Implementation Priority

### Phase 1: Snapshot Support (HIGHEST PRIORITY)
**Effort**: 1-2 days
**Impact**: Enables immediate online-to-native workflow

**Tasks**:
1. ✅ Verify UploadFile() uses subcode parameter (DONE - confirmed in Transfer.pbl)
2. Add libzip dependency to Makefile
3. Implement ZipExtractor class
4. Add "Import Snapshot" menu item
5. Implement ImportSnapshot() UI flow
6. Update ExperimentRunner to detect and use upload.json
7. Test with real snapshot from PEBLOnlinePlatform

### Phase 2: Manual Configuration UI (MEDIUM PRIORITY)
**Effort**: 1 day
**Impact**: Allows native-only users to enable uploading

**Tasks**:
1. Add UploadConfig struct to Study class
2. Add upload configuration section to Study Settings dialog
3. Implement JSON generation from UI settings
4. Add connection test feature
5. Update chain runner to generate upload.json when needed

### Phase 3: Self-Hosted Server (LOW PRIORITY)
**Effort**: 2-3 days
**Impact**: Useful for advanced users, but most will use hub

**Tasks**:
1. Create simple PHP upload server
2. Add server type selection to UI
3. Document self-hosting setup
4. Add example configuration files

---

## Compatibility

- **Emscripten**: No changes needed, already works ✅
- **Windows**: UploadFile() uses HTTP lib, should work ✅
- **macOS**: UploadFile() uses HTTP lib, should work ✅
- **Linux**: UploadFile() uses HTTP lib, works (tested) ✅

All platforms support HTTP POST via PEBL's built-in `PostHTTPFile()` function.

---

## Conclusion

The upload system is **already built into PEBL**. We just need to:

1. ✅ Extract upload.json from snapshots (needs ZIP library)
2. ✅ Pass --upload flag when launching tests
3. ✅ Add UI for manual configuration (optional but nice)

**Participant code handling is NOT an issue** - UploadFile() already uses gSubNum parameter correctly.

**Recommendation**: Start with Phase 1 (snapshot support) as it requires minimal work and enables the primary use case (online-to-native workflow).
