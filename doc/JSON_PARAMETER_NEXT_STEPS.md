# JSON Parameter System - Next Steps

## Current Status: ✅ Core Infrastructure Complete

### What We've Accomplished:

✅ **Phase 1: Core Implementation** (COMPLETE)
- Utility.pbl updated with JSON parameter support
- Launcher.pbl updated to read/write JSON schemas and parameters
- Auto-detection of JSON vs CSV formats
- 100% backward compatible

✅ **Phase 2: Schema Conversion** (COMPLETE)
- Converted 108 battery tests from `.schema` to `.schema.json`
- Created `convert_params_to_json.py` conversion tool
- Added type inference (boolean, integer, float, string)
- Removed old `.schema` files from repository

✅ **Phase 3: Testing & Validation** (COMPLETE)
- All JSON parameter loading tests pass
- Launcher correctly lists and loads JSON files
- Parameters save in JSON format by default
- Legacy `.par` files still work

---

## Next Steps: Web Platform Integration

### Priority 1: Enhanced Schema Metadata (Optional but Recommended)

The current schemas are functional but minimal. For a better web UI, enhance schemas with:

**Current schema:**
```json
{
  "name": "isi",
  "type": "integer",
  "default": 1000,
  "description": "Inter-stimulus interval in ms."
}
```

**Enhanced schema:**
```json
{
  "name": "isi",
  "type": "integer",
  "default": 1000,
  "label": "Inter-Stimulus Interval",
  "description": "Time between stimuli in milliseconds",
  "min": 0,
  "max": 5000,
  "step": 100,
  "unit": "ms",
  "group": "Timing Parameters"
}
```

**Additional metadata fields:**
- `label` - User-friendly display name
- `min` / `max` - Validation bounds for numeric inputs
- `step` - Increment for sliders/spinners
- `unit` - Display unit (ms, px, %, etc.)
- `group` - Organize parameters into sections
- `help` - Extended help text with examples

**Tool needed:** Update `convert_params_to_json.py` to add these fields (or manually enhance key tests)

---

### Priority 2: Generate Default Parameter Files

Create `.par.json` files from schema defaults for all tests.

**Why:** Provides complete, ready-to-use parameter sets for web platform.

**Script to create:**
```python
# generate_default_params.py
import json
import glob

for schema_file in glob.glob('battery/**/params/*.pbl.schema.json', recursive=True):
    with open(schema_file) as f:
        schema = json.load(f)

    # Extract defaults from schema
    defaults = {}
    for param in schema['parameters']:
        defaults[param['name']] = param['default']

    # Write to .par.json
    par_file = schema_file.replace('.schema.json', '.par.json')
    with open(par_file, 'w') as f:
        json.dump(defaults, f, indent=2)

    print(f"Created: {par_file}")
```

**Impact:** Each test has a complete default configuration ready to customize.

---

### Priority 3: Web Platform API Implementation

Build the backend API to serve parameters to PEBL tests.

#### 3.1 API Endpoint: Get Study Configuration

**Endpoint:** `GET /api/params`

**Parameters:**
- `token` - Study authentication token
- `test` - Test name (e.g., "corsi", "stroop")

**Response:**
```json
{
  "dopractice": 1,
  "isi": 1200,
  "iti": 800,
  "direction": 1
}
```

**Backend logic:**
1. Validate token
2. Load study configuration from database
3. Merge study-specific overrides with test defaults
4. Return JSON

**Example (Node.js/Express):**
```javascript
app.get('/api/params', async (req, res) => {
  const { token, test } = req.query;

  // Validate token
  const study = await validateStudyToken(token);
  if (!study) {
    return res.status(401).json({ error: 'Invalid token' });
  }

  // Load test defaults from schema
  const schemaPath = `battery/${test}/params/${test}.pbl.schema.json`;
  const schema = JSON.parse(fs.readFileSync(schemaPath));
  const defaults = schema.parameters.reduce((acc, p) => {
    acc[p.name] = p.default;
    return acc;
  }, {});

  // Load study-specific overrides
  const overrides = await db.getStudyConfig(study.id, test);

  // Merge and return
  const params = { ...defaults, ...overrides };
  res.json(params);
});
```

