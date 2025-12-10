# Translation Completeness Checker

A comprehensive tool to check the completeness of PEBL test translations across all battery tests.

## Purpose

This script scans translation JSON files in PEBL test batteries and compares non-English translations against the English reference to identify:
- Missing translation keys
- Extra keys not present in English
- Translation completeness percentages
- Overall language coverage statistics

## Usage

### Basic Usage

```bash
# Check all tests in upload-battery (default)
cd /path/to/pebl_CL
python3 scripts/check-translations.py

# Check all tests in specific battery directory
python3 scripts/check-translations.py battery

# Check all tests in custom path
python3 scripts/check-translations.py /path/to/custom/battery
```

### Single Test Evaluation

```bash
# Check single test by path
python3 scripts/check-translations.py battery/iowa

# Check single test by running from test directory
cd battery/iowa
python3 ../../scripts/check-translations.py

# The script auto-detects if you're in a test directory
cd battery/stroop
python3 ../../scripts/check-translations.py  # Checks only stroop
```

### Output

The script produces:

1. **Console Report**: Summary statistics and detailed per-test analysis
2. **JSON Report**:
   - For battery scans: `translation-report.json` in the parent directory of the battery
   - For single test scans: `translation-report.json` in the test directory

## Report Contents

### Summary Section
- Total tests with translations
- Complete vs incomplete tests
- Language coverage statistics

### Detailed Section (for incomplete/error tests)
- Test name and status
- Number of keys in English reference
- Per-language analysis:
  - Key count
  - Completeness percentage
  - Missing keys
  - Extra keys

### JSON Export
Complete machine-readable report for further analysis or automation.

## Features

### Intelligent Key Comparison
- Handles nested JSON structures
- Treats `KEY_keyboard` and `KEY_mouse` as variants of `KEY` for flexible comparison
- Supports multiple character encodings (UTF-8, ISO-8859-1, Windows-1252)

### Error Detection
- Missing English translation files
- JSON parse errors
- Encoding issues

### Language Normalization
- Automatically identifies language codes from filenames
- Pattern: `testname.pbl-XX.json` where XX is the language code

## Example Output

```
================================================================================
PEBL TRANSLATION COMPLETENESS REPORT
================================================================================

Total tests with translations: 92
  Complete: 77
  Incomplete: 13
  Errors: 2

--------------------------------------------------------------------------------
LANGUAGE COVERAGE
--------------------------------------------------------------------------------
Language        Tests      Complete     Incomplete
--------------------------------------------------------------------------------
de              7          3            3
es              16         8            6
fr              9          5            2
...

================================================================================
Test: iowa
Status: INCOMPLETE
English keys: 21

Translations: 10 languages
--------------------------------------------------------------------------------
  [de] ⚠ 95.2% complete (20 keys)
       Missing keys (2): BEGIN_PROMPT, CONTINUE_PROMPT
  [es] ⚠ 95.2% complete (20 keys)
       Missing keys (2): BEGIN_PROMPT, CONTINUE_PROMPT
  ...
```

## Common Issues Found

### Missing Keys
When a translation is missing keys present in English, translators need to add them.

### Extra Keys
Often legacy keys from old translation formats. These should be reviewed:
- If they're old variants, remove them
- If they're needed, add to English as well

### Suffix Variants
The script understands that `PROMPT_keyboard` and `PROMPT_mouse` are variants.
If both exist in a language but the generic `PROMPT` exists in English, it's not marked as missing.

## Workflow for Fixing Incomplete Translations

1. Run the script to identify incomplete translations
2. Review the JSON report or console output
3. For each language with missing keys:
   - Open the translation file
   - Add missing keys with appropriate translations
   - Remove any extra keys that aren't needed
4. Re-run to verify completeness

## Integration with CI/CD

The JSON output can be used in automated workflows:

```bash
# Generate report
python3 scripts/check-translations.py

# Check if there are any incomplete translations
if grep -q '"status": "incomplete"' translation-report.json; then
    echo "Warning: Some translations are incomplete"
    exit 1
fi
```

## File Format

Expected translation file format:

```json
{
  "KEY_NAME": "Translated text",
  "VARIANT_keyboard": "Press any key",
  "VARIANT_mouse": "Click to continue",
  "NESTED": {
    "SUBKEY": "Value"
  }
}
```

Filename pattern: `testname.pbl-XX.json` in `translations/` directory

## Requirements

- Python 3.6+
- No external dependencies (uses only stdlib)

## Author

PEBL Project - Translation Management Tools
