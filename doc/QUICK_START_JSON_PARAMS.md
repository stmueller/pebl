# Quick Start: JSON Parameters

## For Researchers - Using JSON Parameters

### No Code Changes Required!

Your existing PEBL tests work with both `.par` and `.par.json` files automatically.

```pebl
## This works with BOTH formats:
parpairs <- [["dopractice",1], ["isi",1000]]
gParams <- CreateParameters(parpairs, gParamFile)
```

### Running Tests with JSON Parameters

```bash
# Use local JSON file
./pebl2 battery/corsi/corsi.pbl --pfile params/corsi.pbl.par.json

# Use remote URL (web platform)
./pebl2 battery/corsi/corsi.pbl --pfile https://your-server.com/api/params?token=XYZ
```

---

## For Test Developers - Converting Files

### Convert Existing Parameter Files

```bash
# Convert all files in upload-battery
python3 convert_params_to_json.py --all

# Convert specific test directory
python3 convert_params_to_json.py battery/mytast/

# Convert single file
python3 convert_params_to_json.py battery/mytest/params/mytest.pbl.schema
```

### Create New JSON Parameter File

**1. Create schema file:** `mytest.pbl.schema.json`
```json
{
  "test": "mytest",
  "version": "1.0",
  "description": "My Test Description",
  "parameters": [
    {
      "name": "trials",
      "type": "integer",
      "default": 10,
      "description": "Number of trials to run"
    },
    {
      "name": "isi",
      "type": "integer",
      "default": 1000,
      "description": "Inter-stimulus interval in ms"
    }
  ]
}
```

**2. Create parameter file:** `mytest.pbl.par.json`
```json
{
  "trials": 20,
  "isi": 1500
}
```

**3. Use in PEBL code:**
```pebl
defaults <- [["trials", 10], ["isi", 1000]]
gParams <- CreateParameters(defaults, "mytest.pbl.par.json")
```

That's it!

---

## For Web Platform Developers

### 1. Read Schema to Build UI

```javascript
// Fetch schema
const response = await fetch('battery/corsi/params/corsi.pbl.schema.json');
const schema = await response.json();

// Build form from schema
schema.parameters.forEach(param => {
  if (param.type === 'integer') {
    // Create number input
    createNumberInput(param.name, param.default, param.description);
  } else if (param.type === 'boolean') {
    // Create checkbox
    createCheckbox(param.name, param.default, param.description);
  }
});
```

### 2. Generate Custom Parameters

```javascript
// User customizes parameters in web UI
const userParams = {
  dopractice: 1,
  isi: 1200,  // User changed from default 1000
  iti: 800    // User changed from default 1000
};

// Save to server
await fetch('/api/studies/ABC123/config/corsi', {
  method: 'POST',
  body: JSON.stringify(userParams)
});
```

### 3. Serve Parameters to PEBL

```javascript
// API endpoint
app.get('/api/params', (req, res) => {
  const token = req.query.token;
  const test = req.query.test;

  // Load custom config from database
  const params = loadStudyConfig(token, test);

  // Serve as JSON
  res.json(params);
});
```

### 4. Launch PEBL with Remote Parameters

```html
<!-- Web launcher passes URL to PEBL -->
<script>
  const url = `https://your-server.com/api/params?token=${studyToken}&test=corsi`;
  launchPEBL({
    test: 'corsi',
    paramFile: url  // PEBL fetches this via HTTP
  });
</script>
```

---

## Common Tasks

### Check if JSON Support is Working

```bash
# Run the test script
./bin/pebl2 test-json-params-simple.pbl

# Should see:
# ✓ JSON parameter files load correctly
# ✓ Legacy .par files still work
```

### View Converted Files

```bash
# List all JSON parameter files
find upload-battery -name "*.pbl.*.json"

# View a schema file
cat upload-battery/corsi/params/corsi.pbl.schema.json | less

# View a parameter file
cat upload-battery/ANT.pbl.par.json
```

### Re-run Conversion (Overwrites)

```bash
# Remove existing JSON files
find upload-battery -name "*.pbl.*.json" -delete

# Re-convert
python3 convert_params_to_json.py --all
```

---

## File Naming Convention

```
test.pbl                    # Main test script
test.pbl.par                # Legacy parameter file (CSV)
test.pbl.par.json           # JSON parameter file (NEW)
test.pbl.schema             # Legacy schema file (pipe-delimited)
test.pbl.schema.json        # JSON schema file (NEW)
```

**Rule:** Add `.json` extension to legacy filename.

---

## Parameter Types

| Type | Example | JSON Value | Description |
|------|---------|------------|-------------|
| `boolean` | On/Off flags | `0` or `1` | Binary choices |
| `integer` | Trials, timing | `1000` | Whole numbers |
| `float` | Probabilities | `0.75` | Decimal numbers |
| `string` | Key names | `"<lshift>"` | Text values |

**Auto-detection:** Conversion script infers types from default values.

---

## Troubleshooting

### JSON File Not Loading

```pebl
## Check file exists
if(FileExists("mytest.pbl.par.json"))
{
    Print("File found!")
} else {
    Print("File NOT found - check path")
}

## Test JSON parsing directly
json <- ParseJSON(FileReadText("mytest.pbl.par.json"))
Print("Loaded: " + JSONText(json))
```

### Legacy File Still Being Used

```bash
# PEBL picks first matching file in this order:
# 1. Exact filename passed to --pfile
# 2. Auto-search: mytest.pbl.par then mytest.pbl.par.json

# Force JSON version:
./pebl2 test.pbl --pfile params/test.pbl.par.json
```

### Conversion Script Errors

```bash
# Check Python version (needs 3.6+)
python3 --version

# Validate JSON syntax
python3 -m json.tool upload-battery/ANT.pbl.par.json

# Re-run with verbose output
python3 convert_params_to_json.py upload-battery/ 2>&1 | tee conversion.log
```

---

## Examples by Use Case

### Local Testing
```bash
./pebl2 battery/corsi/corsi.pbl -v subnum=123
# Uses: battery/corsi/params/corsi.pbl.par (or .par.json if it exists)
```

### Custom Parameters
```bash
./pebl2 battery/corsi/corsi.pbl --pfile my-custom-params.json
# Uses: my-custom-params.json instead of default
```

### Web Platform
```bash
./pebl2 battery/corsi/corsi.pbl --pfile https://server.com/api/params?study=ABC
# PEBL fetches JSON from URL, uses those parameters
```

### Batch Processing
```bash
# Test all converted files
for test in upload-battery/*/params/*.pbl.schema.json; do
  echo "Testing: $test"
  python3 -m json.tool "$test" > /dev/null && echo "  ✓ Valid JSON"
done
```

---

## Need Help?

**Documentation:**
- Full guide: `JSON_PARAMETER_FORMAT.md`
- Migration plan: `PARAMETER_SYSTEM_MIGRATION.md`
- Complete status: `CONVERSION_COMPLETE.md`

**Examples:**
- Test scripts: `test-json-params-simple.pbl`, `test-converted-json.pbl`
- Example params: `test-params.pbl.par.json`

**Tools:**
- Conversion script: `python3 convert_params_to_json.py --help`

**Source Code:**
- Implementation: `pebl-lib/Utility.pbl` (search for "Parameter file handling")
