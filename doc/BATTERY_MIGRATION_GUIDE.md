# PEBL Battery Test Migration Guide

**Purpose**: Guide for migrating native PEBL battery tests to work on the online platform

**Last Updated**: October 19, 2025

---

## Overview

Migrating a test from the native PEBL battery to the online platform (`upload-battery/`) requires several adaptations to ensure proper functionality in a web browser environment. This guide provides a systematic checklist and procedures for successful migration.

## Directory Structure

```
upload-battery/
└── testname/                           # One test per directory
    ├── taskfile.pbl                    # Main test file (filename may differ from directory)
    ├── taskfile.pbl.schema.json        # Parameter schema JSON (REQUIRED)
    ├── taskfile.pbl.about.txt          # Test description (REQUIRED)
    ├── screenshots/                    # Screenshots for catalog (REQUIRED)
    │   ├── screenshot1.png
    │   └── screenshot2.png
    ├── params/
    │   └── taskfile.pbl.par            # Default parameters (optional)
    └── translations/
        ├── taskfile.pbl-en.json        # English translations (REQUIRED)
        ├── taskfile.pbl-es.json        # Spanish translations (optional)
        ├── taskfile.pbl-fr.json        # French translations (optional)
        └── ...                         # Other language files
```

**Important Notes**:
- **One test per directory**: Unlike native battery (which may have multiple test variations per directory), upload-battery should have one test per directory
- **Filename independence**: The .pbl filename does NOT need to match the directory name (e.g., `bcst/cardsort.pbl` is valid)
- **Do NOT rename .pbl files**: Keep the original filename from the native battery
- **No data directories**: Do NOT include `data/` directories in `upload-battery/` - these are excluded during deployment
- **Schema format**: Use `.schema.json` files (JSON format), NOT pipe-delimited `.schema` files

---

## Migration Checklist

### Phase 1: Pre-Migration Assessment

- [ ] **Test compatibility check**
  - [ ] Test does not require special hardware (parallel port, serial port, eye tracker)
  - [ ] Test does not require local file system beyond data output
  - [ ] Test timing requirements are reasonable for web (<1ms precision may not be achievable)
  - [ ] Test does not use platform-specific features (Windows registry, etc.)

