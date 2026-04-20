# Lab Streaming Layer (LSL) Integration in PEBL

## Overview

PEBL now supports Lab Streaming Layer (LSL) for synchronized multi-modal data acquisition. LSL provides sub-millisecond timestamp synchronization across multiple data streams, making it ideal for neuroscience experiments combining PEBL behavioral tasks with EEG, eye-tracking, physiological sensors, or other data streams.

## Quick Start

### Running PEBL with LSL

To enable LSL in any PEBL experiment, use the `--lsl` flag:

```bash
bin/pebl2 battery/gonogo/gonogo.pbl -s 001 --lsl "PEBL_gonogo"
```

This creates an LSL outlet named "PEBL_gonogo" that other LSL applications (like LabRecorder) can detect and record.

### Basic Usage in PEBL Scripts

Add just three lines to any PEBL experiment:

```pebl
define Start(p)
{
  InitializeUpload()  ## Existing line

  ## ... parameter setup ...

  gWin <- MakeWindow("black")  ## Existing line
  InitializeLSL(gWin)          ## NEW: Creates LSL outlet, then shows prompt (when --lsl flag used)

  ## ... rest of your experiment setup ...

  ## During your experiment loop:
  LSLMarker("stimulus_onset")    ## NEW: Send event markers

  ## ... show stimulus, get response ...

  LSLMarker("response_correct")  ## NEW: Send markers for responses

  ## At the end (optional - cleanup happens automatically):
  FinalizeLSL()
}
```

## Complete Example

Here's how to add LSL to the Go/No-Go task:

```pebl
define Start(p)
{
  InitializeUpload()
  InitializeLSL()  ## Initialize LSL outlet

  ## ... parameter setup ...

  ## Run experiment blocks
  DoBlock(practice, gParams.pstim, "practice", gParams.color)
}

define DoBlock(trials, correctresponse, type, col)
{
  loop(i, trials)
  {
    ## Show stimulus
    Show(stim)
    Draw()
    LSLMarker("stimulus_" + i)  ## Send marker: "stimulus_P" or "stimulus_R"

    ## Wait for response
    resp <- WaitForLayoutResponse(gLayout, gParams.isi)

    ## Send response marker
    if(resp == "<timeout>")
    {
      LSLMarker("response_none")
    } else {
      LSLMarker("response_made")
    }

    ## Send accuracy marker
    if(corr)
    {
      LSLMarker("trial_correct")
    } else {
      LSLMarker("trial_error")
    }
  }
}
```

## Available Functions

### InitializeLSL()

Initialize LSL outlet at experiment start.

- **Call once** at the beginning of `Start()`
- Creates LSL outlet only if `--lsl` flag was used
- Auto-generates stream name from script name if not provided via command line
- Returns 1 on success, 0 if LSL not enabled

**Example:**
```pebl
success <- InitializeLSL()
if(success)
{
  Print("LSL outlet ready")
}
```

### LSLMarker(marker)

Send an event marker to the LSL stream.

- **Arguments:** `marker` - String or integer marker to send
- **Returns:** 1 if marker sent, 0 if LSL not enabled
- Automatically timestamped by LSL
- Safe to call even when LSL is disabled (becomes a no-op)

**Examples:**
```pebl
LSLMarker("trial_start")           ## String marker
LSLMarker("stimulus_" + stimType)  ## Dynamic string
LSLMarker(trialNumber)             ## Integer marker
LSLMarker("response_" + resp)      ## Response marker
```

### FinalizeLSL()

Close LSL outlet at experiment end (optional).

- Cleanup happens automatically when PEBL exits
- Only needed if you want explicit cleanup control
- Returns 1 on success

**Example:**
```pebl
FinalizeLSL()
Print("LSL outlet closed")
```

## Marker Naming Conventions

Use descriptive, hierarchical marker names:

**Good Examples:**
- `"trial_start"`, `"trial_end"`
- `"stimulus_target"`, `"stimulus_foil"`
- `"response_correct"`, `"response_error"`
- `"block_1_start"`, `"block_1_end"`
- `"fixation_on"`, `"fixation_off"`

