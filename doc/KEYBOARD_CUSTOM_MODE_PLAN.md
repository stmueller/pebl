# Keyboard Custom Mode - User-Selected Response Keys

## Problem Statement

The current `keyboardSafe` mode uses hardcoded keys (Z and /) which are problematic for:
- **Non-US keyboard layouts** (AZERTY, QWERTZ, Dvorak, etc.)
- **Non-Latin keyboards** (Arabic, Hebrew, Cyrillic, etc.)
- **Accessibility needs** (one-handed users, motor limitations)

While scancodes would provide physical-position-based keys (long-term solution), a more immediate and flexible solution is to let participants choose their own response keys.

## Proposed Solution: `keyboardCustom` Response Mode

Allow participants to select their preferred response keys at experiment start. This approach:
- ✅ Works on ANY keyboard layout
- ✅ Accommodates accessibility needs
- ✅ Requires no C++ changes (pure PEBL library)
- ✅ Gives experimenters control over when to use it
- ✅ Empowers participants with personalized setup

---

## Implementation Design

### 1. Key Selection Prompt Function

Create a PEBL library function that prompts participants to choose keys:

```pebl
define PromptForCustomKeys(win, numKeys, labels:[])
{
    ## Prompts participant to choose response keys
    ##
    ## Args:
    ##   win: Window object
    ##   numKeys: Number of keys to choose (2, 3, 4, etc.)
    ##   labels: Optional semantic labels (["LEFT", "RIGHT"], ["ODD", "EVEN"], etc.)
    ##           If not provided, uses generic labels
    ##
    ## Returns:
    ##   List of chosen keys (e.g., ["a", "l"])

    ## Default labels if none provided
    if(Length(labels) == 0)
    {
        defaultLabels <- ["LEFT", "RIGHT", "MIDDLE", "UP", "DOWN", "OPTION 1",
                         "OPTION 2", "OPTION 3", "OPTION 4", "OPTION 5"]
        labels <- SubList(defaultLabels, 1, numKeys)
    }

    chosenKeys <- []

    ## Create instruction label
    instructions <- EasyLabel("", gVideoWidth/2, gVideoHeight/2, win, 32)
    feedback <- EasyLabel("", gVideoWidth/2, gVideoHeight/2 + 100, win, 24)

    loop(i, numKeys)
    {
        instructions.text <- "Press the key you want to use for " +
                            Nth(labels, i) + " responses"
        feedback.text <- ""
        Draw()

        key <- WaitForAnyKeyPress()

        ## Validate: not already chosen
        if(IsMember(key, chosenKeys))
        {
            feedback.text <- "That key is already in use! Press a different key."
            Draw()
            Wait(2000)
            i <- i - 1  ## Retry this position
        }
        ## Validate: not a problematic system key
        elseif(not IsValidResponseKey(key))
        {
            feedback.text <- "That key cannot be used. Please choose a different key."
            Draw()
            Wait(2000)
            i <- i - 1  ## Retry
        }
        else
        {
            PushOnEnd(chosenKeys, key)
            feedback.text <- "Key recorded: " + key
            Draw()
            Wait(500)
        }
    }

    Hide(instructions)
    Hide(feedback)
    Draw()

    return chosenKeys
}


define IsValidResponseKey(key)
{
    ## Validates that a key is suitable for use as a response key
    ## Rejects system keys that might interfere with browser/OS
    ##
    ## Args:
    ##   key: Key string to validate
    ##
    ## Returns:
    ##   1 if valid, 0 if invalid

    ## Blacklist of problematic keys
    blacklist <- ["<esc>", "<escape>",
                  "<f1>", "<f2>", "<f3>", "<f4>", "<f5>",
                  "<f6>", "<f7>", "<f8>", "<f9>", "<f10>", "<f11>", "<f12>",
                  "<lalt>", "<ralt>",
                  "<lctrl>", "<rctrl>",
                  "<tab>",
                  "<lshift>", "<rshift>",  ## Allow regular keys, but not modifiers alone
                  "<capslock>", "<numlock>", "<scrolllock>"]

    if(IsMember(key, blacklist))
    {
        return 0
    }

    return 1
}
```

### 2. Integration with Layout System

Modify `pebl-lib/Layout.pbl` to support the custom mode:

**In `LoadResponseModes()` or `GetDefaultResponseModes()`:**

```pebl
## keyboardCustom mode - participant chooses keys
modes.keyboardCustom <- MakeCustomObject("Mode")
modes.keyboardCustom.type <- "keyboard"
modes.keyboardCustom.needsSetup <- 1  ## Flag indicating setup required
modes.keyboardCustom.keys <- []  ## Will be filled during setup
modes.keyboardCustom.labels <- []  ## Will be filled during setup
modes.keyboardCustom.semantic <- ["left", "right"]  ## Default for two-choice
```

**In `CreateLayout()`:**

```pebl
define CreateLayout(testName, win, params:0)
{
    ## ... existing code ...

    ## Determine response mode
    responseMode <- DetermineResponseMode(params, config, modes)

    ## Check if mode needs custom setup
    if(PropertyExists(responseMode, "needsSetup"))
    {
        if(responseMode.needsSetup)
        {
            ## Determine number of choices and labels
            numChoices <- 2  ## Default
            labels <- []

            if(params != 0)
            {
                if(PropertyExists(params, "numchoices"))
                {
                    numChoices <- params.numchoices
                }
                if(PropertyExists(params, "customlabels"))
                {
                    labels <- params.customlabels
                }
            }

            ## Prompt participant to choose keys
            chosenKeys <- PromptForCustomKeys(win, numChoices, labels)

            ## Update response mode with chosen keys
            responseMode.keys <- chosenKeys
            responseMode.labels <- chosenKeys  ## Show actual keys

            ## Update semantic values if needed
            if(numChoices != Length(responseMode.semantic))
            {
                ## Generate generic semantic values
                responseMode.semantic <- []
                loop(i, numChoices)
                {
                    PushOnEnd(responseMode.semantic, "choice" + i)
                }
            }
        }
    }

    ## ... rest of existing code ...
}
```