- [ ] **Copy test to upload-battery/**
  ```bash
  cp -r battery/testname/ upload-battery/testname/
  rm -rf upload-battery/testname/data/  # Remove any test data
  ```

- [ ] **CRITICAL: Check ALL translation files exist and are complete**
  ```bash
  # List all translation files for this test
  ls battery/testname/translations/
  ```
  **Required translations**: en, es, de, pt, fr, nl, it
  - [ ] hicks.pbl-en.json (English) - **REQUIRED**
  - [ ] hicks.pbl-es.json (Spanish)
  - [ ] hicks.pbl-de.json (German)
  - [ ] hicks.pbl-pt.json (Portuguese)
  - [ ] hicks.pbl-fr.json (French)
  - [ ] hicks.pbl-nl.json (Dutch)
  - [ ] hicks.pbl-it.json (Italian)

  **If any translations are missing**: Create them BEFORE proceeding with migration
  **All translations must be updated together** when making any text changes

- [ ] **Bundle configuration (REQUIRED for all new tests)**
  - **IMPORTANT**: From now on, all new tests should be in their own bundle
  - Add bundle entry to `bundle-config.json` (in `test_bundles` array)
  - Do NOT add new tests to `core-battery` manifest
  - Each test gets its own `.data` file in `bin/test-bundles/`

### Phase 2: Code Modifications

#### 2.1 Data Upload Implementation

**Goal**: Minimal changes to .pbl files while enabling online data upload.

**Solution**: Two simple additions to the test code.

**Required changes (MINIMAL):**

1. **At the beginning of the test** (in `Start()` or main function), add ONE line:
   ```pebl
   define Start()
   {
       ## Initialize upload system (works for both online and native)
       InitializeUpload()

       ## Rest of your test code...
   }
   ```

2. **At the end of the test** (after creating data files), add upload calls:
   ```pebl
   ## Write data files as normal (native approach)
   gFileOut <- FileOpenWrite("results-" + gSubNum + ".csv")
   FilePrint(gFileOut, "trial,response,rt")
   ## ... more FilePrint calls ...

   ## CRITICAL: Close files BEFORE upload (required for Windows file locking compatibility)
   FileClose(gFileOut)

   ## Upload the files (works online, no-op on native)
   ## IMPORTANT: Must use absolute path "/upload.json" because InitializeUpload()
   ## changes the working directory to the battery test folder
   ## Note: .filename property persists after FileClose(), so this is safe
   datafile <- "data/" + gSubNum + "/results-" + gSubNum + ".csv"
   UploadFile(gSubNum, datafile, "/upload.json")
   ```

   **Critical Notes**:
   - **Files must be closed BEFORE upload**: `UploadFile()` internally calls `FileReadText()` which will fail on Windows if the file is still open due to file locking. Always call `FileClose()` before `UploadFile()`.
   - **File .filename property persists after close**: File objects retain their `.filename` property after `FileClose()`, so accessing `fileOut.filename` after closing is safe.
   - `UploadFile()` requires three parameters:
     - `subcode` - participant ID (typically `gSubNum`)
     - `datafilename` - path to the data file to upload (string) OR file object (will extract .filename automatically)
     - `settings` - **MUST be the absolute path `"/upload.json"`** (not relative `"upload.json"`)

   After `InitializeUpload()` runs, the working directory changes to `/usr/local/share/pebl2/battery/testname/`,
   so a relative path like `"upload.json"` will fail. Always use the absolute path `"/upload.json"`.

**That's it!** No need to wrap file operations in conditionals or check IsOnline() everywhere.

**Example - Complete minimal migration:**
```pebl
define Start()
{
    ## Initialize upload (handles online/native detection automatically)
    InitializeUpload()

    ## Get subject number from parameters
    gSubNum <- GetSubNum(gWin)

    ## Run the test
    results <- RunTrials()

    ## Save data (traditional file writing)
    gFileOut <- FileOpenWrite("results-" + gSubNum + ".csv")
    FilePrint(gFileOut, "trial,stimulus,response,rt,accuracy")
    loop(trial, results)
    {
        FilePrint(gFileOut, trial.trialnum + "," + trial.stimulus + "," +
                           trial.response + "," + trial.rt + "," + trial.acc)
    }

    ## CRITICAL: Close file BEFORE upload (Windows compatibility)
    FileClose(gFileOut)

    ## Upload data (automatic - works online, skipped on native)
    ## Note: .filename property persists after FileClose()
    datafile <- "data/" + gSubNum + "/results-" + gSubNum + ".csv"
    UploadFile(gSubNum, datafile, "/upload.json")

    ## Show completion message
    MessageBox("Thank you for participating!", gWin)
}
```

**How it works:**
- `InitializeUpload()` (in pebl-lib/Utility.pbl) detects environment automatically
- Online: Sets up upload configuration, reads `/upload.json` for token
- Native: Sets flag to skip uploads, allows normal testing
- `UploadFile()` checks the flag and only uploads when online

**Multiple data files?** Close all files first, then upload:
```pebl
## Close all files BEFORE any uploads (Windows compatibility)
FileClose(gFileOut)
FileClose(gSummaryFile)
FileClose(gTrialFile)

## Upload all data files
## Note: .filename property persists after FileClose()
settingsfile <- "/upload.json"
UploadFile(gSubNum, gFileOut.filename, settingsfile)
UploadFile(gSubNum, gSummaryFile.filename, settingsfile)
UploadFile(gSubNum, gTrialFile.filename, settingsfile)
```

**Working examples** in upload-battery/:
- `corsi/corsi.pbl` - Minimal migration example
- `simple-test/simple-test.pbl` - Another working example

**Key points:**
- ✅ Keep ALL existing file writing code unchanged
- ✅ Just add `InitializeUpload()` at start
- ✅ Just add `UploadFile()` calls at end
- ✅ Test works both native (for testing) and online (for deployment)
- ✅ No need to modify pebl-lib/Utility.pbl (already has these functions)

#### 2.2 Instruction Text Cleanup

**Scan for and remove/replace offline-specific language:**

**Common phrases to find and fix:**

| ❌ Offline Language | ✅ Online Replacement |
|---------------------|----------------------|
| "Alert the experimenter when done" | "The study will end automatically when complete" |
| "Press any key when the experimenter says to begin" | "Press any key when ready to begin" |
| "Ask the experimenter if you have questions" | "Contact the researcher via email if you have questions" |
| "Raise your hand when finished" | "Click 'Finish' when complete" |
| "Wait for the experimenter" | "Wait for the next screen" |
| "Tell the experimenter about any problems" | "Report any technical issues using the feedback form" |

**Search commands to find problematic text:**
```bash
# Search main test file
grep -i "experimenter" upload-battery/testname/testname.pbl
grep -i "raise your hand" upload-battery/testname/testname.pbl
grep -i "ask questions" upload-battery/testname/testname.pbl

# Search translation files
grep -i "experimenter" upload-battery/testname/translations/*.json
```

#### 2.3 Layout & Response System Integration (PEBL 2.3+)

**Goal**: Migrate tests to use the unified Layout & Response System for platform-aware responses and consistent UI.

**Benefits**:
- **Platform-aware responses**: Automatic adaptation between desktop (shift keys), web (Z/slash), and touch (click targets)
- **Sticky Keys prevention**: Shift key mode disabled by default on web browsers
- **Consistent UI**: Standardized header/footer/stimulus regions across all battery tasks
- **Zero code changes for response mode**: Users can switch via parameters, not code
- **Semantic responses**: Data files record semantic names (`"left"`, `"right"`) instead of raw keys (`<lshift>`)

**Migration Status** (as of Jan 2026):
- 18 tests fully migrated (35%)
- Available response modes: auto, keyboardShift, keyboardSafe, arrowLR, mousetarget, touchtarget, mousebutton, userselect, spacebar, leftclick, touchscreen, clicktarget

**Who should migrate:**
- ✅ **YES**: Tests with 2-alternative forced choice (left/right, odd/even, yes/no)
- ✅ **YES**: Tests with single-key responses (spacebar, click to respond)
- ⚠️ **MAYBE**: Tests with custom response patterns (consider layout-only migration)
- ❌ **NO**: Tests with mouse tracking, drag-and-drop, or complex integrated interfaces

**Migration Steps:**

**Step 2.3.1: Add responsemode parameter and set responsesemantics**

Add to `params/taskname.pbl.schema.json`:
```json
{
  "name": "responsemode",
  "type": "string",
  "default": "auto",
  "options": ["auto", "keyboardShift", "keyboardSafe", "arrowLR", "userselect", "mousetarget", "mousebutton", "touchtarget"],
  "label": "Response Mode",
  "description": "Response method: auto (platform-aware default), keyboardShift (L/R shift), keyboardSafe (Z/slash for web), arrowLR (left/right arrows), userselect (user-selected keys), mousetarget (click targets), mousebutton (L/R mouse buttons), touchtarget (large touch targets)"
}
```

**CRITICAL: Always include "auto" and "userselect" modes**

- **auto**: Platform-aware default mode. Automatically selects the best response mode for the current platform:
  - Desktop: Uses shift keys (unless Sticky Keys detected)
  - Web browsers: Uses Z/slash (avoids Sticky Keys dialog)
  - Touch devices: Uses touch targets

- **userselect**: Allows users to choose their own response keys at the start of the test. Essential for:
  - Accessibility (users with motor impairments)
  - Keyboard layout differences (QWERTY, AZERTY, QWERTZ)
  - User preference and comfort
  - Testing with alternative input devices

**Default to "auto"** for maximum compatibility, but **always include "userselect"** as an option for accessibility.

**IMPORTANT**: Do NOT add `responsesemantics` to schema - it's an internal parameter set in code.

Update main `.pbl` file in `Start()` function:
```pebl
## BEFORE
parpairs <- [["reps", 10],
             ["isi", 500]]
gParams <- CreateParameters(parpairs, gParamFile)

## AFTER
parpairs <- [["reps", 10],
             ["isi", 500],
             ["responsemode", "auto"]]  ## NEW

gParams <- CreateParameters(parpairs, gParamFile)

## Set semantic requirements (NOT in schema!)
gParams.responsesemantics <- "2afc"  ## Or: ["left", "right"], "gonogo", etc.
```

**Available responsesemantics values:**
- `"2afc"` - Two-alternative forced choice (left/right)
- `"gonogo"` - Single response (go/no-go tasks)
- `["left", "right"]` - Custom semantic labels
- `["odd", "even"]`, `["yes", "no"]`, etc. - Task-specific semantics

**Step 2.3.2: Create layout after parameters**

Add to `Start()` function, AFTER parameters loaded, BEFORE first `Draw()`:

```pebl
## BEFORE
gWin <- MakeWindow("black")
gParams <- CreateParameters(parpairs, gParamFile)
GetStrings(gLanguage)
Draw()

## AFTER
gWin <- MakeWindow("black")
gParams <- CreateParameters(parpairs, gParamFile)
GetStrings(gLanguage)

## NEW: Create layout with response system
gLayout <- CreateLayout("taskname", gWin, gParams)

## Configure layout zones
gLayout.header.text <- gStrings.header
gLayout.subheader.visible <- 0  ## Hide if not needed
## Footer is set automatically

Draw()
```

**Step 2.3.2.1: Add Trial Progress Counter (Optional but Recommended)**

For multi-trial tests, adding a trial counter to the header provides helpful progress feedback to participants. This pattern is especially useful for longer tests or those with many blocks.

**When to add trial counters:**
- Tests with multiple trials (>10 trials)
- Tests with multiple blocks
- Tests where participants benefit from knowing their progress
- Both layout-only and full response system migrations

**Implementation:**

```pebl
## In your main trial loop, dynamically update the header
while (trial <= numtrials)
{
  ## Update header with trial counter
  gLayout.header.text <- gStrings.header + " - Trial " + trial + " of " + numtrials

  ## Rest of trial logic...
  trial <- trial + 1
}
```

**For tests with blocks:**

```pebl
loop(block, blocks)
{
  loop(trial, trialsPerBlock)
  {
    ## Show both block and trial progress
    cumTrial <- (block - 1) * trialsPerBlock + trial
    totalTrials <- Length(blocks) * trialsPerBlock
    gLayout.header.text <- gStrings.header + " - Block " + block + ", Trial " + cumTrial + " of " + totalTrials

    ## Trial logic...
  }
}
```

**Translation file requirements:**

Ensure your base HEADER string doesn't include trial numbers, as they'll be added dynamically:

```json
{
  "HEADER": "Task Name"
  // NOT "Task Name - Trial 1" - trial counter added dynamically in code
}
```

**Example implementations:**
- `battery/timetap/timetap.pbl` - Simple trial counter (layout-only migration)
- `battery/linejudgment/linejudgment.pbl` - Trial counter with Layout & Response System
- `battery/globallocal/globallocal.pbl` - Block and trial counters

**Step 2.3.3: Update MessageBox to fit stimulus region**

Replace instances of `MessageBox()` with constrained versions:

```pebl
## Option 1: Use AdaptiveTextBox (RECOMMENDED)
define MessageKeyBox(message)
{
  tb <- AdaptiveTextBox(message,
                        gLayout.stimulusRegion.x + 20,
                        gLayout.stimulusRegion.y + 20,
                        gWin, 56,
                        gLayout.stimulusRegion.width - 40,
                        gLayout.stimulusRegion.height - 40,
                        "scalefont")
  Draw()
  WaitForAnyKeyPress()
  RemoveObject(tb, gWin)
}

## Option 2: Calculate custom gutters for MessageBox
leftgutter <- gLayout.stimulusRegion.x + 20
rightgutter <- gVideoWidth - (gLayout.stimulusRegion.x + gLayout.stimulusRegion.width) + 20
bottomgutter <- gVideoHeight - (gLayout.stimulusRegion.y + gLayout.stimulusRegion.height) + 20
MessageBox(gStrings.interblock, gWin, 20, leftgutter, rightgutter, bottomgutter)
```

**Step 2.3.4: Replace manual positioning with layout zones**

**CRITICAL**: Replace ALL instances of `gVideoWidth/2` and `gVideoHeight/2` with layout positioning.

```pebl
## BEFORE
stim <- EasyLabel(stimulus, gVideoWidth/2, gVideoHeight/2, gWin, 80)
fixation <- EasyLabel("+", gVideoWidth/2, gVideoHeight/2, gWin, 40)

## AFTER
stim <- EasyLabel(stimulus, gLayout.centerX, gLayout.centerY, gWin, 80)
fixation <- EasyLabel("+", gLayout.centerX, gLayout.centerY, gWin, 40)
```

**Common locations to check:**
- Trial() function - stimulus positioning
- DoInstructions() - demo stimuli positioning
- Any function creating fixation crosses, feedback, or centered elements

**Step 2.3.5: Replace hardcoded key checks with WaitForLayoutResponse**

```pebl
## BEFORE
Draw()
resp <- WaitForListKeyPress(["<lshift>", "<rshift>"])
if(resp == "<lshift>") { response <- "left" }
if(resp == "<rshift>") { response <- "right" }

## AFTER
Draw()
resp <- WaitForLayoutResponse(gLayout)
response <- resp  ## Already semantic: "left" or "right"
```

**Step 2.3.6: Update translation strings for ALL languages**

Check which translations exist:
```bash
ls battery/taskname/translations/
```

**Update each translation file** (`-en.json`, `-es.json`, `-fr.json`, etc.):

```json
// BEFORE
{
  "HEADER": "Task Name - Trial 1",
  "FOOTER": "<left shift> LEFT                    RIGHT <right shift>",
  "INSTRUCTIONS": "Press LEFT SHIFT for left, RIGHT SHIFT for right."
}

// AFTER
{
  "HEADER": "Task Name - Trial 1",
  "FOOTER": "LEFT                                    RIGHT",
  "INSTRUCTIONS": "Press the LEFT response key for left, the RIGHT response key for right."
}
```

**Key changes for ALL languages:**
- Remove hardcoded key names from `FOOTER` (e.g., `"<left shift>"`, `"shift izquierdo"`)
- Update `INSTRUCTIONS` to use generic "response key" language
- Layout system adds actual response keys automatically below footer
- Preserve semantic meanings in each language (LEFT/RIGHT → IZQUIERDA/DERECHA → KAIRĖ/DEŠINĖ)

**Step 2.3.7: Verify data output format**

Response values change from raw keys to semantic names:

```
BEFORE: 001,1,stimulus1,<lshift>,1,523
AFTER:  001,1,stimulus1,left,1,523
```

This is EXPECTED and CORRECT. Analysis scripts may need minor updates.

**Step 2.3.8: Test multiple response modes**

**REQUIRED parameter files** (minimum):
```bash
battery/testname/params/testname-auto.par.json        # Platform-aware (REQUIRED)
battery/testname/params/testname-userselect.par.json  # User-selected keys (REQUIRED for accessibility)
```

**Additional recommended parameter files** (test-dependent):
```bash
battery/testname/params/testname-keyboardSafe.par.json  # Z/slash (web-safe)
battery/testname/params/testname-arrowLR.par.json       # Arrow keys
battery/testname/params/testname-mousetarget.par.json   # Click targets
battery/testname/params/testname-mousebutton.par.json   # Mouse buttons
battery/testname/params/testname-touchtarget.par.json   # Large touch targets
```

**Example parameter file** (testname-auto.par.json):
```json
{
  "responsemode": "auto"
}
```

**Example parameter file** (testname-userselect.par.json):
```json
{
  "responsemode": "userselect"
}
```

**Test each mode:**

Run from the test directory — `--pfile` takes just the filename (PEBL looks in `params/` automatically):
```bash
cd battery/testname

# Test auto mode (REQUIRED)
../../bin/pebl2 testname.pbl --pfile testname-auto.par.json

# Test userselect mode (REQUIRED)
../../bin/pebl2 testname.pbl --pfile testname-userselect.par.json

# Test additional modes
../../bin/pebl2 testname.pbl --pfile testname-keyboardSafe.par.json
../../bin/pebl2 testname.pbl --pfile testname-arrowLR.par.json
```

**Verify userselect mode:**
- Test prompts user to select response keys
- User can choose any two distinct keys
- Selected keys are displayed in footer
- Test works with selected keys
- Data file records semantic responses (not raw keys)

**Migration Checklist:**
- [ ] Added responsemode to schema with "auto" as default
- [ ] Set responsesemantics in code (NOT schema)
- [ ] Created layout with CreateLayout()
- [ ] Configured header/subheader/footer text
- [ ] Updated MessageBox calls to fit stimulus region
- [ ] Replaced gVideoWidth/2 and gVideoHeight/2 with gLayout.centerX/centerY
- [ ] Replaced WaitForListKeyPress with WaitForLayoutResponse
- [ ] Updated ALL translation files (not just English)
- [ ] **REQUIRED**: Created testname-auto.par.json parameter file
- [ ] **REQUIRED**: Created testname-userselect.par.json parameter file
- [ ] Created additional parameter files for other modes (optional but recommended)
- [ ] Tested auto mode works correctly (platform-aware selection)
- [ ] Tested userselect mode prompts for key selection and works correctly
- [ ] Tested additional response modes if parameter files created
- [ ] Verified data output uses semantic names (not raw keys)

**Reference implementations:**
- Simple 2AFC: `battery/evenodd/evenodd.pbl`
- With semantic labels: `battery/luckvogel/luckvogel.pbl`
- Single-key (go/no-go): `battery/gonogo/gonogo.pbl`
- See full guide: `doc/LAYOUT_MIGRATION_GUIDE.md`

**For detailed migration examples, troubleshooting, and advanced patterns, see `doc/LAYOUT_MIGRATION_GUIDE.md`**

---

#### 2.4 Translation File Updates

**For each translation file** (`translations/testname.pbl-*.json`):

1. **Check all instruction strings** for offline language
2. **Update any experimenter references**
3. **Verify browser-appropriate instructions**
4. **Test that translations render correctly in browser**

**⚠️ IMPORTANT: Review final messages in translation files:**

Tests often use different field names for completion messages. Check for:
- `DEBRIEF` - Most common debrief/completion message
- `FINISH` - End-of-test message
- `COMPLETION`, `COMPLETE`, `DONE` - Alternative completion messages
- `THANKYOU` - Thank you messages
- Any field used after `UploadFile()` in the .pbl code

**Completion message guidelines:**

| ❌ Problematic | ✅ Recommended |
|----------------|----------------|
| "Alert the experimenter that you are done" | "Press any key to continue." |
| "You may now leave. Please alert the experimenter." | "Press any key to continue." |
| "Thank you. Close the browser when instructed." | "Press any key to continue." |
| "Congratulations. Hit any key to finish" | "Thank you for participating. Hit any key to continue." |

**Why "Press any key to continue" instead of "close window"?**
- Tests may run standalone OR as part of a test chain
- In a chain, closing the window would break the sequence
- Generic continuation messages work in both contexts
- The HTML shell handles "study complete" messaging when appropriate

Example fix in JSON:
```json
{
  "INSTRUCTIONS": "When you are ready to begin, press the spacebar.",
  "DEBRIEF": "Press any key to continue.",
  "FINISH": "Thank you for participating. Hit any key to continue."
}
```

**Search for completion messages:**
```bash
# Find all possible completion field names
grep -i "debrief\|finish\|complete\|done\|thankyou" upload-battery/testname/translations/*.json
```

### Phase 3: Parameter Schema Creation

#### 3.1 Schema File Requirements

**Every test MUST have a `.schema.json` file** for the web UI to configure parameters.

**File location**: `upload-battery/testname/taskfile.pbl.schema.json`

**Note**: The schema filename must match the .pbl filename, NOT the directory name.
- Example: `bcst/cardsort.pbl` → `bcst/cardsort.pbl.schema.json`

**Schema format** (JSON):
```json
{
  "parameters": [
    {
      "name": "parameter_name",
      "type": "integer|float|string|boolean",
      "default": default_value,
      "description": "Human-readable description"
    }
  ]
}
```

**Complete example** (`stroop/stroop.pbl.schema.json`):
```json
{
  "parameters": [
    {
      "name": "numtrials",
      "type": "integer",
      "default": 40,
      "description": "Number of trials per condition"
    },
    {
      "name": "fixation_time",
      "type": "integer",
      "default": 500,
      "description": "Fixation duration in milliseconds"
    },
    {
      "name": "iti",
      "type": "integer",
      "default": 250,
      "description": "Inter-trial interval in milliseconds"
    },
    {
      "name": "practice",
      "type": "boolean",
      "default": true,
      "description": "Include practice trials"
    },
    {
      "name": "feedback",
      "type": "boolean",
      "default": true,
      "description": "Provide feedback during practice"
    }
  ]
}
```

#### 3.2 Parameter Data Types

The schema supports four data types:

| Type | JSON Default Example | Web UI Element | PEBL Usage |
|------|---------------------|----------------|------------|
| `integer` | `40` | Number input (whole numbers) | `gParams.numtrials` |
| `float` | `1.5` | Number input (decimals) | `gParams.threshold` |
| `string` | `"red"` | Text input | `gParams.color` |
| `boolean` | `true` or `false` | Checkbox | `gParams.practice` |

**Boolean handling in PEBL:**
- JSON `true` → PEBL `1`
- JSON `false` → PEBL `0`
- In PEBL code: `if(gParams.practice) { ... }`

#### 3.3 Creating Schema from Existing Code

If the test already uses parameters, find them in the .pbl file:

```bash
# Search for parameter usage
grep "gParams\." upload-battery/testname/*.pbl

# Example output:
# numtrials <- gParams.numtrials
# if(gParams.practice) { ... }
```

Then create the schema file with each parameter found.

#### 3.4 Common Schema Issues

**Issue**: Using 1/0 instead of true/false for booleans
```json
// ❌ Wrong - integer when should be boolean
{
  "name": "practice",
  "type": "integer",
  "default": 1,
  "description": "Include practice (1=yes, 0=no)"
}

// ✅ Correct - use boolean type
{
  "name": "practice",
  "type": "boolean",
  "default": true,
  "description": "Include practice trials"
}
```

**Issue**: Filename mismatch
```
// ❌ Wrong - schema name matches directory not file
bcst/bcst.pbl.schema.json  (but .pbl file is cardsort.pbl)

// ✅ Correct - schema name matches .pbl file
bcst/cardsort.pbl.schema.json
```

**Issue**: Missing schema entirely
```bash
# Check if schema exists (adjust for actual .pbl filename)
ls upload-battery/testname/*.schema.json

# If missing, create it following the examples above
```

### Phase 4: Screen Size and Aspect Ratio

#### 4.1 Canvas Aspect Ratio Issues

**Problem**: HTML canvas may not preserve aspect ratio, causing distorted visuals.

**Solution**: Use PEBL's built-in screen sizing functions correctly.

**Recommended practice**:
```pebl
## Define standard screen size
gScreenWidth <- 1024
gScreenHeight <- 768

## Create window with fixed aspect ratio
gWin <- MakeWindow("grey")

## For stimuli, use relative positioning
centerX <- gScreenWidth / 2
centerY <- gScreenHeight / 2

## Position elements relative to center
stimulus <- EasyLabel("Text", centerX, centerY, gWin, 20)
```

**Avoid hardcoded pixel positions** that assume specific screen size:
```pebl
## ❌ Bad - assumes exact screen size
target <- Circle(512, 384, 50, MakeColor("red"), 1)

## ✅ Good - uses relative positioning
centerX <- gVideoWidth / 2
centerY <- gVideoHeight / 2
target <- Circle(centerX, centerY, 50, MakeColor("red"), 1)
```

#### 4.2 Testing on Different Screen Sizes

**Test on multiple resolutions**:
- Desktop: 1920x1080, 1366x768, 1024x768
- Tablet: 1024x768, 800x600
- Mobile (if applicable): 375x667, 414x896

**Browser zoom levels**: Test at 100%, 125%, 150%

**Check for**:
- Text overflow
- Overlapping elements
- Off-screen stimuli
- Distorted images
- Unreadable fonts

#### 4.3 Responsive Design Patterns

**Use GetVideoResolution() to adapt**:
```pebl
define AdaptToScreenSize()
{
    ## Get actual screen size
    screenInfo <- GetVideoResolution()
    gScreenWidth <- First(screenInfo)
    gScreenHeight <- Nth(screenInfo, 2)

    ## Adjust font size based on screen
    if(gScreenWidth < 1024)
    {
        gFontSize <- 18
    } else {
        gFontSize <- 24
    }
}
```

### Phase 5: Browser Compatibility Testing

#### 5.1 Required Browser Tests

Test in all major browsers:
- [ ] **Chrome** (latest version)
- [ ] **Firefox** (latest version)
- [ ] **Safari** (latest version, if Mac available)
- [ ] **Edge** (latest version)

#### 5.2 Test Checklist Per Browser

- [ ] Test loads without errors
- [ ] Instructions display correctly
- [ ] All stimuli appear properly
- [ ] Keyboard input works
- [ ] Mouse input works (if used)
- [ ] Timing appears reasonable (no obvious delays)
- [ ] Data uploads successfully
- [ ] Test completes without hanging
- [ ] No console errors (F12 developer tools)

#### 5.3 Common Browser Issues

**Issue**: Audio doesn't autoplay
- **Solution**: Add user interaction before playing sounds

**Issue**: Fullscreen doesn't work
- **Solution**: Fullscreen API requires user gesture, add explicit button

**Issue**: Keyboard layout differences
- **Solution**: Use key codes, not key characters

**Issue**: Font rendering differences
- **Solution**: Test font sizes across browsers, use web-safe fonts

### Phase 6: Performance and Timing

#### 6.1 Timing Accuracy Considerations

**Browser timing is less precise than native**:
- Native PEBL: ~1ms precision possible
- Browser PEBL: ~16ms precision (varies by browser)
- Asyncify overhead: May add small delays

**Mitigation strategies**:
1. **Use longer intervals** where possible (>50ms)
2. **Record actual timestamps**, don't rely on requested times
3. **Test timing empirically** on target platform
4. **Document timing limitations** in test description

**Timing verification code**:
```pebl
## Test timing accuracy
define TestTiming()
{
    ## Request 100ms wait
    start <- GetTime()
    Wait(100)
    end <- GetTime()
    actualWait <- end - start

    ## Log discrepancy
    Print("Requested: 100ms, Actual: " + actualWait + "ms")
}
```

#### 6.2 Performance Optimization

**Large files**: Avoid large images/sounds that slow loading
- Recommended: Images <1MB, sounds <5MB
- Use compressed formats (PNG, MP3)

**Many stimuli**: Pre-load before test starts
```pebl
## Pre-load images
stimuli <- []
loop(i, Sequence(1, 100, 1))
{
    img <- MakeImage("stim" + i + ".png")
    PushOnEnd(stimuli, img)
}
```

**Complex calculations**: Do once, reuse results
```pebl
## Calculate positions once
positions <- MakeGrid(10, 10, gScreenWidth, gScreenHeight)
```

### Phase 7: Testing and Validation

#### 7.1 Functionality Testing

- [ ] **Complete test run** from start to finish
- [ ] **Data output** produces expected files
- [ ] **Upload verification** - check files appear in platform
- [ ] **Parameter changes** - test different parameter values work
- [ ] **Edge cases** - test with extreme parameter values
- [ ] **Error handling** - test what happens if participant exits early

#### 7.2 Data Quality Verification

**Compare native vs. online data**:
```bash
# Run test natively
bin/pebl2 battery/testname/testname.pbl -s 001

# Run test online
# (via browser)

# Compare data files
diff battery/testname/data/results-001.csv \
     PEBLOnlinePlatform/uploads/TOKEN/testname/results-001.csv
```

**Check for**:
- Same columns in output
- Same data types
- Reasonable values
- No missing data
- Correct timestamp format

#### 7.3 User Experience Testing

**Clarity**:
- [ ] Instructions are clear for online testing
- [ ] No references to experimenter/lab environment
- [ ] Progress indicators work (if applicable)
- [ ] Feedback is appropriate

**Accessibility**:
- [ ] Font sizes are readable
- [ ] Color contrast is sufficient
- [ ] Keyboard navigation works
- [ ] Screen reader compatibility (if applicable)

### Phase 8: Update available_tests.json

**CRITICAL**: Every migrated test MUST be added to `PEBLOnlinePlatform/config/available_tests.json`

#### 8.1 Test Catalog Registration

**File location**: `../PEBLOnlinePlatform/config/available_tests.json`

**Purpose**: This file controls which tests appear in the online platform's test selection interface.

**Adding a new test:**

1. **Open the available_tests.json file** in the PEBLOnlinePlatform config directory

2. **Add an entry to the "tests" array** with the following structure:

```json
{
  "id": "testname",
  "name": "Test Display Name",
  "description": "Brief description of what this test measures",
  "category": "memory|attention|executive|perception|other",
  "duration_minutes": 5,
  "enabled": true,
  "directory": "testname",
  "main_file": "taskfile.pbl",
  "screenshots": [
    "screenshots/screenshot1.png",
    "screenshots/screenshot2.png"
  ],
  "references": [
    {
      "citation": "Author, A. (Year). Title. Journal, volume(issue), pages.",
      "doi": "10.xxxx/xxxxx",
      "url": "https://..."
    }
  ],
  "tags": ["validated", "classic", "cognitive"],
  "requirements": {
    "min_screen_width": 1024,
    "min_screen_height": 768,
    "audio_required": false,
    "fullscreen_recommended": true
  }
}
```

**Field descriptions:**
- `id`: Unique identifier (typically directory name, lowercase, no spaces)
- `name`: Human-readable test name shown in UI
- `description`: 1-2 sentence overview
- `category`: Primary category for filtering/organization
- `duration_minutes`: Estimated completion time
- `enabled`: Set to `true` to make test available
- `directory`: Test directory name in battery/
- `main_file`: The .pbl filename (may differ from directory name!)
- `screenshots`: Array of screenshot paths (relative to test directory)
- `references`: Academic citations for the test
- `tags`: Searchable/filterable tags
- `requirements`: Technical requirements (screen size, audio, etc.)

**Example entry:**
```json
{
  "id": "stroop",
  "name": "Stroop Color-Word Test",
  "description": "Classic test measuring interference between word reading and color naming",
  "category": "attention",
  "duration_minutes": 7,
  "enabled": true,
  "directory": "stroop",
  "main_file": "stroop.pbl",
  "screenshots": [
    "screenshots/instructions.png",
    "screenshots/trial.png",
    "screenshots/feedback.png"
  ],
  "references": [
    {
      "citation": "Stroop, J. R. (1935). Studies of interference in serial verbal reactions. Journal of Experimental Psychology, 18(6), 643-662.",
      "doi": "10.1037/h0054651",
      "url": null
    }
  ],
  "tags": ["classic", "validated", "interference", "color"],
  "requirements": {
    "min_screen_width": 800,
    "min_screen_height": 600,
    "audio_required": false,
    "fullscreen_recommended": false
  }
}
```

#### 8.2 Extracting Information

**Where to find the information:**

1. **Test description**: Usually in the native battery's about.txt or comments in .pbl file
2. **References**: Search for citations in:
   - Test .pbl file comments
   - About.txt file
   - Published PEBL papers
   - Original test development papers

3. **Duration**: Estimate from:
   - Trial counts and timing
   - Test run during migration
   - Native battery documentation

4. **Category**: Choose based on primary cognitive domain:
   - `memory`: Working memory, long-term memory tests
   - `attention`: Selective attention, sustained attention
   - `executive`: Planning, set-shifting, inhibition
   - `perception`: Visual/auditory perception tasks
   - `other`: Tasks that don't fit above categories

#### 8.3 Screenshots

**Required**: Capture 2-4 screenshots showing:
1. Instructions screen
2. Example trial/stimulus
3. Feedback screen (if applicable)
4. Completion screen (if relevant)

**How to capture:**
1. Run test in browser
2. Use browser screenshot tool (F12 → Screenshot, or browser extension)
3. Save as PNG
4. Place in `upload-battery/testname/screenshots/`
5. Name descriptively: `instructions.png`, `trial.png`, `feedback.png`

**Screenshot guidelines:**
- Resolution: 1024x768 or higher
- Format: PNG
- Show actual test interface, not just blank screens
- Avoid including participant data in screenshots
- Compress if >1MB (use tools like pngquant)

### Phase 9: Documentation Files

#### 9.1 Update Test Description

**File**: `testname/taskfile.pbl.about.txt`

**Should include**:
- Test name and purpose
- Duration estimate (for online platform)
- Browser requirements
- Any known limitations
- Citation information
- Parameter descriptions

**Example**:
```
Test: Stroop Color-Word Interference Test
Duration: ~5-7 minutes
Browsers: Chrome, Firefox, Safari, Edge (latest versions)

Description:
Classic Stroop task measuring interference between word reading and color naming.
Participants respond to the ink color of color words.

Parameters:
- numtrials: Number of trials per condition (default: 40)
- practice: Include practice trials (default: yes)

Citation:
Stroop, J. R. (1935). Studies of interference in serial verbal reactions.
Journal of Experimental Psychology, 18(6), 643-662.

Known Limitations:
- Timing precision is ~16ms in browsers (sufficient for this task)
```

#### 8.2 Migration Notes

**Create a migration log** in test directory: `MIGRATION_NOTES.txt`

Document:
- Date migrated
- Changes made to code
- Issues encountered
- Browser compatibility notes
- Any deviations from native version

**Example**:
```
Migration Date: 2025-10-19
Migrated By: [Name]

Changes Made:
- Added UploadFile() for data transfer
- Removed "alert experimenter" instructions
- Updated Spanish translations for online context
- Created parameter schema file
- Fixed aspect ratio issues with stimuli

Issues Encountered:
- Font size too small on mobile, increased base size
- Audio autoplay blocked in Safari, added click-to-start

Browser Testing:
✓ Chrome 118 - All tests pass
✓ Firefox 119 - All tests pass
✓ Safari 17 - Audio requires user interaction, documented
✓ Edge 118 - All tests pass

Timing Validation:
- Requested 500ms fixation: Actual 505-520ms (acceptable)
- Requested 100ms ITI: Actual 105-125ms (acceptable)
```

---

## Common Migration Patterns

### Pattern 1: Simple Cognitive Test (No Media)

**Example**: Digit span, simple RT

**Typical changes**:
1. Add data upload call
2. Update instructions
3. Create schema file
4. **Integrate Layout & Response System** (if 2AFC or single-key response)
5. Create auto and userselect parameter files
6. Test in browsers

**Estimated time**: 2-3 hours (includes Layout & Response migration)

### Pattern 2: Visual Stimuli Test (Images)

**Example**: Visual search, recognition memory

**Additional considerations**:
1. Image file sizes and formats
2. Preloading images
3. Aspect ratio preservation
4. Screen size adaptation
5. **Layout & Response System integration** (if applicable)
6. Create auto and userselect parameter files

**Estimated time**: 3-5 hours (includes Layout & Response migration if applicable)

### Pattern 3: Audio-Based Test

**Example**: Tone discrimination, audio memory

**Additional considerations**:
1. Audio format compatibility (MP3, OGG)
2. Autoplay restrictions
3. Volume normalization
4. Latency between keypress and sound
5. **Layout & Response System integration** (if applicable)
6. Create auto and userselect parameter files

**Estimated time**: 4-6 hours (includes Layout & Response migration if applicable)

### Pattern 4: Complex Multi-Modal Test

**Example**: N-back with visual+audio, dual-task

**Additional considerations**:
1. Synchronization between modalities
2. Performance optimization
3. Extensive browser testing
4. Timing validation
5. **Layout & Response System integration** (if applicable)
6. Create auto and userselect parameter files
7. Test accessibility with userselect mode

**Estimated time**: 5-10 hours (includes Layout & Response migration if applicable)

---

## Automated Checking Tools

### Script 1: Find Offline Language

```bash
#!/bin/bash
# check-offline-language.sh
# Scan test for offline-specific language

TEST_DIR=$1

echo "Scanning $TEST_DIR for offline-specific language..."
echo ""

echo "=== Main test file ==="
grep -ni "experimenter\|raise your hand\|ask questions\|alert\|wait for" "$TEST_DIR"/*.pbl

echo ""
echo "=== Translation files ==="
grep -ni "experimenter\|raise your hand\|ask questions\|alert\|wait for" "$TEST_DIR"/translations/*.json

echo ""
echo "=== About file ==="
grep -ni "experimenter\|raise your hand\|laboratory\|lab setting" "$TEST_DIR"/*.about.txt
```

### Script 2: Verify Schema Exists

```bash
#!/bin/bash
# check-schema.sh
# Verify all tests have parameter schemas

for dir in upload-battery/*/; do
    testname=$(basename "$dir")
    schema="$dir/${testname}.pbl.schema"

    if [ ! -f "$schema" ]; then
        echo "❌ Missing schema: $testname"
    else
        echo "✓ Has schema: $testname"
    fi
