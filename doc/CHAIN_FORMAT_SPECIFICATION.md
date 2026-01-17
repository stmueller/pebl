# PEBL Chain Format Specification

## Version

**Specification Version:** 1.0
**Date:** 2026-01-09
**Status:** Draft

## Overview

Chain files define sequences of tests and pages (instructions, consent, completion) that execute in order. Chains are used to create complete experimental sessions with proper informed consent and instructions.

## File Location

Chain files are stored in the `chains/` directory within a study:

```
study-name/
└── chains/
    ├── main-chain.json
    ├── practice-chain.json
    └── consent-only.json
```

## File Naming

- **Format:** `chain-name.json`
- **Rules:** Lowercase with hyphens, no spaces
- **Extension:** `.json` (required)
- **Examples:**
  - ✓ `main-chain.json`
  - ✓ `practice-session.json`
  - ✓ `full-battery.json`
  - ✗ `Main Chain.json` (spaces!)
  - ✗ `main_chain.json` (underscore, use hyphen)

## JSON Structure

### Basic Format

```json
{
  "chain_name": "string",
  "description": "string",
  "items": []
}
```

### Complete Example

```json
{
  "chain_name": "main-chain",
  "description": "Full experimental session with consent",
  "items": [
    {
      "item_type": "instruction",
      "title": "Welcome",
      "content": "Welcome to the study.\n\nPress OK to continue."
    },
    {
      "item_type": "consent",
      "title": "Informed Consent",
      "content": "By participating you agree to...\n\nPress OK to consent."
    },
    {
      "item_type": "test",
      "test_name": "BART",
      "param_variant": "default",
      "language": "en",
      "title": "Balloon Task"
    },
    {
      "item_type": "completion",
      "title": "Thank You",
      "content": "Study complete!\n\nThank you for participating."
    }
  ]
}
```

## Field Descriptions

### chain_name
- **Type:** String
- **Required:** Yes
- **Description:** Internal name for the chain
- **Constraints:** Non-empty
- **Recommendation:** Match filename (without .json)
- **Example:** `"main-chain"` for `main-chain.json`

### description
- **Type:** String
- **Required:** No
- **Description:** Human-readable purpose of chain
- **Default:** Empty string
- **Example:** `"Practice session for training participants"`

### items
- **Type:** Array of item objects
- **Required:** Yes
- **Constraints:** Can be empty but must exist
- **Description:** Ordered list of chain items to execute
- **Execution:** Items run in array order (first to last)

## Item Types

There are four item types:

1. **instruction** - Informational page
2. **consent** - Consent form page
3. **test** - Execute a test
4. **completion** - Thank you/completion page

## Instruction Item

Displays informational text to participant.

### Structure

```json
{
  "item_type": "instruction",
  "title": "Page Title",
  "content": "Page content text here.\n\nSupports line breaks."
}
```

### Fields

#### item_type
- **Value:** `"instruction"`
- **Required:** Yes
- **Type:** String

#### title
- **Type:** String
- **Required:** Yes
- **Description:** Page title displayed at top
- **Display:** Large font at top of page
- **Example:** `"Welcome to the Study"`

#### content
- **Type:** String
- **Required:** Yes
- **Description:** Page content text
- **Formatting:** Supports `\n` for line breaks
- **Display:** Scrollable text box
- **Example:** `"In this study, you will...\n\nPlease read carefully."`

### Usage

Instruction pages are used for:
- Welcome messages
- Task instructions
- Break notifications
- Transition messages between tests
- Debriefing information

## Consent Item

Displays consent form requiring acknowledgment.

### Structure

```json
{
  "item_type": "consent",
  "title": "Informed Consent",
  "content": "Consent text here.\n\nBy clicking OK, you consent..."
}
```

### Fields

Same as instruction item:
- `item_type`: `"consent"`
- `title`: Page title
- `content`: Consent form text

### Usage

Consent pages are used for:
- Informed consent forms
- IRB-required consent
- Parental consent
- Data usage agreements

### Implementation Notes

- Currently no checkbox/signature requirement
- OK button indicates consent
- Future: May add explicit acknowledgment checkbox
- Consider logging consent timestamp

## Completion Item

Displays thank you or completion message.

### Structure

```json
{
  "item_type": "completion",
  "title": "Thank You",
  "content": "Thank you for participating!\n\nYou may now close the window."
}
```

### Fields

Same as instruction/consent items:
- `item_type`: `"completion"`
- `title`: Page title
- `content`: Completion message

### Usage

Completion pages are used for:
- Thank you messages
- Study completion notification
- Next steps information
- Debrief

ing information
- Compensation instructions

## Test Item

Executes a test from the study.

### Structure

```json
{
  "item_type": "test",
  "test_name": "TestName",
  "param_variant": "variant-name",
  "language": "en",
  "title": "Display Name"
}
```

