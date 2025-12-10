# Token-Based Multi-Test Hosting Architecture

## Overview

This document specifies the architecture for hosting multiple PEBL battery tests online using token-based authentication and URL routing. The system allows researchers to deploy studies using unique tokens and URLs that route participants to specific tests with automatic data organization.

## Goals

- Host multiple battery tests in a single Emscripten build
- Use tokens to identify studies and control test access
- Track individual participants across tests
- Organize data by token and test
- Leverage existing PEBL command-line arguments
- Require minimal changes to battery tests

## URL Structure

### Format
```
https://pebl.example.com/?test=TESTNAME&token=TOKEN&participant=PARTICIPANT&language=LANG
```

### Parameters

- **test** (required): Battery test name (e.g., "stroop", "flanker", "corsi")
- **token** (required): Study identifier for authentication and data organization
- **participant** (required): Participant ID for tracking individuals
- **language** (optional): Two-character language code (defaults to "en")

### Examples
```
https://pebl.example.com/?test=stroop&token=STUDY_ABC123&participant=P001
https://pebl.example.com/?test=flanker&token=STUDY_ABC123&participant=P001&language=es
https://pebl.example.com/?test=corsi&token=STUDY_XYZ789&participant=P042
```

## Server-Side Configuration

### Token Database Schema

```sql
CREATE TABLE tokens (
    token_id TEXT PRIMARY KEY,
    researcher_email TEXT NOT NULL,
    allowed_tests TEXT NOT NULL,  -- JSON array: ["stroop", "flanker"]
    upload_url TEXT,
    expires_at DATETIME,
    max_participants INTEGER,
    active BOOLEAN DEFAULT 1,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);
```

### Validation API Endpoint

**Request**: `GET /api/validate?token=TOKEN&test=TESTNAME&participant=PARTICIPANT`

**Response** (success):
```json
{
  "valid": true,
  "uploadURL": "https://pebl.example.com/api/upload",
  "researcher": "researcher@university.edu"
}
```

**Response** (failure):
```json
{
  "valid": false,
  "error": "Token expired" | "Test not allowed" | "Invalid token"
}
```

## Client-Side Implementation

### JavaScript Launcher (pebl-launcher.html)

```javascript
var Module = {
    onRuntimeInitialized: async function() {
        // 1. Read URL parameters
        const params = new URLSearchParams(window.location.search);
        const test = params.get('test');
        const token = params.get('token');
        const participant = params.get('participant');
        const language = params.get('language') || 'en';

        // Validate required parameters
        if (!test || !token || !participant) {
            alert('Missing required parameters: test, token, and participant');
            return;
        }

        // 2. Validate token with server
        try {
            const response = await fetch(
                `/api/validate?token=${token}&test=${test}&participant=${participant}`
            );
            const config = await response.json();

            if (!config.valid) {
                alert('Invalid study configuration: ' + config.error);
                return;
            }

            // 3. Write upload.json for PEBL to read
            const uploadConfig = {
                token: token,
                test: test,
                participant: participant,
                uploadURL: config.uploadURL
            };
            Module.FS.writeFile('/upload.json', JSON.stringify(uploadConfig));

            // 4. Launch PEBL with command-line arguments
            const scriptPath = `/usr/local/share/pebl2/battery/${test}/${test}.pbl`;
            const args = [scriptPath, '-s', participant];

            // Add optional language
            if (language !== 'en') {
                args.push('--language', language);
            }

            Module.callMain(args);

        } catch (error) {
            alert('Error connecting to server: ' + error.message);
        }
    }
};
```

### upload.json Format

Written by JavaScript launcher, read by PEBL:

```json
{
  "token": "STUDY_ABC123",
  "test": "stroop",
  "participant": "P001",
  "uploadURL": "https://pebl.example.com/api/upload"
}
```

## PEBL-Side Implementation

### EM.pbl Functions

#### 1. InitializeUpload()

Called at the start of battery tests to configure token mode:

