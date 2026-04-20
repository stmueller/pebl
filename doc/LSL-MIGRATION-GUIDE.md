# LSL Migration Guide for PEBL Battery Tasks

## Overview

This guide provides step-by-step instructions for adding LSL (Lab Streaming Layer) support to PEBL battery tasks. Each task must be migrated individually because trial structures vary significantly.

## Three-Step Migration Process

### Step 1: Initialize LSL (Standardized)

Add `InitializeLSL(gWin)` immediately after `MakeWindow()` in the `Start()` function.

**Location:** After `gWin <- MakeWindow(...)`

**Code to add:**
```pebl
gWin <- MakeWindow("black")
InitializeLSL(gWin)  ## Initialize LSL outlet if --lsl flag was used (shows prompt)
```

**Effect:**
- Creates LSL outlet if `--lsl` flag is used
- Shows visual prompt for experimenter to start LabRecorder
- Does nothing if `--lsl` flag not set (zero overhead)

---

### Step 2: Add Trial Markers (Task-Specific)

Identify the trial loop and add markers at key experimental events.

**Essential markers for most tasks:**

1. **Block/Section markers**
   - At start of each block: `LSLMarker("block_N_NAME_start")`
   - At end of each block: `LSLMarker("block_N_NAME_end")`

2. **Stimulus onset markers**
   - Immediately after `Draw()` when stimulus appears
   - `LSLMarker("stimulus_TYPE_trial_N")`

3. **Response markers**
   - After response collection
   - `LSLMarker("response_TYPE")` or `LSLMarker("response_none")` for timeouts

4. **Accuracy markers**
   - After scoring
   - `LSLMarker("trial_correct")` or `LSLMarker("trial_error")`

**Example pattern (from gonogo):**
```pebl
define DoBlock(trials, correctresponse, type, col)
{
  trial <- 1
  loop(i, trials)
  {
    ## Show stimulus
    Show(stim)
    Draw()
    starttime <- GetTime()
    LSLMarker("stimulus_" + i + "_trial_" + trial)  ## Stimulus onset

    ## Get response
    resp <- WaitForLayoutResponse(gLayout, timeout)

    ## Check response
    if(resp == "<timeout>")
    {
      LSLMarker("response_none")
      responded <- 0
    } else {
      LSLMarker("response_made")
      responded <- 1
    }

    ## Score
    if(corr)
    {
      LSLMarker("trial_correct")
    } else {
      LSLMarker("trial_error")
    }

    trial <- trial + 1
  }
}
```

---

### Step 3: Finalize LSL (Standardized)

Add `FinalizeLSL()` near the end of `Start()`, before final debrief/message.

**Location:** After all trials/blocks complete, before final message

**Code to add:**
```pebl
## Finalize LSL
FinalizeLSL()

## Debrief at the end
MessageKeyBox(gDebrief)
```

**Effect:** Cleanly closes LSL outlet (optional but recommended)

---

## Task-Specific Marker Placement

Each task requires analysis to identify:

1. **Where is the trial loop?**
   - Look for `loop(i, trials)` or similar iteration

2. **Where does the stimulus appear?**
   - Usually after `Show(stim)` and `Draw()`
   - Marker goes immediately after `Draw()`

3. **Where is the response collected?**
   - `WaitForKeyPress()`, `WaitForMouseButton()`, `WaitForLayoutResponse()`, etc.
   - Add markers based on response type (correct/error, timeout, etc.)

4. **Are there multiple blocks/conditions?**
   - Add block start/end markers around each block

---

## Common Task Patterns

### Pattern 1: Simple Response Time Task

```pebl
loop(trial, trials)
{
  ## Fixation
  Show(fixation)
  Draw()
  LSLMarker("fixation_on")
  Wait(500)

  ## Stimulus
  Show(stimulus)
  Draw()
  LSLMarker("stimulus_trial_" + trial)

  ## Response
  resp <- WaitForKeyPress(keys)
  LSLMarker("response_" + resp)
}
```

### Pattern 2: Go/No-Go or Accuracy Task

