# Layout & Response System: Semantic-First Label Architecture

## Problem Statement

The current system treats labels as **physical** (describing the input method) when they should be **semantic** (describing the response meaning).

**Current (wrong) assumption**: Labels describe the physical input
- "LEFT-SHIFT" - what key to press
- "LEFT-CLICK" - what button to click
- "Click for LEFT" - what to click on

**Reality**: Labels should describe the response **meaning**
- "RED" or "BLUE" (simon task)
- "SAME" or "DIFFERENT" (luckvogel, dotjudgment)
- "LEFT" or "RIGHT" (spatial tasks like manikin)

**Core Issue**: The system has the architecture backwards. Semantic meaning should be PRIMARY, with physical input method being the MODIFIER.

Current architecture (WRONG):
```
Physical Input (keys/buttons) → Labels
                               → Semantic mapping
```

Should be (RIGHT):
```
Semantic Meaning → Labels (modified by input method)
                  → Physical Input (keys/buttons)
```

## Current Workaround (Problematic)

Tasks like Simon manually modify labels after creation:

```pebl
gLayout <- CreateLayout("simon", gWin, gParams)
llab <- First(gLayout.responseLabels)
rlab <- Second(gLayout.responseLabels)

if(gLayout.responseMode.type == "mousetarget")
{
   llab.text <- "click for " + gParams.leftcolor
   rlab.text <- "click for " + gParams.rightcolor
} else {
   llab.text <- llab.text + " (" + gParams.leftcolor + ")"  ## FRAGILE!
   rlab.text <- rlab.text + " (" + gParams.rightcolor + ")"
}
```

**Problems**:
1. String concatenation assumes label format
2. Breaks if default label changes
3. Requires mode-specific conditionals
4. Not translatable (hardcoded English strings)
5. Must be repeated in every task with custom semantics

## Proposed Solution: Semantic-First Labels

### New Architecture: Semantic Labels are Primary

**Every task defines its response semantics FIRST**, then the system adapts labels to the input method.

```pebl
## luckvogel task - declares semantic meaning
parpairs <- [
   ["responsemode", "auto"],
   ["responselabels", ["SAME", "DIFFERENT"]]  ## What the responses MEAN
]
gParams <- CreateParameters(parpairs, gParamFile)
gLayout <- CreateLayout("luckvogel", gWin, gParams)
```

The system then automatically adapts these semantic labels to the input method:

**Keyboard modes**: Show key + semantic
- `Z (SAME)` | `/ (DIFFERENT)`
- `LEFT-SHIFT (SAME)` | `RIGHT-SHIFT (DIFFERENT)`

**Mouse button mode**: Show button + semantic
- `LEFT-CLICK (SAME)` | `RIGHT-CLICK (DIFFERENT)`

**Mouse target mode**: Show semantic only (targets are self-evident)
- `SAME` | `DIFFERENT`

**Footer**: Always show pure semantic
- `SAME | DIFFERENT`

### Key Insight: Three Display Contexts

Labels appear in THREE contexts, each with different needs:

1. **Response zone labels** (bottom of screen)
   - Keyboard: Need to show both key AND semantic
   - Mousebutton: Need to show both button AND semantic
   - Mousetarget: Show semantic only (targets show themselves)

2. **Footer** (informational reminder)
   - Always show pure semantic meaning
   - Hidden for mousetarget (redundant with visible targets)

3. **Instruction text** (how to respond)
   - Mode-specific: "Press Z or /" vs "Click left or right"
   - Uses translation strings, not labels

### Implementation: Label Formatting Rules

```pebl
## In CreateResponseLabels() - apply formatting based on mode type

semanticLabels <- params.responselabels  ## ["SAME", "DIFFERENT"]

if(mode.type == "keyboard")
{
    ## Format: "KEY (semantic)"
    displayLabels <- []
    loop(i, Length(mode.keys))
    {
        key <- Uppercase(Nth(mode.keys, i))
        semantic <- Nth(semanticLabels, i)
        PushOnEnd(displayLabels, key + " (" + semantic + ")")
    }
}
elseif(mode.type == "mousebutton")
{
    ## Format: "BUTTON (semantic)"
    buttonNames <- ["LEFT-CLICK", "MIDDLE-CLICK", "RIGHT-CLICK"]
    displayLabels <- []
    loop(i, Length(mode.buttons))
    {
        buttonNum <- Nth(mode.buttons, i)
        buttonName <- Nth(buttonNames, buttonNum)
        semantic <- Nth(semanticLabels, i)
        PushOnEnd(displayLabels, buttonName + " (" + semantic + ")")
    }
}
elseif(mode.type == "mousetarget")
{
    ## Format: Just semantic (targets are self-evident)
    displayLabels <- semanticLabels
}

## Footer always shows pure semantic
layout.footer.text <- Nth(semanticLabels, 1) + " | " + Nth(semanticLabels, 2)
```

