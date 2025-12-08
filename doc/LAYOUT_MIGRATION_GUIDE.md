# Migration Guide: Adapting Existing PEBL Tasks to Layout & Response System

This guide demonstrates how to migrate existing PEBL battery tasks to use the unified Layout & Response System, using the `evenodd` task as a reference implementation.

## Table of Contents

1. [Overview](#overview)
2. [Benefits of Migration](#benefits-of-migration)
3. [Step-by-Step Migration Process](#step-by-step-migration-process)
4. [Before/After Comparison](#beforeafter-comparison)
5. [Common Migration Patterns](#common-migration-patterns)
6. [Testing Checklist](#testing-checklist)
7. [Troubleshooting](#troubleshooting)

## Overview

The Layout & Response System provides:
- **Platform-aware responses**: Automatic adaptation between desktop (shift keys), web (Z/slash), and touch (click targets)
- **Automatic screen scaling**: Layout zones that adapt to any screen size
- **Consistent UI**: Standardized header/footer/stimulus regions across all battery tasks
- **Zero code changes for response mode**: Users can switch via parameters, not code

**Key principle**: Preserve 95% of your existing code. Only replace the UI/response layer.

## Benefits of Migration

### Before Migration
- Hardcoded response keys (e.g., `<lshift>`, `<rshift>`)
- Manual label positioning (e.g., `gVideoWidth/2`, `gVideoHeight-200`)
- Platform-specific issues (shift keys trigger Windows Sticky Keys, don't work on web)
- No adaptation for tablets/touch screens

### After Migration
- Semantic responses (`"left"`, `"right"`)
- Automatic layout scaling
- Platform-aware response modes
- Touch-friendly interfaces available
- Configurable via parameters

## Step-by-Step Migration Process

### Step 1: Add responsemode Parameter

**File**: `params/taskname.pbl.schema.json`

Add a new parameter to your schema:

```json
{
  "name": "responsemode",
  "type": "string",
  "default": "auto",
  "options": ["auto", "keyboardShift", "keyboardSafe", "mousetarget", "mousebutton"],
  "label": "Response Mode",
  "description": "Response method: auto (platform-aware), keyboardShift (L/R shift), keyboardSafe (Z/slash for web), mousetarget (click targets), mousebutton (L/R mouse buttons)"
}
```

**File**: Main `.pbl` script (in `Start()` function)

Add to parameter pairs:

```pebl
## BEFORE
parPairs <- [["reps", 10],
             ["isi", 500]]

## AFTER
parPairs <- [["reps", 10],
             ["isi", 500],
             ["responsemode", "auto"]]  ## NEW
```

### Step 2: Create Layout in Start()

**Location**: In `Start()` function, after creating window and loading parameters, but before first Draw()

**BEFORE:**
```pebl
gWin <- MakeWindow("black")
gParams <- CreateParameters(parpairs, gParamFile)
GetStrings(gLanguage)

## Get subject number...
## Create data file...

Draw()
MessageBox(gStrings.inst1, gWin)
```

**AFTER:**
```pebl
gWin <- MakeWindow("black")
gParams <- CreateParameters(parpairs, gParamFile)
GetStrings(gLanguage)

## Get subject number...
## Create data file...

## NEW: Create layout with response system
gLayout <- CreateLayout("taskname", gWin, gParams)

## NEW: Configure layout zones
gLayout.header.text <- gStrings.header
gLayout.subheader.visible <- 0  ## Hide if not needed
## Footer is set automatically by layout system

Draw()
MessageBox(gStrings.inst1, gWin)
```

**Key points:**
- Call `CreateLayout()` AFTER parameters are loaded (it reads `gParams.responsemode`)
- Call BEFORE first `Draw()`
- Set header text from translations
- Hide subheader if your task doesn't use it
- Footer is automatically generated based on response mode

### Step 3: Update MessageBox to Fit Stimulus Region

**IMPORTANT**: When using `MessageBox()` or `EasyTextBox()` for instructions/breaks, these need to fit within the stimulus region to avoid overlapping with the layout's header and footer.

**Option 1: Use AdaptiveTextBox (Recommended)**

`AdaptiveTextBox()` automatically adapts to fit content within the stimulus region, preventing overflow and overlap with layout zones. It's especially useful for long instruction text that might not fit on smaller screens.

```pebl
AdaptiveTextBox(text, x, y, window, fontsize, width, height, adaptive, maxlines)
```

**Adaptation strategies:**
- `"scalefont"` - Reduces font size to fit text (preserves box dimensions)
- `"scalebox"` - Expands box maintaining aspect ratio, then scales down using zoom (preserves font size)
- `0` - No adaptation (creates standard textbox)

**BEFORE:**
```pebl
define MessageKeyBox(message)
{
  tb <- EasyTextBox(message, 100, 100, gWin, 28, gVideoWidth-200, gVideoHeight-200)
  Draw()
  WaitForAnyKeyPress()
  RemoveObject(tb, gWin)
}
```

**AFTER:**
```pebl
define MessageKeyBox(message)
{
  ## Use AdaptiveTextBox to fit within stimulus region
  ## Automatically reduces font or scales box if text doesn't fit
  tb <- AdaptiveTextBox(message,
                        gLayout.stimulusRegion.x + 20,
                        gLayout.stimulusRegion.y + 20,
                        gWin, 56,
                        gLayout.stimulusRegion.width - 40,
                        gLayout.stimulusRegion.height - 40,
                        "scalefont")  ## or "scalebox" to preserve font size
  Draw()
  WaitForAnyKeyPress()
  RemoveObject(tb, gWin)
}
```

**Benefits:**
- Automatically handles text overflow
- Adapts to different screen sizes
- Prevents overlap with header/footer
- No need to manually calculate gutters
- Choice of adaptation strategy (scale font vs scale box)

**Option 2: Use MessageBox with custom gutters**

`MessageBox()` has gutter parameters that can be overridden:

```pebl
MessageBox(text, win, fontsize:20, xleftgutter:100, xrightgutter:100,
           bottomgutter:200, useBG:1, ack:"<OK>")
```

**BEFORE:**
```pebl
MessageBox(gStrings.interblock, gWin)  ## Uses default gutters
```

**AFTER:**
```pebl
## Calculate gutters to fit stimulus region
leftgutter <- gLayout.stimulusRegion.x + 20
rightgutter <- gVideoWidth - (gLayout.stimulusRegion.x + gLayout.stimulusRegion.width) + 20
bottomgutter <- gVideoHeight - (gLayout.stimulusRegion.y + gLayout.stimulusRegion.height) + 20

MessageBox(gStrings.interblock, gWin, 20, leftgutter, rightgutter, bottomgutter)
```

**Option 3: Create custom MessageKeyBox with EasyTextBox**

For tasks that use a custom message function without adaptive features:

**BEFORE:**
```pebl
define MessageKeyBox(message)
{
  tb <- EasyTextBox(message, 100, 100, gWin, 28, gVideoWidth-200, gVideoHeight-200)
  Draw()
  WaitForAnyKeyPress()
  RemoveObject(tb, gWin)
}
```

**AFTER:**
```pebl
define MessageKeyBox(message)
{
  ## Use layout stimulus region to avoid overlap with header/footer
  ## Textbox positioned by upper-left corner
  tb <- EasyTextBox(message,
                    gLayout.stimulusRegion.x + 20,
                    gLayout.stimulusRegion.y + 20,
                    gWin, 28,
                    gLayout.stimulusRegion.width - 40,
                    gLayout.stimulusRegion.height - 40)
  Draw()
  WaitForAnyKeyPress()
  RemoveObject(tb, gWin)
}
```

**Note**: This option may still overflow if text is too long. Use AdaptiveTextBox (Option 1) if overflow is a concern.

**Option 4: Temporarily hide header/footer**

If you prefer to keep using default MessageBox parameters:

```pebl
## Hide layout zones during MessageBox
gLayout.header.visible <- 0
gLayout.footer.visible <- 0
Draw()
MessageBox(message, gWin)
gLayout.header.visible <- 1
gLayout.footer.visible <- 1
Draw()
```

### Step 4: Replace Manual Labels with Layout Zones

**BEFORE (typical Trial() function):**
```pebl
define Trial(stimulus)
{
  ## Create labels manually
  header <- EasyLabel(gStrings.header, gVideoWidth/2, 100, gWin, 40)
  stim <- EasyLabel(stimulus, gVideoWidth/2, gVideoHeight/2, gWin, 80)
  footer <- EasyLabel(gStrings.footer, gVideoWidth/2, gVideoHeight-200, gWin, 30)

  Draw()
  ## ... response collection ...

  ## Clean up
  Hide(header)
  Hide(stim)
  Hide(footer)
}
```

**AFTER:**
```pebl
define Trial(stimulus)
{
  ## Header and footer are already set in layout (in Start())
  ## Just create stimulus in center of stimulus region
  stim <- EasyLabel(stimulus, gLayout.centerX, gLayout.centerY, gWin, 80)

  Draw()
  ## ... response collection ...

  ## Only clean up stimulus
  Hide(stim)
}
```

**Key changes:**
- Remove header/footer label creation (set once in `Start()`)
- Use `gLayout.centerX` and `gLayout.centerY` instead of `gVideoWidth/2, gVideoHeight/2`
- Alternatively: `gLayout.stimulusRegion.centerX/Y` (same values, more verbose)
- Only hide/remove the stimulus object

**Convenience note:** `gLayout.centerX` and `gLayout.centerY` are shorthand for `gLayout.stimulusRegion.centerX/Y`. Both work identically - use whichever you prefer. The top-level properties are provided as a convenient replacement for the common pattern of `gVideoWidth/2, gVideoHeight/2`.

**Available Layout Properties:**

The layout object provides these properties for positioning elements:

**Stimulus region** (most commonly used):
- `gLayout.stimulusRegion.x` - Left edge of stimulus region
- `gLayout.stimulusRegion.y` - Top edge of stimulus region
- `gLayout.stimulusRegion.width` - Width of stimulus region
- `gLayout.stimulusRegion.height` - Height of stimulus region
- `gLayout.stimulusRegion.centerX` - Horizontal center of stimulus region
- `gLayout.stimulusRegion.centerY` - Vertical center of stimulus region

**Convenience shortcuts** (same as stimulus region centers):
- `gLayout.centerX` - Same as `gLayout.stimulusRegion.centerX`
- `gLayout.centerY` - Same as `gLayout.stimulusRegion.centerY`

**UI elements** (header, footer, etc.):
- `gLayout.header` - Header label object (`.text`, `.visible` properties)
- `gLayout.subheader` - Subheader label object (`.text`, `.visible` properties)
- `gLayout.footer` - Footer label object (`.text`, `.visible` properties)

**Zone boundaries** (advanced usage - for positioning feedback, status messages, etc.):
- `gLayout.zones.header.y` - Y position of header zone
- `gLayout.zones.header.height` - Height of header zone
- `gLayout.zones.subheader.y` - Y position of subheader zone
- `gLayout.zones.subheader.height` - Height of subheader zone
- `gLayout.zones.response.y` - Y position of response zone (between stimulus and footer)
- `gLayout.zones.response.height` - Height of response zone
- `gLayout.zones.footer.y` - Y position of footer zone
- `gLayout.zones.footer.height` - Height of footer zone

**Example usage:**
```pebl
## Position stimulus in center of stimulus region
stim <- EasyLabel("TEXT", gLayout.centerX, gLayout.centerY, gWin, 80)

## Position feedback in response zone
feedbackY <- gLayout.zones.response.y + gLayout.zones.response.height/2
feedback <- MakeLabel("Correct!", font)
Move(feedback, gVideoWidth/2, feedbackY)

## Position textbox within stimulus region
tb <- AdaptiveTextBox(message,
                      gLayout.stimulusRegion.x + 20,
                      gLayout.stimulusRegion.y + 20,
                      gWin, 56,
                      gLayout.stimulusRegion.width - 40,
                      gLayout.stimulusRegion.height - 40,
                      "scalefont")
```

### Step 4: Replace Response Collection

**BEFORE:**
```pebl
define Trial(stimulus)
{
  ## ... create stimuli ...
  Draw()

  time1 <- GetTime()
  resp <- WaitForListKeyPress(["<lshift>", "<rshift>"])
  time2 <- GetTime()
  rt <- time2 - time1

  ## Check correctness with raw key names
  if(correctResponse == "left")
  {
    correct <- (resp == "<lshift>")
  } else {
    correct <- (resp == "<rshift>")
  }

  return [stimulus, resp, correct, rt]
}
```

**AFTER:**
```pebl
define Trial(stimulus)
{
  ## ... create stimuli ...
  Draw()

  time1 <- GetTime()

  ## Returns semantic response: "left", "right", or "<timeout>"
  resp <- WaitForLayoutResponse(gLayout, 5000)

  time2 <- GetTime()
  rt <- time2 - time1

  ## Check correctness with semantic names
  correct <- (resp == correctResponse)

  return [stimulus, resp, correct, rt]
}
```

**Key changes:**
- `WaitForListKeyPress(["<lshift>", "<rshift>"])` → `WaitForLayoutResponse(gLayout, 5000)`
- Response is now semantic: `"left"`, `"right"`, or `"<timeout>"`
- Add timeout (5000ms recommended, adjust as needed)
- Correctness logic uses semantic names, not raw keys

### Step 5: Update Translation Strings

**IMPORTANT**: You must update ALL available language translations, not just English. Tasks typically have translations in multiple languages (en, es, lt, etc.).

**Files**: `translations/taskname.pbl-en.json`, `translations/taskname.pbl-es.json`, `translations/taskname.pbl-lt.json`, etc.

**Check which translations exist:**
```bash
ls battery/taskname/translations/
```

**BEFORE (English):**
```json
{
  "HEADER": "Task Name - Trial 1",
  "FOOTER": "<left shift> LEFT                    RIGHT <right shift>",
  "INSTRUCTIONS": "Press LEFT SHIFT for left, RIGHT SHIFT for right.",
  "FOOTERL": "left-shift",
  "FOOTERR": "right-shift"
}
```

**AFTER (English):**
```json
{
  "HEADER": "Task Name - Trial 1",
  "FOOTER": "LEFT                                    RIGHT",
  "INSTRUCTIONS": "Press the LEFT response key for left, the RIGHT response key for right.",
  "FOOTERL": "LEFT",
  "FOOTERR": "RIGHT"
}
```

**AFTER (Spanish example):**
```json
{
  "HEADER": "Tarea - Prueba 1",
  "FOOTER": "IZQUIERDA                                    DERECHA",
  "INSTRUCTIONS": "Presiona el botón de respuesta IZQUIERDA para izquierda, el botón de respuesta DERECHA para derecha.",
  "FOOTERL": "IZQUIERDA",
  "FOOTERR": "DERECHA"
}
```

**AFTER (Lithuanian example):**
```json
{
  "HEADER": "Užduotis - Bandymas 1",
  "FOOTER": "KAIRĖ                                    DEŠINĖ",
  "INSTRUCTIONS": "Spauskite KAIRĮJĮ atsakymo mygtuką kairėje, DEŠINĮJĮ atsakymo mygtuką dešinėje.",
  "FOOTERL": "KAIRĖ",
  "FOOTERR": "DEŠINĖ"
}
```

**Key changes for ALL languages:**
- Update `FOOTER` to show semantic mapping (e.g., "ODD ... EVEN", "LEFT ... RIGHT") WITHOUT hardcoded keys
- The layout system adds response key labels automatically below your footer text
- Update `INSTRUCTIONS` to use generic "response key" language instead of hardcoded "shift key", "left shift", "shift izquierdo", etc.
- Add or update `HEADER` string if not already present
- Update `FOOTERL` and `FOOTERR` to show semantic labels (used in instruction screens)
- Translate `INTERBLOCK` and `DEBRIEF` messages if they weren't already translated

**Example for evenodd task (English):**
```json
{
  "HEADER": "Decide whether odd or even",
  "FOOTER": "ODD                                      EVEN",
  "INSTRUCTIONS": "When a number is ODD, press the LEFT response key...",
  "FOOTERL": "ODD",
  "FOOTERR": "EVEN"
}
```

The footer shows the semantic mapping (ODD/EVEN), and the layout system automatically displays the actual response keys (shift/Z/click target) below it based on the current response mode.

**Translation workflow:**
1. Start with English (`taskname.pbl-en.json`)
2. Identify what needs to change (remove shift key references, add HEADER/FOOTER if missing)
3. Update each available language translation following the same pattern
4. Ensure semantic meanings are preserved (e.g., "LEFT"/"RIGHT", "IZQUIERDA"/"DERECHA", "KAIRĖ"/"DEŠINĖ")

### Step 6: Update Data Output (If Needed)

If your data file records the response:

**BEFORE:**
```
subnum,trial,stimulus,response,correct,rt
001,1,stimulus1,<lshift>,1,523
001,2,stimulus2,<rshift>,1,456
```

**AFTER:**
```
subnum,trial,stimulus,response,correct,rt
001,1,stimulus1,left,1,523
001,2,stimulus2,right,1,456
```

**Note**: Response values change from raw keys (`<lshift>`) to semantic names (`left`). This is EXPECTED and CORRECT. Your analysis scripts may need minor updates to handle semantic names.

## Common Issues Checklist

These issues were discovered during migrations and should be addressed proactively in every task migration:

### Issue 1: Layout Creation Timing

**Problem**: Layout footer appears before GetSubNum() dialog if layout is created too early.

**Solution**: Always create layout AFTER GetSubNum(), not before.

**Code pattern:**
```pebl
gWin <- MakeWindow("black")
Initialize()  ## This calls GetStrings()

## Get subject number FIRST
if(gSubNum+""=="0")
{
  gSubNum <- GetSubNum(gWin)
}

## THEN create layout
gLayout <- CreateLayout("taskname", gWin, gParams)
gLayout.header.text <- gStrings.header
gLayout.subheader.visible <- 1
gLayout.footer.text <- gStrings.footer
```

### Issue 2: Subheader Trial Counter

**Problem**: Tasks don't show trial progress to participants.

**Solution**: Enable subheader and update it in Trial() function.

**Code pattern:**
```pebl
## In Start() after CreateLayout():
gLayout.subheader.visible <- 1  ## Enable subheader

## In Trial() function:
define Trial(stim, practice, totalTrials)
{
  gLayout.subheader.text <- "Trial " + gTrial + " of " + totalTrials
  ## ... rest of trial code
}
```

**For tasks with practice and test phases:**
```pebl
## Calculate separate trial counts
gPracticeTrials <- Length(practrials)
gTestTrials <- Length(basetrials)

## Practice phase
gTrial <- 1
loop(trial, practrials)
{
  Trial(trial, 1, gPracticeTrials)  ## Pass practice trial count
  gTrial <- gTrial + 1
}

## Reset counter for test phase
gTrial <- 1
loop(trial, basetrials)
{
  Trial(trial, 0, gTestTrials)  ## Pass test trial count
  gTrial <- gTrial + 1
}
```

### Issue 3: Complete Translation Coverage

**Problem**: Tasks only have English translations, missing standard languages (es, lt, pt).

**Solution**: Create translations for all standard PEBL languages before finalizing migration.

**Standard languages:**
- `en` - English (always required)
- `es` - Spanish
- `lt` - Lithuanian
- `pt` - Portuguese

**Workflow:**
```bash
## Check existing translations
ls battery/taskname/translations/

## Create missing translations
## Copy en.json as template, translate all strings
cp taskname.pbl-en.json taskname.pbl-es.json
## Edit taskname.pbl-es.json with Spanish translations
```

**Key translation strings for layout migration:**
- `PRACTICE_HEADER` - "PRACTICE PHASE" / "FASE DE PRÁCTICA" / etc.
- `HEADER` - "TESTING PHASE" / "FASE DE PRUEBA" / etc.
- `FOOTER` - Semantic labels without key names
- `FOOTERL` / `FOOTERR` - Semantic response labels
- `INSTRUCTIONS` - Remove "shift key" references, use "response key"
- `INTERBLOCK` - Between-block messages
- `DEBRIEF` - End-of-task message

### Issue 4: MessageBox Overlap with Layout

**Problem**: MessageBox or EasyTextBox overlaps with layout header/footer.

**Solution**: Use AdaptiveTextBox to constrain message boxes to stimulus region and handle text overflow.

**Code pattern (Recommended):**
```pebl
define MessageKeyBox(message)
{
  ## Use AdaptiveTextBox to fit within stimulus region
  ## Automatically adapts if text doesn't fit
  tb <- AdaptiveTextBox(message,
                        gLayout.stimulusRegion.x + 20,
                        gLayout.stimulusRegion.y + 20,
                        gWin, 56,
                        gLayout.stimulusRegion.width - 40,
                        gLayout.stimulusRegion.height - 40,
                        "scalefont")  ## or "scalebox" to preserve font size
  Draw()
  WaitForAnyKeyPress()
  RemoveObject(tb, gWin)
}
```

**Alternative (without adaptation):**
```pebl
define MessageKeyBox(message)
{
  ## Use layout stimulus region to avoid overlap with header/footer
  ## Textbox positioned by upper-left corner
  tb <- EasyTextBox(message,
                    gLayout.stimulusRegion.x + 20,
                    gLayout.stimulusRegion.y + 20,
                    gWin, 56,
                    gLayout.stimulusRegion.width - 40,
                    gLayout.stimulusRegion.height - 40)
  Draw()
  WaitForAnyKeyPress()
  RemoveObject(tb, gWin)
}
```

**Note**: Use 20px margins to prevent text from touching edges. AdaptiveTextBox is preferred because it handles text overflow automatically.

### Issue 5: Manual Footer Cleanup in Instructions

**Problem**: Instruction functions create manual footer labels that aren't removed, causing double footers.

**Solution**: Remove all manual footer labels created during instructions.

**Check these functions:**
- `DoInstructions()`
- `ShowInstructions()`
- Any function that displays instructions before trials

**Code pattern:**
```pebl
define DoInstructions()
{
  tb <- EasyTextbox(gInst1text, 100, 50, gWin, 32, gVideoWidth-200, 200)

  ## Example stimuli for instructions
  stim1 <- Circle(x1, y1, 75, MakeColor("red"), 1)
  stim2 <- Square(x2, y2, 150, MakeColor("red"), 1)

  ## REMOVE manual footer - layout handles this
  ## footer <- EasyLabel(gFooterL + " ... " + gFooterR, ...)  ## DELETE THIS

  AddObject(stim1, gWin)
  AddObject(stim2, gWin)
  Draw()
  WaitForAnyKeyPress()

  ## Clean up ALL created objects
  RemoveObject(stim1, gWin)
  RemoveObject(stim2, gWin)
  ## RemoveObject(footer, gWin)  ## DELETE THIS LINE TOO
  RemoveObject(tb, gWin)
}
```

### Issue 6: Practice vs Test Phase Headers

**Problem**: Tasks with practice phases don't distinguish between practice and test.

**Solution**: Add PRACTICE_HEADER translation and switch headers between phases.

**Translation additions:**
```json
{
  "PRACTICE_HEADER": "PRACTICE PHASE",
  "HEADER": "TESTING PHASE"
}
```

**Code pattern:**
```pebl
## Before practice trials
gLayout.header.text <- gStrings.practice_header
loop(trial, practrials)
{
  ## ... practice trials
}

MessageKeyBox(gInterblock)

## Switch to test phase and restart trial numbering
gLayout.header.text <- gStrings.header
gTrial <- 1

loop(trial, basetrials)
{
  ## ... test trials
}
```

### Issue 7: Instruction Font Sizes Too Small

**Problem**: Default font sizes in instruction textboxes are too small relative to box size.

**Solution**: Double the font sizes for instruction textboxes.

**Font size guidelines:**
- **DoInstructions() textbox**: 32 (was 16)
- **MessageKeyBox() textbox**: 56 (was 28)
- **MessageBox() calls**: 40 (was 20) - if using standard MessageBox

**Code changes:**
```pebl
## In DoInstructions()
tb <- EasyTextbox(gInst1text, 100, 50, gWin, 32, gVideoWidth-200, 200)
##                                         ^^ doubled from 16

## In MessageKeyBox()
tb <- EasyTextBox(message,
                  gLayout.stimulusRegion.x + 20,
                  gLayout.stimulusRegion.y + 20,
                  gWin, 56,  ## doubled from 28
                  gLayout.stimulusRegion.width - 40,
                  gLayout.stimulusRegion.height - 40)
```

### Migration Checklist Summary

Use this checklist for every task migration:

- [ ] **Layout timing**: CreateLayout() called AFTER GetSubNum()
- [ ] **Subheader enabled**: `gLayout.subheader.visible <- 1`
- [ ] **Trial counter**: Update subheader in Trial() with current trial and total
- [ ] **Practice/test phases**: Separate headers and restart trial numbering if applicable
- [ ] **Complete translations**: All standard languages (en, es, lt, pt) updated
- [ ] **Translation keys**: PRACTICE_HEADER, HEADER, FOOTER, FOOTERL, FOOTERR, INSTRUCTIONS
- [ ] **MessageBox constrained**: MessageKeyBox uses stimulus region coordinates
- [ ] **Font sizes doubled**: Instructions 32, MessageBox 56
- [ ] **Manual footers removed**: No footer labels in DoInstructions()
- [ ] **Footer cleanup**: RemoveObject() calls removed for manual footers
- [ ] **Files copied**: All changes copied from battery/ to upload-battery/

## Before/After Comparison

### Complete evenodd Example

#### BEFORE (Original)

**evenodd.pbl:**
```pebl
define Start(p)
{
  gWin <- MakeWindow("black")
  parPairs <- [["reps",10], ["isi",500], ["minval",100], ["maxval",999]]
  gParams <- CreateParameters(parpairs, gParamFile)
  GetStrings(gLanguage)

  if(gSubNum+""=="0")
    { gSubNum <- GetSubNum(gWin) }

  header <- "subnum,trial,starttime,number,isodd,resp,corr,rt"
  gFileOut <- GetNewDataFile(gSubNum, gWin, "evenodd", "csv", header)

  Draw()
  MessageBox(gStrings.inst1, gWin)

  ## ... trial generation ...
  loop(i, trials) {
    out <- Trial(i)
    FilePrintList(gFileout, Merge([gSubNum, trial, starttime], out), ",")
  }
}

define Trial(number)
{
  header <- EasyLabel(gStrings.header, gVideoWidth/2, 100, gWin, 40)
  stim <- EasyLabel(number+"", gVideoWidth/2, gVideoHeight/2, gWin, 80)
  footer <- EasyLabel(gStrings.footer, gVideoWidth/2, gVideoHeight-200, gWin, 30)

  Draw()
  time1 <- GetTime()
  resp <- WaitForListKeyPress(["<lshift>","<rshift>"])
  time2 <- GetTime()
  rt <- (time2-time1)
  odd <- IsOdd(number)
  corr <- (odd and resp=="<lshift>") or ((not odd) and resp=="<rshift>")
  Hide(stim)
  Draw()
  return [number, odd, resp, corr, rt]
}
```

**translations/evenodd.pbl-en.json:**
```json
{
  "INST1": "...When a number is odd, press the LEFT shift key. When a number is even, press the RIGHT shift key...",
  "HEADER": "Decide whether odd or even",
  "FOOTER": "<left shift> ODD                    EVEN <right shift>"
}
```

#### AFTER (Migrated)

**evenodd.pbl:**
```pebl
define Start(p)
{
  gWin <- MakeWindow("black")
  parPairs <- [["reps",10], ["isi",500], ["minval",100], ["maxval",999],
               ["responsemode","auto"]]  ## ADDED
  gParams <- CreateParameters(parpairs, gParamFile)
  GetStrings(gLanguage)

  if(gSubNum+""=="0")
    { gSubNum <- GetSubNum(gWin) }

  header <- "subnum,trial,starttime,number,isodd,resp,corr,rt"
  gFileOut <- GetNewDataFile(gSubNum, gWin, "evenodd", "csv", header)

  ## ADDED: Create layout
  gLayout <- CreateLayout("evenodd", gWin, gParams)
  gLayout.header.text <- gStrings.header
  gLayout.subheader.visible <- 0
  gLayout.footer.text <- gStrings.footer

  Draw()
  MessageBox(gStrings.inst1, gWin)

  ## ... trial generation (UNCHANGED) ...
  loop(i, trials) {
    out <- Trial(i)
    FilePrintList(gFileout, Merge([gSubNum, trial, starttime], out), ",")
  }
}

define Trial(number)
{
  ## CHANGED: Use layout for positioning, remove header/footer
  stim <- EasyLabel(number+"", gLayout.centerX, gLayout.centerY, gWin, 80)

  Draw()
  time1 <- GetTime()

  ## CHANGED: Use layout response system
  resp <- WaitForLayoutResponse(gLayout, 5000)

  time2 <- GetTime()
  rt <- (time2-time1)
  odd <- IsOdd(number)

  ## CHANGED: Semantic response mapping
  corr <- (odd and resp=="left") or ((not odd) and resp=="right")

  Hide(stim)
  Draw()
  return [number, odd, resp, corr, rt]
}
```

**translations/evenodd.pbl-en.json:**
```json
{
  "INST1": "...When a number is ODD, press the LEFT response key. When a number is EVEN, press the RIGHT response key...",
  "HEADER": "Decide whether odd or even",
  "FOOTER": "ODD                                      EVEN"
}
```

**params/evenodd.pbl.schema.json:**
```json
{
  "parameters": [
    {
      "name": "responsemode",
      "type": "string",
      "default": "auto",
      "options": ["auto", "keyboardShift", "keyboardSafe", "mousetarget", "mousebutton"],
      "description": "Response method..."
    }
  ]
}
```

## Common Migration Patterns

### Pattern 1: Two-Alternative Forced Choice

**Task Type**: Odd/Even, Left/Right, Yes/No, etc.

**Key Mapping**: Left response = Option A, Right response = Option B

**Migration**:
- Replace `WaitForListKeyPress(["<lshift>", "<rshift>"])` with `WaitForLayoutResponse(gLayout, timeout)`
- Map correctness: `correct <- (resp == "left")` or `correct <- (resp == "right")`
- Response values in data: `"left"` or `"right"` instead of `"<lshift>"` or `"<rshift>"`

### Pattern 2: Go/No-Go Tasks

**Task Type**: Press for target, withhold for non-target

**Key Mapping**: Any response = "go", timeout = "no-go"

**Migration**:
```pebl
## BEFORE
resp <- WaitForKeyPress(1000)
go <- (resp != "<timeout>")

## AFTER
resp <- WaitForLayoutResponse(gLayout, 1000)
go <- (resp != "<timeout>")
```

**Note**: For go/no-go, you might want a single-button response mode. Consider adding a parameter for this.

### Pattern 3: Multiple Choice (>2 options)

**Task Type**: 3+ response options

**Current limitation**: Layout system currently optimized for 2-alternative responses.

**Recommended approach**: Keep manual response handling for now, OR contribute a multi-alternative layout mode.

### Pattern 4: Continuous Response

**Task Type**: Drag-and-drop, mouse tracking, analog input

**Migration**: Not applicable - layout system is for discrete responses. Keep existing implementation.

## Testing Checklist

After migrating your task, verify:

### Functional Tests

- [ ] Task runs without errors
- [ ] Stimuli display correctly in center of stimulus region
- [ ] Header and footer display correctly
- [ ] Response collection works (try both left and right responses)
- [ ] Timeout handling works (wait without responding)
- [ ] Data file format matches expectations
- [ ] Summary/feedback displays correctly

### Response Mode Tests

**CRITICAL**: All migrated tasks MUST be tested with ALL response modes, especially mousebutton and mousetarget. Mouse-based modes have different event handling than keyboard modes and require thorough testing.

Run your task with each response mode:

```bash
# Default (auto - should use keyboardShift on native)
bin/pebl2 battery/taskname/taskname.pbl -s 1

# Explicit keyboard modes
bin/pebl2 battery/taskname/taskname.pbl -s 2 --pfile params/keyboardShift.json
bin/pebl2 battery/taskname/taskname.pbl -s 3 --pfile params/keyboardSafe.json

# Mouse modes (REQUIRED TESTING)
bin/pebl2 battery/taskname/taskname.pbl -s 4 --pfile params/mousetarget.json
bin/pebl2 battery/taskname/taskname.pbl -s 5 --pfile params/mousebutton.json
```

**Create preset files** (`params/*.json`):

**params/mousetarget.json:**
```json
{
  "responsemode": "mousetarget"
}
```

**params/mousebutton.json:**
```json
{
  "responsemode": "mousebutton"
}
```

**params/keyboardShift.json:**
```json
{
  "responsemode": "keyboardShift"
}
```

**params/keyboardSafe.json:**
```json
{
  "responsemode": "keyboardSafe"
}
```

### Mouse/Touch Mode Testing Checklist

For **mousebutton** mode, verify:
- [ ] Left mouse click registers as "left" response
- [ ] Right mouse click registers as "right" response
- [ ] Clicks work on instruction screens
- [ ] Clicks work during trials
- [ ] Timeout still functions when no click occurs
- [ ] Response feedback displays correctly
- [ ] Footer shows "LEFT-CLICK" and "RIGHT-CLICK" labels

For **mousetarget** mode, verify:
- [ ] Click targets appear in response zone
- [ ] Clicking left target registers as "left" response
- [ ] Clicking right target registers as "right" response
- [ ] Targets are large enough (especially for touch screens)
- [ ] Targets don't overlap with stimuli
- [ ] Footer shows clickable target labels
- [ ] Targets respond to both mouse and touch input

### Single-Response Tasks (Go-NoGo, CPT, etc.)

For tasks with single-response modes (spacebar, leftclick, touchscreen):

**Test with spacebar mode:**
```bash
bin/pebl2 battery/taskname/taskname.pbl -s 6 --pfile params/spacebar.json
```

**params/spacebar.json:**
```json
{
  "responsemode": "spacebar"
}
```

**Test with leftclick mode:**
```bash
bin/pebl2 battery/taskname/taskname.pbl -s 7 --pfile params/leftclick.json
```

**params/leftclick.json:**
```json
{
  "responsemode": "leftclick"
}
```

**Test with touchscreen mode:**
```bash
bin/pebl2 battery/taskname/taskname.pbl -s 8 --pfile params/touchscreen.json
```

**params/touchscreen.json:**
```json
{
  "responsemode": "touchscreen"
}
```

Verify for single-response modes:
- [ ] Instruction screens advance on response
- [ ] Trials register responses correctly
- [ ] No-response trials (timeouts) handled properly
- [ ] Footer shows appropriate single-response label
- [ ] Response feedback (if any) displays correctly

### Screen Size Tests

- [ ] 800x600 (baseline)
- [ ] 1024x768 (common desktop)
- [ ] 1920x1080 (HD)
- [ ] 1280x800 (laptop)
- [ ] Fullscreen mode

Verify labels don't overlap and stimuli remain centered.

### Data Validation

Compare data files from before and after migration:

**Check**:
- Same number of columns
- Same trial count
- RT values similar (should be identical)
- Accuracy similar (should be identical)
- Only response column should differ (raw keys → semantic names)

### Translation Tests

If your task has translations:

```bash
bin/pebl2 battery/taskname/taskname.pbl -s 10 --language es
bin/pebl2 battery/taskname/taskname.pbl -s 11 --language fr
```

Verify footer labels update appropriately.

## Troubleshooting

### Error: "gLayout is not defined"

**Cause**: `CreateLayout()` not called or called after it's used

**Fix**: Ensure `CreateLayout()` is called in `Start()` AFTER parameters loaded, BEFORE first `Draw()`

### Error: "No translation file exists"

**Cause**: Translation file not found

**Fix**: Run from task directory: `cd battery/taskname && ../../bin/pebl2 taskname.pbl`

### Response always returns "<timeout>"

**Cause**: Wrong response keys being pressed, or timeout too short

**Fix**:
- Check which response mode is active (look at footer labels)
- Increase timeout: `WaitForLayoutResponse(gLayout, 10000)` (10 seconds)
- Print response mode: `Print("Response mode: " + gLayout.responseMode.type)`

### Footer shows wrong key labels

**Cause**: Response mode not set correctly

**Fix**: Check `gParams.responsemode` is being loaded from parameters

**Debug**:
```pebl
Print("Response mode parameter: " + gParams.responsemode)
Print("Layout response mode: " + gLayout.responseMode.type)
```

### Stimuli not centered

**Cause**: Using old `gVideoWidth/2` instead of layout regions

**Fix**: Replace with `gLayout.stimulusRegion.centerX` and `.centerY`

### Labels overlap

**Cause**: Screen too small for layout zones

**Fix**: Layout system handles this automatically. If still an issue, check font sizes - you may want to use layout's scaled font sizes instead of hardcoded values.

### Data format changed

**Expected**: Response column now shows `"left"`/`"right"` instead of `"<lshift>"`/`"<rshift>"`

**This is correct**: Semantic names are platform-independent. Update analysis scripts:

**R example**:
```r
# OLD
data$correct <- data$response == "<lshift>"

# NEW
data$correct <- data$response == "left"
```

**Python example**:
```python
# OLD
df['correct'] = df['response'] == '<lshift>'

# NEW
df['correct'] = df['response'] == 'left'
```

## Summary

Migration to the Layout & Response System involves:

1. **Add responsemode parameter** to schema and parameter pairs
2. **Call CreateLayout()** in `Start()` after parameters, before Draw()
3. **Remove manual header/footer labels** from Trial() function
4. **Use layout regions** for stimulus positioning
5. **Replace WaitForListKeyPress()** with `WaitForLayoutResponse()`
6. **Update correctness logic** to use semantic names (`"left"`, `"right"`)
7. **Update translations** to use generic "response key" language
8. **Test thoroughly** with multiple response modes

**What stays the same**:
- Trial generation logic
- Data file structure (except response column values)
- Summary statistics
- Overall task flow

**What improves**:
- Platform compatibility (native, web, tablet)
- User configurability (no code changes needed)
- Consistent UI across battery tasks
- Automatic screen scaling

For questions or issues, consult the PEBL manual Chapter 6 (Layout & Response System) or ask on the PEBL mailing list.