### Fields

#### item_type
- **Value:** `"test"`
- **Required:** Yes
- **Type:** String

#### test_name
- **Type:** String
- **Required:** Yes
- **Description:** Name of test to run
- **Validation:** Must exist in study-info.json `tests` array
- **Example:** `"BART"`, `"corsi"`, `"stroop"`

#### param_variant
- **Type:** String
- **Required:** Yes
- **Description:** Which parameter variant to use
- **Default:** `"default"`
- **Validation:** Must exist in test's `parameter_variants`
- **Example:** `"default"`, `"large"`, `"short-version"`

#### language
- **Type:** String
- **Required:** No (recommended)
- **Description:** Language code for test
- **Format:** Two-letter ISO 639-1 code
- **Default:** Test's default (usually "en")
- **Examples:** `"en"`, `"es"`, `"de"`, `"fr"`, `"cn"`
- **Validation:** Should match available translation file

#### title
- **Type:** String
- **Required:** No
- **Description:** Display name for this chain item
- **Purpose:** Shown in chain editor, progress indicators
- **Default:** Test name
- **Example:** `"Balloon Risk Task"` for BART test

### Execution

When a test item executes:

1. **Verify test exists** in study
2. **Build command:**
   ```
   pebl2 tests/TestName/TestName.pbl -v subnum=<id>
   ```
3. **Add parameter variant** (if not default):
   ```
   -v pfile=tests/TestName/params/variant.par.json
   ```
4. **Add language** (if specified):
   ```
   -v language=<lang>
   ```
5. **Execute test** as subprocess
6. **Wait for completion**
7. **Check exit code**
8. **Continue to next item** or abort on error

## Item Ordering

Items execute in array order from first to last.

### Typical Chain Structure

```json
{
  "items": [
    {"item_type": "instruction", ...},     // 1. Welcome
    {"item_type": "consent", ...},         // 2. Consent
    {"item_type": "test", ...},            // 3. First test
    {"item_type": "instruction", ...},     // 4. Transition
    {"item_type": "test", ...},            // 5. Second test
    {"item_type": "test", ...},            // 6. Third test
    {"item_type": "completion", ...}       // 7. Thank you
  ]
}
```

### Best Practices

1. **Start with instruction** - Welcome and overview
2. **Consent early** - Before any data collection
3. **Group tests logically** - Related tasks together
4. **Use transitions** - Between different task types
5. **End with completion** - Thank you and debrief

## Text Formatting

### Line Breaks

Use `\n` for line breaks in content:

```json
{
  "content": "First paragraph.\n\nSecond paragraph after blank line."
}
```

Renders as:
```
First paragraph.

Second paragraph after blank line.
```

### Special Characters

JSON escaping required for:
- Quotes: `\"`
- Backslash: `\\`
- Newline: `\n`
- Tab: `\t`

### Example

```json
{
  "content": "He said \"Hello\"\n\nPress OK to continue."
}
```

### Future: Markdown Support

Future versions may support markdown formatting:
- **Bold:** `**text**`
- *Italic:* `*text*`
- Lists, headers, etc.

Not currently supported - plain text only.

## Validation Rules

### Chain-Level Validation

1. **Valid JSON** - Must parse successfully
2. **Required fields:**
   - `chain_name` (non-empty)
   - `items` (array)
3. **Items array:**
   - Can be empty
   - All items have valid structure

### Item Validation

1. **item_type required** - Must be valid type
2. **Type-specific requirements:**
   - instruction/consent/completion: title + content
   - test: test_name + param_variant
3. **Test references:**
   - test_name exists in study
   - param_variant exists for test
   - language has translation (warning if missing)

### Warnings (Not Errors)

1. Empty chain (no items)
2. No consent item
3. Test language missing translation
4. Duplicate test items (usually intentional)
5. Very long chains (> 20 items)

## ChainPage Configuration

Instruction/consent/completion items are executed via `ChainPage.pbl`.

### Runtime Conversion

Chain item:
```json
{
  "item_type": "instruction",
  "title": "Welcome",
  "content": "Welcome text...\n\nPress OK."
}
```

Converts to temporary ChainPage config:
```json
{
  "title": "Welcome",
  "content": "Welcome text...\n\nPress OK.",
  "page_type": "instruction"
}
```

Executed as:
```bash
pebl2 ChainPage.pbl -v temp-config.json
```

## Multiple Chains Per Study

Studies can have multiple chains for different purposes.

### Common Scenarios

1. **main-chain.json** - Full experimental session
2. **practice-chain.json** - Practice/training only
3. **consent-only.json** - Just consent form
4. **short-version.json** - Abbreviated battery
5. **retest-chain.json** - Follow-up session

