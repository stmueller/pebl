# JSON Parameter Format for PEBL Tests

## Overview

PEBL now supports JSON format for test parameter files, in addition to the legacy CSV format. This provides better integration with web platforms and modern tooling.

## File Formats

### JSON Parameter File (`.par.json`)

**Format:** Simple key-value pairs

**Example:** `corsi.pbl.par.json`
```json
{
  "dopractice": 1,
  "direction": 1,
  "isi": 1200,
  "iti": 1000,
  "usebeep": 1
}
```

**Advantages:**
- Easy to read and edit
- Web-friendly (can be generated/parsed by JavaScript)
- Supports all JSON data types (numbers, strings, booleans, arrays, objects)
- Can be fetched via URLs using `--pfile` command-line argument

### Legacy CSV Parameter File (`.par`)

**Format:** `parameter,value` (one per line)

**Example:** `corsi.pbl.par`
```
dopractice,1
direction,1
isi,1200
iti,1000
usebeep,1
```

**Note:** Legacy format is still fully supported for backward compatibility.

## Usage in PEBL Code

The `CreateParameters()` function automatically detects the file format based on the file extension:

```pebl
## Define default parameters
parpairs <- [["dopractice",1],
             ["direction",1],
             ["isi",1000]]

## Load parameters - automatically detects .json or .par format
gParams <- CreateParameters(parpairs, gParamFile)

## Access parameters
if(gParams.dopractice == 1)
{
    ## Run practice trials
}

Wait(gParams.isi)
```

## Auto-Detection Logic

- **Files ending in `.json`**: Parsed as JSON format
- **All other files**: Parsed as legacy CSV format
- **File doesn't exist**: Uses default values from `parpairs`

## Parameter Merging

Parameters are merged in this order (later values override earlier):

1. **Default values** from hardcoded `parpairs` in the test script
2. **File values** from the parameter file (`.par` or `.par.json`)

This ensures:
- Tests always have valid parameters (from defaults)
- Researchers can customize specific parameters without specifying all
- Missing parameters in files don't cause errors

## Using with Web Platforms

### Remote Parameter Loading

PEBL can fetch parameter files from URLs using the `--pfile` command-line argument:

```bash
./pebl2 battery/corsi/corsi.pbl --pfile http://server.com/api/params?token=STUDY123&test=corsi
```

The web server can:
1. Read the test's `.pbl.schema.json` file to understand available parameters
2. Generate a custom `.par.json` file based on study configuration
3. Serve it to PEBL via HTTP

### Example Web Workflow

```
1. Researcher configures study on web platform:
   - ISI: 1200ms (changed from default 1000ms)
   - Direction: Backward

2. Web platform generates custom parameter file:
   {
     "dopractice": 1,
     "direction": -1,
     "isi": 1200
   }

3. Launcher passes URL to PEBL:
   --pfile https://pebl-platform.com/api/params?token=ABC123&test=corsi

4. PEBL fetches JSON, merges with defaults, runs test
```

## Converting Existing Tests

No code changes are required in existing battery tests! The `CreateParameters()` function is backward compatible.

**To add JSON support:**

1. Create JSON parameter file alongside existing `.par` file:
   ```bash
   # Current files:
   battery/corsi/params/corsi.pbl.par
   battery/corsi/params/corsi.pbl.schema

   # Add JSON versions:
   battery/corsi/params/corsi.pbl.par.json
   battery/corsi/params/corsi.pbl.schema.json
   ```

2. Test continues to work with either format!

## Testing

Test the JSON parameter support:

```bash
# Run the test script
./bin/pebl2 test-json-params-simple.pbl

# Tests verify:
# 1. JSON parameter files load correctly
# 2. Defaults are used when file doesn't exist
# 3. Legacy CSV format still works
```

## Schema Files (Future)

JSON schema files (`.pbl.schema.json`) provide rich metadata for web UIs:

```json
{
  "test": "corsi",
  "version": "1.0",
  "description": "Corsi Block Tapping Test",
  "parameters": [
    {
      "name": "isi",
      "type": "integer",
      "default": 1000,
      "label": "Inter-Stimulus Interval",
      "description": "Time between stimuli in milliseconds",
      "min": 0,
      "max": 5000,
      "step": 100,
      "unit": "ms"
    }
  ]
}
```

This enables web platforms to:
- Auto-generate configuration forms
- Validate parameter values
- Provide helpful UI elements (sliders, dropdowns, etc.)
- Show parameter descriptions and units

## Migration Checklist

For each battery test:

- [ ] Test works with existing `.par` files (no changes needed)
- [ ] Create `.par.json` version for web compatibility
- [ ] Create `.schema.json` version with rich metadata
- [ ] Test with both formats
- [ ] Test with remote URL loading

## Summary

✅ **Backward compatible** - existing tests work unchanged
✅ **Web-friendly** - JSON format for modern platforms
✅ **Auto-detection** - no code changes required
✅ **Remote loading** - fetch parameters from URLs
✅ **Default fallback** - tests always have valid parameters
