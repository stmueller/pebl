# PEBL Naming Conventions

## General Rules

**CRITICAL: Never use spaces in filenames or directory names**

Spaces cause problems across different platforms, shells, and tools. Always use hyphens (`-`) or underscores (`_`) instead.

## Conventions by Type

### Study Directories
- Use lowercase with hyphens
- Format: `study-name`
- Examples:
  - ✓ `my-cognitive-battery`
  - ✓ `adhd-assessment`
  - ✓ `memory-study`
  - ✗ `My Cognitive Battery` (spaces!)
  - ✗ `ADHD_Assessment` (uppercase)

### Chain Files
- Use lowercase with hyphens
- Format: `chain-name.json`
- Examples:
  - ✓ `main-chain.json`
  - ✓ `practice-chain.json`
  - ✓ `consent-only.json`
  - ✗ `Main Chain.json` (spaces!)

### Snapshot Directories/Files
- Use lowercase with hyphens
- Include version and date
- Format: `study-name_vN_YYYY-MM-DD`
- Examples:
  - ✓ `my-battery_v1_2026-01-09`
  - ✓ `my-battery_v1_2026-01-09.zip`
  - ✗ `My Battery v1 2026-01-09` (spaces!)

### Parameter Files
- Use test name prefix with variant name
- Format: `testname-variant.par.json`
- Examples:
  - ✓ `BART-large.par.json`
  - ✓ `corsi-elderly.par.json`
  - ✓ `stroop-short.par.json`
  - ✗ `BART large.par.json` (spaces!)

### Test Directories (in battery)
- Follow existing battery conventions
- Usually CamelCase or lowercase
- Examples:
  - ✓ `BART`
  - ✓ `corsi`
  - ✓ `stroop`
  - ✓ `TOL-2`

### Schema Files
- Use lowercase with hyphens
- Format: `descriptor.schema.json`
- Examples:
  - ✓ `study-info.schema.json`
  - ✓ `chain.schema.json`
  - ✓ `chainpage-config.schema.json`

### Workspace Directories
- Use lowercase with underscores for multi-word directories
- Examples:
  - ✓ `my_studies/`
  - ✓ `snapshots/`
  - ✓ `pebl-exp.2.3/`
  - ✗ `My Studies/` (spaces!)

## Display Names vs Filenames

**Important Distinction:**

- **Filenames:** Never use spaces (use hyphens/underscores)
- **Display Names (in JSON):** Can use spaces for human readability

### Example:

```json
{
  "study_name": "My Cognitive Battery",     // ✓ Display name, can have spaces
  "chain_name": "Main Experimental Chain",  // ✓ Display name, can have spaces
  ...
}
```

But the files themselves:
- Directory: `my-cognitive-battery/`  (no spaces!)
- Chain file: `main-chain.json`       (no spaces!)

## Subject IDs

Subject IDs should also avoid spaces:
- ✓ `101`
- ✓ `subject-101`
- ✓ `pilot_01`
- ✗ `pilot 01` (spaces!)

## Rationale

### Cross-Platform Compatibility
- Windows, Linux, and Mac handle spaces differently
- Spaces require quoting in command line: `"file name.txt"`
- No spaces means simpler scripts: `filename.txt`

### Shell Safety
- Bash/cmd.exe scripts break easily with spaces
- No spaces = no quoting needed
- Easier automation

### URL/Path Safety
- Spaces become `%20` in URLs
- Can cause web server issues
- Simpler upload/download

### Tool Compatibility
- Many tools assume space = separator
- grep, find, make, etc. work better without spaces
- JSON parsing is cleaner

## Quick Reference

| Type | Format | Example |
|------|--------|---------|
| Study dir | `study-name` | `my-battery` |
| Chain file | `chain-name.json` | `main-chain.json` |
| Snapshot | `study_vN_date` | `study_v1_2026-01-09` |
| Parameter | `test-variant.par.json` | `BART-large.par.json` |
| Subject ID | `id` or `prefix-id` | `101` or `pilot-101` |
| Display name | Any | "My Cognitive Battery" |

## Validation

All validation tools should check for spaces in filenames and reject them.