```pebl
## Initialize token-based upload configuration
## Call this at the start of battery tests that will be hosted online
define InitializeUpload()
{
    if(FileExists("upload.json"))
    {
        settings <- ReadJSON("upload.json")
        gToken <- settings.token
        gTestName <- settings.test
        gUploadURL <- settings.uploadURL
        gParticipant <- settings.participant

        ## Set centralized data directory for token mode
        gDataDirectory <- "/data/" + gToken + "/" + gTestName + "/"
        MakeDirectory(gDataDirectory)

        gUseUpload <- 1
        gUploadSettings <- settings  ## Make full config available

        return(1)
    }
    return(0)
}
```

#### 2. GetNewDataFile() Override

Modify to respect gDataDirectory when in token mode:

```pebl
## Override in EM.pbl to use token-based paths
define GetNewDataFile(subnum, win, basename, ext, header)
{
    if(IsDefined(gDataDirectory))
    {
        ## Token mode - use centralized directory
        filename <- gDataDirectory + basename + "-" + subnum + "." + ext
    }
    else
    {
        ## Desktop mode - use test's local data/ directory
        filename <- "data/" + basename + "-" + subnum + "." + ext
    }

    fileout <- FileOpenWrite(filename)
    if(header != "")
    {
        FilePrint(fileout, header)
    }
    return(fileout)
}
```

#### 3. FileOpenWrite() Override (if needed)

For tests that use FileOpenWrite() directly:

```pebl
define FileOpenWrite(filename)
{
    if(IsDefined(gDataDirectory))
    {
        ## If filename starts with "data/", replace with gDataDirectory
        if(SubString(filename, 1, 5) == "data/")
        {
            filename <- gDataDirectory + SubString(filename, 6, StringLength(filename))
        }
    }

    ## Call the built-in FileOpenWrite
    return(FileOpenWriteBuiltin(filename))
}
```

### Battery Test Modification

Minimal change required - add one line at start:

```pebl
define Start(p)
{
    InitializeUpload()  ## Sets up token mode if upload.json exists

    ## Existing test code continues normally
    gVideoWidth <- 800
    gVideoHeight <- 600
    gWin <- MakeWindow()

    if(not IsDefined(gSubNum))
    {
        gSubNum <- GetSubNum(gWin)
    }

    ## Load parameters, run test, save data - all unchanged
    ## ...
}
```

## Data Organization

### IDBFS Structure

```
/data/
  ├── STUDY_ABC123/
  │   ├── stroop/
  │   │   ├── stroop-P001.csv
  │   │   ├── stroop-P002.csv
  │   ├── flanker/
  │   │   ├── flanker-P001.csv
  │   │   ├── flanker-P002.csv
  ├── STUDY_XYZ789/
  │   ├── corsi/
  │   │   ├── corsi-P042.csv
```

### Server Storage Structure

```
/var/pebl/data/
  ├── researcher1@university.edu/
  │   ├── STUDY_ABC123/
  │   │   ├── stroop/
  │   │   │   ├── stroop-P001.csv
  │   │   │   ├── stroop-P002.csv
  │   │   ├── flanker/
  │   │   │   ├── flanker-P001.csv
```

## Complete Data Flow

1. **Participant receives URL** from researcher:
   ```
   https://pebl.example.com/?test=stroop&token=ABC123&participant=P001
   ```

2. **Browser loads** pebl2.html with JavaScript launcher

3. **JavaScript validates** token with server API

4. **JavaScript writes** `/upload.json` to Emscripten filesystem:
   ```json
   {
     "token": "ABC123",
     "test": "stroop",
     "participant": "P001",
     "uploadURL": "https://pebl.example.com/api/upload"
   }
   ```

5. **JavaScript launches** PEBL:
   ```javascript
   Module.callMain([
     '/usr/local/share/pebl2/battery/stroop/stroop.pbl',
     '-s', 'P001'
   ])
   ```

6. **PEBL test starts**, calls `InitializeUpload()`:
   - Reads `/upload.json`
   - Sets `gToken = "ABC123"`
   - Sets `gTestName = "stroop"`
   - Sets `gDataDirectory = "/data/ABC123/stroop/"`
   - Sets `gSubNum = "P001"` (from `-s` argument)

