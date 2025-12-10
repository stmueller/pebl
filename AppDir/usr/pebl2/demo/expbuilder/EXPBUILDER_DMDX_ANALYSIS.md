# PEBL Experiment Builder and DMDX Integration Analysis

**Date:** 2025-11-18
**Purpose:** Analyze demo/expbuilder code for generic experiment builder capabilities and DMDX integration

---

## Executive Summary

**Current State:**
- ✅ **Working GUI experiment builder** (`expbuilder.pbl`) for creating experiments via point-and-click
- ✅ **Functional DMDX JSON parser** (`dmdx.pbl`) that can run experiments from visualdmdx.com format
- ⚠️ **Partial implementation** - Both tools are functional but incomplete

**Key Capabilities:**
1. **Experiment Builder** - Create experiments with instruction screens, test blocks, and resources
2. **DMDX JSON Runner** - Parse and execute DMDX experiments from JSON format
3. **Compilation** - Generate standalone PEBL scripts from builder definitions
4. **JSON Export** - Save experiment designs to JSON

**Gaps:**
- Native DMDX .rtf format parsing (only JSON from visualdmdx.com)
- Limited stage types in builder (no loops, conditionals, advanced features)
- No CSV-based experiment runner (despite dmdx.csv file existing)
- Missing data collection/output in DMDX runner
- Builder UI needs polish and additional features

**Recommendation:**
- **Short-term:** Document and polish existing tools for PEBL 2.2 release
- **Long-term:** Expand to full generic experiment framework with CSV/JSON backends

---

## Architecture Overview

### Component 1: PEBL Experiment Builder (`expbuilder.pbl`)

**Purpose:** GUI tool for building psychological experiments via point-and-click interface

**Main Components:**

```
┌─────────────────────────────────────────────┐
│          Experiment Builder GUI             │
├─────────────────────────────────────────────┤
│  Menu Bar: File | Edit | Options | Help    │
├─────────────────────────────────────────────┤
│  Stage List (ScrollBox)                     │
│  ┌─────────────────────────────────────┐   │
│  │ □ INST13: Instruction Screen       │   │
│  │ □ INST24: Instruction Screen       │   │
│  │ □ TEST23: Test Block               │   │
│  │ □ RES7:   Resources                │   │
│  └─────────────────────────────────────┘   │
├─────────────────────────────────────────────┤
│  [Add] [Delete] [Edit] [Compile & Run]     │
│  [Run Stage] [Save JSON] [Load JSON]       │
└─────────────────────────────────────────────┘
```

**Architecture:**

```pebl
define Start(p)
{
  gVideoWidth <- 1200
  gVideoHeight <- 800
  gWin <- MakeWindow("grey90")

  ## Global experiment state
  gExperiment <- []  ## List of stage definitions

  ## Main UI
  sb <- MakeScrollBox(gExperiment, "Experiment Stages", ...)
  menubar <- CreateMenuBar()

  ## Control buttons
  compile <- MakeButton("Compile & run", ...)
  compilestage <- MakeButton("Run stage", ...)
  addstage <- MakeButton("Add stage", ...)

  ## Main event loop
  gQuit <- 0
  while(not gQuit)
  {
    wait(20)
    eventloop <- WaitForIt()  ## Event handler
  }
}
```

**Stage Types Supported:**

| Stage Type | ID Prefix | Purpose | Implementation Status |
|------------|-----------|---------|----------------------|
| Text Instructions | INST | Display text instructions | ✅ Complete |
| Image Instructions | IMGINST | Display image-based instructions | ✅ Complete |
| Resources | RES | Load images/sounds/fonts | ✅ Complete |
| Test Block | TEST | Present stimuli and collect responses | ⚠️ Partial |

**Key Functions:**

```pebl
## Stage Management
define AddStage(type)           ## Add new stage to experiment
define EditStage(stageobj)      ## Edit existing stage
define DeleteStage(stageobj)    ## Remove stage from experiment

## Compilation
define CompileExperiment(exp)   ## Generate .pbl script
define RunExperiment(exp)       ## Execute compiled script

## Persistence
define SaveToJSON(exp, filename)
define LoadFromJSON(filename)
```

**Generated Code Example:**

From `demoexp.pbl` (auto-generated output):