### Usage Examples

#### Luckvogel (Same/Different)

```pebl
parpairs <- [["responselabels", ["SAME", "DIFFERENT"]]]
gParams <- CreateParameters(parpairs, gParamFile)
gLayout <- CreateLayout("luckvogel", gWin, gParams)

## Results (automatic):
## Keyboard: "Z (SAME)" | "/ (DIFFERENT)"
## Mousetarget: "SAME" | "DIFFERENT"
## Footer: "SAME | DIFFERENT"
```

#### Simon (Colors with Translation)

```pebl
parpairs <- [
   ["leftcolor", "red"],
   ["rightcolor", "blue"],
   ["responselabels", [Uppercase(gParams.leftcolor), Uppercase(gParams.rightcolor)]]
]
gParams <- CreateParameters(parpairs, gParamFile)
gLayout <- CreateLayout("simon", gWin, gParams)

## Results (automatic):
## Keyboard: "Z (RED)" | "/ (BLUE)"
## Mousetarget: "RED" | "BLUE"
## Footer: "RED | BLUE"
```

#### Dotjudgment (More/Fewer)

```pebl
parpairs <- [["responselabels", [gStrings.fewer, gStrings.more]]]
gParams <- CreateParameters(parpairs, gParamFile)
gLayout <- CreateLayout("dotjudgment", gWin, gParams)

## Results (translatable):
## English: "Z (fewer)" | "/ (more)"
## Spanish: "Z (menos)" | "/ (más)"
```

#### Manikin (Spatial - Default)

```pebl
## No responselabels parameter = use semantic values as labels
parpairs <- [["responsemode", "auto"]]
gParams <- CreateParameters(parpairs, gParamFile)
gLayout <- CreateLayout("manikin", gWin, gParams)

## The mode's semantic values ["left", "right"] become the labels
## Results:
## Keyboard: "Z (left)" | "/ (right)"
## Mousetarget: "left" | "right"
## Footer: "left | right"
```

### Default Behavior (Backward Compatible)

If `responselabels` is NOT provided:
1. Use the mode's semantic values as labels
2. Capitalize them: ["left", "right"] → ["LEFT", "RIGHT"]
3. Apply same formatting rules

This means **existing tasks continue to work unchanged**.

### Implementation Changes in Layout.pbl

#### 1. Modify CreateResponseLabels() to use semantic-first approach

```pebl
define CreateResponseLabels(layout, fontSize)
{
    mode <- layout.responseMode
    win <- layout.win
    fgcol <- layout.fgcol

    ## Get semantic labels from params, or default to mode semantic values
    semanticLabels <- 0
    if(PropertyExists(layout.params, "responselabels"))
    {
        semanticLabels <- layout.params.responselabels
    } else {
        ## Default: capitalize semantic values
        semanticLabels <- []
        loop(sem, mode.semantic)
        {
            PushOnEnd(semanticLabels, Uppercase(sem))
        }
    }

    ## Format labels based on mode type
    displayLabels <- FormatLabelsForMode(mode, semanticLabels)

    ## Create labels and borders...
    ## (rest of existing code)

    ## Set footer to pure semantic
    layout.footer.text <- Join(semanticLabels, " | ")
}

define FormatLabelsForMode(mode, semanticLabels)
{
    displayLabels <- []

    if(mode.type == "keyboard")
    {
        ## Format: "KEY (semantic)"
        loop(i, Length(mode.keys))
        {
            key <- FormatKeyName(Nth(mode.keys, i))
            semantic <- Nth(semanticLabels, i)
            PushOnEnd(displayLabels, key + " (" + semantic + ")")
        }
    }
    elseif(mode.type == "mousebutton")
    {
        ## Format: "BUTTON (semantic)"
        loop(i, Length(mode.buttons))
        {
            buttonNum <- Nth(mode.buttons, i)
            buttonName <- GetButtonName(buttonNum)
            semantic <- Nth(semanticLabels, i)
            PushOnEnd(displayLabels, buttonName + " (" + semantic + ")")
        }
    }
    elseif(mode.type == "mousetarget")
    {
        ## Format: Just semantic (target is self-evident)
        displayLabels <- semanticLabels
    }
    elseif(mode.type == "singlekey")
    {
        ## Single key modes: show semantic only
        displayLabels <- semanticLabels
    }

    return(displayLabels)
}

define FormatKeyName(key)
{
    ## Convert key codes to display names
    if(key == "<lshift>") { return("LEFT-SHIFT") }
    elseif(key == "<rshift>") { return("RIGHT-SHIFT") }
    elseif(key == "<space>") { return("SPACEBAR") }
    elseif(key == "<return>") { return("ENTER") }
    elseif(key == "<left>") { return("←") }
    elseif(key == "<right>") { return("→") }
    elseif(key == "<up>") { return("↑") }
    elseif(key == "<down>") { return("↓") }
    else { return(Uppercase(key)) }
}

define GetButtonName(buttonNum)
{
    buttonNames <- ["", "LEFT-CLICK", "MIDDLE-CLICK", "RIGHT-CLICK"]
    if(buttonNum > 0 and buttonNum <= Length(buttonNames))
    {
        return(Nth(buttonNames, buttonNum))
    }
    return("BUTTON-" + buttonNum)
}

define Join(list, separator)
{
    if(Length(list) == 0) { return("") }
    result <- First(list)
    loop(i, Sequence(2, Length(list), 1))
    {
        result <- result + separator + Nth(list, i)
    }
    return(result)
}
```