done
```

### Script 3: Test Data Upload

```pebl
## test-upload.pbl
## Minimal test to verify data upload works

define Start()
{
    gToken <- "TEST_TOKEN_123"

    ## Write test file
    file <- FileOpenWrite("upload-test.txt")
    FilePrint(file, "Test data upload")
    FilePrint(file, "Timestamp: " + GetTime())
    FileClose(file)

    ## Upload
    result <- UploadFile(gSubNum, "data/" + gSubNum + "/upload-test.txt", "/upload.json")

    if(result)
    {
        Print("✓ Upload successful")
    } else {
        Print("❌ Upload failed")
    }
}
```

---

## Migration Priority Tiers

### Tier 1: High Priority (Classic Cognitive Tests)

These tests are widely used and should be migrated first:

1. **Stroop** - Color-word interference
2. **Flanker** - Response competition
3. **N-back** - Working memory
4. **Corsi** - Spatial memory
5. **Tower of London** - Planning
6. **Wisconsin Card Sort** - Set shifting
7. **Go/No-Go** - Response inhibition
8. **Simon** - Spatial interference
9. **Iowa Gambling Task** - Decision making
10. **Digit Span** - Verbal memory

**Estimated effort**: 30-40 hours total

### Tier 2: Standard Tests (Commonly Used)

Important but less critical:

11. **Trail Making** - Visual scanning
12. **Verbal Fluency** - Language production
13. **Continuous Performance Test** - Sustained attention
14. **Change Detection** - Visual working memory
15. **Posner Cueing** - Spatial attention
... (40 more tests)

**Estimated effort**: 80-120 hours total

### Tier 3: Specialized Tests

Niche or experimental tests:

- Specialized attention tests
- Novel paradigms
- Research-specific measures

**Estimated effort**: 60-80 hours total

---

## Quality Assurance Checklist

Before marking a test as "migrated and ready":

### Code Quality
- [ ] No hardcoded file paths
- [ ] Uses UploadFile() for data output
- [ ] Parameters read from schema
- [ ] No experimenter references in code
- [ ] Error handling for edge cases

### Documentation Quality
- [ ] Schema file complete with descriptions
- [ ] About file updated for online context
- [ ] Migration notes documented
- [ ] Known limitations listed

### Testing Quality
- [ ] Tested in Chrome, Firefox, Safari, Edge
- [ ] Tested at different screen sizes
- [ ] Data output verified
- [ ] Upload confirmed working
- [ ] Timing validated (if critical)
- [ ] No console errors

### Translation Quality
- [ ] All translation files updated
- [ ] No offline language in any language
- [ ] Translations render correctly
- [ ] Special characters display properly

---

## Common Migration Mistakes - Lessons Learned

This section documents real mistakes made during test migrations to help avoid them in the future.

### Mistake 1: Creating requirements.json Without Checking Existing Tests

**What happened (TNT migration)**:
- Created a requirements.json file with made-up fields like `min_version`, `required_functions`, and `required_libraries`
- This schema didn't match any existing tests
- Only discovered the error when asked if other tests use `required_libraries`

**Why this was wrong**:
- There's a documented schema in `PEBLOnlinePlatform/docs/TEST_REQUIREMENTS_SYSTEM.md`
- All other tests use a consistent format with `version`, `display`, `input`, `audio`, `performance`, `accessibility`, and `device_support` sections
- Making up a new schema creates inconsistency

**Correct approach**:
1. **FIRST** check existing tests: `cat upload-battery/simon/params/requirements.json`
2. **THEN** read the documentation: `PEBLOnlinePlatform/docs/TEST_REQUIREMENTS_SYSTEM.md`
3. **ONLY THEN** create requirements.json following the established pattern

**Example - Correct requirements.json format**:
```json
{
  "version": "1.0",
  "display": {
    "minimum_width": 1280,
    "minimum_height": 800,
    "recommended_width": 1280,
    "recommended_height": 800,
    "fullscreen_recommended": false
  },
  "input": {
    "keyboard": true,
    "mouse_buttons": false,
    "mouse_movement": false,
    "touch": false,
    "notes": "Uses left and right arrow keys only"
  },
  "audio": {
    "required": false,
    "output": false,
    "input": false
  },
  "performance": {
    "timing_critical": true,
    "max_acceptable_latency_ms": 50,
    "notes": "Reaction time measurement requires accurate timing"
  },
  "accessibility": {
    "color_discrimination_required": false,
    "motion_sensitivity_warning": false,
    "keyboard_only": true
  },
  "device_support": {
    "desktop": true,
    "tablet": false,
    "mobile": false,
    "notes": "Large screen required for arithmetic problem display"
  }
}
```

### Mistake 2: Not Keeping battery/ and upload-battery/ Identical

**What happened (TNT migration)**:
- Created upload-battery/TNT/ with `InitializeUpload()` and `UploadFile()` calls
- Left battery/TNT/ without these calls, thinking they were "online-only"
- This created two different versions that had to be maintained separately

**Why this was wrong**:
- `InitializeUpload()` and `UploadFile()` are **no-ops when `--upload` flag is not set**
- They can and should be in BOTH versions
- Having identical files makes maintenance much simpler
- Native testing should test the EXACT same code that runs online

**Correct approach**:
- **Always** put `InitializeUpload()` and `UploadFile()` in battery/ version
- **Copy** the exact same .pbl file to upload-battery/
- Keep battery/ and upload-battery/ **identical** (except data/ directories which are excluded)

**What should be identical**:
- All .pbl files
- All translation JSON files
- All params/ files (including requirements.json)
- All .about.txt files
- Example data files in data/example/

**Command to verify they're identical**:
```bash
diff -qr battery/testname/ upload-battery/testname/ --exclude="data"
# Should output nothing if files are identical
```

### Mistake 3: Missing requirements.json in battery/ Directory

**What happened (TNT migration)**:
- Created requirements.json only in upload-battery/TNT/params/
- Forgot to also put it in battery/TNT/params/

**Why this was wrong**:
- battery/ and upload-battery/ should be identical
- Native tests can also benefit from requirements metadata
- Creates maintenance burden when files don't match

**Correct approach**:
- Create requirements.json in upload-battery/testname/params/
- **Immediately copy** to battery/testname/params/
- Use `diff` to verify they match

### Mistake 4: Forgetting Example Data for Test Library

**What happened (TNT migration)**:
- Copied example data to battery/TNT/data/example/ and upload-battery/TNT/data/example/
- But forgot to copy to `PEBLOnlinePlatform/public/battery/TNT/data/example/`
- Sample data didn't appear on the test library page

**Why this was wrong**:
- The test library web page serves files from `public/battery/`
- Example data needs to be web-accessible for documentation
- The `sample_data_url` in test-metadata/testname.json points to this location

**Correct approach**:
When adding example data, copy to **THREE locations**:
1. `battery/testname/data/example/` - For reference
2. `upload-battery/testname/data/example/` - For consistency
3. `PEBLOnlinePlatform/public/battery/testname/data/example/` - **For web access**

**Command to copy all three**:
```bash
# After creating example data in battery/testname/data/example/
cp -r battery/testname/data/example/ upload-battery/testname/data/
mkdir -p PEBLOnlinePlatform/public/battery/testname/data/
cp -r battery/testname/data/example/ PEBLOnlinePlatform/public/battery/testname/data/
```

### Mistake 5: Not Checking for All Required Translations BEFORE Starting Migration

**What happened (Multiple migrations)**:
- Started migration with only English translation
- Made code changes, updated parameters, added LSL markers
- Only discovered missing translations (es, de, pt, fr, nl, it) at the end
- Had to create 6+ translations retroactively

**Why this was wrong**:
- **Standard PEBL tests require 7 translations**: en, es, de, pt, fr, nl, it
- Creating translations after migration requires re-reviewing all text changes
- Incomplete translations break the test in other languages
- Missing translations result in blank/undefined strings
- Much harder to create consistent translations after the fact

**Correct approach - CHECK TRANSLATIONS FIRST (Phase 1 of migration)**:

```bash
# STEP 1: List all existing translation files
ls battery/testname/translations/

