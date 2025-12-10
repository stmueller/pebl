# GetParFiles() Function Fix for JSON Support

## Issue

The original `GetParFiles()` function used glob patterns that could potentially:
1. Cause duplicates when matching both `.par` and `.par.json` files
2. Not properly list `.par.json` files in the launcher UI dropdown

## Solution

Updated the function to explicitly separate `.par` from `.par.json` files using the `EndsWith()` helper function.

## Changes Made to `bin/launcher.pbl` (Lines 3179-3210)

### Before:
```pebl
define GetParFiles()
{
 paramsdir <- DirListToText(gDirChain)+"params"
 if(FileExists(paramsdir))
  {
    parfiles <- Merge(["default"],
                       Second(FilterDir(GetDirectoryListing(paramsdir),
                              gDirChain,"*.par")))
  } else {
    parfiles <- []
  }

 return parfiles
}
```

**Problem:** Only looked for `*.par` files, missing `.par.json` files completely.

### After:
```pebl
define GetParFiles()
{
 paramsdir <- DirListToText(gDirChain)+"params"
 if(FileExists(paramsdir))
  {
    ## Get all .par* files
    allParFiles <- Second(FilterDir(GetDirectoryListing(paramsdir),
                              gDirChain,"*.par*"))

    ## Separate .par.json from .par files
    legacyParFiles <- []
    jsonParFiles <- []

    loop(f, allParFiles)
    {
      if(EndsWith(f, ".par.json"))
      {
        PushOnEnd(jsonParFiles, f)
      } elseif(EndsWith(f, ".par"))
      {
        PushOnEnd(legacyParFiles, f)
      }
    }

    ## Merge all parameter files together
    parfiles <- Merge(["default"], Merge(legacyParFiles, jsonParFiles))
  } else {
    parfiles <- []
  }

 return parfiles
}
```

**Solution:**
1. Gets all files matching `*.par*` pattern
2. Explicitly separates files ending with `.par.json` vs `.par`
3. Merges both lists together with "default" option
4. Avoids duplicates by checking `.par.json` first

## Expected Behavior

When the launcher opens a test's parameter selection:

1. **"default"** - Always appears (uses schema defaults)
2. **test.par** - Legacy CSV parameter files (if present)
3. **test.par.json** - JSON parameter files (if present)
4. **custom.par.json** - Any custom JSON parameter sets
5. **custom.par** - Any custom legacy parameter sets

## Testing with ANT Test

In `~/Documents/pebl-exp.2.1/battery/ANT/params/`:
- ✓ `ANT.pbl.schema.json` (will be detected by SetParameters)
- ✓ `ANT.pbl.par.json` (will appear in parameter dropdown)

### Test Steps:

1. Launch the launcher:
   ```bash
   cd ~/Documents/pebl-exp.2.1
   ./pebl launcher.pbl
   ```

2. Navigate to `battery/ANT/ANT.pbl`

3. Click "Set Parameters" button

4. Expected results:
   - Parameter dropdown should show:
     - "default"
     - "ANT.pbl.par.json" ← **THIS IS THE KEY TEST**

5. Select "ANT.pbl.par.json" - should load values:
   - iti: 400
   - showfooter: 1
   - leftresponse: <lshift>
   - rightresponse: <rshift>
   - reps: 2
   - showRT: 0
   - showNeutralFlankers: 0

6. Modify a parameter and click "Save and return"
   - Should update `ANT.pbl.par.json` in JSON format

7. Click "New parameter set", type name like "custom.par.json"
   - Should create new JSON parameter file
   - Should appear in dropdown on next open

## Summary

✅ Fixed `GetParFiles()` to list `.par.json` files
✅ Properly separates legacy `.par` from `.par.json`
✅ Avoids duplicates
✅ Maintains "default" option
✅ Backward compatible with legacy `.par` files

The launcher can now fully read, write, and select JSON parameter files!
