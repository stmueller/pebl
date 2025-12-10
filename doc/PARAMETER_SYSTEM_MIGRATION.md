# PEBL Parameter System Migration to JSON

## Overview

Migrating from the current .par/.schema format to JSON for better web integration and modern tooling.

## Current System

**Parameter file (`.par`):**
```
dopractice,1
direction,1
isi,1000
```

**Schema file (`.pbl.schema`):**
```
dopractice|1|Whether to do practice round
direction|1|Direction of recall (forward=1, backward=-1)
isi|1000|Inter-stimulus interval in ms
```

**In PEBL code:**
```pebl
parpairs <- [["dopractice",1], ["direction",1], ["isi",1000]]
gParams <- CreateParameters(parpairs, gParamFile)
```

## New JSON System

### 1. Schema File (`test.pbl.schema.json`)

Describes available parameters with metadata:

```json
{
  "test": "corsi",
  "version": "1.0",
  "description": "Corsi Block Tapping Test",
  "parameters": [
    {
      "name": "dopractice",
      "type": "boolean",
      "default": 1,
      "label": "Practice Trials",
      "description": "Whether to do a short practice round",
      "options": [0, 1]
    },
    {
      "name": "direction",
      "type": "select",
      "default": 1,
      "label": "Direction",
      "description": "Direction of recall",
      "options": [
        {"value": 1, "label": "Forward"},
        {"value": -1, "label": "Backward"}
      ]
    },
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

### 2. Parameter File (`test.pbl.par.json`)

Simple key-value pairs:

```json
{
  "dopractice": 1,
  "direction": 1,
  "isi": 1200,
  "iti": 1000,
  "usebeep": 1
}
```

### 3. Updated PEBL Code

Backward compatible - works with both formats:

```pebl
parpairs <- [["dopractice",1], ["direction",1], ["isi",1000]]
gParams <- CreateParameters(parpairs, gParamFile)
// Automatically detects .par vs .par.json
```

## Implementation Plan

### Phase 1: Update Utility.pbl (Week 1)

**Add new functions:**
- `ReadJSONParameters(filename)` - Read .par.json files
- `ReadJSONSchema(filename)` - Read .schema.json files
- Update `CreateParameters()` to auto-detect file type

**Changes to CreateParameters():**
```pebl
define CreateParameters(defaults, file)
{
    // Create base parameter object from defaults
    if(IsList(defaults)) {
        par1 <- MakeParameterObject(defaults)
    } else {
        par1 <- MakeCustomObject("parameters")
    }

    // Read file if it exists
    if(FileExists(file)) {
        // Check if JSON or CSV
        if(EndsWith(file, ".json")) {
            par2 <- ReadJSONParameters(file)
        } else {
            // Legacy CSV format
            parampairs <- ReadCSV(file)
            par2 <- MakeParameterObject(parampairs)
        }

        // Merge file parameters over defaults
        props <- GetPropertyList(par2)
        loop(p, props) {
            SetProperty(par1, p, GetProperty(par2, p))
        }
    }

    return par1
}

