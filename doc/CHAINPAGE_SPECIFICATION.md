# ChainPage.pbl Configuration Specification

## Overview

ChainPage.pbl is a PEBL script that displays instruction, consent, and completion pages during chain execution. The C++ launcher creates temporary JSON configuration files and executes ChainPage.pbl to display these pages.

## File Location

**ChainPage.pbl:** `pebl-lib/ChainPage.pbl`
- Auto-discovered in PEBL library path
- No need to specify full path when executing

## Execution

```bash
pebl2 ChainPage.pbl -v config-file.json
```

## Configuration File Format

### Structure

```json
{
  "title": "Page Title",
  "content": "Page content text.\n\nSupports line breaks.",
  "page_type": "instruction"
}
```

### Required Fields

All three fields are required:

#### title
- **Type:** String
- **Description:** Page title shown at top
- **Display:** Large font (28pt), centered, top of window
- **Example:** `"Welcome to the Study"`

#### content
- **Type:** String
- **Description:** Page content text
- **Formatting:** `\n` converted to line breaks
- **Display:** Adaptive scrollable textbox, 18pt font
- **Example:** `"Welcome!\n\nPress OK to continue."`

#### page_type
- **Type:** String
- **Description:** Type of page being displayed
- **Values:** `"instruction"`, `"consent"`, `"completion"`
- **Purpose:** Currently informational; future use for special handling

## Text Formatting

### Line Breaks

The `\n` sequence is automatically converted to actual line breaks:

**Input:**
```json
{
  "content": "First line.\n\nThird line (blank line between)."
}
```

**Display:**
```
First line.

Third line (blank line between).
```

### Special Characters

JSON escaping required:
- Quote: `\"`
- Backslash: `\\`
- Newline: `\n`
- Tab: `\t`

**Example:**
```json
{
  "content": "He said \"Hello\"\nThen left."
}
```

## Display Layout

```
┌─────────────────────────────────────────────────────┐
│                                                     │
│                    Page Title                       │ ← 28pt, centered
│                                                     │
│  ┌───────────────────────────────────────────────┐ │
│  │                                               │ │
│  │  Page content text here.                     │ │ ← 18pt, scrollable
│  │                                               │ │
│  │  Supports multiple paragraphs.               │ │
│  │                                               │ │
│  │  Text box is 900px wide and fills most      │ │
│  │  of the screen height.                       │ │
│  │                                               │ │
│  └───────────────────────────────────────────────┘ │
│                                                     │
│                 ┌──────────┐                        │
│                 │    OK    │                        │ ← 300×50px button
│                 └──────────┘                        │
│                                                     │
└─────────────────────────────────────────────────────┘
```

### Dimensions

- **Title:** 28pt font, centered at y=50
- **Content box:** 900px wide, height = screen height - 270px
- **Content box position:** Centered horizontally, starts at y=120
- **OK button:** 300px × 50px, 24pt font
- **OK button position:** Centered horizontally, y = screen height - 100

### Responsive Behavior

- **Content text:** Adaptive scrolling if too long
- **Long content:** Scrollbar appears automatically
- **Small screens:** Layout scales appropriately
- **Button:** Always fully clickable (entire 300×50 area)

## User Interaction

1. Page displays with title and content
2. User reads content (can scroll if needed)
3. User clicks OK button (anywhere in button area)
4. ChainPage.pbl exits
5. Chain continues to next item

## File Lifecycle

### Creation

1. **Launcher reads chain item** (instruction/consent/completion)
2. **Creates temporary JSON** file:
   ```
   temp-chainpage-<uuid>.json
   ```
3. **Writes configuration:**
   ```json
   {
     "title": "<from chain item>",
     "content": "<from chain item>",
     "page_type": "<instruction|consent|completion>"
   }
   ```

### Execution

4. **Launcher executes:**
   ```bash
   pebl2 ChainPage.pbl -v temp-chainpage-<uuid>.json
   ```
5. **ChainPage.pbl runs:**
   - Reads config file
   - Displays page
   - Waits for OK click
   - Exits

### Cleanup

6. **Launcher deletes** temporary file after completion

## Error Handling

### Missing Config File

If config file doesn't exist:
```
Error: ChainPage.pbl requires config file: pebl2 ChainPage.pbl -v <config.json>
```

### Invalid JSON

If JSON is malformed:
- ParseJSON fails
- ChainPage.pbl crashes
- Launcher should catch error

### Missing Fields

If required fields missing:
- PropertyExists checks fail
- Empty values used
- Page displays but may be incomplete

## Examples

### Instruction Page