#### 2. Modify CreateLayout() to pass params to CreateResponseLabels()

```pebl
define CreateLayout(testName, win, params:0)
{
    ...
    layout.win <- win
    layout.params <- params  ## <-- Store params in layout
    ...
    ## Create response labels (now has access to params.responselabels)
    layout <- CreateResponseLabels(layout, Round(config.zones.response.fontSize * scale))
    ...
}
```

## Migration Path

### Phase 1: Implement Core System (Layout.pbl changes)

1. Add `layout.params <- params` to CreateLayout()
2. Modify CreateResponseLabels() to check for responselabels parameter
3. Implement FormatLabelsForMode(), FormatKeyName(), GetButtonName(), Join()
4. Test with existing tasks (should work unchanged with defaults)

### Phase 2: Update Tasks with Non-Spatial Semantics

Priority order (tasks that currently show wrong labels):

1. **luckvogel** - Change footer from keys to "SAME | DIFFERENT"
   ```pebl
   parpairs <- [["responselabels", ["SAME", "DIFFERENT"]], ...]
   ```

2. **dotjudgment** - Currently shows "left/right", should show dot comparison
   ```pebl
   parpairs <- [["responselabels", [gStrings.fewer, gStrings.more]], ...]
   ```

3. **simon** - Remove manual label concatenation
   ```pebl
   ## BEFORE (manual manipulation):
   llab.text <- llab.text + " (" + gParams.leftcolor + ")"

   ## AFTER (declarative):
   parpairs <- [["responselabels", [
      Uppercase(gParams.leftcolor),
      Uppercase(gParams.rightcolor)
   ]], ...]
   ```

### Phase 3: Remove Workarounds

Once system is implemented, remove fragile manual label modifications from:
- simon.pbl (lines 39-53 - manual label concatenation)
- Any other tasks doing post-creation label manipulation

### Backward Compatibility

✅ **100% backward compatible**:
- Tasks without `responselabels` use mode.semantic values (capitalized)
- Existing behavior unchanged: spatial tasks show "LEFT | RIGHT"
- No breaking changes to any existing code

## Benefits

1. **Semantic-first**: Labels describe meaning, not physical input
2. **Automatic adaptation**: One semantic definition works for all modes
3. **Translatable**: Uses gStrings for language support
4. **Maintainable**: No mode-specific conditionals in tasks
5. **Type-safe**: Labels always match semantic values
6. **Self-documenting**: `["SAME", "DIFFERENT"]` is clearer than manual formatting

## Summary: The Key Architectural Fix

**BEFORE (physical-first, broken)**:
```
Response Mode → Physical Labels → Manual Semantic Hacks
"mousetarget"   "Click LEFT"     append "(SAME)" manually
```

**AFTER (semantic-first, correct)**:
```
Semantic Labels → Response Mode → Formatted Display
["SAME", "DIFFERENT"]  "mousetarget"   "SAME | DIFFERENT"
                       "keyboard"       "Z (SAME) | / (DIFFERENT)"
```

The system now correctly models the reality: **Tasks define what responses mean, modes define how to collect them.**