# STEP 2: Check against required list
# Required: en, es, de, pt, fr, nl, it
```

**Create a checklist and verify BEFORE any code changes**:
- [ ] testname.pbl-en.json (English) - REQUIRED
- [ ] testname.pbl-es.json (Spanish)
- [ ] testname.pbl-de.json (German)
- [ ] testname.pbl-pt.json (Portuguese)
- [ ] testname.pbl-fr.json (French)
- [ ] testname.pbl-nl.json (Dutch)
- [ ] testname.pbl-it.json (Italian)

**If ANY translation is missing**:
1. **STOP the migration**
2. Create missing translations FIRST
3. Get translations reviewed by native speakers if possible
4. THEN proceed with code migration

**When updating ANY string during migration**:
1. Update in ALL 7 languages simultaneously
2. Never update just English and "come back to translations later"
3. Keep all translation files in sync throughout migration

### Mistake 6: Using Actual Line Breaks in JSON Strings

**What happened (TNT migration)**:
- French translation had actual newlines embedded in the "reminder" string
- This creates invalid JSON that some parsers reject

**Why this was wrong**:
- JSON strings must use escape sequences for newlines
- Actual line breaks in strings are not valid JSON
- Some JSON validators/prettifiers will reject or corrupt the file

**Correct approach**:
- **Always** use `\n` for line breaks in JSON strings
- Never use actual newlines within string values

**Example**:
```json
// ❌ Wrong - actual line breaks
"reminder": "Line 1
Line 2
Line 3"