```pebl
## Auto-generated PEBL experiment.
define Start(p)
{
  gVideoWidth<-800
  gVideoHeight<-600
  gWin <- MakeWindow("black")
  gSleepEasy <- 1

  FINST13(gWin)  ## Call instruction screen 1
  FINST24(gWin)  ## Call instruction screen 2
}

define FINST13(win)
{
  ## Generated instruction screen code
  box <- EasyTextBox("INSTRUCTIONS...", ...)
  ok <- EasyLabel("OK", ...)
  WaitForClickOnTarget([back],[1])
  ## Cleanup code
}
```

**Current Limitations:**

1. **Limited stage types** - No loops, conditionals, counterbalancing
2. **No test block editor** - Placeholder only, not fully implemented
3. **No data collection** - Compiled scripts don't save data files
4. **Simple compilation** - Linear sequence only, no flow control
5. **UI polish** - Functional but not production-ready

---

### Component 2: DMDX JSON Runner (`dmdx.pbl`)

**Purpose:** Parse and execute experiments from DMDX JSON format (visualdmdx.com)

**DMDX Background:**

DMDX (Display Multi Dimensional eXperiments) is a Windows-based psychology experiment software from University of Arizona. Native DMDX uses:
- **Input:** `.rtf` (Rich Text Format) item scripts
- **Output:** `.azk`, `.dtp` data files
- **Images:** `.img` (DMTG format) or `.bmp`

**PEBL Implementation:**

The current implementation targets **JSON export from visualdmdx.com**, not native DMDX .rtf files.

**JSON Format Structure:**

```json
{
  "id": "6w6kvo",
  "version": 0,
  "parameters": {
    "timeout": "1000",
    "keyboard": "true",
    "videomode": {"width": "640", "height": "480"},
    "color": "#ffffff",
    "bgcolor": "#000000",
    "fontsize": "12"
  },
  "items": [
    {
      "type": "instructions",
      "content": "Experiment instructions 1",
      "response": "no-response"
    },
    {
      "type": "item",
      "response": "positive",
      "clockon": 1,
      "stimuli": [
        {
          "format": "text",
          "val": "text",
          "duration": "30",
          "horizontal": "center",
          "vertical": "middle"
        }
      ]
    },
    {
      "type": "loop",
      "description": "4",
      "variables": [
        {"name": "1", "values": [...]}
      ]
    }
  ]
}
```

**Supported Item Types:**

| Type | Purpose | Implementation | Data Collection |
|------|---------|----------------|-----------------|
| `instructions` | Display instruction screen | ✅ Complete | N/A |
| `item` | Test trial with stimuli | ✅ Complete | ❌ Missing |
| `loop` | Repeat items with variables | ✅ Complete | ❌ Missing |

**Supported Stimulus Formats:**

| Format | Type | Implementation |
|--------|------|----------------|
| `text` | Text string | ✅ Complete |
| `bmp` | Bitmap image | ✅ Complete |
| `blank` | Blank screen (fixation) | ✅ Complete |

**Key Functions:**

```pebl
define Start(p)
{
  expText <- FileReadText("dmdx.json")
  expObj <- ParseJSON(exptext)
  RunEXP(expObj)
}

define RunEXP(exp)
{
  ## Parse global parameters
  params <- exp.parameters
  gVideoWidth <- params.videomode.width
  gVideoHeight <- params.videomode.height

  ## Initialize window
  gWin <- MakeWindow(HexToColor(params.bgcolor))

  ## Run each item
  loop(item, exp.items)
  {
    if(item.type == "instructions")
    {
      RunInstructions(item, params)
    } elseif(item.type == "item")
    {
      RunItem(item, params)
    } elseif(item.type == "loop")
    {
      RunLoop(item, params)
    }
  }
}

define RunItem(item, params)
{
  ## Handle test trial
  loop(stim, item.stimuli)
  {
    if(stim.format == "text")
    {
      ## Display text stimulus
      label <- EasyLabel(stim.val, ...)
      AddObject(label, gWin)
    } elseif(stim.format == "bmp")
    {
      ## Display image stimulus
      img <- MakeImage(stim.val)
      AddObject(img, gWin)
    } elseif(stim.format == "blank")
    {
      ## Blank screen
      Draw()
    }

    Draw()
    Wait(stim.duration * params.frameduration)

    if(stim.clear)
    {
      ## Clear screen after stimulus
    }
  }

  ## Collect response (keyboard/mouse)
  resp <- WaitForListKeyPress(["+", "-"])  ## Example

  ## TODO: Save data to file
}

define HexToColor(hexstring)
{
  ## Convert "#ffffff" to RGB color
  ## Implementation uses string parsing and ToNumber()
}
```

