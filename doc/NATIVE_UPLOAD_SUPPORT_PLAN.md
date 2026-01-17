# Native PEBL Launcher: upload.json Support Plan

## Overview

The PEBL Online Platform uses an `upload.json` file to configure data uploading for tests. This same mechanism can work with the native launcher with minimal changes.

## Current System (Emscripten)

### upload.json Format

```json
{
  "token": "STUDY_ABC12345",
  "taskname": "stroop",
  "participant": "P001",
  "uploadURL": "https://server.com/api/upload.php",
  "host": "server.com",
  "page": "/api/upload.php",
  "port": 443
}
```

### How It Works

1. **Launcher creates upload.json**: JavaScript writes this file to Emscripten's virtual filesystem at `/upload.json`

2. **Launcher passes `--upload` flag**:
   ```bash
   pebl2 test.pbl -s P001 --upload /upload.json
   ```
   This sets `gUpload=1` and `gUploadFile="/upload.json"`

3. **Test calls InitializeUpload()**: (Emscripten only)
   ```pebl
   define Start(p) {
       InitializeUpload()  ## Sets up data directory, reads participant ID
       ## ... rest of test
   }
   ```

4. **Test uploads data with UploadFile()**:
   ```pebl
   UploadFile(gSubNum, dataFileName)
   ```
   - Reads `upload.json` from disk
   - Extracts `host`, `page`, `port`, `taskname`, `token`
   - POSTs data file to server via HTTP

### Key Insight

**Native PEBL already supports this!** The `--upload` flag and `UploadFile()` function work on all platforms. We just need UI to help users create and manage `upload.json` files.

## Proposed Solution: Three-Tiered Approach

### Tier 1: Snapshot Support (Immediate, Zero UI Changes)

**Use Case**: Researcher downloads a configured snapshot from PEBLOnlinePlatform with `upload.json` already included.

**How It Works**:
1. On PEBLOnlinePlatform, researcher creates study with upload token
2. Researcher creates snapshot (study export) - includes `upload.json` in study root
3. Researcher downloads snapshot ZIP
4. Researcher imports snapshot into native launcher
5. When tests run, they automatically upload data (no extra configuration needed)

**Implementation**:
- Snapshot export on web platform should include `upload.json`
- Native launcher already supports `--upload` flag
- Add `upload.json` path to chain configuration

**Status**: Nearly complete. Just need to ensure snapshot export includes upload.json.

### Tier 2: Manual upload.json Creation (Simple UI Addition)

**Use Case**: Researcher wants to add upload capability to an existing native study.

