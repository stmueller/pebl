# URL Parameter File Fix - Implementation Complete

**Date:** October 16, 2025
**Status:** ✅ COMPLETE

## Problem Identified

When using `--pfile` with a URL, PEBL was incorrectly prepending "params/" to the argument, resulting in malformed URLs like:

```
params/http://localhost:8080/PEBLDataServer/api/params.php?token=ADMIN_STUDY_001
```

Instead of the correct:

```
http://localhost:8080/PEBLDataServer/api/params.php?token=ADMIN_STUDY_001
```

This prevented URL-based parameter loading from working with the `--pfile` command-line argument.

## Root Cause

In `src/apps/PEBL.cpp` line 475, the code unconditionally prepended "params/" to any `--pfile` argument:

```cpp
else if(strcmp(argv[j].c_str(),"--pfile")==0)
{
    pfile = Variant("params/") +Variant(argv[++j]);
}
```

## Solution Implemented

Modified `src/apps/PEBL.cpp` to check if the argument is a URL before prepending the directory:

```cpp
else if(strcmp(argv[j].c_str(),"--pfile")==0)
{
    std::string pfileArg = argv[++j];
    // Check if it's a URL - if so, use it directly without prepending "params/"
    if(pfileArg.compare(0, 7, "http://") == 0 || pfileArg.compare(0, 8, "https://") == 0)
    {
        pfile = Variant(pfileArg);
    }
    else
    {
        pfile = Variant("params/") + Variant(pfileArg);
    }
}
```

**Logic:**
1. Check if argument starts with "http://" or "https://"
2. If YES: Use URL directly (no "params/" prefix)
3. If NO: Prepend "params/" (legacy behavior for local files)

## Complete URL Parameter Loading System

With this fix, the complete URL parameter loading system now works end-to-end:

### 1. C++ Code (`src/apps/PEBL.cpp`)
- Detects URLs in `--pfile` argument
- Skips "params/" prefix for URLs
- Sets `gParamFile` global variable

### 2. PEBL Library (`pebl-lib/Utility.pbl`)
- `IsURL()` - Detects http:// and https:// URLs
- `FetchText()` - Fetches from URLs via `GetHTTPText()` or local files via `FileReadText()`
- `ReadJSONParameters()` - Uses `FetchText()` to support both URLs and local files
- `CreateParameters()` - Checks `IsURL()` before `FileExists()`

### 3. Web Platform Integration
- Launcher passes URL via `--pfile` flag
- Server-side parameter API serves JSON
- Token-based parameter isolation

## Usage Examples

### Command Line (Native PEBL)

```bash
# Local parameter file (legacy behavior - prepends "params/")
bin/pebl2 battery/corsi/corsi.pbl --pfile corsi.pbl.par.json

# URL parameter file (new behavior - uses URL directly)
bin/pebl2 battery/corsi/corsi.pbl --pfile "http://localhost:8080/api/params.php?token=STUDY123&test=corsi"
```

### Web Platform (Emscripten)

JavaScript launcher passes URL to PEBL:

```javascript
Module.callMain([
    'battery/corsi/corsi.pbl',
    '--pfile',
    'http://localhost:8080/api/params.php?token=STUDY123&test=corsi'
]);
```

PEBL now correctly:
1. Recognizes the URL (no "params/" prefix)
2. Passes URL to `CreateParameters()`
3. `FetchText()` detects URL and calls `GetHTTPText()`
4. JSON parameters loaded from server
5. Test runs with remote configuration

## Files Modified

1. **`src/apps/PEBL.cpp`** (lines 473-485)
   - Added URL detection in `--pfile` processing
   - Only prepends "params/" for non-URL arguments

2. **`pebl-lib/Utility.pbl`**
   - Added `IsURL()` function (lines 1493-1513)
   - Added `FetchText()` function (lines 1516-1595)
   - Updated `ReadJSONParameters()` to use `FetchText()` (lines 1598-1619)
   - Updated `CreateParameters()` to check `IsURL()` (lines 1622-1676)

3. **`emscripten/pebl-lib/Utility.pbl`**
   - Copied all changes from pebl-lib/Utility.pbl

## Build Instructions

**Native build:**
```bash
make clean && make main
```

**Emscripten build:**
```bash
make clean && make em
make fp  # Repackage pebl-lib after changes
```

## Testing

### Test 1: Local Parameter File (Legacy)
```bash
bin/pebl2 battery/corsi/corsi.pbl --pfile corsi.pbl.par.json
```
**Expected:** Loads from `params/corsi.pbl.par.json` (params/ prepended)

### Test 2: URL Parameter File (New)
```bash
bin/pebl2 battery/corsi/corsi.pbl --pfile "http://localhost:8080/api/params.php?token=STUDY123&test=corsi"
```
**Expected:** Fetches from URL (no params/ prefix)

### Test 3: Verify gParamFile Value
Add to test script:
```pebl
Print("gParamFile: " + gParamFile)
```

**Expected output for URL:**
```
gParamFile: http://localhost:8080/api/params.php?token=STUDY123&test=corsi
```

**NOT:**
```
gParamFile: params/http://localhost:8080/api/params.php?token=STUDY123&test=corsi
```

## Backward Compatibility

✅ **Fully backward compatible:**
- Local files still work (params/ prepended as before)
- Legacy `.par` format still supported
- JSON `.par.json` format still supported
- Only adds URL support as new functionality

## Integration with Web Platform

This fix completes the token-based parameter configuration system:

1. **Researcher configures parameters** → Web interface saves to `data/{token}/{test}.par.json`
2. **Participant visits study URL** → JavaScript launcher constructs parameter URL
3. **Launcher calls PEBL** → Passes URL via `--pfile`
4. **PEBL detects URL** → No "params/" prefix (this fix!)
5. **CreateParameters() loads** → FetchText() fetches from URL
6. **Test runs** → With remote configuration

## Summary

✅ Fixed C++ code to detect URLs in `--pfile` argument
✅ URLs no longer get "params/" prefix
✅ Local files still get "params/" prefix (backward compatible)
✅ Native build tested and working
✅ Complete URL parameter loading system operational

**The URL parameter loading feature now works correctly from command line to server! 🎉**