// ✅ Correct - escaped newlines
"reminder": "Line 1\nLine 2\nLine 3"
```

### Mistake 7: Wrong Metadata JSON Field Names

**What happened (simon/TNT migration)**:
- Used `"files_created"` instead of `"files"` in test-metadata JSON
- Used template filenames like `"simon-<SUBNUM>.csv"` instead of actual example filenames
- Added unnecessary `"sample_data_url"` field

**Why this was wrong**:
- The test library web interface reads the `data_output.files` array to display sample data
- It expects actual example filenames (e.g., `"example-data.csv"`), not templates with placeholders
- The `sample_data_url` field is not used when `files` array is present

**Correct approach**:
- **Always check** an existing working test (e.g., switcher) for the correct format
- Use `"files"` array with actual example filenames
- Each file entry should include `"filename"`, `"type"`, and `"description"`

**Example - Wrong format**:
```json
"data_output": {
  "summary": "Creates two data files...",
  "files_created": [
    {
      "filename": "simon-<SUBNUM>.csv",
      "description": "Trial-level data..."
    }
  ],
  "sample_data_url": "battery/simon/data/"
}
```

**Example - Correct format**:
```json
"data_output": {
  "summary": "Creates two data files...",
  "files": [
    {
      "filename": "example-data.csv",
      "type": "primary",
      "description": "Trial-level data..."
    },
    {
      "filename": "example-data-summary.txt",
      "type": "summary",
      "description": "Block-level summary..."
    }
  ]
}
```

**Verification command**:
```bash
# Check format matches switcher (a working example)
jq '.data_output.files' PEBLOnlinePlatform/config/test-metadata/switcher.json
jq '.data_output.files' PEBLOnlinePlatform/config/test-metadata/yourtest.json
```

### Mistake 8: Wrong Location for Example Data Files

**What happened (simon/TNT migration)**:
- Created example data in `PEBLOnlinePlatform/public/battery/testname/data/`
- But the correct location is `PEBLOnlinePlatform/battery/testname/data/`
- The `public/` directory should not contain battery files

**Why this was wrong**:
- The test library serves files from the `battery/` directory (not `public/battery/`)
- Creating a `public/battery/` directory adds unnecessary duplication
- The web platform is configured to serve from the correct location

**Correct approach**:
- Example data goes in `PEBLOnlinePlatform/battery/testname/data/`
- **NOT** in `public/battery/testname/data/`
- The `public/` directory structure is separate from battery tests

**File naming conventions**:
- Use `example-data.csv` or `example-data-*.csv` for primary data
- Use `example-data-summary.txt` or similar for summary files
- Prefix all example files with `example-` for clarity

**Correct workflow for example data**:

1. **Generate** example data by running test with subject code "example":
   ```bash
   bin/pebl2 battery/testname/testname.pbl -s example
   # This creates: battery/testname/data/example/testname-example.csv
   ```

2. **Rename and move** to root of data/ directory with standard name:
   ```bash
   mv battery/testname/data/example/testname-example.csv \
      battery/testname/data/example-data.csv
   ```

3. **Copy** to deployment locations (directly in data/, NOT in subdirectory):
   ```bash
   cp battery/testname/data/example-data.csv \
      upload-battery/testname/data/example-data.csv

   cp battery/testname/data/example-data.csv \
      PEBLOnlinePlatform/testname/data/example-data.csv
   ```

**CRITICAL**: Example data must be placed **directly** in the `data/` directory, NOT in participant subdirectories like `data/example/` or `data/example-participant/`.

**Final correct file placement**:
- ✅ `battery/testname/data/example-data.csv` (directly in data/)
- ✅ `upload-battery/testname/data/example-data.csv` (directly in data/)
- ✅ `PEBLOnlinePlatform/testname/data/example-data.csv` (directly in data/)

**Deployment script pattern matching**:
- Files matching `example-*` are copied during deployment
- Files matching `*-example-*` are also copied
- All other files in `data/` directories are excluded

**Verification**:
```bash
# Check example data in correct locations (all directly in data/)
ls battery/testname/data/example-data*
ls upload-battery/testname/data/example-data*
ls PEBLOnlinePlatform/testname/data/example-data*

