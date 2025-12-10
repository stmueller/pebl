# Parameter Variant System - Implementation Complete

**Date:** October 17, 2025
**Status:** ✅ COMPLETE

## Overview

The PEBL web platform now supports multiple parameter sets (variants) per test, allowing researchers to create different versions of the same test with different configurations (e.g., "fast", "slow", "practice").

## Clean URL Architecture

### Simple URL Format
```
http://localhost:8080/pebl-launcher.html?test=corsi&token=STUDY_ABC123&participant=P001&param=fast
```

**URL Parameters:**
- `test` - Test name (e.g., "corsi", "stroop")
- `token` - Study identifier
- `participant` - Participant ID
- `param` - Optional parameter variant name (e.g., "fast", "slow", "practice")
  - If omitted or empty: uses default parameters from schema
  - If specified: loads the named variant

### Why This is Better

**Before (ugly):**
```
?test=corsi&token=X&participant=P001&pfile=http%3A%2F%2Flocalhost%3A8080%2Fapi%2Fparams.php%3Ftoken%3DX%26test%3Dcorsi
```

**After (clean):**
```
?test=corsi&token=X&participant=P001&param=fast
```

**Benefits:**
1. **No URL encoding mess** - Clean, readable URLs
2. **Same-origin security** - JavaScript constructs API URL internally
3. **Shorter URLs** - Easier to share and QR code
4. **Cross-site safety** - No CORS issues since API is same origin

## How It Works

### 1. JavaScript Launcher (pebl-launcher.html)

The launcher reads URL parameters and constructs the parameter API URL internally:

```javascript
function getURLParams() {
    const params = new URLSearchParams(window.location.search);
    return {
        test: params.get('test'),
        token: params.get('token'),
        participant: params.get('participant'),
        param: params.get('param') || '',  // Optional variant
        language: params.get('language') || 'en'
    };
}

function launchPEBL(Module, params) {
    const args = [scriptPath, '-s', params.participant];

    // Construct parameter file URL internally
    if (params.token && params.test) {
        let pfileURL = `/PEBLDataServer/api/params.php?token=${params.token}&test=${params.test}`;

        if (params.param) {
            pfileURL += `&variant=${params.param}`;
        }

        args.push('--pfile', pfileURL);
    }

    Module.callMain(args);
}
```

### 2. Backend API (api/params.php)

Already supports variants via `&variant=` parameter:

```php
$variant = $_GET['variant'] ?? '';
$testIdentifier = empty($variant) ? $test : $test . '-' . $variant;
$paramsFile = $DATA_DIR . '/' . $token . '/' . $testIdentifier . '.par.json';
```

**File Storage:**
- `data/{token}/corsi.par.json` - Default parameters
- `data/{token}/corsi-fast.par.json` - "fast" variant
- `data/{token}/corsi-slow.par.json` - "slow" variant
- `data/{token}/corsi-practice.par.json` - "practice" variant

### 3. Web UI (manage_tokens_web.php)

**Auto-detects existing variants:**
- Scans `data/{token}/` directory for parameter files
- Shows all variants with their own URLs
- Each variant gets a clean URL with `&param=` parameter

**Example Display:**
```
Corsi Block Test

  Default Parameters:
  http://localhost:8080/pebl-launcher.html?test=corsi&token=STUDY_ABC&participant={PARTICIPANT_ID}
  [Copy] [QR Code]

  Fast:
  http://localhost:8080/pebl-launcher.html?test=corsi&token=STUDY_ABC&participant={PARTICIPANT_ID}&param=fast
  [Copy] [QR Code]

  Practice:
  http://localhost:8080/pebl-launcher.html?test=corsi&token=STUDY_ABC&participant={PARTICIPANT_ID}&param=practice
  [Copy] [QR Code]

  ⚙️ Configure Parameters
```

### 4. Parameter Configuration (configure_params.php)

**Create New Variant:**
- Form to enter variant name (e.g., "fast", "slow", "practice")
- Validates name (lowercase, numbers, hyphens, underscores only)
- Creates new parameter set with schema defaults

**Switch Between Variants:**
- Shows existing variants as clickable buttons
- Clicking a variant loads its parameters for editing
- Each variant saves to its own file

**UI Elements:**
```
Existing Parameter Sets:
[Default ✓] [Fast] [Slow] [Practice]

Create New Parameter Set:
Name: [____________] [Create New Set]

---

Configure Test Parameters
Corsi Block Test - Fast Version

[Parameter form with all settings...]

[💾 Save Configuration]
```

## Desktop/Native PEBL Behavior

For desktop PEBL, researchers can still use different parameter files directly:

