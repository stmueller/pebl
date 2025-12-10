# Launcher Default to JSON Format - Final Fix

## Date: 2025-10-16

## Issue

When creating a new parameter file without specifying an extension (typing just "custom" instead of "custom.par.json"), the launcher was automatically adding ".par" extension and saving in legacy CSV format instead of JSON format.

## Root Cause

Two locations in the code had hardcoded ".par" as the default extension:
1. Line 2735-2736: SetParameters() function - when saving from "Set Parameters" dialog
2. Line 2930-2931: "New parameter set" button handler

## Solution

Changed the default extension from ".par" to ".par.json" in both locations so that new parameter files are created in JSON format by default.

---

## Changes Made

### 1. SetParameters() Function (Lines 2733-2737)

**Before:**
```pebl
if(not hasParExtension)
{
  parname <- parname + ".par"
  paramsfile <- paramsfile + ".par"
}
```

**After:**
```pebl
if(not hasParExtension)
{
  parname <- parname + ".par.json"
  paramsfile <- paramsfile + ".par.json"
}
```

### 2. "New Parameter Set" Handler (Lines 2928-2932)

**Before:**
```pebl
if(not hasParExt)
{
  parname <- parname + ".par"
  paramsfile <- paramsfile + ".par"
}
```

**After:**
```pebl
if(not hasParExt)
{
  parname <- parname + ".par.json"
  paramsfile <- paramsfile + ".par.json"
}
```

---

## New Behavior

### Creating Parameter Files:

1. **Type just a name:** `custom`
   - Result: `custom.par.json` (JSON format) ✅

2. **Type with .par.json extension:** `custom.par.json`
   - Result: `custom.par.json` (JSON format) ✅

3. **Type with .par extension:** `custom.par`
   - Result: `custom.par` (legacy CSV format) ✅

### The launcher now:
- ✅ Defaults to JSON format for new parameter files
- ✅ Respects explicit `.par` extension if user wants legacy format
- ✅ Accepts `.par.json` extension explicitly
- ✅ Writes correct format based on file extension

---

## Testing

### Test 1: Create parameter file without extension
1. Open launcher, navigate to test
2. Click "New parameter set"
3. Type: `custom`
4. Click save
5. **Expected:** Creates `custom.par.json` with JSON content ✅

### Test 2: Create parameter file with .par extension
1. Click "New parameter set"
2. Type: `custom.par`
3. Click save
4. **Expected:** Creates `custom.par` with CSV content ✅

### Test 3: Create parameter file with .par.json extension
1. Click "New parameter set"
2. Type: `custom.par.json`
3. Click save
4. **Expected:** Creates `custom.par.json` with JSON content ✅

### Test 4: Save from Set Parameters dialog
1. Click "Set Parameters"
2. Modify a value
3. Type filename: `modified`
4. Click "Save and return"
5. **Expected:** Creates `modified.par.json` with JSON content ✅

---

## File Format Reference

### JSON Format (.par.json):
```json
{
  "iti": 400,
  "showfooter": 1,
  "leftresponse": "<lshift>",
  "rightresponse": "<rshift>",
  "reps": 2,
  "showRT": 0,
  "showNeutralFlankers": 0
}
```

### Legacy CSV Format (.par):
```
iti,400
showfooter,1
leftresponse,<lshift>
rightresponse,<rshift>
reps,2
showRT,0
showNeutralFlankers,0
```

---

## Note on FilePrint vs FilePrint_

- `FilePrint(file, text)` - Writes text with newline at end
- `FilePrint_(file, text)` - Writes text without newline

For JSON files, we use `FilePrint_(pfile, JSONText(jsonObj))` to write the JSON content without adding extra newlines, keeping the JSON properly formatted.

---

## Complete JSON Support Summary

The launcher now has complete JSON parameter support:

✅ Reads `.schema.json` files (preferentially over `.schema`)
✅ Reads `.par.json` files (via CreateParameters in Utility.pbl)
✅ Lists `.par.json` files in parameter dropdown
✅ **Defaults to JSON format for new parameter files**
✅ Writes JSON format when extension is `.par.json`
✅ Writes CSV format when extension is `.par`
✅ 100% backward compatible with legacy files

---

## Migration Path

For researchers and test developers:

1. **New tests:** Use `.par.json` and `.schema.json` exclusively
2. **Existing tests:** Can continue using `.par` and `.schema` files
3. **Converting:** Use `convert_params_to_json.py` script to convert legacy files
4. **Launcher:** Now creates JSON files by default, making migration seamless

---

## Related Documentation

- `LAUNCHER_JSON_COMPLETE_FIX.md` - All launcher JSON updates
- `CONVERSION_COMPLETE.md` - Utility.pbl JSON support
- `JSON_PARAMETER_FORMAT.md` - JSON format specification
- `QUICK_START_JSON_PARAMS.md` - Quick reference

---

## Files Modified

- `bin/launcher.pbl` (lines 2733-2737, 2928-2932) - Changed default extension to `.par.json`

## Result

🎉 New parameter files are now created in JSON format by default!