define ReadJSONParameters(filename)
{
    // Read JSON file and convert to parameter object
    jsontext <- ReadFile(filename)
    jsondata <- FromJSON(jsontext)  // Built-in PEBL function

    parameters <- MakeCustomObject("parameters")
    keys <- GetPropertyList(jsondata)
    loop(key, keys) {
        SetProperty(parameters, key, GetProperty(jsondata, key))
    }

    return parameters
}
```

### Phase 2: Convert Schema Files (Week 1-2)

**Create conversion script:** `convert-schemas.sh`

Converts all `.pbl.schema` → `.pbl.schema.json`

```bash
#!/bin/bash
for schema in upload-battery/*/params/*.pbl.schema; do
    python3 convert_schema_to_json.py "$schema"
done
```

**Python conversion tool:** `convert_schema_to_json.py`

```python
#!/usr/bin/env python3
import sys
import json
import os

def convert_schema(schema_file):
    """Convert .pbl.schema to .pbl.schema.json"""

    params = []
    test_name = os.path.basename(schema_file).replace('.pbl.schema', '')

    with open(schema_file, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue

            parts = line.split('|', 2)
            if len(parts) == 3:
                name, default, description = [p.strip() for p in parts]

                # Infer type from default value
                param_type = infer_type(default)

                param = {
                    "name": name,
                    "type": param_type,
                    "default": convert_value(default, param_type),
                    "description": description
                }

                params.append(param)

    schema_json = {
        "test": test_name,
        "version": "1.0",
        "parameters": params
    }

    output_file = schema_file + '.json'
    with open(output_file, 'w') as f:
        json.dump(schema_json, f, indent=2)

    print(f"Converted: {schema_file} → {output_file}")

def infer_type(value):
    """Infer parameter type from default value"""
    if value in ['0', '1']:
        return 'boolean'
    try:
        if '.' in value:
            float(value)
            return 'float'
        else:
            int(value)
            return 'integer'
    except ValueError:
        return 'string'

def convert_value(value, param_type):
    """Convert string value to appropriate type"""
    if param_type == 'boolean':
        return int(value)
    elif param_type == 'integer':
        return int(value)
    elif param_type == 'float':
        return float(value)
    else:
        return value

if __name__ == '__main__':
    convert_schema(sys.argv[1])
```

### Phase 3: Update Battery Tests (Week 2)

**No code changes required!**

Tests continue to work exactly as before because `CreateParameters()` is backward compatible.

Optional: Convert parameter definitions to load from schema:

```pebl
// OLD (hardcoded):
parpairs <- [["dopractice",1], ["direction",1], ...]
gParams <- CreateParameters(parpairs, gParamFile)

// NEW (load from schema):
gParams <- CreateParametersFromSchema("corsi", gParamFile)
```

### Phase 4: Web Platform Integration (Week 3)

Now the web platform can:

1. **Read schemas** - Parse `.pbl.schema.json` to build configuration UI
2. **Generate parameter files** - Create custom `.par.json` files per study
3. **Serve via URL** - PEBL tests fetch parameters via `--pfile http://...`

**Example workflow:**
```
1. Researcher configures test on web:
   - ISI: 1200ms (changed from default 1000ms)
   - Direction: Backward

2. Web platform generates:
   data/STUDY_ABC123/config/corsi.pbl.par.json

3. Launcher passes to PEBL:
   --pfile http://server.com/api/params?token=STUDY_ABC123&test=corsi

4. PEBL fetches JSON, loads parameters, runs test
```

## Migration Timeline

### Week 1: Core Infrastructure
- ✅ Update Utility.pbl with JSON support
- ✅ Create conversion tools
- ✅ Convert all schema files
- ✅ Test backward compatibility

### Week 2: Battery Updates
- Update tests to use schema-based loading (optional)
- Create sample .par.json files
- Test all battery tests with both formats

### Week 3: Web Integration
- Build parameter configuration UI
- Implement parameter file generation API
- Update launcher to fetch remote parameters
- End-to-end testing

## Testing Strategy

**Test backward compatibility:**
```bash
# Test with legacy .par files
./pebl2 battery/corsi/corsi.pbl --pfile battery/corsi/params/corsi.pbl.par

# Test with new .par.json files
./pebl2 battery/corsi/corsi.pbl --pfile battery/corsi/params/corsi.pbl.par.json

# Test with remote URL
./pebl2 battery/corsi/corsi.pbl --pfile http://localhost:8080/api/params?test=corsi
```

## Benefits

1. **Web UI** - Auto-generate configuration forms from schemas
2. **Validation** - Type checking, min/max, options enforcement
3. **Documentation** - Self-documenting parameters
4. **Flexibility** - Easy to extend with new metadata fields
5. **Compatibility** - Old .par files still work

## File Structure

```
upload-battery/corsi/
├── corsi.pbl
├── params/
│   ├── corsi.pbl.schema        # Legacy (keep for reference)
│   ├── corsi.pbl.schema.json   # NEW: Rich schema
│   ├── corsi.pbl.par            # Legacy default params
│   └── corsi.pbl.par.json       # NEW: JSON default params
```

## Next Steps

1. Implement `ReadJSONParameters()` in Utility.pbl
2. Test with one battery test (corsi)
3. Convert all schema files
4. Update emscripten build
5. Integrate with web platform