**Avoid:**
- Single characters: `"P"`, `"R"` (not descriptive)
- Ambiguous: `"1"`, `"2"` (what do they mean?)
- Spaces: `"trial start"` (use underscores instead)

## Recording LSL Data

### Using LabRecorder (Standalone)

1. **Start LabRecorder:**
   ```bash
   cd libs/labstreaminglayer/Apps/LabRecorder
   ./LabRecorder
   ```

2. **Run your PEBL experiment with LSL:**
   ```bash
   bin/pebl2 battery/gonogo/gonogo.pbl -s 001 --lsl "PEBL_gonogo"
   ```

3. **In LabRecorder:**
   - You'll see "PEBL_gonogo" appear in the stream list
   - Click the stream to select it
   - Click "Start" to begin recording
   - Run your PEBL experiment
   - Click "Stop" when done
   - Data saved to `.xdf` file

### Synchronizing with EEG/Eye-Tracking

LSL automatically synchronizes timestamps across all streams:

1. **Start your EEG/eye-tracker LSL outlet** (device-specific)
2. **Start LabRecorder** and select all streams
3. **Start recording in LabRecorder**
4. **Run PEBL experiment** with `--lsl` flag
5. **Stop recording** when experiment ends

All markers and data streams are synchronized with sub-millisecond accuracy in the `.xdf` file.

## Advanced Usage

### Custom Stream Names

Specify custom stream name via command line:

```bash
bin/pebl2 myexp.pbl --lsl "Experiment1_Participant001"
```

Or auto-generate from script name (default):

```bash
bin/pebl2 gonogo.pbl --lsl  ## Creates stream "PEBL_gonogo"
```

### Checking LSL Status

```pebl
if(gLSLEnabled)
{
  Print("LSL is enabled with stream: " + gLSLStreamName)
}
```

### Conditional LSL Markers

LSL functions are safe to call even when disabled:

```pebl
## This pattern always works - no need to check gLSLEnabled
LSLMarker("trial_start")  ## Automatically skipped if LSL not enabled
```

## Technical Details

### Implementation

- Uses LSL C API (liblsl)
- String markers via irregular-rate channel
- Automatic LSL timestamp synchronization
- Zero overhead when LSL disabled

### Stream Properties

- **Type:** Markers (event markers)
- **Channel format:** String
- **Sampling rate:** Irregular (event-driven)
- **Channel count:** 1

### Library Dependencies

PEBL includes a bundled copy of liblsl from LabRecorder. No additional installation required.

## Troubleshooting

### Stream doesn't appear in LabRecorder

1. Check that `--lsl` flag is used when running PEBL
2. Check console output for "LSL outlet created" message
3. Click "Refresh" in LabRecorder
4. Ensure LabRecorder and PEBL are on same network (if remote)

### Build errors about LSL

Ensure `USE_LSL=1` in Makefile and rebuild:

```bash
make clean
make main
```

## Example: Complete Stroop Task with LSL

```pebl
define Start(p)
{
  InitializeLSL()

  gWin <- MakeWindow()

  ##... parameter setup ...

  loop(trial, trials)
  {
    LSLMarker("trial_start")

    ## Show fixation
    Show(fixation)
    Draw()
    LSLMarker("fixation_on")
    Wait(500)
    Hide(fixation)

    ## Show stimulus
    Show(stimulus)
    Draw()
    LSLMarker("stimulus_" + congruency)  ## "stimulus_congruent" or "stimulus_incongruent"

    ## Get response
    resp <- WaitForKeyPress(["<lshift>", "<rshift>"])
    rt <- GetTime() - stimOnset

    LSLMarker("response_" + resp + "_RT_" + rt)

    ## Score response
    if(IsCorrect(resp, trial))
    {
      LSLMarker("trial_correct")
    } else {
      LSLMarker("trial_error")
    }

    Hide(stimulus)
  }

  FinalizeLSL()
}
```

## See Also

- Test script: `test-lsl.pbl`
- LSL documentation: https://labstreaminglayer.readthedocs.io/
- LabRecorder: `libs/labstreaminglayer/Apps/LabRecorder/`

## Version History

- **PEBL 2.3+**: LSL integration added (January 2026)
