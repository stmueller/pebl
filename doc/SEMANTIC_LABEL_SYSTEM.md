# PEBL Semantic Label System

## Overview

The semantic-first label architecture allows tasks to declare response **meanings** (e.g., "SAME", "DIFFERENT") independent of the input method. The system automatically adapts labels to the current response mode (keyboard, mouse buttons, touch targets).

## Problem Solved

**Before**: Tasks had to manage physical input labels ("LEFT-SHIFT", "RIGHT-CLICK") which:
- Created confusion about response meaning
- Required per-mode customization
- Made cross-platform adaptation difficult

**After**: Tasks declare semantic meanings once, system handles all formatting:
- `["SAME", "DIFFERENT"]` works for keyboard, mouse, touch
- Labels automatically show both physical input and semantic meaning
- 100% backward compatible

## Usage

### Basic Declaration

In your task's parameter pairs:

```pebl
parpairs <- [
    ["responsemode", "auto"],
    ["responselabels", ["SAME", "DIFFERENT"]]
]
```

### What Happens

The Layout & Response System automatically:

1. **Gets semantic labels** from `params.responselabels`
2. **Formats for current mode**:
   - **keyboard**: "LEFT-SHIFT (SAME)" | "RIGHT-SHIFT (DIFFERENT)"
   - **mousebutton**: "LEFT-CLICK (SAME)" | "RIGHT-CLICK (DIFFERENT)"
   - **mousetarget**: "SAME" | "DIFFERENT" (targets are self-evident)
3. **Sets footer** to pure semantic: "SAME | DIFFERENT"
4. **Returns semantic values** from `WaitForLayoutResponse()`: "left" or "right"

### Response Mapping

`WaitForLayoutResponse()` always returns semantic values from `mode.semantic`:
- keyboard/mousebutton/mousetarget: ["left", "right"]
- Match these in your task logic

Example from luckvogel.pbl:
```pebl
resp <- WaitForLayoutResponse(gLayout, 10000)

if(change)
{
    corr <- resp == "right"  ## "right" means "DIFFERENT"
}
else
{
    corr <- resp == "left"   ## "left" means "SAME"
}
```

## Complete Example

```pebl
define Start(p)
{
    ## Declare semantic labels in parameters
    parpairs <- [
        ["responsemode", "auto"],
        ["responselabels", ["CORRECT", "INCORRECT"]]
    ]
    gParams <- CreateParameters(parpairs, gParamFile)

    ## Create layout (automatically uses semantic labels)
    gLayout <- CreateLayout("mytask", gWin, gParams)

    ## Response labels now show:
    ## - keyboard: "LEFT-SHIFT (CORRECT)" | "RIGHT-SHIFT (INCORRECT)"
    ## - mousebutton: "LEFT-CLICK (CORRECT)" | "RIGHT-CLICK (INCORRECT)"
    ## - mousetarget: "CORRECT" | "INCORRECT"

    ## Footer shows: "CORRECT | INCORRECT"

    ## Wait for response (returns "left" or "right")
    resp <- WaitForLayoutResponse(gLayout, 5000)

    ## Map semantic to your task logic
    if(resp == "left")
    {
        Print("User indicated CORRECT")
    }
    elseif(resp == "right")
    {
        Print("User indicated INCORRECT")
    }
}
```

## Default Behavior

If `responselabels` is **not** provided:
- System uses capitalized `mode.semantic` values
- Example: ["LEFT", "RIGHT"]

```pebl
## Without responselabels parameter
parpairs <- [["responsemode", "auto"]]
gParams <- CreateParameters(parpairs, gParamFile)
gLayout <- CreateLayout("mytask", gWin, gParams)

## Labels show: "LEFT-SHIFT (LEFT)" | "RIGHT-SHIFT (RIGHT)"
## Footer shows: "LEFT | RIGHT"
```

## Three Display Contexts

Semantic labels appear in three places:

1. **Response zone labels** (formatted with input method):
   - keyboard: "KEY (semantic)"
   - mousebutton: "BUTTON (semantic)"
   - mousetarget: just semantic

2. **Footer text** (pure semantic):
   - Always: "SEMANTIC1 | SEMANTIC2"
   - No physical input shown

3. **Instruction text** (task-specific):
   - Tasks can reference `gParams.responselabels` in instruction strings
   - Example: "Press " + First(gParams.responselabels) + " if stimuli match"

## Response Modes

All modes support semantic labels:

- **keyboardShift**: Left/Right Shift keys
- **keyboardSafe**: Z and / keys (web-safe)
- **mousebutton**: Left/Right mouse buttons
- **mousetarget**: Clickable target zones
- **touchtarget**: Touch-screen targets
- **arrow**: Arrow keys (←/→)
- **number**: Number keys (1/2)
- **spacebar**: Single spacebar response

## Migration Guide

### Old Style (physical labels)
```pebl
## Old: Task had to handle each mode separately
if(responsemode == "keyboard")
{
    instructText <- "Press LEFT-SHIFT or RIGHT-SHIFT"
}
elseif(responsemode == "mousebutton")
{
    instructText <- "Click LEFT or RIGHT button"
}
```

### New Style (semantic labels)
```pebl
## New: Declare once, works everywhere
parpairs <- [
    ["responsemode", "auto"],
    ["responselabels", ["SAME", "DIFFERENT"]]
]

## System handles formatting automatically
## Instruction can reference semantic meaning:
instructText <- "Indicate if stimuli are " +
                First(gParams.responselabels) + " or " +
                Second(gParams.responselabels)
```

## Implementation Details

### Core Functions

**`CreateResponseLabels(layout, fontSize)`**
- Gets semantic labels from `layout.params.responselabels`
- Calls `FormatLabelsForMode()` to format for current mode
- Creates label objects and sets footer text

**`FormatLabelsForMode(mode, semanticLabels)`**
- Formats semantic labels based on `mode.type`
- Returns display label strings

**`FormatKeyName(key)`**
- Converts key codes to display names
- Example: `<lshift>` → "LEFT-SHIFT"

**`GetButtonName(buttonNum)`**
- Converts button numbers to names
- Example: 1 → "LEFT-CLICK", 3 → "RIGHT-CLICK"

**`JoinStrings(list, separator)`**
- Joins semantic labels for footer text
- Example: ["SAME", "DIFFERENT"] → "SAME | DIFFERENT"

### Files Modified

- `pebl-lib/Layout.pbl` - Core implementation
- `emscripten/pebl-lib/Layout.pbl` - Web version (identical)

## Testing

Test script: `test-semantic-labels.pbl`

```bash
bin/pebl2 test-semantic-labels.pbl -v subnum=999
```

Expected output:
```
=== TEST 1: With responselabels parameter ===
Response mode type: keyboard
Footer text: SAME | DIFFERENT
Response labels:
  - LEFT-SHIFT (SAME)
  - RIGHT-SHIFT (DIFFERENT)

=== TEST 2: Mousebutton mode with semantic labels ===
Response mode type: mousebutton
Footer text: YES | NO
Response labels:
  - LEFT-CLICK (YES)
  - RIGHT-CLICK (NO)

=== TEST 3: Mousetarget mode with semantic labels ===
Response mode type: mousetarget
Footer text: SAME | DIFFERENT
Response labels:
  - SAME
  - DIFFERENT
```

## Real-World Example: Luckvogel Task

The luckvogel (visual change detection) task uses semantic labels:

```pebl
parpairs <- [
    ["responsemode", "auto"],
    ["responselabels", ["SAME", "DIFFERENT"]]
]
```

Works correctly with:
- **Keyboard**: "LEFT-SHIFT (SAME)" | "RIGHT-SHIFT (DIFFERENT)"
- **Mouse buttons**: "LEFT-CLICK (SAME)" | "RIGHT-CLICK (DIFFERENT)"
- **Touch targets**: "SAME" | "DIFFERENT"

Response handling:
```pebl
resp <- WaitForLayoutResponse(gLayout, 10000)

if(change)
{
    corr <- resp == "right"  ## "right" = DIFFERENT
}
else
{
    corr <- resp == "left"   ## "left" = SAME
}
```

## Benefits

1. **Clarity**: Users see both physical input AND semantic meaning
2. **Flexibility**: Single declaration works across all input methods
3. **Maintainability**: No mode-specific code in tasks
4. **Accessibility**: Consistent labeling improves usability
5. **Backward Compatible**: Tasks without `responselabels` use defaults

## See Also

- `doc/LAYOUT_SEMANTIC_LABELS.md` - Original design document
- `doc/LAYOUT_RESPONSE_SYSTEM_PLAN.md` - Full Layout & Response System
- `test-semantic-labels.pbl` - Comprehensive test suite
- `test-luckvogel-modes.pbl` - Real-world task test