**How It Works**:
1. Open study settings in launcher
2. New section: "Data Upload Configuration"
3. Options:
   - ☐ Enable data upload
   - Server URL: [https://pebl.example.com/api/upload.php]
   - Study Token: [STUDY_ABC123]
   - [Test Connection] button
4. Launcher creates `upload.json` in study root directory
5. Chain configuration automatically includes `--upload` flag for all tests

**UI Location**: Study Settings dialog (already exists)

**Implementation**:
```cpp
struct UploadConfig {
    bool enabled;
    char serverURL[512];
    char studyToken[128];
};

// In Study class:
UploadConfig uploadConfig;

// Save to study-info.json:
{
    "study_name": "My Study",
    "upload_config": {
        "enabled": true,
        "server_url": "https://pebl.example.com/api/upload.php",
        "study_token": "STUDY_ABC123"
    }
}

// Generate upload.json when running chain:
void WriteUploadJSON(const std::string& studyPath, const UploadConfig& config,
                     const std::string& participant, const std::string& testName) {
    // Parse serverURL to extract host, port, page
    // Create JSON with format shown above
    // Write to studyPath/upload.json
}
```

**Test Connection Feature**:
- Send HTTP GET to `{serverURL}?test=connection&token={token}`
- Check for 200 OK response
- Show success/error dialog

### Tier 3: Self-Hosted Server Support (Advanced)

**Use Case**: Researcher running local PEBL studies wants data uploaded to local server (not PEBLOnlinePlatform).

**Two Options**:

#### Option A: Simple PHP Server (Included)
- Include lightweight PHP upload server in PEBL distribution
- Located in `bin/upload-server/`
- Start with: `php -S localhost:8080 upload-server/server.php`
- No token authentication (suitable for lab-local use)
- Saves to `uploads/{taskname}/{participant}/` directory structure

#### Option B: Token Server (Self-Hosted PEBLOnlinePlatform)
- Researcher installs full PEBLOnlinePlatform on their server
- Creates studies and tokens via web interface
- Native launcher connects to their server instead of public hub
- Full authentication and study management

**Implementation**:
- Add "Server Type" dropdown in upload configuration:
  - PEBLOnlinePlatform Hub (requires token)
  - Self-Hosted Platform (requires token + custom URL)
  - Simple Local Server (no authentication)
- Show appropriate fields based on selection

## Implementation Priority

### Phase 1: Snapshot Support (Highest Priority)
**Effort**: 2-3 hours
**Impact**: Enables immediate online-to-native workflow

Tasks:
1. Ensure PEBLOnlinePlatform snapshot export includes `upload.json`
2. Update launcher to pass `--upload` flag when `upload.json` exists in study root
3. Test with real snapshot from platform

### Phase 2: Manual Configuration UI (Medium Priority)
**Effort**: 1 day
**Impact**: Allows native-only users to enable uploading

Tasks:
1. Add upload configuration section to Study Settings dialog
2. Implement JSON generation from UI settings
3. Add connection test feature
4. Update chain runner to include `--upload` flag

### Phase 3: Self-Hosted Server (Low Priority)
**Effort**: 2-3 days
**Impact**: Useful for advanced users, but most will use hub

Tasks:
1. Create simple PHP upload server
2. Add server type selection to UI
3. Document self-hosting setup
4. Add example configuration files

## Command-Line Integration

### Current Chain Execution
```cpp
std::vector<std::string> args;
args.push_back(scriptPath);
args.push_back("-s");
args.push_back(participantCode);
args.push_back("--language");
args.push_back(language);
// ... other args

ExperimentRunner runner;
runner.RunExperiment(scriptPath, args, ...);
```

### With Upload Support
```cpp
std::vector<std::string> args;
args.push_back(scriptPath);
args.push_back("-s");
args.push_back(participantCode);
args.push_back("--language");
args.push_back(language);

// Check for upload.json in study directory
std::string uploadPath = studyPath + "/upload.json";
if (FileExists(uploadPath)) {
    // Update upload.json with current test name and participant
    UpdateUploadJSON(uploadPath, testName, participantCode);

    args.push_back("--upload");
    args.push_back(uploadPath);
}

ExperimentRunner runner;
runner.RunExperiment(scriptPath, args, ...);
```

## upload.json Template

For Phase 2 implementation:

```cpp
std::string GenerateUploadJSON(const std::string& serverURL,
                                const std::string& token,
                                const std::string& testName,
                                const std::string& participant) {
    // Parse serverURL to extract components
    URL parsed = ParseURL(serverURL);

    nlohmann::json config;
    config["token"] = token;
    config["taskname"] = testName;
    config["participant"] = participant;
    config["uploadURL"] = serverURL;
    config["host"] = parsed.host;
    config["page"] = parsed.path;
    config["port"] = parsed.port;

    return config.dump(2);  // Pretty-print with 2-space indent
}
```

## Testing Strategy

### Test 1: Snapshot Import (Phase 1)
1. Create study on PEBLOnlinePlatform with token
2. Create snapshot with one test (e.g., stroop)
3. Download and import snapshot to native launcher
4. Run test with participant code
5. Verify data appears in PEBLOnlinePlatform data browser

### Test 2: Manual Configuration (Phase 2)
1. Create new study in native launcher
2. Add test from battery
3. Open study settings, enable upload
4. Enter server URL and token
5. Click "Test Connection" - should succeed
6. Run test with participant code
7. Verify data appears on server

### Test 3: Self-Hosted Server (Phase 3)
1. Start simple PHP server: `php -S localhost:8080 upload-server/server.php`
2. Configure native launcher to use localhost:8080
3. Run test with participant code
4. Verify data file appears in `upload-server/uploads/`

## Security Considerations

1. **Token Storage**: Tokens stored in `study-info.json` are accessible to anyone with file system access. This is acceptable for lab use but warn users in documentation.

2. **HTTPS Requirement**: Production deployments should use HTTPS. Add warning in UI if using HTTP with non-localhost server.

3. **Token Validation**: Server validates tokens, not client. Client just needs to pass token correctly.

4. **No Credential Storage**: Don't store usernames/passwords in native launcher. Use tokens only.

## Documentation Requirements

1. **User Guide**: "Uploading Data from Native PEBL"
   - How to import snapshot with upload pre-configured
   - How to manually configure upload for existing study
   - How to verify upload is working

2. **Self-Hosting Guide**: "Running Your Own Upload Server"
   - How to start simple PHP server
   - How to install full PEBLOnlinePlatform
   - Security best practices

3. **Token Management**: "Managing Study Tokens"
   - How to create tokens on PEBLOnlinePlatform
   - How to view uploaded data
   - How to revoke/rotate tokens

## Open Questions

1. **Multi-test chains**: Should each test get its own `upload.json` or share one?
   - **Answer**: Share one, update `taskname` and `participant` fields before each test

2. **Offline mode**: What happens if upload fails (no network)?
   - **Answer**: `UploadFile()` already handles this gracefully - just logs warning. Data remains in local `data/` directory.

3. **Retry logic**: Should launcher retry failed uploads?
   - **Answer**: Phase 1/2 - No. Phase 3 - Maybe add retry queue

4. **Progress indication**: Show upload progress in UI?
   - **Answer**: Phase 1/2 - No (uploads are fast). Phase 3 - Optional enhancement

## Compatibility

- **Emscripten**: No changes needed, already works
- **Windows**: `UploadFile()` uses HTTP lib, should work
- **macOS**: `UploadFile()` uses HTTP lib, should work
- **Linux**: `UploadFile()` uses HTTP lib, works (tested)

All platforms support HTTP POST via PEBL's built-in `PostDataToHTTP()` function.

## Conclusion

The upload system is **already built into PEBL**. We just need to:

1. Help users create `upload.json` files (or import them from snapshots)
2. Pass `--upload` flag when launching tests
3. Add UI for configuration (optional but nice)

**Recommendation**: Start with Phase 1 (snapshot support) as it requires minimal work and enables the primary use case (online-to-native workflow).