**Current Capabilities:**

✅ **Working:**
- Parse DMDX JSON from visualdmdx.com
- Display instruction screens
- Present text/image/blank stimuli
- Handle timing and positioning
- Support loops and variables
- Hex color conversion
- Keyboard/mouse response collection

❌ **Missing:**
- **Data file output** - No data collection implemented
- **Subject number handling** - No gSubNum integration
- **Parameter files** - No .schema support
- **Translation support** - No multi-language
- **DMDX .rtf parsing** - Only JSON supported
- **Advanced DMDX features** - Scrambling, counterbalancing incomplete
- **Error handling** - Minimal validation

---

### Component 3: CSV-Based Experiments

**File Found:** `dmdx.csv` (2227 bytes)

**Format Analysis:**

```csv
global,id,wpgqjr
global,version,0
parameter,timeout,1000
parameter,keyboard,true
parameter,videomodewidth,640
parameter,videomodeheight,480
parameter,color,white
parameter,bgcolor,black
item1,
  "items": [
    {
      "type": "instructions",
      ...
    }
  ]
}
```

**Status:** ⚠️ **Hybrid format - CSV headers with JSON body**

This appears to be an incomplete or experimental format. The file has CSV-style parameter definitions but embeds JSON for item definitions.

**No CSV Runner Implemented** - The dmdx.csv file exists but there's no corresponding parser in dmdx.pbl.

---

## Integration with PEBL Battery Architecture

### Comparison: Experiment Builder vs Battery Tasks

| Feature | Experiment Builder | Battery Tasks |
|---------|-------------------|---------------|
| **Creation method** | GUI point-and-click | Hand-coded .pbl |
| **Parameter schema** | No | Yes (`params/*.schema`) |
| **Translations** | No | Yes (`translations/*.json`) |
| **Data output** | No (incomplete) | Yes (CSV via `GetNewDataFile()`) |
| **Subject number** | No | Yes (`gSubNum`) |
| **Reusability** | Low (custom each time) | High (library functions) |
| **Complexity** | Simple experiments | Complex designs |

**Current Disconnect:**

The experiment builder generates standalone scripts but doesn't leverage the PEBL battery task infrastructure:
- No `CreateParameters()` integration
- No `GetStrings()` for translations
- No standardized data file output
- Generated code doesn't follow battery conventions

---

## Gap Analysis

### What Works Today

1. **Experiment Builder GUI** ✅
   - Create experiments via point-and-click
   - Add/edit/delete stages
   - Compile to standalone .pbl scripts
   - Save/load to JSON

2. **DMDX JSON Runner** ✅
   - Parse visualdmdx.com JSON format
   - Display instructions
   - Present stimuli (text, images, blanks)
   - Handle timing and positioning
   - Collect responses

### What's Missing for Production Use

1. **Data Collection** ❌
   - DMDX runner doesn't save response data
   - No integration with PEBL data file conventions
   - No subject number handling

2. **Battery Task Integration** ❌
   - Generated code doesn't use parameter schemas
   - No translation support
   - Doesn't follow battery directory structure

3. **Advanced Experiment Features** ❌
   - No counterbalancing in builder
   - No within-subject designs
   - No conditional branching
   - Limited loop support

4. **DMDX Format Support** ❌
   - Only JSON from visualdmdx.com
   - No native .rtf DMDX file parsing
   - CSV format incomplete/abandoned

5. **Documentation** ❌
   - No user guide for experiment builder
   - No DMDX compatibility documentation
   - No tutorial for JSON format

---

## Use Cases and Workflows

### Current Workflow 1: Build Experiment with GUI

```
1. Run bin/pebl2 demo/expbuilder/expbuilder.pbl
2. Click "Add Stage" → Select "Text Instructions"
3. Enter instruction text
4. Repeat for multiple stages
5. Click "Compile & run"
6. Generated script runs immediately
7. Edit tmp.pbl for final tweaks
```

**Limitations:**
- Simple linear experiments only
- No data collection
- No parameter customization