---

## Usage Examples

### Example 1: Basic Two-Choice Task

```pebl
## In parameter file:
{
  "responsemode": "keyboardCustom"
}

## Or in code:
gParams.responsemode <- "keyboardCustom"
gLayout <- CreateLayout("stroop", gWin, gParams)

## Participant will be prompted:
##   "Press the key you want to use for LEFT responses"  → they press 'A'
##   "Press the key you want to use for RIGHT responses" → they press 'L'
##
## Footer will show: "A                                    L"
```

### Example 2: Three-Choice Task with Custom Labels

```pebl
gParams.responsemode <- "keyboardCustom"
gParams.numchoices <- 3
gParams.customlabels <- ["ODD", "EVEN", "NEITHER"]
gLayout <- CreateLayout("oddeven", gWin, gParams)

## Participant prompted for three keys with semantic labels:
##   "Press the key you want to use for ODD responses"
##   "Press the key you want to use for EVEN responses"
##   "Press the key you want to use for NEITHER responses"
```

### Example 3: Four-Choice Spatial Task

```pebl
gParams.responsemode <- "keyboardCustom"
gParams.numchoices <- 4
gParams.customlabels <- ["UP", "DOWN", "LEFT", "RIGHT"]
gLayout <- CreateLayout("spatialresponse", gWin, gParams)
```

---

## Testing Checklist

### Functionality Tests
- [ ] Participant can choose 2 keys successfully
- [ ] Participant can choose 3+ keys successfully
- [ ] Duplicate key rejection works
- [ ] System key blacklist works (ESC, F1, etc. rejected)
- [ ] Chosen keys work correctly during experiment
- [ ] Footer displays chosen keys correctly
- [ ] Response labels show chosen keys

### Accessibility Tests
- [ ] Works for one-handed users (keys on same side of keyboard)
- [ ] Works for left-handed users (reversed positions)
- [ ] Works for users with limited reach (adjacent keys)

### International Keyboard Tests
- [ ] Works on AZERTY layout (French)
- [ ] Works on QWERTZ layout (German)
- [ ] Works on Dvorak layout
- [ ] Works on non-Latin keyboards (Arabic, Hebrew, Cyrillic with Latin mode)

### Edge Cases
- [ ] What happens if participant presses same key twice? (Should reject and re-prompt)
- [ ] What happens if participant presses ESC during setup? (Should reject and re-prompt)
- [ ] What happens if participant chooses very unusual keys? (Should work fine)

---

## Future Enhancements

### 1. Save/Load Key Preferences
Allow participants to save their key choices for future sessions:

```pebl
## Save chosen keys
SaveCustomKeys(gSubNum, gLayout.responseMode.keys)

## Load on next session
savedKeys <- LoadCustomKeys(gSubNum)
if(Length(savedKeys) > 0)
{
    gParams.customkeys <- savedKeys
    gParams.responsemode <- "keyboard"  ## Use saved keys directly
}
```

### 2. Visual Key Display
Show visual keyboard layout during selection to help participants understand positions.

### 3. Practice Trial
Include a brief practice trial immediately after key selection to confirm choices work well.

### 4. Reconfiguration Option
Allow participants to change their key choices mid-experiment if needed.

---

## Implementation Priority

1. **Phase 1** (Immediate): Basic two-choice custom key selection
2. **Phase 2** (Soon): Multi-choice support (3, 4, 5 choices)
3. **Phase 3** (Later): Save/load preferences, practice trials
4. **Phase 4** (Future): Visual keyboard display, reconfiguration

---

## Long-Term: Scancode Support

While `keyboardCustom` solves the immediate problem, true keyboard layout independence requires scancode support:

### What Scancodes Provide:
- **Physical position** independence from layout
- `SDL_SCANCODE_Z` = bottom-left key on ANY keyboard
- QWERTY: produces "z"
- AZERTY: produces "w"
- Dvorak: produces ";"
- **Same physical key**, different characters

### Required C++ Changes:
1. Expose `scancode` field from `PEBL_KeyboardEvent` struct (already captured, line 303 in PEvent.cpp)
2. Create `TranslateScancode()` function alongside `TranslateKeycode()` in PEBLUtility.cpp
3. Add scancode-to-string mapping (e.g., `SDL_SCANCODE_Z` → `"<scan_z>"`)
4. Allow PEBL scripts to specify scancode-based keys

### PEBL API Changes:
```pebl
## New function to wait for scancode instead of keycode
WaitForScancode()

## Response mode using scancodes
modes.keyboardScancode <- MakeCustomObject("Mode")
modes.keyboardScancode.type <- "keyboard"
modes.keyboardScancode.scancodes <- [SDL_SCANCODE_Z, SDL_SCANCODE_SLASH]
modes.keyboardScancode.labels <- ["Bottom-Left", "Bottom-Right"]
modes.keyboardScancode.semantic <- ["left", "right"]
```

---

## Conclusion

The `keyboardCustom` mode provides an **immediate, practical solution** for international keyboard support and accessibility, requiring only PEBL library changes. Scancode support remains a **long-term enhancement** for true layout independence without user configuration.

**Recommendation:** Implement `keyboardCustom` now, add to documentation as recommended mode for international/accessible experiments. Plan scancode support for a future release.
