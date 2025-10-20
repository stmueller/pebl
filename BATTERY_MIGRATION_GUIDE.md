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
   FileClose(gFileOut)

   ## Upload the files (works online, no-op on native)
   ## IMPORTANT: Must use absolute path "/upload.json" because InitializeUpload()
   ## changes the working directory to the battery test folder
   datafile <- "data/" + gSubNum + "/results-" + gSubNum + ".csv"
   UploadFile(gSubNum, datafile, "/upload.json")
   ```

   **Critical Note**: `UploadFile()` requires three parameters:
   - `subcode` - participant ID (typically `gSubNum`)
   - `datafilename` - path to the data file to upload
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
    FileClose(gFileOut)

    ## Upload data (automatic - works online, skipped on native)
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

**Multiple data files?** Just add multiple upload calls:
```pebl
## Upload all data files
settingsfile <- "/upload.json"
UploadFile(gSubNum, "data/" + gSubNum + "/results-" + gSubNum + ".csv", settingsfile)
UploadFile(gSubNum, "data/" + gSubNum + "/summary-" + gSubNum + ".txt", settingsfile)
UploadFile(gSubNum, "data/" + gSubNum + "/trials-" + gSubNum + ".dat", settingsfile)
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

#### 2.3 Translation File Updates

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
bin/pebl2 battery/testname/testname.pbl -v subnum=001

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
4. Test in browsers

**Estimated time**: 1-2 hours

### Pattern 2: Visual Stimuli Test (Images)

**Example**: Visual search, recognition memory

**Additional considerations**:
1. Image file sizes and formats
2. Preloading images
3. Aspect ratio preservation
4. Screen size adaptation

**Estimated time**: 2-4 hours

### Pattern 3: Audio-Based Test

**Example**: Tone discrimination, audio memory

**Additional considerations**:
1. Audio format compatibility (MP3, OGG)
2. Autoplay restrictions
3. Volume normalization
4. Latency between keypress and sound

**Estimated time**: 3-5 hours

### Pattern 4: Complex Multi-Modal Test

**Example**: N-back with visual+audio, dual-task

**Additional considerations**:
1. Synchronization between modalities
2. Performance optimization
3. Extensive browser testing
4. Timing validation

**Estimated time**: 4-8 hours

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
1. **Data upload** - Replace file writes with HTTP uploads
2. **Instructions** - Remove offline language, update translations
3. **Parameters** - Create complete schema files
4. **Display** - Handle screen sizes and aspect ratios
5. **Testing** - Verify across browsers and screen sizes
6. **Documentation** - Update about files and create migration notes

**Estimated timeline per test**: 1-8 hours depending on complexity

**Current status**: ~15 tests migrated, ~130 tests remaining

**Next steps**: Focus on Tier 1 high-priority tests first