### Current Workflow 2: Run DMDX Experiment

```
1. Export experiment from visualdmdx.com as JSON
2. Save as dmdx.json
3. Edit dmdx.pbl to load your JSON file
4. Run bin/pebl2 demo/expbuilder/dmdx.pbl
5. Experiment runs (but doesn't save data)
```

**Limitations:**
- No data output
- Must edit code to change JSON file
- No command-line parameters

### Ideal Workflow 3: Generic Experiment from JSON (NOT IMPLEMENTED)

```
1. Create experiment.json with standard format:
   {
     "metadata": {"title": "...", "author": "..."},
     "parameters": {"timeout": 1000, ...},
     "trials": [
       {"type": "instruction", "text": "..."},
       {"type": "choice", "stimulus": "...", "choices": [...]}
     ]
   }

2. Run: bin/pebl2 generic-experiment.pbl -v expfile=experiment.json -v subnum=123

3. Data automatically saved to data/generic-experiment-123.csv
```

**Status:** 🔴 **Not implemented** - Would require new generic runner

### Ideal Workflow 4: Generic Experiment from CSV (NOT IMPLEMENTED)

```
1. Create experiment.csv:
   trial,type,stimulus,response_keys,correct,duration
   1,instruction,"Welcome",space,,
   2,choice,cat.jpg,"y,n",y,2000
   3,choice,dog.jpg,"y,n",n,2000

2. Run: bin/pebl2 csv-experiment.pbl -v expfile=experiment.csv -v subnum=123

3. Data saved with trial-by-trial responses
```

**Status:** 🔴 **Not implemented** - Would require CSV parser and generic runner

---

## Recommendations

### Phase 1: Polish Existing Tools (PEBL 2.2)

**Goal:** Make current tools usable and documented

**Tasks:**

1. **✅ Document Current State** (this document)

2. **Add Data Collection to DMDX Runner**
   ```pebl
   ## In RunItem():
   gFileOut <- GetNewDataFile(gSubNum, gWin, "dmdx-experiment", "csv",
                               "subject,item,response,rt,correct")
   FilePrint(gFileOut, gSubNum + "," + item.description + "," + resp + "," + rt + "," + correct)
   ```

3. **Add Command-Line Parameters**
   ```pebl
   ## At start of dmdx.pbl:
   gSubNum <- GetSubNum(gWin)  ## Prompt or use -v subnum=123
   expfile <- "dmdx.json"      ## Default
   if(IsDefined(gExpFile))
   {
     expfile <- gExpFile
   }
   ```

4. **Create User Documentation**
   - `doc/EXPBUILDER_GUIDE.md` - How to use the GUI builder
   - `doc/DMDX_JSON_FORMAT.md` - JSON format specification
   - Add examples to demo/expbuilder/examples/

5. **Bug Fixes**
   - Test with various JSON files
   - Handle missing fields gracefully
   - Add error messages for invalid JSON

**Estimated Effort:** 1-2 days

---

### Phase 2: Generic Experiment Framework (PEBL 2.3)

**Goal:** Create production-ready generic experiment system

**Architecture:**

```
Generic Experiment Runner (generic-exp.pbl)
├── JSON Backend (json-parser.pbl)
├── CSV Backend (csv-parser.pbl)
├── Trial Types Library (trial-types.pbl)
│   ├── InstructionTrial
│   ├── ChoiceRT Trial
│   ├── YesNoTrial
│   ├── RatingScaleTrial
│   └── FreeResponseTrial
├── Data Collection (auto-saves to CSV)
└── Battery Task Integration (params, translations)
```

**Standard JSON Format:**

```json
{
  "metadata": {
    "title": "Generic Choice RT Experiment",
    "author": "Researcher Name",
    "version": "1.0",
    "description": "Simple choice reaction time task"
  },
  "parameters": {
    "timeout": 5000,
    "iti": 500,
    "fixation_duration": 500,
    "feedback": true
  },
  "trials": [
    {
      "trial_type": "instruction",
      "text": "Press Y for animals, N for objects",
      "advance_key": "space"
    },
    {
      "trial_type": "choice_rt",
      "stimulus": "cat.jpg",
      "stimulus_type": "image",
      "choices": ["y", "n"],
      "correct_response": "y",
      "timeout": 5000,
      "position": "center"
    },
    {
      "trial_type": "choice_rt",
      "stimulus": "chair.jpg",
      "stimulus_type": "image",
      "choices": ["y", "n"],
      "correct_response": "n",
      "timeout": 5000,
      "position": "center"
    }
  ],
  "design": {
    "randomize": true,
    "blocks": 3,
    "practice_trials": 5
  }
}
```