```bash
# Use default parameters (from params/ directory)
bin/pebl2 battery/corsi/corsi.pbl --pfile params/corsi.pbl.par.json

# Use custom parameter set
bin/pebl2 battery/corsi/corsi.pbl --pfile params/corsi-fast.pbl.par.json

# Use remote parameter URL
bin/pebl2 battery/corsi/corsi.pbl --pfile "http://server/api/params.php?token=X&test=corsi&variant=fast"
```

The `--pfile` argument accepts:
- Local file paths (gets "params/" prefix if not a URL)
- Full HTTP/HTTPS URLs (used as-is, no prefix)

## Files Modified

### 1. `/bin/PEBLDataServer/manage_tokens_web.php`
- Removed ugly encoded parameter URLs
- Auto-detects parameter variants from filesystem
- Shows clean URLs with `&param=` parameter
- Links to configure parameters for each test
- Updated CSS for better multi-row textarea display

### 2. `/bin/PEBLDataServer/configure_params.php`
- Added variant selector showing existing parameter sets
- Added form to create new parameter variants
- Shows which variant is currently being edited
- Updated header to display variant name

### 3. `/bin/pebl-launcher.html`
- Added `param` to URL parameter parsing
- Constructs `--pfile` URL internally from token, test, and param
- Uses relative URL to avoid CORS issues
- Passes constructed URL to `Module.callMain()`

### 4. `/pebl-lib/Utility.pbl` and `/emscripten/pebl-lib/Utility.pbl`
- Added `IsURL()` helper function
- Added `FetchText()` to fetch from URLs or local files
- Updated `CreateParameters()` to support URL-based parameters
- Graceful error handling with fallback to defaults

### 5. `/src/apps/PEBL.cpp`
- Fixed `--pfile` to not prepend "params/" to URLs
- Detects http:// and https:// URLs
- Backward compatible with local file paths

## Complete Workflow

### Researcher Setup
1. **Create Study Token** (via manage_tokens_web.php)
   - Select tests to include
   - Set expiration, max participants, etc.

2. **Configure Parameters** (via configure_params.php)
   - Create "default" parameter set with standard values
   - Create "practice" variant with easier settings
   - Create "fast" variant with shorter timings
   - Save each variant

3. **Share URLs** (from manage_tokens_web.php)
   - Copy clean URLs for each variant
   - Generate QR codes
   - Share with participants

### Participant Experience
1. **Open URL:**
   ```
   http://server/pebl-launcher.html?test=corsi&token=STUDY_ABC&participant=P001&param=fast
   ```

2. **Launcher Process:**
   - Validates token with server
   - Shows study info
   - Constructs parameter URL: `/api/params.php?token=STUDY_ABC&test=corsi&variant=fast`
   - Launches PEBL with: `--pfile /api/params.php?token=STUDY_ABC&test=corsi&variant=fast`

3. **PEBL Execution:**
   - Detects URL in `--pfile`
   - Calls `CreateParameters()` with URL
   - `FetchText()` fetches JSON via `GetHTTPText()`
   - Merges with defaults
   - Test runs with remote configuration

## Benefits

✅ **Clean URLs** - No ugly URL encoding
✅ **Multiple Variants** - Unlimited parameter sets per test
✅ **Easy Management** - Web UI for creating and editing variants
✅ **Backward Compatible** - Desktop PEBL still works with local files
✅ **Same-Origin Security** - No CORS issues
✅ **Graceful Fallback** - Uses defaults if fetch fails
✅ **QR Code Friendly** - Short URLs work well in QR codes

## Testing

### Test 1: Default Parameters
```
URL: http://localhost:8080/pebl-launcher.html?test=corsi&token=STUDY_ABC&participant=P001

Expected:
- Fetches: /api/params.php?token=STUDY_ABC&test=corsi
- Loads: data/STUDY_ABC/corsi.par.json
- Or uses schema defaults if file doesn't exist
```

### Test 2: Variant Parameters
```
URL: http://localhost:8080/pebl-launcher.html?test=corsi&token=STUDY_ABC&participant=P001&param=fast

Expected:
- Fetches: /api/params.php?token=STUDY_ABC&test=corsi&variant=fast
- Loads: data/STUDY_ABC/corsi-fast.par.json
- Falls back to defaults if variant doesn't exist
```

### Test 3: Desktop Native
```bash
bin/pebl2 battery/corsi/corsi.pbl --pfile "http://localhost:8080/api/params.php?token=STUDY_ABC&test=corsi&variant=fast"

Expected:
- No "params/" prefix added (URL detected)
- Fetches from URL via GetHTTPText()
- Parses JSON parameters
- Test runs with remote config
```

## Summary

The parameter variant system provides a clean, flexible way to manage multiple configurations per test. Researchers can create unlimited variants (practice, fast, slow, etc.) through a web interface, and participants access them via clean, shareable URLs. The system works seamlessly across web and desktop PEBL deployments.

**The complete URL parameter loading and variant system is now operational! 🎉**