7. **Test runs**, saves data using `GetNewDataFile()`:
   - Files saved to `/data/ABC123/stroop/stroop-P001.csv`
   - Data persists in IDBFS

8. **Test completes**, calls `UploadFile()` or `SyncDataFile()`:
   - Reads file from `/data/ABC123/stroop/`
   - POSTs to `gUploadSettings.uploadURL` with token authentication
   - Server validates token, saves to researcher's directory

## Emscripten Build Configuration

### Makefile Changes

Package multiple battery tests in one build:

```makefile
em-opt: $(DIRS) $(EMMAIN_OBJ) $(EMMAIN_INC)
	$(CXX) $(CXXFLAGS) \
	# ... existing flags ...
	--preload-file battery/stroop@/usr/local/share/pebl2/battery/stroop \
	--preload-file battery/flanker@/usr/local/share/pebl2/battery/flanker \
	--preload-file battery/corsi@/usr/local/share/pebl2/battery/corsi \
	--preload-file battery/bcst@/usr/local/share/pebl2/battery/bcst \
	# ... more tests as needed ...
	--preload-file emscripten/pebl-lib@/usr/local/share/pebl2/pebl-lib \
	--preload-file emscripten/media/@/usr/local/share/pebl2/media
```

## Implementation Steps

### Phase 1: Core Infrastructure
1. Add `InitializeUpload()` to EM.pbl
2. Override `GetNewDataFile()` in EM.pbl to use `gDataDirectory`
3. Create JavaScript launcher with token validation
4. Set up server validation API endpoint
5. Test with single battery test (e.g., stroop)

### Phase 2: Multiple Tests
1. Update Makefile to package multiple tests
2. Modify selected battery tests to call `InitializeUpload()`
3. Test with multiple tests using different tokens
4. Verify data organization in IDBFS

### Phase 3: Upload Integration
1. Enhance upload API to accept token authentication
2. Organize server-side storage by researcher/token/test
3. Test file upload with token validation
4. Add server-side logging and monitoring

### Phase 4: Researcher Interface
1. Create token management UI for researchers
2. Allow researchers to generate tokens with test permissions
3. Provide URL generator for researchers
4. Add data download interface by token

## Testing Plan

### Test Cases

1. **Single test, single participant**
   - URL with valid token
   - Verify data saves to correct path
   - Verify upload works

2. **Multiple tests, same participant**
   - Same token, same participant, different tests
   - Verify data separated by test
   - Verify participant ID consistent

3. **Same test, multiple participants**
   - Same token, different participants, same test
   - Verify data separated by participant

4. **Invalid token**
   - Verify rejection before test loads

5. **Expired token**
   - Verify rejection at validation stage

6. **Unauthorized test**
   - Token valid but test not in allowed_tests
   - Verify rejection

7. **Language support**
   - Test with language parameter
   - Verify correct translations load

## Security Considerations

1. **Token validation**: Always validate server-side before allowing test access
2. **Upload authentication**: Verify token on every upload request
3. **Rate limiting**: Prevent abuse of validation and upload endpoints
4. **HTTPS only**: All communication over encrypted connections
5. **Token expiration**: Enforce expiration dates
6. **Participant privacy**: Don't log identifiable information unnecessarily

## Future Enhancements

1. **Test sequences**: Support multiple tests in one session
2. **Progress tracking**: Track completion status per participant
3. **Custom parameters**: Allow per-token parameter overrides via `--pfile`
4. **Real-time monitoring**: Dashboard showing active participants
5. **Automated data export**: Scheduled exports to researcher's storage
6. **Multi-language UI**: Translate launcher interface

## Notes

- Desktop PEBL remains unchanged - tests work identically on desktop
- Token mode only activates when `upload.json` exists (Emscripten)
- Battery tests require only one line change: `InitializeUpload()`
- Data functions that use standard library work automatically
- Tests using direct `FileOpenWrite()` may need manual path adjustment