### Chain Selection

User selects which chain to run:
- From study editor
- Before starting session
- Based on research protocol

## Chain Execution Context

### Subject ID

All test items receive subject ID:
- Prompted once before chain starts
- Passed to all tests via `-v subnum=<id>`
- Used for data file naming

### Working Directory

Tests execute with working directory:
- Set to test directory
- Ensures relative paths work
- Data saves in `data/` subdirectory

### Error Handling

If test fails:
1. **Capture exit code**
2. **Show error message**
3. **Options:**
   - Abort chain
   - Skip test and continue
   - Retry test

## Examples

### Minimal Chain

```json
{
  "chain_name": "minimal",
  "items": [
    {
      "item_type": "test",
      "test_name": "BART",
      "param_variant": "default"
    }
  ]
}
```

### Full Featured Chain

```json
{
  "chain_name": "full-session",
  "description": "Complete assessment battery with all elements",
  "items": [
    {
      "item_type": "instruction",
      "title": "Welcome to the Cognitive Assessment",
      "content": "Thank you for participating.\n\nThis session will take approximately 30 minutes.\n\nYou will complete several computerized tasks.\n\nPlease read all instructions carefully.\n\nPress OK when ready to begin."
    },
    {
      "item_type": "consent",
      "title": "Informed Consent",
      "content": "INFORMED CONSENT FOR RESEARCH\n\nPurpose: This study examines cognitive function.\n\nProcedure: You will complete computerized tasks.\n\nRisks: Minimal - possible mild fatigue.\n\nBenefits: Contribute to research.\n\nConfidentiality: Data is anonymous.\n\nVoluntary: You may withdraw anytime.\n\nBy clicking OK, you consent to participate."
    },
    {
      "item_type": "test",
      "test_name": "BART",
      "param_variant": "default",
      "language": "en",
      "title": "Balloon Analogue Risk Task"
    },
    {
      "item_type": "instruction",
      "title": "Break - Halfway Done",
      "content": "You have completed half of the session.\n\nFeel free to take a short break.\n\nPress OK when ready to continue."
    },
    {
      "item_type": "test",
      "test_name": "corsi",
      "param_variant": "default",
      "language": "en",
      "title": "Corsi Block Tapping"
    },
    {
      "item_type": "test",
      "test_name": "stroop",
      "param_variant": "short",
      "language": "en",
      "title": "Stroop Color-Word Task"
    },
    {
      "item_type": "completion",
      "title": "Session Complete - Thank You!",
      "content": "Thank you for completing the cognitive assessment!\n\nYour data has been saved.\n\nPlease notify the experimenter.\n\nYou may now close this window."
    }
  ]
}
```

### Practice Chain

```json
{
  "chain_name": "practice",
  "description": "Practice session for participant training",
  "items": [
    {
      "item_type": "instruction",
      "title": "Practice Session",
      "content": "This is a practice session.\n\nYour data will not be saved.\n\nThis helps you learn the task."
    },
    {
      "item_type": "test",
      "test_name": "BART",
      "param_variant": "practice",
      "language": "en",
      "title": "BART Practice"
    },
    {
      "item_type": "instruction",
      "title": "Practice Complete",
      "content": "Practice is complete!\n\nDo you have any questions?\n\nWhen ready, we will begin the real session."
    }
  ]
}
```

## File Size and Limits

### Recommended

- **Chain file:** < 100 KB
- **Items per chain:** < 50
- **Content length:** < 10,000 characters per item

### Practical Limits

- Most chains: 3-15 items
- Consent forms: Can be long (several thousand words)
- Multiple short chains better than one huge chain

## Character Encoding

- **Encoding:** UTF-8
- **Line endings:** Any (LF or CRLF)
- **BOM:** Not required
- **Special characters:** Properly JSON-escaped

## JSON Formatting Style

### Recommended

```json
{
  "chain_name": "example",
  "description": "Example chain",
  "items": [
    {
      "item_type": "instruction",
      "title": "Title",
      "content": "Content"
    }
  ]
}
```

- **Indentation:** 2 spaces
- **Quotes:** Double quotes only
- **Commas:** No trailing commas
- **Booleans:** `true`/`false` (lowercase)

## Related Documents

- `STUDY_FORMAT_SPECIFICATION.md` - Study structure
- `CHAINPAGE_SPECIFICATION.md` - ChainPage.pbl format
- `NAMING_CONVENTIONS.md` - File naming rules
- `NATIVE_LAUNCHER_STUDY_SYSTEM.md` - System design

## JSON Schema

See `schemas/chain.schema.json` for formal validation.

## Examples

See `examples/example-study/chains/` for complete examples.

---

**Document Version:** 1.0
**Last Updated:** 2026-01-09
**Status:** Draft - Subject to change