**Standard CSV Format:**

```csv
trial_type,stimulus,stimulus_type,choices,correct,timeout,text
instruction,,,,,,"Press Y for animals"
choice_rt,cat.jpg,image,"y,n",y,5000,
choice_rt,chair.jpg,image,"y,n",n,5000,
choice_rt,dog.jpg,image,"y,n",y,5000,
```

**Implementation Plan:**

1. **Create Trial Type Library** (`pebl-lib/TrialTypes.pbl`)
   ```pebl
   define RunInstructionTrial(params)
   define RunChoiceRTTrial(params)
   define RunYesNoTrial(params)
   define RunRatingScaleTrial(params)
   ```

2. **Create JSON Backend** (`generic-json-runner.pbl`)
   ```pebl
   define Start(p)
   {
     gSubNum <- GetSubNum(gWin)
     expfile <- gExpFile  ## From -v expfile=...

     expText <- FileReadText(expfile)
     exp <- ParseJSON(expText)

     ## Setup data file
     basename <- GetFileBasename(expfile)
     gFileOut <- GetNewDataFile(gSubNum, gWin, basename, "csv",
                                 "subject,trial,type,stimulus,response,rt,correct")

     ## Run trials
     trial_num <- 1
     loop(trial, exp.trials)
     {
       result <- RunTrial(trial, trial_num)
       SaveResult(result, trial_num)
       trial_num <- trial_num + 1
     }
   }
   ```

3. **Create CSV Backend** (`generic-csv-runner.pbl`)
   ```pebl
   define Start(p)
   {
     gSubNum <- GetSubNum(gWin)
     expfile <- gExpFile

     trials <- ReadCSV(expfile)
     ## Similar to JSON runner but parse CSV
   }
   ```

4. **Integrate with Battery Architecture**
   - Support `params/*.schema` files
   - Support `translations/*.json` files
   - Follow standard battery directory structure

**Estimated Effort:** 1-2 weeks

---

### Phase 3: DMDX Native Format Support (Future)

**Goal:** Parse native DMDX .rtf files

**Challenge:**
- DMDX uses Rich Text Format (.rtf) with custom markup
- Complex scripting language embedded in RTF
- Would require RTF parser + DMDX syntax parser

**Recommendation:**
- **Low priority** - Most users can export to JSON from visualdmdx.com
- Only implement if there's strong user demand
- Alternative: Create DMDX-to-JSON converter tool

---

## Technical Considerations

### JSON vs CSV Trade-offs

| Aspect | JSON | CSV |
|--------|------|-----|
| **Complexity** | Supports nested structures | Flat table only |
| **Readability** | Good for programmers | Excellent for Excel users |
| **Flexibility** | Can represent any structure | Limited to rows/columns |
| **Tooling** | ParseJSON() built-in | ReadCSV() needs implementation |
| **File Size** | Larger (verbose) | Smaller (compact) |
| **Use Case** | Complex experiments | Simple trial lists |

**Recommendation:** Support both - JSON for complex designs, CSV for simple trial lists

### Data Collection Best Practices

**Current PEBL Battery Standard:**

```pebl
## In battery tasks:
gFileOut <- GetNewDataFile(gSubNum, gWin, "taskname", "csv", "sub,trial,rt,acc")
FilePrint(gFileOut, gSubNum + "," + trialnum + "," + rt + "," + accuracy)
```

**Generic Experiment Should Follow Same Pattern:**

```pebl
## Auto-generate header from trial structure
header <- "subject,trial,trial_type,stimulus,response,rt,correct,timestamp"
gFileOut <- GetNewDataFile(gSubNum, gWin, expname, "csv", header)

## Auto-save each trial
define SaveTrialData(trial, response, rt)
{
  FilePrint(gFileOut, gSubNum + "," + trial.number + "," +
            trial.type + "," + trial.stimulus + "," +
            response + "," + rt + "," + trial.correct)
}
```

### Backward Compatibility

**Critical:** Don't break existing demo files