```pebl
loop(trial, trials)
{
  Show(stim)
  Draw()
  LSLMarker("stimulus_" + stimType + "_trial_" + trial)

  resp <- WaitForLayoutResponse(gLayout, timeout)

  if(resp == "<timeout>")
  {
    LSLMarker("response_none")
  } else {
    LSLMarker("response_made")
  }

  if(corr)
  {
    LSLMarker("trial_correct")
  } else {
    LSLMarker("trial_error")
  }
}
```

### Pattern 3: Multi-Block Design

```pebl
gBlock <- 1
LSLMarker("block_1_start")
DoBlock(block1Trials)
LSLMarker("block_1_end")

gBlock <- 2
LSLMarker("block_2_start")
DoBlock(block2Trials)
LSLMarker("block_2_end")
```

---

## Marker Naming Conventions

**Use descriptive, hierarchical names:**
- `"block_N_NAME_start"` / `"block_N_NAME_end"`
- `"stimulus_TYPE_trial_N"`
- `"response_KEYNAME"` or `"response_correct"` / `"response_error"`
- `"trial_correct"` / `"trial_error"`
- `"fixation_on"` / `"fixation_off"`

**Avoid:**
- Single characters: `"P"`, `"R"`
- Numbers only: `"1"`, `"2"`
- Spaces: `"trial start"` (use underscores)

---

## Testing Checklist

After migrating a task:

1. **Test without LSL** (normal operation should be unchanged):
   ```bash
   bin/pebl2 battery/TASKNAME/TASKNAME.pbl -s 001
   ```

2. **Test with LSL** (stream should appear in LabRecorder):
   ```bash
   bin/pebl2 battery/TASKNAME/TASKNAME.pbl -s 001 --lsl "PEBL_TASKNAME"
   ```

3. **Check console output:**
   - Should see: `"LSL outlet created: [PEBL_TASKNAME]"`
   - Should NOT see: `"WARNING: LSL outlet creation failed"`

4. **Check LabRecorder:**
   - Stream appears in list before clicking OK on prompt
   - Markers appear during task execution

5. **Check data files:**
   - Task should produce normal data files
   - LSL data captured in `.xdf` file from LabRecorder

---

## Migration Status Tracking

Use this checklist to track which tasks have been migrated:

### Completed (1 task)
- [x] gonogo - Full LSL support with block/trial markers

### To Migrate (57 tasks)
- [ ] ANT
- [ ] BPM
- [ ] Corsi
- [ ] flanker
- [ ] stroop
- [ ] ... (add remaining tasks)

### Migration Notes
- Each task requires ~10-30 minutes for analysis and implementation
- Some tasks may have complex trial structures requiring custom marker logic
- Test thoroughly after each migration

---

## Common Issues

### Issue: Stream doesn't appear in LabRecorder
**Solution:** Check that `--lsl` flag is used when running PEBL

### Issue: Markers don't appear
**Solution:** Verify `LSLMarker()` calls are placed after actual events (e.g., after `Draw()` for stimulus onset)

### Issue: Task crashes after adding LSL
**Solution:** Check for syntax errors (missing commas, unmatched parentheses)

---

## Example: Complete Migration of Simple Task

**Before:**
```pebl
define Start(p)
{
  gWin <- MakeWindow("black")

  loop(trial, trials)
  {
    Show(stim)
    Draw()
    resp <- WaitForKeyPress(["<lshift>", "<rshift>"])
  }

  MessageBox("Thank you!", gWin)
}
```

**After:**
```pebl
define Start(p)
{
  gWin <- MakeWindow("black")
  InitializeLSL(gWin)  ## ADDED

  LSLMarker("block_start")  ## ADDED
  loop(trial, trials)
  {
    Show(stim)
    Draw()
    LSLMarker("stimulus_trial_" + trial)  ## ADDED
    resp <- WaitForKeyPress(["<lshift>", "<rshift>"])
    LSLMarker("response_" + resp)  ## ADDED
  }
  LSLMarker("block_end")  ## ADDED

  FinalizeLSL()  ## ADDED
  MessageBox("Thank you!", gWin)
}
```

---

## Next Steps

1. Choose a task from the "To Migrate" list
2. Read through the task code to understand trial structure
3. Apply the three-step process
4. Test without and with LSL
5. Update migration status
6. Move to next task

**Estimated total time:** 10-30 hours for all 57 remaining tasks