#### 3.2 API Endpoint: Save Study Configuration

**Endpoint:** `POST /api/config/:studyId/:test`

**Body:**
```json
{
  "isi": 1200,
  "iti": 800,
  "dopractice": 0
}
```

**Backend logic:**
1. Validate authentication
2. Validate parameters against schema
3. Save to database
4. Return success

#### 3.3 PEBL Integration

**Usage:**
```bash
./pebl2 battery/corsi/corsi.pbl --pfile "https://server.com/api/params?token=ABC123&test=corsi"
```

PEBL already supports HTTP fetching via `FileReadText()`, so this should work with current code!

**Test it:**
```bash
# Start local server serving params
python3 -m http.server 8080 &

# Create test parameter file
echo '{"dopractice": 0, "isi": 2000}' > params.json

# Test PEBL can fetch it
./bin/pebl2 test-remote-params.pbl --pfile "http://localhost:8080/params.json"
```

---

### Priority 4: Web UI for Parameter Configuration

Build the frontend to configure test parameters.

#### 4.1 Schema-Driven Form Generation

**Component: ParameterForm.jsx**
```javascript
import { useState, useEffect } from 'react';

export function ParameterForm({ test, studyId }) {
  const [schema, setSchema] = useState(null);
  const [params, setParams] = useState({});

  useEffect(() => {
    // Load schema
    fetch(`/battery/${test}/params/${test}.pbl.schema.json`)
      .then(r => r.json())
      .then(setSchema);

    // Load current params
    fetch(`/api/config/${studyId}/${test}`)
      .then(r => r.json())
      .then(setParams);
  }, [test, studyId]);

  if (!schema) return <div>Loading...</div>;

  return (
    <form onSubmit={handleSave}>
      <h2>{schema.description}</h2>

      {schema.parameters.map(param => (
        <ParameterInput
          key={param.name}
          param={param}
          value={params[param.name]}
          onChange={v => setParams({...params, [param.name]: v})}
        />
      ))}

      <button type="submit">Save Configuration</button>
    </form>
  );
}

function ParameterInput({ param, value, onChange }) {
  switch(param.type) {
    case 'boolean':
      return (
        <label>
          <input
            type="checkbox"
            checked={value}
            onChange={e => onChange(e.target.checked ? 1 : 0)}
          />
          {param.label || param.name}
          <span className="help">{param.description}</span>
        </label>
      );

    case 'integer':
      return (
        <label>
          {param.label || param.name}
          <input
            type="number"
            value={value}
            min={param.min}
            max={param.max}
            step={param.step || 1}
            onChange={e => onChange(parseInt(e.target.value))}
          />
          {param.unit && <span>{param.unit}</span>}
          <span className="help">{param.description}</span>
        </label>
      );

    case 'select':
      return (
        <label>
          {param.label || param.name}
          <select value={value} onChange={e => onChange(e.target.value)}>
            {param.options.map(opt => (
              <option key={opt.value} value={opt.value}>
                {opt.label}
              </option>
            ))}
          </select>
          <span className="help">{param.description}</span>
        </label>
      );

    default:
      return (
        <label>
          {param.label || param.name}
          <input
            type="text"
            value={value}
            onChange={e => onChange(e.target.value)}
          />
          <span className="help">{param.description}</span>
        </label>
      );
  }
}
```

#### 4.2 Parameter Validation

**Client-side:**
```javascript
function validateParams(params, schema) {
  const errors = {};

  schema.parameters.forEach(param => {
    const value = params[param.name];

    // Type validation
    if (param.type === 'integer' && !Number.isInteger(value)) {
      errors[param.name] = 'Must be an integer';
    }

    // Range validation
    if (param.min !== undefined && value < param.min) {
      errors[param.name] = `Must be >= ${param.min}`;
    }
    if (param.max !== undefined && value > param.max) {
      errors[param.name] = `Must be <= ${param.max}`;
    }

    // Options validation
    if (param.options && !param.options.includes(value)) {
      errors[param.name] = 'Invalid option';
    }
  });

  return errors;
}
```