**Strategy:**
- Keep `expbuilder.pbl` and `dmdx.pbl` as-is
- Create new files for generic framework:
  - `generic-json-exp.pbl`
  - `generic-csv-exp.pbl`
  - `pebl-lib/TrialTypes.pbl`
- Old demos continue to work unchanged

---

## Testing Plan

### Test Cases for DMDX Runner

1. **Basic instruction screen** ✅ Works (tested with dmdx.json)
2. **Text stimulus presentation** ✅ Works
3. **Image stimulus presentation** ✅ Works
4. **Blank screen (fixation)** ✅ Works
5. **Keyboard response collection** ✅ Works
6. **Loop with variables** ✅ Works
7. **Data file output** ❌ Not implemented
8. **Error handling for malformed JSON** ❌ Not tested

### Test Cases for Generic Framework (When Implemented)

1. Simple instruction-only experiment
2. Choice RT task (2AFC)
3. Yes/No judgment task
4. Rating scale task
5. Mixed trial types in single experiment
6. Randomization and blocking
7. CSV and JSON format compatibility
8. Data output validation

---

## Conclusion

### Summary

The `demo/expbuilder` code contains two functional but incomplete tools:

1. **Experiment Builder GUI** - Creates experiments via point-and-click, generates standalone PEBL scripts
2. **DMDX JSON Runner** - Parses and executes experiments from visualdmdx.com JSON format

Both tools demonstrate proof-of-concept functionality but lack:
- Data collection and output
- Integration with PEBL battery architecture
- Documentation and examples
- Production-ready polish

### Recommendations by Release

**PEBL 2.2 (Immediate):**
- ✅ Document current state (this document)
- Add data collection to DMDX runner (1 day)
- Add command-line parameters (1 day)
- Create user guide (1 day)
- **Total: 3 days effort**

**PEBL 2.3 (Future):**
- Design generic experiment JSON/CSV format
- Implement trial type library
- Create generic JSON/CSV runners
- Full battery task integration
- **Total: 1-2 weeks effort**

**PEBL 3.0+ (Long-term):**
- Native DMDX .rtf parser (if demand exists)
- Advanced experiment builder features
- Visual experiment designer (web-based?)

### Immediate Next Steps

For PEBL 2.2 release:

1. Add basic data collection to `dmdx.pbl`:
   ```pebl
   ## At top:
   gSubNum <- GetSubNum(gWin)
   gFileOut <- GetNewDataFile(gSubNum, gWin, "dmdx-exp", "csv",
                               "subject,item,type,response,rt")

   ## In RunItem():
   FilePrint(gFileOut, gSubNum + "," + item.description + "," +
             item.type + "," + resp + "," + rt)
   ```

2. Create `doc/EXPBUILDER_GUIDE.md` with:
   - How to use the GUI builder
   - How to run DMDX JSON experiments
   - JSON format specification
   - Limitations and future plans

3. Add example JSON files to `demo/expbuilder/examples/`

---

## References

- **DMDX Official Site:** https://psy1.psych.arizona.edu/~kforster/dmdx/
- **Visual DMDX:** https://visualdmdx.com (JSON export tool)
- **PEBL Battery Tasks:** `battery/*/` (standard task architecture)
- **PEBL Standard Library:** `pebl-lib/Utility.pbl` (parameter and data file functions)

---

## Appendix A: File Inventory

**Main Files:**

| File | Lines | Purpose | Status |
|------|-------|---------|--------|
| expbuilder.pbl | 871 | GUI experiment builder | ✅ Functional |
| dmdx.pbl | 372 | DMDX JSON parser/runner | ⚠️ Works, no data output |
| dmdx.json | 140 | Example DMDX JSON format | ✅ Valid example |
| dmdx.csv | 108 | Hybrid CSV/JSON format | ❌ Incomplete |
| demoexp.pbl | 120 | Generated experiment example | ✅ Valid output |
| tmp.pbl | 77 | Temporary compilation output | ✅ Valid output |

**Supporting Files Needed (Not Present):**

- `doc/EXPBUILDER_GUIDE.md` - User documentation
- `doc/DMDX_JSON_FORMAT.md` - Format specification
- `demo/expbuilder/examples/*.json` - More examples
- `generic-json-exp.pbl` - Future generic runner
- `generic-csv-exp.pbl` - Future CSV runner
- `pebl-lib/TrialTypes.pbl` - Future trial type library