# Should NOT exist (subdirectory structure)
ls upload-battery/testname/data/example/ 2>/dev/null  # Should not exist
ls PEBLOnlinePlatform/testname/data/example/ 2>/dev/null  # Should not exist

# Should NOT exist (public/ directory)
ls PEBLOnlinePlatform/public/battery/testname/data/ 2>/dev/null
# Should return "No such file or directory"
```

### Mistake 9: Attempting to Include Example Data in Bundles

**What happened (simon/TNT migration)**:
- Tried to modify `bundle-config.json` to include example data files
- Attempted to change exclude patterns from `"*/data/*"` to only exclude non-example files
- Thought bundles needed to contain example data for web display

**Why this was wrong**:
- **Bundles should NEVER contain any data files** (example or otherwise)
- Example data is served directly from the git repository at `battery/testname/data/`
- Bundles are for executable code only (`.pbl` files, translations, params)
- Including data would bloat bundle sizes unnecessarily

**Correct approach**:
- **Keep** `"*/data/*"` in bundle excludes
- **Do not** try to include example files in bundles
- **Manually copy** example data to `PEBLOnlinePlatform/battery/testname/data/`
- **Commit** example data to PEBLOnlinePlatform git repository

**Correct bundle-config.json**:
```json
{
  "name": "testname",
  "excludes": [
    "*/data/*",    // ✓ Correct - exclude ALL data
    "*~",
    "*.pbl.png"
  ]
}
```

**Wrong bundle-config.json**:
```json
{
  "name": "testname",
  "excludes": [
    "*/data/[0-9]*",       // ❌ Wrong - trying to selectively include
    "*/data/*[0-9]",       // ❌ Wrong - overly complex
    "*/data/[a-z][a-z]*",  // ❌ Wrong - will still cause issues
    "*~",
    "*.pbl.png"
  ]
}
```

### Mistake 10: Attempting to Auto-Deploy Example Data

**What happened (simon/TNT migration)**:
- Tried to modify `deploy-to-online-platform.sh` to automatically copy example data
- Added rsync rules to include `example-data*` files while excluding other data
- Thought deployment script should handle example data distribution

**Why this was wrong**:
- Example data is **manually managed** in the PEBLOnlinePlatform git repository
- Deployment script should **exclude all data directories** (including example data)
- Auto-deployment would overwrite git-tracked files
- Example data should be committed once and versioned in git, not deployed repeatedly

**Correct approach**:
- **Do not** modify deployment script to copy example data
- **Keep** the existing exclude pattern: `--exclude='*/data/'`
- **Manually copy** example data to PEBLOnlinePlatform when creating/updating tests
- **Commit** example data to PEBLOnlinePlatform git repository

**Correct deploy script (unchanged)**:
```bash
rsync -av \
    --exclude='*/data/' \      # ✓ Correct - exclude all data
    --exclude='*~' \
    "$BATTERY_SRC/" "$BATTERY_DIR/"