**Config:**
```json
{
  "title": "Welcome to the Study",
  "content": "Thank you for participating in this cognitive assessment.\n\nIn this session, you will complete several computerized tasks.\n\nPlease read all instructions carefully.\n\nPress OK when you are ready to begin.",
  "page_type": "instruction"
}
```

**Execution:**
```bash
pebl2 ChainPage.pbl -v instruction.json
```

### Consent Page

**Config:**
```json
{
  "title": "Informed Consent",
  "content": "INFORMED CONSENT FOR RESEARCH PARTICIPATION\n\nPurpose: This study examines cognitive function.\n\nProcedure: You will complete computerized tasks for approximately 30 minutes.\n\nRisks: Minimal risk of mild fatigue.\n\nBenefits: Contribute to scientific knowledge.\n\nConfidentiality: Your data will be stored anonymously.\n\nVoluntary Participation: You may withdraw at any time.\n\nBy clicking OK, you indicate your informed consent to participate.",
  "page_type": "consent"
}
```

### Completion Page

**Config:**
```json
{
  "title": "Study Complete - Thank You!",
  "content": "Thank you for completing the cognitive assessment!\n\nYour responses have been recorded.\n\nPlease notify the experimenter that you have finished.\n\nYou may now close this window.",
  "page_type": "completion"
}
```

## Implementation Notes for C++ Launcher

### Creating Config Files

```cpp
std::string CreateChainPageConfig(const ChainItem& item) {
    // Generate unique filename
    std::string uuid = GenerateUUID();
    std::string tempFile = GetTempDirectory() + "/temp-chainpage-" + uuid + ".json";

    // Build JSON
    json config;
    config["title"] = item.title;
    config["content"] = item.content;
    config["page_type"] = ItemTypeToString(item.item_type);

    // Write file
    std::ofstream out(tempFile);
    out << config.dump(2);  // Pretty print with 2-space indent
    out.close();

    return tempFile;
}
```

### Executing ChainPage

```cpp
int ExecuteChainPage(const std::string& configFile) {
    // Build command
    std::string peblPath = GetPEBLExecutable();  // Path to pebl2
    std::string command = peblPath + " ChainPage.pbl -v " + configFile;

    // Execute and wait
    int exitCode = ExecuteCommand(command);

    return exitCode;
}
```

### Cleanup

```cpp
void CleanupChainPageConfig(const std::string& configFile) {
    if (FileExists(configFile)) {
        DeleteFile(configFile);
    }
}
```

### Complete Workflow

```cpp
bool ExecutePageItem(const ChainItem& item) {
    // 1. Create config
    std::string configFile = CreateChainPageConfig(item);

    // 2. Execute
    int exitCode = ExecuteChainPage(configFile);

    // 3. Cleanup
    CleanupChainPageConfig(configFile);

    // 4. Check result
    return (exitCode == 0);
}
```

## Future Enhancements

### Possible Future Features

1. **Consent checkboxes** - Explicit acknowledgment
2. **Signature capture** - For formal consent
3. **Timer** - Minimum time before OK enabled
4. **Markdown rendering** - Rich text formatting
5. **Images** - Embedded diagrams/logos
6. **Multiple buttons** - Yes/No, Agree/Disagree
7. **Form fields** - Collect additional info

### Current Limitations

- No forced minimum reading time
- No explicit acknowledgment checkbox
- Plain text only (no formatting)
- Single OK button only
- No images or media
- No data collection (just displays)

## Testing

### Manual Test

1. Create test config file:
   ```bash
   echo '{
     "title": "Test Page",
     "content": "This is a test.\n\nPress OK to close.",
     "page_type": "instruction"
   }' > test-page.json
   ```

2. Run ChainPage:
   ```bash
   bin/pebl2 pebl-lib/ChainPage.pbl -v test-page.json
   ```

3. Verify:
   - Title displays correctly
   - Content shows with line breaks
   - OK button is clickable
   - Clicking OK closes window

### Automated Test

Use PEBL validator to check syntax:
```bash
bin/pebl-validator pebl-lib/ChainPage.pbl
```

Should return: `✓ Syntax: VALID`

## JSON Schema

See `schemas/chainpage-config.schema.json` for formal validation.

## Related Documents

- `CHAIN_FORMAT_SPECIFICATION.md` - Chain file format
- `STUDY_FORMAT_SPECIFICATION.md` - Overall study structure
- `NATIVE_LAUNCHER_STUDY_SYSTEM.md` - System design

## ChainPage.pbl Source

Located at: `pebl-lib/ChainPage.pbl`

See source code for implementation details.

---

**Document Version:** 1.0
**Last Updated:** 2026-01-09
**Status:** Draft - Current implementation
