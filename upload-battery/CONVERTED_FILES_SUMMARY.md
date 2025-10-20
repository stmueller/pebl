# JSON Conversion Summary - upload-battery/

## Conversion Complete ✓

All parameter and schema files in the `upload-battery/` directory have been converted to JSON format.

**Date:** 2025-10-16
**Tool:** `convert_params_to_json.py`
**Files converted:** 12 total (11 schemas + 1 parameter file)

---

## Schema Files Converted (.pbl.schema → .pbl.schema.json)

| Test | Path | Parameters | Status |
|------|------|------------|--------|
| ANT | `ANT.pbl.schema.json` | 7 | ✓ |
| BART | `BART/params/bart.pbl.schema.json` | 13 | ✓ |
| BCST-64 | `bcst/params/bcst-64.pbl.schema.json` | 3 | ✓ |
| Card Sort | `bcst/params/cardsort.pbl.schema.json` | 6 | ✓ |
| Corsi | `corsi/params/corsi.pbl.schema.json` | 10 | ✓ |
| Digit Span | `dspan/params/digitspan.pbl.schema.json` | 15 | ✓ |
| Face Ratings | `face-ratings/params/faces.pbl.schema.json` | 7 | ✓ |
| Flanker | `flanker/params/flanker.pbl.schema.json` | 9 | ✓ |
| Go/No-Go | `gonogo/params/gonogo.pbl.schema.json` | 9 | ✓ |
| P-Trails | `ptrails/params/ptrails.pbl.schema.json` | 13 | ✓ |
| Spatial Grid | `spatialgrid/params/PCST.pbl.schema.json` | 12 | ✓ |

**Total: 104 parameters across 11 tests**

---

## Parameter Files Converted (.pbl.par → .pbl.par.json)

| Test | Path | Parameters | Status |
|------|------|------------|--------|
| ANT | `ANT.pbl.par.json` | 7 | ✓ |

---

## Validation Results

All converted files have been tested and validated:

### ✓ Test 1: Schema JSON Parsing
- Schema files parse correctly with `ParseJSON()`
- All metadata preserved: test name, version, parameter descriptions
- Type information correctly inferred (boolean, integer, float, string)

### ✓ Test 2: Parameter JSON with CreateParameters()
- Parameter files work with `CreateParameters()` function
- Values correctly loaded and merged with defaults
- JSON format auto-detected by `.json` extension

### ✓ Test 3: Backward Compatibility
- Original `.par` files continue to work unchanged
- Legacy CSV format still fully supported
- No breaking changes to existing tests

---

## JSON Schema Format

The converted schema files use this rich metadata structure:

```json
{
  "test": "corsi",
  "version": "1.0",
  "description": "CORSI Test",
  "parameters": [
    {
      "name": "dopractice",
      "type": "boolean",
      "default": 1,
      "description": "Whether to do a short practice round...",
      "options": [0, 1]
    },
    {
      "name": "isi",
      "type": "integer",
      "default": 1000,
      "description": "Inter-stimulus interval in ms."
    }
  ]
}
```

**Fields:**
- `name` - Parameter identifier
- `type` - Data type: `boolean`, `integer`, `float`, or `string`
- `default` - Default value (properly typed for JSON)
- `description` - Human-readable description
- `options` - Available values (for booleans and enums)

---

## JSON Parameter Format

The converted parameter files use simple key-value pairs:

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

**Advantages:**
- Web-friendly (JavaScript can easily parse/generate)
- Type-safe (numbers are numbers, strings are strings)
- Can be fetched via URLs for web-based testing
- Human-readable and easy to edit

---

## Usage Examples

### In PEBL Code (No Changes Required!)

```pebl
## Works with both .par and .par.json
parpairs <- [["dopractice",1], ["isi",1000]]
gParams <- CreateParameters(parpairs, gParamFile)
```

### Command-Line Usage

```bash
# Local JSON file
./pebl2 battery/corsi/corsi.pbl --pfile params/corsi.pbl.par.json

# Remote URL (for web platform)
./pebl2 battery/corsi/corsi.pbl --pfile https://server.com/api/params?token=XYZ
```

### Web Platform Integration

```javascript
// Fetch schema to build configuration UI
const schema = await fetch('battery/corsi/params/corsi.pbl.schema.json');
const config = await schema.json();

// Generate parameter file based on user choices
const params = {
  dopractice: 1,
  isi: 1200,  // User customized this
  iti: 800    // User customized this
};

// Serve to PEBL via API
res.json(params);
```

---

## File Locations

All converted files are located alongside their original versions:

```
upload-battery/
├── ANT.pbl.par             (original)
├── ANT.pbl.par.json        (NEW - converted)
├── ANT.pbl.schema          (original)
├── ANT.pbl.schema.json     (NEW - converted)
├── corsi/
│   └── params/
│       ├── corsi.pbl.schema        (original)
│       └── corsi.pbl.schema.json   (NEW - converted)
└── [other tests...]
```

**Note:** Original files are preserved for backward compatibility.

---

## Next Steps

1. **Create .par.json files for all tests**
   - Currently only ANT has a .par file
   - Can create default .par.json from schema defaults

2. **Web Platform Integration**
   - Use schema files to generate configuration UI
   - Validate user inputs against schema metadata
   - Generate custom .par.json per study

3. **Enhanced Metadata (Optional)**
   - Add `min`, `max`, `step` for numeric parameters
   - Add `unit` for parameters with units (ms, px, etc.)
   - Add `label` for friendly UI display names

4. **Testing**
   - Test each battery test with JSON parameters
   - Verify remote URL loading works
   - Test web platform parameter generation

---

## Conversion Script

To convert additional files:

```bash
# Single file
python3 convert_params_to_json.py battery/test/params/test.pbl.schema

# Entire directory
python3 convert_params_to_json.py battery/

# All upload-battery files
python3 convert_params_to_json.py --all
```

Script location: `convert_params_to_json.py`

---

## Support

For questions about:
- **JSON format**: See `JSON_PARAMETER_FORMAT.md`
- **Migration plan**: See `PARAMETER_SYSTEM_MIGRATION.md`
- **Conversion script**: Run `python3 convert_params_to_json.py --help`


## Notes by S Mueller
Retain these notes when updating this file.

* ptrails task: includes text to ask instructor
* Does not show simple data output at end
* data records do not have a column for 'practice' and 'test'  or record trial number (should appear in main battery too)