```

**Wrong deploy script (don't do this)**:
```bash
rsync -av \
    --include='*/' \
    --include='example-data*' \   # ❌ Wrong - trying to auto-copy
    --exclude='*/data/*' \
    "$BATTERY_SRC/" "$BATTERY_DIR/"
```

**Workflow for example data**:
1. Create example data in `battery/testname/data/`
2. Manually copy to `PEBLOnlinePlatform/battery/testname/data/`
3. Commit to PEBLOnlinePlatform git: `git add battery/testname/data/example-data*`
4. Deploy script **ignores** all data/ directories (as it should)

### Mistake 11: Incomplete PEBLOnlinePlatform Deployment Configuration

**What happened (linejudgment migration - January 2026)**:
- Successfully migrated test to PEBL 2.3 with Layout & Response System
- Copied entire test directory to `PEBLOnlinePlatform/battery/linejudgment/`
- Updated `config/library-tests.json` to register the test
- Created comprehensive `config/test-metadata/linejudgment.json`
- BUT: Web launcher couldn't find the test - error: "Test directory does not exist: /usr/local/share/pebl2/battery/linejudgment"
- Missing critical configuration in `config/test_catalog.json`

**Why this was wrong**:
- PEBLOnlinePlatform uses a **bundle loading system** for web deployment
- Tests are packaged into `.data` files and loaded on-demand
- The launcher requires **TWO separate entries** in `test_catalog.json`:
  1. **data_bundles entry**: Defines where to fetch the bundle file
  2. **tests entry**: Associates the test ID with its bundle
- Without both entries, the launcher doesn't know the bundle exists or how to load it
- Simply copying files to `battery/` directory is insufficient - bundles must be explicitly configured

**CRITICAL: Proper Development and Deployment Workflow**:

**⚠️ IMPORTANT**: Deploy to `PEBLOnlinePlatform/` (NOT `/var/www/pebl/`)

The correct workflow is:
1. **Work in battery/ FIRST** - All development happens in the main repository
2. **Complete ALL translations** - Update all language files before deployment
3. **Test thoroughly** - Run and test the battery version extensively
4. **THEN deploy to PEBLOnlinePlatform/** - Copy completed, tested code
5. **Update configuration files** - All FOUR .json files in PEBLOnlinePlatform
6. **Add example data** - Copy example data with standard naming

**Never deploy incomplete or untranslated tests to PEBLOnlinePlatform.**

**Complete deployment checklist for PEBLOnlinePlatform**:

**Prerequisites** (must be completed in battery/ BEFORE deployment):
- ✅ Test fully migrated to PEBL 2.3 (if applicable)
- ✅ All translations completed and updated
- ✅ Parameter files created (including -auto.par.json and -userselect.par.json)
- ✅ Test thoroughly tested in battery/
- ✅ Example data generated

**Required PEBLOnlinePlatform configuration updates** (ALL FOUR required):
1. `config/library-tests.json` - Register test in library
2. `config/test-metadata/testname.json` - Complete test documentation
3. `config/test_catalog.json` (data_bundles section) - Define bundle
4. `config/test_catalog.json` (tests section) - Link test to bundle

**Step 1: Copy test files from battery/ to PEBLOnlinePlatform/**
```bash
# Copy entire test directory from battery/ (NOT upload-battery/)
cp -r battery/linejudgment/ PEBLOnlinePlatform/battery/linejudgment/