---

## Appendix B: DMDX JSON Format Specification

**Based on visualdmdx.com export format:**

### Top-Level Structure

```json
{
  "id": "string",           // Experiment ID
  "version": number,         // Format version (0)
  "description": "string",   // Optional description
  "parameters": {...},       // Global settings
  "items": [...]            // Experiment items (trials/instructions)
}
```

### Parameters Object

```json
"parameters": {
  "timeout": "1000",                          // Response timeout (ms)
  "keyboard": "true",                         // Enable keyboard
  "mouse": "false",                           // Enable mouse
  "videomode": {
    "width": "640",
    "height": "480",
    "bpp": "8",
    "hz": "0"
  },
  "feedback": "none",                         // Feedback type
  "frameduration": "30",                      // Frame duration (ms)
  "delay": "5",                               // ITI delay
  "color": "#ffffff",                         // Foreground color (hex)
  "bgcolor": "#000000",                       // Background color (hex)
  "fontsize": "12",                           // Font size
  "outputascii": "true",                      // Output format
  "recordclockontime": "true"                 // Record timing
}
```

### Item Types

**Instruction Item:**
```json
{
  "type": "instructions",
  "description": "e1",
  "content": "Experiment instructions text",
  "response": "no-response",
  "scramble": "false",
  "clockon": "0",
  "stimuli": []
}
```

**Test Item:**
```json
{
  "type": "item",
  "description": "stim1",
  "response": "positive",              // Response type
  "scramble": "false",
  "clockon": 1,                        // RT clock starts on stimulus N
  "stimuli": [...]                     // Stimulus sequence
}
```

**Loop Item:**
```json
{
  "type": "loop",
  "description": "4",
  "response": "no-response",
  "scramble": "false",
  "variables": [
    {
      "name": "1",
      "values": [
        {"loop": "1", "v": "1"},
        {"loop": "2", "v": "3"}
      ]
    }
  ]
}
```

### Stimulus Formats

**Text Stimulus:**
```json
{
  "format": "text",
  "val": "displayed text",
  "duration": "30",                    // Frames (30 = ~1 sec at 30fps)
  "clear": "true",                     // Clear after duration
  "horizontal": "center",              // Positioning
  "vertical": "middle"
}
```

**Image Stimulus:**
```json
{
  "format": "bmp",
  "val": "image.bmp",                  // Filename
  "duration": "30",
  "clear": "true",
  "syncText": "true",                  // Sync with text
  "horizontal": "center",
  "vertical": "middle"
}
```

**Blank Stimulus:**
```json
{
  "format": "blank",
  "val": "",
  "duration": "30",
  "clear": "true"
}
```

---

## Appendix C: Generic JSON Format Proposal

**Proposed standard format for PEBL generic experiments:**

```json
{
  "metadata": {
    "title": "Experiment Title",
    "author": "Author Name",
    "contact": "author@email.edu",
    "version": "1.0",
    "description": "Experiment description",
    "created": "2025-11-18"
  },
  "window": {
    "width": 800,
    "height": 600,
    "bgcolor": "black",
    "fgcolor": "white"
  },
  "parameters": {
    "timeout": 5000,
    "iti": 500,
    "fixation_duration": 500,
    "feedback": true,
    "feedback_duration": 1000
  },
  "design": {
    "randomize_trials": true,
    "blocks": 1,
    "trials_per_block": null,
    "practice_trials": 0,
    "counterbalance": false
  },
  "trials": [
    {
      "trial_type": "instruction",
      "text": "Instruction text here",
      "text_size": 24,
      "advance_key": "space"
    },
    {
      "trial_type": "choice_rt",
      "stimulus": "cat.jpg",
      "stimulus_type": "image",
      "stimulus_position": "center",
      "choices": ["y", "n"],
      "choice_labels": ["Animal", "Object"],
      "correct_response": "y",
      "timeout": 5000,
      "fixation": true,
      "feedback": true
    }
  ],
  "output": {
    "format": "csv",
    "filename_prefix": "generic-exp",
    "columns": ["subject", "trial", "block", "stimulus",
                "response", "rt", "correct", "timestamp"]
  }
}
```

This format is more structured and PEBL-specific than DMDX JSON, with explicit support for battery task conventions.