**Server-side (Express middleware):**
```javascript
async function validateAgainstSchema(req, res, next) {
  const { test } = req.params;
  const schemaPath = `battery/${test}/params/${test}.pbl.schema.json`;
  const schema = JSON.parse(fs.readFileSync(schemaPath));

  const errors = validateParams(req.body, schema);
  if (Object.keys(errors).length > 0) {
    return res.status(400).json({ errors });
  }

  next();
}
```

---

### Priority 5: Testing & Deployment

#### 5.1 Test Remote Parameter Loading

**Test script: `test-remote-params.pbl`**
```pebl
define Start(p)
{
    Print("Testing remote parameter loading...")

    # Test with local HTTP server
    url <- "http://localhost:8080/api/params?test=corsi"

    defaults <- [["isi", 1000], ["iti", 1000]]
    params <- CreateParameters(defaults, url)

    Print("Loaded parameters:")
    Print("  isi: " + params.isi)
    Print("  iti: " + params.iti)

    if(params.isi > 0) {
        Print("✓ Remote parameter loading works!")
    }
}
```

#### 5.2 End-to-End Test

1. Configure test on web platform
2. Generate study token
3. Launch PEBL with remote URL
4. Verify parameters loaded correctly
5. Run test and verify data upload

---

## Implementation Timeline

### Week 1: Enhanced Schemas & Defaults
- [ ] Add metadata fields to key tests (corsi, stroop, flanker)
- [ ] Generate `.par.json` defaults for all tests
- [ ] Test enhanced schema with web UI mockup

### Week 2: Backend API
- [ ] Implement `/api/params` endpoint
- [ ] Implement `/api/config` CRUD endpoints
- [ ] Add parameter validation middleware
- [ ] Test remote parameter loading from PEBL

### Week 3: Frontend UI
- [ ] Build schema-driven parameter form component
- [ ] Add validation and error handling
- [ ] Integrate with study management UI
- [ ] Add parameter preview/reset functionality

### Week 4: Testing & Polish
- [ ] End-to-end testing with real studies
- [ ] Performance testing with multiple tests
- [ ] Documentation for researchers
- [ ] Deploy to production

---

## Quick Wins

These can be done immediately:

1. **Generate default .par.json files** (30 minutes)
   - Run generation script on all schemas
   - Commit to repository

2. **Test remote parameter loading** (1 hour)
   - Create simple HTTP server
   - Test PEBL can fetch parameters from URL
   - Document in QUICK_START_JSON_PARAMS.md

3. **Enhance 5 key test schemas** (2-3 hours)
   - Add min/max/step/unit/label to popular tests
   - Creates reference examples for others

---

## Questions to Resolve

1. **Parameter storage:** Database or filesystem?
   - **Filesystem:** Easy to backup, version control
   - **Database:** Better for multi-user, querying

2. **Parameter inheritance:** Study → Test → Default?
   - Study-level overrides (apply to all tests)
   - Test-level overrides (specific to one test)
   - Participant-level overrides (runtime changes)

3. **Schema versioning:** How to handle schema updates?
   - Version field in schema
   - Migration tools for parameter files
   - Backward compatibility strategy

4. **Parameter preview:** Show researcher what participants will see?
   - Live preview of test configuration
   - "Test run" with sample participant

---

## Resources

**Documentation:**
- Current: `JSON_PARAMETER_FORMAT.md`
- Current: `PARAMETER_SYSTEM_MIGRATION.md`
- Current: `QUICK_START_JSON_PARAMS.md`

**Code:**
- `pebl-lib/Utility.pbl` - Parameter loading
- `bin/launcher.pbl` - Parameter UI
- `convert_params_to_json.py` - Conversion tool

**Examples:**
- `battery/*/params/*.pbl.schema.json` - 108 converted schemas
- `test-json-params-simple.pbl` - Basic test

---

## Summary

The PEBL JSON parameter system is **production-ready** for basic use. Next steps focus on:

1. ✨ **Enhanced UX** - Better metadata for web UI
2. 🌐 **Web Integration** - API + frontend
3. ✅ **Validation** - Type checking, bounds
4. 🧪 **Testing** - End-to-end verification

**Everything is backward compatible** - existing tests work unchanged!

The foundation is solid. Now we build the web platform on top of it! 🚀