# Verify all required files present
ls PEBLOnlinePlatform/battery/linejudgment/linejudgment.pbl
ls PEBLOnlinePlatform/battery/linejudgment/linejudgment.pbl.png
ls PEBLOnlinePlatform/battery/linejudgment/params/*.par.json
ls PEBLOnlinePlatform/battery/linejudgment/translations/*.json
```

**Step 2: Update library-tests.json (CONFIG FILE 1 of 4)**

Add test entry in `PEBLOnlinePlatform/config/library-tests.json`:

```json
{
  "id": "linejudgment",
  "name": "Line Judgment Task",
  "main_file": "linejudgment.pbl",
  "path": "battery/linejudgment",
  "category": "Processing Speed",
  "available": true,
  "estimated_minutes": 10
}
```

**Step 3: Create comprehensive test metadata (CONFIG FILE 2 of 4)**

Create `PEBLOnlinePlatform/config/test-metadata/linejudgment.json` with:
- Complete overview section (brief_description, task_description, what_it_measures, cognitive_domains, duration_minutes, suitable_for)
- Scientific background (references, validation_status)
- **CRITICAL**: Complete data_output section with:
  - summary
  - files_created array (with filename and description for each output file)
  - **key_variables array** documenting ALL CSV columns
  - **sample_data_url** pointing to example data location
- Parameters section referencing schema
- Assets (screenshots, videos, demo_url)
- Platform info (category, browser_compatibility, known_issues)
- Related tests
- Notes and tags
- Sources

**Step 4: ⚠️ CRITICAL - Update test_catalog.json with BOTH required entries (CONFIG FILES 3 & 4 of 4)**

This is the step that's most often forgotten but **absolutely required** for web deployment.

Edit `PEBLOnlinePlatform/config/test_catalog.json` and add **TWO separate entries** in different sections:

**4a. Add data bundle entry (CONFIG FILE 3 of 4)** - in "data_bundles" section:
```json
"linejudgment": {
  "file": "linejudgment.data",
  "url": "/runtime/test-bundles/linejudgment.data",
  "size_mb": 0.1,
  "description": "Line Judgment Task - Speed-accuracy tradeoff task comparing line lengths (Henmon, 1910)",
  "tests": [
    "linejudgment"
  ],
  "base_url": "/runtime/test-bundles"
}
```

**4b. Add test entry (CONFIG FILE 4 of 4)** - in "tests" section:
```json
"linejudgment": {
  "id": "linejudgment",
  "name": "Line Judgment Task",
  "directory": "linejudgment",
  "main_file": "linejudgment.pbl",
  "screenshot": "battery/linejudgment/linejudgment.pbl.png",
  "version": "1.0",
  "collection": "basic_cognitive",
  "data_bundle": "linejudgment",
  "platform": [
    "web",
    "native"
  ],
  "visibility": "public",
  "min_tier": "free",
  "tags": [
    "processing_speed",
    "speed-accuracy-tradeoff",
    "perception",
    "visuospatial",
    "classic",
    "migrated-2.3"
  ],
  "size_mb": 0.1,
  "duration_minutes": 10,
  "copyright_status": "open",
  "description": "Speed-accuracy tradeoff task where participants quickly discriminate which of two vertical lines is longer under varying deadline conditions. Based on Henmon (1910), measures the relationship between response time constraints and decision accuracy across progressively shorter deadlines (10s, 2s, 1s, 700ms, 500ms).",
  "citation": "Henmon, V. A. C. (1911). The relation of the time of a judgment to its accuracy. Psychological Review, 18(3), 186-201."
}
```

**Key fields to note**:
- `collection`: Test collection (e.g., "basic_cognitive", "spatial_cognition")
- `data_bundle`: **MUST match** the bundle name from data_bundles section
- `tags`: List primary cognitive domain FIRST (e.g., "processing_speed")

**Step 5: Create and copy example data files**

Generate example data by running the test, then copy with **standard naming**:

```bash
# Run test to generate example data
bin/pebl2 battery/linejudgment/linejudgment.pbl -s example-participant

# Copy with standard names to PEBLOnlinePlatform
cp battery/linejudgment/data/example-participant/linejudgment-data-example-participant.csv \
   PEBLOnlinePlatform/battery/linejudgment/data/example-data.csv

cp battery/linejudgment/data/example-participant/linejudgment-report-example-participant.txt \
   PEBLOnlinePlatform/battery/linejudgment/data/example-data-report.txt
```

**Standard example data naming conventions**:
- Primary data: `example-data.csv`
- Summary report: `example-data-report.txt`
- Additional files: `example-data-*.ext`
- **Always** use `example-` prefix
- Place in `battery/testname/data/` (NOT `public/battery/`)

**Step 6: Document data format in test-metadata.json**

Update the `key_variables` array with **ALL** CSV columns:

```json
"key_variables": [
  {
    "name": "subnum",
    "description": "Participant identifier code"
  },
  {
    "name": "block",
    "description": "Block number (1-5 depending on numblocks parameter)"
  },
  {
    "name": "trial",
    "description": "Cumulative trial number across all blocks"
  }
  // ... document EVERY column ...
],
"sample_data_url": "battery/linejudgment/data/example-data.csv"
```

**Common deployment errors and solutions**:

| Error | Symptom | Cause | Solution |
|-------|---------|-------|----------|
| Bundle not found | "Test directory does not exist" in console | Missing `test_catalog.json` entries | Add BOTH data_bundles and tests entries |
| Test not in library | Test doesn't appear in test selection UI | Missing `library-tests.json` entry | Add test to library-tests.json |
| No example data on test page | Sample data links don't work | Missing example files OR wrong `sample_data_url` | Copy example data with standard naming, update metadata |
| Screenshot not showing | Broken image icon | Screenshot not copied OR wrong path | Copy .pbl.png to battery directory, verify path |
| Incomplete metadata | Missing test information | Incomplete test-metadata.json | Fill in all required sections |

**Verification commands**:

```bash
# 1. Verify test_catalog.json has both entries
jq '.data_bundles.linejudgment' PEBLOnlinePlatform/config/test_catalog.json
jq '.tests.linejudgment' PEBLOnlinePlatform/config/test_catalog.json

# 2. Verify library registration
jq '.tests[] | select(.id == "linejudgment")' PEBLOnlinePlatform/config/library-tests.json

# 3. Verify metadata completeness
jq '.data_output.key_variables | length' PEBLOnlinePlatform/config/test-metadata/linejudgment.json
# Should show count matching number of CSV columns

# 4. Verify example data exists
ls PEBLOnlinePlatform/battery/linejudgment/data/example-data*

# 5. Verify screenshot exists
ls PEBLOnlinePlatform/battery/linejudgment/linejudgment.pbl.png
```

**Why this error is particularly problematic**:
- The test appears to be "deployed" (files are in battery/ directory)
- library-tests.json makes it visible in the UI
- BUT: The web launcher **silently fails** to load the bundle
- Browser console shows cryptic filesystem errors
- Easy to miss if you don't check console logs
- Can waste hours debugging before discovering the missing test_catalog.json entries

**Lesson learned**:
**PEBLOnlinePlatform deployment requires FOUR distinct configuration files**:
1. `config/library-tests.json` - Makes test visible in library
2. `config/test-metadata/testname.json` - Provides test documentation
3. `config/test_catalog.json` (data_bundles section) - Defines bundle location
4. `config/test_catalog.json` (tests section) - Associates test with bundle

All four must be updated for successful web deployment.

**📋 DEPLOYMENT WORKFLOW SUMMARY**:

```
✅ CORRECT WORKFLOW:
1. Work in battery/ first - Complete development, translations, testing
2. Copy completed test → PEBLOnlinePlatform/battery/
3. Update ALL FOUR .json configuration files
4. Add example data with standard naming
5. Commit to PEBLOnlinePlatform git

❌ WRONG APPROACHES:
- Deploying to /var/www/ instead of PEBLOnlinePlatform/
- Copying incomplete or untranslated code
- Forgetting any of the four configuration files
- Skipping example data or using wrong naming
```

### Migration Pre-Flight Checklist

Before starting any test migration, check off these items:

- [ ] **FIRST: Check translation files exist (CRITICAL - do this BEFORE any code changes)**
  ```bash
  ls battery/testname/translations/
  ```
  - [ ] Verify all 7 required translations exist: en, es, de, pt, fr, nl, it
  - [ ] If ANY are missing, STOP and create them FIRST
  - [ ] **Do NOT proceed with migration until ALL translations are complete**

- [ ] Read existing test's requirements.json to understand the schema
- [ ] Check TEST_REQUIREMENTS_SYSTEM.md for current format
- [ ] Plan to keep battery/ and upload-battery/ identical
- [ ] Remember upload code goes in BOTH versions (it's a no-op in native)
- [ ] Plan where example data needs to be copied (2 locations: battery/ and PEBLOnlinePlatform/battery/)
- [ ] **Do NOT** create public/battery/ directory
- [ ] Check switcher test-metadata JSON for correct `data_output.files` format
- [ ] **NEW REQUIREMENT**: Plan to create test's own bundle in bundle-config.json
- [ ] **Do NOT** add new tests to core-battery manifest
- [ ] Remember: bundles exclude ALL data - do not modify bundle-config.json data excludes
- [ ] Remember: deploy script excludes ALL data - do not modify deploy script
- [ ] Have `diff` command ready to verify file synchronization

### Post-Migration Verification Commands

After completing a migration, run these commands to verify:

```bash
# 1. Check battery/ and upload-battery/ are identical
diff -qr battery/testname/ upload-battery/testname/ --exclude="data"
# Should output nothing if identical

# 2. Verify requirements.json exists in both locations
ls battery/testname/params/requirements.json
ls upload-battery/testname/params/requirements.json

# 3. Check example data in correct locations (TWO places only)
ls battery/testname/data/example-data*
ls PEBLOnlinePlatform/battery/testname/data/example-data*

# 4. Verify NO example data in public/ directory (should not exist)
ls PEBLOnlinePlatform/public/battery/testname/ 2>/dev/null
# Should return "No such file or directory"

# 5. Verify test-metadata JSON uses correct format
jq '.data_output.files[].filename' PEBLOnlinePlatform/config/test-metadata/testname.json
# Should show actual example filenames like "example-data.csv", NOT templates like "test-<SUBNUM>.csv"

# 6. Verify all translation files have same keys
for lang in en fr de es it nl pt; do
  echo "Checking $lang..."
  jq -r 'keys[]' battery/testname/translations/testname.pbl-$lang.json 2>/dev/null | sort
done | sort | uniq -c
# All files should have same count of each key

# 7. Validate JSON syntax in all translation files (no actual line breaks)
for file in battery/testname/translations/*.json; do
  echo "Validating $file..."
  jq empty "$file" && echo "✓ Valid JSON" || echo "✗ Invalid JSON"
done

# 8. Verify bundle config still excludes all data
jq '.bundles.test_bundles[] | select(.name == "testname") | .excludes[]' bundle-config.json | grep "data"
# Should show "*/data/*" (excludes ALL data)

# 9. Check that example data is NOT in upload-battery (optional - OK if it is)
ls upload-battery/testname/data/example-data* 2>/dev/null || echo "No example data in upload-battery (OK)"
```

---

## Troubleshooting Common Issues

### Issue: Upload fails silently

**Symptoms**: Test completes but no data in platform

**Debug steps**:
1. Check browser console (F12) for errors
2. Verify token is correct
3. Test upload with simple file
4. Check network tab for failed requests

**Common causes**:
- Token not passed to test
- File path incorrect
- Network connectivity issues
- Server-side upload API error

### Issue: Timing is inconsistent

**Symptoms**: Requested waits don't match actual waits

**Debug steps**:
1. Add timing verification code
2. Test in different browsers
3. Measure multiple iterations
4. Check for Asyncify overhead

**Solutions**:
- Use longer intervals
- Accept browser timing limitations
- Record actual times, not requested
- Document timing precision in notes

### Issue: Images don't appear

**Symptoms**: Blank screen or missing stimuli

**Debug steps**:
1. Check image file paths
2. Verify images exist in test directory
3. Check image formats (PNG, JPG, GIF supported)
4. Look for console errors

**Solutions**:
- Use relative paths
- Ensure images packaged in .data file
- Convert unsupported formats
- Preload images before use

### Issue: Test hangs/freezes

**Symptoms**: Test stops responding, no progress

**Debug steps**:
1. Check browser console for errors
2. Look for infinite loops
3. Check for awaiting user input
4. Test with browser debugger

**Solutions**:
- Add timeout mechanisms
- Break long loops into chunks
- Add explicit user prompts
- Use GetTime() to detect hangs

---

## Future Enhancements

### Automated Migration Tools

**Desired capabilities**:
1. Script to auto-generate basic schema from parameter usage
2. Automated scanning for offline language with suggested replacements
3. Browser compatibility test runner
4. Timing accuracy measurement suite
5. Screenshot comparison (native vs. online)

### Platform Improvements

**Needed for better migration experience**:
1. Upload progress indicators in test code
2. Better error reporting for failed uploads
3. Timing measurement tools
4. Browser compatibility indicators
5. Automated data validation

---

## Summary

Successful migration requires attention to:
1. **Data upload** - Add `InitializeUpload()` and `UploadFile()` calls
2. **Instructions** - Remove offline language, update translations
3. **Parameters** - Create complete schema files with responsemode
4. **Layout & Response System** (PEBL 2.3+) - Integrate for 2AFC and single-key tasks
5. **Response modes** - **ALWAYS include "auto" (default) and "userselect" (accessibility)**
6. **Display** - Handle screen sizes and aspect ratios with layout zones
7. **Testing** - Verify across browsers, screen sizes, and response modes
8. **Documentation** - Update about files and create migration notes

**CRITICAL for PEBL 2.3 migrations:**
- ✅ Set `"responsemode": "auto"` as default in schema
- ✅ Always include "userselect" option for accessibility
- ✅ Create `testname-auto.par.json` parameter file (REQUIRED)
- ✅ Create `testname-userselect.par.json` parameter file (REQUIRED)
- ✅ Test both modes to ensure they work correctly

**Estimated timeline per test**: 2-10 hours depending on complexity (includes Layout & Response migration)

**Current status** (Jan 2026):
- ~52 tests in upload-battery
- 18 tests with Layout & Response System (35%)
- ~34 priority tests ready for migration

**Next steps**: Focus on Tier 1 high-priority tests (Phase 1: 10 tests for PEBL 2.3 release)
