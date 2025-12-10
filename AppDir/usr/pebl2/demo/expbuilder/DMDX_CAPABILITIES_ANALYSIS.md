# DMDX Capabilities Analysis: Features to Expose in PEBL

**Date:** 2025-11-18
**Purpose:** Identify DMDX capabilities that would need to be exposed in a PEBL DMDX parser/runner
**Sources:** DMDX official documentation at psy1.psych.arizona.edu/~jforster/dmdx/help/

---

## Executive Summary

DMDX is a sophisticated Windows-based experiment presentation system with 20+ years of development. To provide meaningful DMDX compatibility in PEBL, we would need to implement a subset of its extensive feature set.

**Complexity Assessment:**
- 🟢 **Easy** - Already available in PEBL or trivial to implement
- 🟡 **Medium** - Requires moderate implementation effort
- 🔴 **Hard** - Significant implementation challenges or PEBL architectural changes needed
- ⚫ **Not Possible** - Platform limitations make implementation infeasible

**Key Insights:**

1. **Branching:** DMDX's branching logic is trivial to implement in PEBL using `while()` loops instead of `loop()`. This is a natural feature of frame-based playback, not a special architectural requirement. Simply manipulate the list index to jump between items.

2. **Expressions:** DMDX expressions can be implemented as a meta-language using PEBL's `CallFunction()`. The parser converts DMDX expression syntax (e.g., `<mwb (&counter > 5) 100>`) to PEBL function calls at parse time. No runtime expression evaluator needed - leverage PEBL's existing function system.

**Recommendation:**
- **Phase 1:** Implement core features (item presentation, timing, responses, scrambling, **branching**)
- **Phase 2:** Add enhanced features (feedback, audio, RTF formatting, positioning)
- **Phase 3:** Consider complex features (expression evaluation, digital video) only if user demand exists

---

## DMDX Architecture Overview

### Item File Format

DMDX experiments are defined in **RTF (Rich Text Format)** files with special syntax:

```
<keyword1 value> <keyword2 value> ... <keywordN value>
+001 "First frame text" / * "Second frame text" / "Third frame text";
+002 "Another" , "item" / * "with frames";
-003 "Item expecting negative response" *;
...
```

**Key Elements:**
1. **Parameter line** (first line): Global keywords controlling experiment behavior
2. **Items**: Numbered experimental trials
3. **Frames**: Within-trial display stages separated by `/` or `,`
4. **Delimiters**:
   - `/` - Sequential frames (temporal separation)
   - `,` - Simultaneous frames (same screen)
   - `;` - Item terminator
   - `*` - Clock start marker
5. **Response indicators**: `+` (positive), `-` (negative), `^` (no response), `=` (either)

---

## Category 1: Item File Parsing

### Basic Syntax (🟢 Easy)

**DMDX Features:**
- RTF file with embedded keywords
- Item numbers and response indicators (+, -, ^, =)
- Frame delimiters (/, ,, ;)
- Text in double quotes
- Clock start marker (*)

**PEBL Status:** ⚠️ **Partial**
- ✅ JSON parser implemented (dmdx.pbl)
- ❌ No RTF parser
- ❌ No native DMDX .rtf format support

**Implementation:**
```pebl
define ParseDMDXFile(filename)
{
  ## Would need RTF stripper
  rtfText <- FileReadText(filename)
  plainText <- StripRTFFormatting(rtfText)  ## NOT IMPLEMENTED

  ## Parse parameter line
  params <- ParseParameterLine(FirstLine(plainText))

  ## Parse items
  items <- []
  loop(line, SplitByItemDelimiter(plainText, ";"))
  {
    item <- ParseItem(line)
    PushOnEnd(items, item)
  }

  return [params, items]
}

define ParseItem(itemText)
{
  ## Extract response indicator (+, -, ^, =)
  respType <- First(itemText)

  ## Extract item number
  itemNum <- ExtractNumber(itemText)

  ## Split by frame delimiters (/, ,)
  frames <- SplitFrames(itemText)

  ## Identify clock start (*)
  clockFrame <- FindClockStart(frames)

  return MakeAssociativeArray("num", itemNum,
                               "response", respType,
                               "frames", frames,
                               "clockon", clockFrame)
}
```

**Effort:** 2-3 days for basic RTF parsing + item syntax

---

### Display-Control Characters (🟡 Medium)

**DMDX Features:**
- `B` - Blank frame (clear screen)
- `C` - Continue (don't clear previous frame)
- `\n` - New line
- RTF formatting (bold, italic, underline, color, font)

**PEBL Status:** ⚠️ **Partial**
- ✅ Can clear/not clear between stimuli
- ✅ Has RTL and font formatting support
- ❌ No inline RTF parsing
- ❌ No display-control character syntax

**Implementation Approach:**
- Parse RTF control words and convert to PEBL formatting
- Use PlatformFont capabilities for text styling
- Implement frame clearing logic

**Effort:** 3-4 days (requires RTF parser)

---

## Category 2: Timing System

### Frame-Based Timing (🟢 Easy)

**DMDX Features:**
- Default frame duration (typically 16.67ms for 60Hz displays)
- `<fd N>` - Frame duration keyword (e.g., `<fd 4>` = 4 frames ≈ 67ms)
- `<msfd N>` - Millisecond frame duration (e.g., `<msfd 50>`)
- Precise vertical retrace synchronization

**PEBL Status:** ✅ **Already Available**
- `Wait(ms)` for delays
- Frame-based timing via `gSleepEasy`
- Vertical sync available in SDL2

**Implementation:**
```pebl
define DisplayFrame(frame, durationMS)
{
  ## Create and display frame content
  stim <- CreateFrameStimuli(frame)
  AddObject(stim, gWin)
  Draw()

  ## Wait for specified duration
  Wait(durationMS)

  ## Clear if needed
  if(frame.clear)
  {
    RemoveObject(stim, gWin)
  }
}
```

**Effort:** <1 day (mostly parameter conversion)

---

### Response Timing (🟢 Easy)

**DMDX Features:**
- Clock starts on frame marked with `*`
- Timeout parameter (default 4000ms)
- `<timeout N>` keyword
- RT measured from clock start to response

**PEBL Status:** ✅ **Already Available**
- `GetTime()` for timestamps
- `WaitForListKeyPressWithTimeout()` for timed responses
- RT = responseTime - stimulusTime

**Implementation:**
```pebl
define CollectResponse(item, timeoutMS)
{
  ## Start clock
  clockStart <- GetTime()

  ## Wait for response
  resp <- WaitForListKeyPressWithTimeout(item.validKeys, timeoutMS)
  rt <- GetTime() - clockStart

  ## Determine correctness
  correct <- CheckResponse(resp, item.correctResponse, item.responseType)

  return [resp, rt, correct]
}
```

**Effort:** <1 day

---

### Inter-Stimulus Interval (🟢 Easy)

**DMDX Features:**
- `<delay N>` keyword for ITI
- `<d N>` shorthand
- Delay in milliseconds

**PEBL Status:** ✅ **Already Available**
- `Wait(ms)` function

**Implementation:**
```pebl
define RunItem(item, params)
{
  ## Present item...
  result <- PresentItemAndCollect(item)

  ## Inter-trial interval
  if(IsDefined(params.delay))
  {
    Wait(params.delay)
  }

  return result
}
```

**Effort:** Trivial

---

## Category 3: Display and Presentation

### Video Modes (🟡 Medium)

**DMDX Features:**
- `<vm NxMxB>` - Video mode (width x height x bits)
- `<vm desktop>` - Use desktop resolution
- Full-screen exclusive mode
- Multi-monitor support

**PEBL Status:** ⚠️ **Partial**
- ✅ `gVideoWidth`, `gVideoHeight` for window size
- ✅ Fullscreen via SDL2
- ❌ No multi-monitor control
- ❌ No bits-per-pixel control (SDL2 handles automatically)

**Implementation:**
```pebl
define SetupDisplay(params)
{
  if(params.videomode == "desktop")
  {
    ## Use desktop resolution
    gVideoWidth <- 1920  ## Would need to query actual desktop
    gVideoHeight <- 1080
  } else {
    ## Parse "640x480x8" format
    dims <- SplitString(params.videomode, "x")
    gVideoWidth <- ToNumber(First(dims))
    gVideoHeight <- ToNumber(Nth(dims, 2))
  }

  gWin <- MakeWindow(params.bgcolor)
  if(params.fullscreen)
  {
    ## PEBL doesn't have fullscreen toggle built-in
    ## Would need SDL2 call
  }
}
```

**Effort:** 1-2 days (mostly SDL2 integration)

---

### Color Control (🟢 Easy)

**DMDX Features:**
- `<dbc R,G,B>` - Display background color
- `<dwc R,G,B>` - Display writing color (foreground)
- `<bgc R,G,B>` - Background color for text
- Hex color support: `<dbc #RRGGBB>`

**PEBL Status:** ✅ **Already Available**
- `MakeColor("name")` and `MakeColor(r, g, b)`
- Window background color
- Text foreground/background colors

**Implementation:**
```pebl
define ParseColor(colorSpec)
{
  if(StartsWith(colorSpec, "#"))
  {
    ## Hex format
    return HexToColor(colorSpec)  ## Already in dmdx.pbl
  } else {
    ## R,G,B format
    parts <- SplitString(colorSpec, ",")
    return MakeColor(ToNumber(parts[1]),
                     ToNumber(parts[2]),
                     ToNumber(parts[3]))
  }
}
```

**Effort:** <1 day

---

### Text Positioning (🟡 Medium)

**DMDX Features:**
- `<XYJustification option>` - Controls text alignment
  - Options: LEFTBOTTOM, LEFTTOP, CENTER, RIGHTTOP, RIGHTBOTTOM, CENTERBOTTOM
- `<xy X,Y>` - Absolute positioning
- `<inst>` keyword for instruction formatting with detailed positioning

**PEBL Status:** ⚠️ **Partial**
- ✅ Absolute positioning via `Move(obj, x, y)`
- ✅ Text justification in PTextBox (LEFT, CENTER, RIGHT)
- ❌ No combined X/Y justification system like DMDX

**Implementation:**
```pebl
define PositionStimulus(stim, xyjust)
{
  if(xyjust == "CENTER")
  {
    x <- gVideoWidth / 2
    y <- gVideoHeight / 2
  } elseif(xyjust == "LEFTTOP")
  {
    x <- stim.width / 2
    y <- stim.height / 2
  } elseif(xyjust == "RIGHTBOTTOM")
  {
    x <- gVideoWidth - stim.width / 2
    y <- gVideoHeight - stim.height / 2
  }
  ## ... other cases

  Move(stim, x, y)
}
```

**Effort:** 1-2 days

---

### Stimulus Types (🟢 Easy to 🟡 Medium)

**DMDX Features:**

| Type | DMDX Syntax | PEBL Status | Effort |
|------|-------------|-------------|--------|
| Text | `"text string"` | ✅ `EasyLabel()` | 🟢 Trivial |
| Bitmap | `<bmp filename.bmp>` | ✅ `MakeImage()` | 🟢 Trivial |
| WAV Audio | `<wav filename.wav>` | ✅ `LoadSound()` | 🟢 Trivial |
| Digital Video | `<dv filename.avi>` | ❌ Not supported | 🔴 Hard |
| Blank Frame | `B` control char | ✅ Clear screen | 🟢 Trivial |

**Implementation:**
```pebl
define CreateStimulus(frame)
{
  if(frame.type == "text")
  {
    return EasyLabel(frame.content, x, y, gWin, fontSize)
  } elseif(frame.type == "bmp")
  {
    img <- MakeImage(frame.filename)
    Move(img, x, y)
    return img
  } elseif(frame.type == "wav")
  {
    return LoadSound(frame.filename)
  } elseif(frame.type == "blank")
  {
    ## Just clear screen, no object
    return "BLANK"
  }
}
```

**Effort:** 1-2 days (mostly format conversion)

---

## Category 4: Response Collection

### Input Devices (🟢 Easy to 🟡 Medium)

**DMDX Features:**
- `<id keyboard>` - Keyboard input (default)
- `<id mouse>` - Mouse input
- `<id touchscreen>` - Touch screen input
- `<id pio12>` - Parallel port button box
- `<id serialvox>` - Serial voice key
- `<id digitalvox>` - Digital voice onset detector

**PEBL Status:** ⚠️ **Partial**

| Input Type | PEBL Support | Effort |
|------------|--------------|--------|
| Keyboard | ✅ `WaitForKeyPress()` | 🟢 Trivial |
| Mouse | ✅ `WaitForClickOnTarget()` | 🟢 Trivial |
| Touch | ✅ Same as mouse in SDL2 | 🟢 Trivial |
| Parallel Port | ⚠️ Requires USE_PORTS=1 | 🟡 Medium |
| Voice Key | ❌ Not supported | 🔴 Hard |

**Implementation:**
```pebl
define CollectResponse(item, inputDevice, timeout)
{
  if(inputDevice == "keyboard")
  {
    return WaitForListKeyPressWithTimeout(item.validKeys, timeout)
  } elseif(inputDevice == "mouse")
  {
    targets <- item.clickableTargets
    return WaitForClickOnTarget(targets, [1])
  } elseif(inputDevice == "pio12")
  {
    ## Would require PComPort or PParallelPort
    ## NOT IMPLEMENTED
  }
}
```

**Effort:** 1 day for keyboard/mouse, 3-5 days for hardware devices

---

### Response Types (🟢 Easy to 🟡 Medium)

**DMDX Features:**

1. **Binary Response** (default)
   - Two keys (e.g., left/right shift)
   - Positive (+) or negative (-) expected

2. **Zillion Response** `<zil>`
   - Multiple keys (1 of N choice)
   - Any key can be correct

3. **Zillion One Response** `<zor>`
   - Collect single keypress, any key valid

4. **Zillion Typed Response** `<ztr>`
   - Collect typed text input
   - Like PEBL's `GetInput()`

5. **Mouse Point-and-Click** `<mpr>`
   - Click on target regions

6. **Continuous Rating** `<mnr>`
   - Mouse position as continuous response

**PEBL Status:** ⚠️ **Partial**

| Type | PEBL Equivalent | Effort |
|------|-----------------|--------|
| Binary | `WaitForListKeyPress([key1, key2])` | 🟢 Trivial |
| Zillion | `WaitForListKeyPress(keyList)` | 🟢 Trivial |
| Zillion One | `WaitForKeyPress()` | 🟢 Trivial |
| Typed | `GetInput()` | 🟢 Trivial |
| Mouse Click | `WaitForClickOnTarget()` | 🟢 Easy |
| Continuous Rating | Track mouse position | 🟡 Medium |

**Implementation:**
```pebl
define GetResponseByType(responseType, params, timeout)
{
  if(responseType == "binary")
  {
    return WaitForListKeyPressWithTimeout(["+", "-"], timeout)
  } elseif(responseType == "zillion")
  {
    return WaitForListKeyPressWithTimeout(params.validKeys, timeout)
  } elseif(responseType == "typed")
  {
    ## DMDX allows timed text entry, PEBL GetInput() doesn't have timeout
    return GetInput(gWin, "Enter response:", params.maxChars)
  } elseif(responseType == "continuous")
  {
    ## Track mouse position over time
    ## NOT FULLY IMPLEMENTED
  }
}
```

**Effort:** 1-2 days

---

## Category 5: Experimental Design

### Scrambling/Randomization (🟢 Easy to 🟡 Medium)

**DMDX Features:**
- `<scramble N>` - Block size for randomization
- `<grouping N>` - Keep N items together
- `<seed N>` - Random seed for reproducibility
- `$` delimiter - Prevent scrambling of sections
- `\` delimiter - Scramble segments separately but maintain relative order
- Multiple scrambling passes

**PEBL Status:** ✅ **Already Available**
- `Shuffle()` - Randomize list
- `SeedRNG()` - Set random seed
- `DesignFullCounterbalance()` - Factorial designs
- `SampleNWithReplacement()` - Sampling

**Implementation:**
```pebl
define ScrambleItems(items, blockSize, grouping, seed)
{
  ## Set random seed if specified
  if(IsDefined(seed))
  {
    SeedRNG(seed)
  }

  ## Group items if needed
  if(grouping > 1)
  {
    items <- GroupItems(items, grouping)
  }

  ## Scramble in blocks
  if(blockSize > 0)
  {
    scrambled <- []
    blocks <- ChunkList(items, blockSize)
    loop(block, blocks)
    {
      shuffled <- Shuffle(block)
      scrambled <- Merge(scrambled, shuffled)
    }
    return scrambled
  } else {
    ## No scrambling
    return items
  }
}

define GroupItems(items, groupSize)
{
  ## Keep groupSize items together as single unit
  grouped <- []
  i <- 1
  while(i <= Length(items))
  {
    group <- SubList(items, i, Min(i + groupSize - 1, Length(items)))
    PushOnEnd(grouped, group)
    i <- i + groupSize
  }
  return grouped
}
```

**Effort:** 1-2 days for basic scrambling, 3-4 days for advanced features

---

### Branching Logic (🟢 Easy)

**DMDX Features:**

1. **Unconditional Branch** `<bu N>` or `<branch N>`
   - Jump to item number N

2. **Conditional Branches**
   - `<bic N>` - Branch if correct
   - `<biw N>` - Branch if wrong
   - `<binr N>` - Branch if no response

3. **Call/Return** `<call N>` and `<return>`
   - Subroutine-like behavior

4. **Multi-Way Branch** `<mwb>`
   - Branch based on expression evaluation (requires expression evaluator)

**PEBL Status:** ✅ **Straightforward to Implement**
- DMDX branching is a feature of frame-based playback
- Simply use `while()` loop with index instead of `loop()`
- Jump to arbitrary items by manipulating index
- No architectural changes needed

**Implementation:**
```pebl
define RunExperiment(items)
{
  ## Frame-based playback with branching support
  i <- 1
  callStack <- []  ## For <call>/<return> support

  while(i <= Length(items))
  {
    item <- items[i]
    result <- RunItem(item)

    ## Check for branch commands
    if(IsDefined(item.branch))
    {
      if(item.branch.type == "unconditional")
      {
        ## <bu N> - unconditional branch
        i <- FindItemByNumber(items, item.branch.target)
      }
      elseif(item.branch.type == "bic" and result.correct)
      {
        ## <bic N> - branch if correct
        i <- FindItemByNumber(items, item.branch.target)
      }
      elseif(item.branch.type == "biw" and not result.correct)
      {
        ## <biw N> - branch if wrong
        i <- FindItemByNumber(items, item.branch.target)
      }
      elseif(item.branch.type == "binr" and result.response == "")
      {
        ## <binr N> - branch if no response
        i <- FindItemByNumber(items, item.branch.target)
      }
      elseif(item.branch.type == "call")
      {
        ## <call N> - push return address and branch
        PushOnEnd(callStack, i + 1)
        i <- FindItemByNumber(items, item.branch.target)
      }
      elseif(item.branch.type == "return")
      {
        ## <return> - pop return address
        if(Length(callStack) > 0)
        {
          i <- PopOffEnd(callStack)
        } else {
          i <- i + 1  ## No return address, just continue
        }
      }
      else {
        ## No branch taken, continue to next item
        i <- i + 1
      }
    } else {
      ## No branch, continue sequentially
      i <- i + 1
    }
  }
}

define FindItemByNumber(items, itemNumber)
{
  ## Find list index of item with given number
  index <- 1
  loop(item, items)
  {
    if(item.num == itemNumber)
    {
      return index
    }
    index <- index + 1
  }

  ## Item not found, continue to next
  return index
}
```

**Advantages:**
- Natural implementation in PEBL
- while() loops are standard PEBL
- List indexing is straightforward
- Call stack for subroutines is just a list
- No special architecture needed

**Multi-Way Branch Note:**
Multi-way branch `<mwb>` with expressions would still require an expression evaluator (see Counters section), but basic conditional branching is trivial.

**Effort:** 1-2 days for basic branching (bu, bic, biw, binr, call, return)

---

### Counters and Expressions (🟡 Medium)

**DMDX Features:**
- `<Counter Name Value>` - Define counter variable
- `<inc Name>` - Increment counter
- `<dec Name>` - Decrement counter
- `<set Name Value>` - Set counter value
- Expressions in branches: `<mwb (&Counter > 5) 100>`
- Persistent counters across sessions

**PEBL Status:** ✅ **Can Implement via Meta-Language**
- ✅ Variables/counters are just PEBL global variables
- ✅ Expressions can be PEBL functions called via `CallFunction()`
- ✅ No expression parser needed - use PEBL's existing function system

**Implementation via Meta-Language:**

Instead of parsing DMDX expressions at runtime, convert them to PEBL function calls during item file parsing:

```pebl
## Global counter storage
gCounters <- AssociativeArray()

## Define counter operations
define DMDXCounter(name, value)
{
  gCounters[name] <- value
}

define DMDXInc(name)
{
  gCounters[name] <- gCounters[name] + 1
}

define DMDXDec(name)
{
  gCounters[name] <- gCounters[name] - 1
}

define DMDXSet(name, value)
{
  gCounters[name] <- value
}

## Define expression evaluators as PEBL functions
define DMDXExpr_CounterGreaterThan(counterName, value)
{
  return gCounters[counterName] > value
}

define DMDXExpr_CounterLessThan(counterName, value)
{
  return gCounters[counterName] < value
}

define DMDXExpr_CounterEquals(counterName, value)
{
  return gCounters[counterName] == value
}

## Complex expressions can be composed
define DMDXExpr_AndCondition(func1, args1, func2, args2)
{
  result1 <- CallFunction(func1, args1)
  result2 <- CallFunction(func2, args2)
  return result1 and result2
}
```

**Parser converts DMDX expressions to PEBL function calls:**

```pebl
## DMDX: <Counter TrialNum 0>
## Becomes: DMDXCounter("TrialNum", 0)

## DMDX: <inc TrialNum>
## Becomes: DMDXInc("TrialNum")

## DMDX: <mwb (&TrialNum > 5) 100>
## Becomes:
item.multiwayBranch <- [
  ["DMDXExpr_CounterGreaterThan", ["TrialNum", 5], 100]
]

## In runner:
if(IsDefined(item.multiwayBranch))
{
  loop(branch, item.multiwayBranch)
  {
    funcName <- branch[1]
    funcArgs <- branch[2]
    targetItem <- branch[3]

    result <- CallFunction(funcName, funcArgs)
    if(result)
    {
      i <- FindItemByNumber(items, targetItem)
      break
    }
  }
}
```

**For complex expressions:**

```pebl
## DMDX: <mwb (&TrialNum > 5) and (&ErrorCount < 3) 100>
## Parser creates:
item.multiwayBranch <- [
  ["DMDXExpr_AndCondition",
   ["DMDXExpr_CounterGreaterThan", ["TrialNum", 5],
    "DMDXExpr_CounterLessThan", ["ErrorCount", 3]],
   100]
]
```

**Advantages of Meta-Language Approach:**
- ✅ No expression parser needed - leverage PEBL's existing function system
- ✅ `CallFunction()` already exists in PEBL
- ✅ Can support arbitrarily complex expressions by defining more PEBL functions
- ✅ Type-safe - PEBL handles type checking
- ✅ Extensible - users can add custom expression functions
- ✅ Maintainable - expression logic is readable PEBL code, not string parsing

**Disadvantages:**
- Parser must convert DMDX expression syntax to PEBL function calls
- More complex expressions require recursive parsing
- Not a direct 1:1 runtime interpretation

**Effort:** 3-5 days for basic counters and simple expressions, 1-2 weeks for complex nested expressions

---

## Category 6: Feedback

### Feedback Types (🟢 Easy)

**DMDX Features:**
- `<nfb>` - No feedback
- `<clfb>` - Clear feedback (default)
- `<fbocb>` - Feedback only clears behind (doesn't clear stimulus)
- `<cfbo>` - Correct feedback only
- `<wfbo>` - Wrong feedback only
- `<cfb "text">` - Custom correct feedback
- `<wfb "text">` - Custom wrong feedback
- `<tlfb>` - Too late feedback
- `<nfbt N>` - No feedback time (duration)

**PEBL Status:** ⚠️ **Partial**
- ✅ Can display feedback text
- ✅ Can control timing with `Wait()`
- ❌ No built-in feedback system

**Implementation:**
```pebl
define ShowFeedback(correct, response, feedbackParams)
{
  ## Determine feedback type
  if(feedbackParams.type == "none")
  {
    return  ## No feedback
  }

  ## Select feedback message
  if(correct and feedbackParams.correctOnly)
  {
    msg <- feedbackParams.correctText
  } elseif(not correct and feedbackParams.wrongOnly)
  {
    msg <- feedbackParams.wrongText
  } elseif(response == "" and feedbackParams.tooLate)
  {
    msg <- feedbackParams.tooLateText
  } else {
    return  ## No feedback for this condition
  }

  ## Display feedback
  fb <- EasyLabel(msg, gVideoWidth/2, gVideoHeight/2, gWin, 24)
  AddObject(fb, gWin)
  Draw()

  ## Wait for feedback duration
  Wait(feedbackParams.duration)

  ## Clear feedback (or not, depending on fbocb)
  if(feedbackParams.clearFeedback)
  {
    RemoveObject(fb, gWin)
    Draw()
  }
}
```

**Effort:** 1-2 days

---

## Category 7: Advanced Features

### Remote Testing (⚫ Not Possible in Native PEBL)

**DMDX Features:**
- `<RemoteTesting>` keyword
- Network-based experiment distribution
- Remote data collection
- Web-based participant access

**PEBL Status:** ⚠️ **Emscripten Only**
- ✅ PEBL can run in browser via Emscripten
- ✅ Can upload data via HTTP
- ❌ No native "remote testing" framework
- ✅ PEBL Online Platform provides web-based testing

**Assessment:**
- DMDX remote testing is Windows-specific
- PEBL's Emscripten build provides similar capability
- Not a direct translation but same end goal

**Effort:** N/A (use existing PEBL Online Platform)

---

### Digital Video (🔴 Hard)

**DMDX Features:**
- `<dv filename.avi>` - Display video files
- Frame-accurate video presentation
- Video synchronization with other stimuli

**PEBL Status:** ❌ **Not Supported**
- Old PEBL_MOVIES code (WAAVE library) is non-functional
- No SDL2 video support currently

**Implementation:**
Would require integrating video library like:
- SDL2_video (not a standard library)
- FFmpeg (large dependency)
- GStreamer (complex)

**Effort:** 2-3 weeks (major undertaking)

---

### Eye Tracking Integration (🔴 Hard)

**DMDX Features:**
- Eye tracker support
- Gaze-contingent displays
- Eye movement recording

**PEBL Status:** ❌ **Not Supported**
- No eye tracker integration

**Effort:** 3-4 weeks (requires hardware driver integration)

---

### fMRI Scanner Integration (🔴 Hard)

**DMDX Features:**
- Scanner trigger detection
- Precise timing synchronization with scanner
- TR-locked stimulus presentation

**PEBL Status:** ⚠️ **Limited**
- Can detect keyboard input (scanner triggers often mapped to keyboard)
- No specialized scanner integration
- Serial/parallel port support via USE_PORTS (could read TTL triggers)

**Effort:** 1-2 weeks if scanner triggers via keyboard/serial, otherwise 3-4 weeks

---

## Priority Feature Matrix

### Must-Have Features (Phase 1)

| Feature | Complexity | Effort | Impact |
|---------|-----------|--------|--------|
| RTF item file parsing | 🟡 Medium | 2-3 days | Critical |
| Basic item syntax (+, -, /, ,, ;, *) | 🟢 Easy | 2 days | Critical |
| Frame-based timing | 🟢 Easy | 1 day | Critical |
| Text/image stimulus display | 🟢 Easy | 1 day | Critical |
| Keyboard response collection | 🟢 Easy | 1 day | Critical |
| Response timing (RT) | 🟢 Easy | 1 day | Critical |
| Color control (dbc, dwc) | 🟢 Easy | 1 day | High |
| Basic scrambling | 🟢 Easy | 1-2 days | High |
| **Basic branching (bu, bic, biw, binr)** | 🟢 Easy | 1-2 days | High |
| **Call/Return subroutines** | 🟢 Easy | <1 day | Medium |
| **Total Phase 1:** | | **~2 weeks** | |

### Should-Have Features (Phase 2)

| Feature | Complexity | Effort | Impact |
|---------|-----------|--------|--------|
| Display-control characters | 🟡 Medium | 3-4 days | High |
| Text positioning (XYJustification) | 🟡 Medium | 1-2 days | Medium |
| Audio stimuli (WAV) | 🟢 Easy | 1 day | High |
| Multiple response types (zil, zor, ztr) | 🟢 Easy | 1-2 days | Medium |
| Feedback system | 🟢 Easy | 1-2 days | High |
| Advanced scrambling (grouping, seed) | 🟡 Medium | 2-3 days | Medium |
| **Total Phase 2:** | | **~2 weeks** | |

### Nice-to-Have Features (Phase 3)

| Feature | Complexity | Effort | Impact |
|---------|-----------|--------|--------|
| Counters with simple expressions | 🟡 Medium | 3-5 days | Medium |
| Multi-way branch with complex expressions | 🟡 Medium | 1 week | Medium |
| Video mode control | 🟡 Medium | 1-2 days | Low |
| Mouse continuous rating | 🟡 Medium | 2-3 days | Low |
| **Total Phase 3:** | | **~2 weeks** | |

### Not Recommended

| Feature | Reason |
|---------|--------|
| Digital video | Major effort, low demand, PEBL_MOVIES broken |
| Eye tracking | Requires hardware integration, niche use case |
| fMRI integration | Specialized, can work via keyboard/serial port |
| Remote testing | PEBL Online Platform already provides this |
| Voice key | Hardware-dependent, limited demand |

---

## Implementation Strategy

### Phase 1: Minimal Viable DMDX Runner (2 weeks)

**Goal:** Run simple DMDX experiments with basic features including branching

**Deliverables:**
1. RTF parser (strip formatting, extract plain text)
2. DMDX syntax parser (items, frames, delimiters, branch commands)
3. Frame-based item runner with while() loop (enables branching)
4. Basic item presentation (text/image display, keyboard response, RT)
5. Branching logic (unconditional, conditional, call/return)
6. Simple scrambling support
7. Data file output (CSV format)

**Test Case:**
```
<scramble 4> <dbc 0,0,0> <dwc 255,255,255> <timeout 5000>
+001 "Press + for CAT, - for DOG" *;
+002 <bmp cat.bmp> * <bic 4>;
-003 <bmp dog.bmp> * <biw 5>;
~004 "Correct!" / +006;
~005 "Wrong - try again" / +006;
+006 "Thank you for participating";
```

**Expected Behavior:**
- Parse and display instructions
- Show images with response collection
- Measure RT from image onset
- Branch to item 4 if correct, item 5 if wrong
- Randomize items in blocks of 4 (respecting grouping)
- Save data to CSV

---

### Phase 2: Enhanced DMDX Runner (2 weeks)

**Goal:** Support common DMDX features used in most experiments

**Additional Features:**
- Feedback display (correct/wrong/too late)
- Audio stimuli (WAV files)
- Multiple response types (zil, zor, ztr, mouse)
- Advanced scrambling options (grouping, seed)
- Text positioning control (XYJustification)
- Display-control characters (RTF formatting)

**Test Case:**
```
<scramble 8> <grouping 2> <seed 12345> <cfb "Correct!"> <wfb "Wrong"> <zil>
+001 "Lexical Decision Task - Press 1=WORD, 2=NONWORD" *;
+002 <wav beep.wav> / "HOUSE" * <bic 6>;
-003 <wav beep.wav> / "BLORK" * <biw 6>;
+004 <wav beep.wav> / "TABLE" * <bic 6>;
-005 <wav beep.wav> / "FRANE" * <biw 6>;
+006 "End of experiment";
```

---

### Phase 3: Advanced Features (2 weeks, optional)

**Goal:** Support complex DMDX experiments with counters and expressions

**Features:**
- Counters with meta-language expression evaluation
- Multi-way branching using `CallFunction()`
- Continuous mouse rating
- Video mode control

**Meta-Language Expression System:**

The key insight is that DMDX expressions can be implemented as PEBL functions that get called via `CallFunction()`. The parser converts DMDX expression syntax to PEBL function calls at parse time.

**Example:**
```pebl
## DMDX expression:
<mwb (&TrialCount > 10) and (&ErrorRate < 0.3) 100>

## Parser converts to PEBL function call:
item.branch <- ["DMDXExpr_And",
                ["DMDXExpr_GT", ["TrialCount", 10],
                 "DMDXExpr_LT", ["ErrorRate", 0.3]],
                100]

## Runner executes:
result <- CallFunction("DMDXExpr_And", args)
if(result) { i <- FindItemByNumber(items, 100) }
```

**Advantages:**
- No runtime expression parser needed
- Leverages PEBL's existing `CallFunction()` mechanism
- Extensible - add new expression functions as needed
- Type-safe - PEBL handles evaluation

**Assessment:**
- Medium complexity, not hard
- Reasonable effort (2 weeks total for Phase 3)
- Consider for PEBL 2.3 if time permits, otherwise PEBL 3.0+

---

## Compatibility Considerations

### Full DMDX Compatibility is Not Realistic

**Why:**
1. **Platform Differences**
   - DMDX is Windows-only with DirectX
   - PEBL is cross-platform with SDL2
   - Different timing models

2. **Architecture Differences**
   - DMDX is compiled C++ with preprocessor
   - PEBL is interpreted language
   - Expression evaluation would be very difficult

3. **Scope**
   - DMDX has 20+ years of features
   - Full compatibility would require rewriting DMDX

**Realistic Goal:**
- Support **common DMDX use cases** (80% of experiments)
- Focus on **educational/research experiments**
- Provide **migration path** from DMDX to PEBL
- Not a drop-in replacement

---

## Alternative Approach: DMDX-to-PEBL Converter

Instead of runtime DMDX interpretation, consider a **conversion tool**:

```
Input:  experiment.rtf (DMDX format)
Output: experiment.pbl (PEBL format)
```

**Advantages:**
- No runtime overhead
- Generated PEBL code can be edited
- Easier to debug
- Leverages existing PEBL capabilities

**Disadvantages:**
- Not transparent to users
- May require manual tweaks
- Loses some DMDX features

**Implementation:**
```python
#!/usr/bin/env python3
# dmdx2pebl.py - Convert DMDX .rtf to PEBL .pbl

import sys
import re

def parse_dmdx_file(rtf_file):
    # Strip RTF formatting
    # Parse parameter line
    # Parse items
    # Return structured representation

def generate_pebl_script(dmdx_data):
    # Create PEBL script
    # Use standard library functions
    # Generate trial loop
    # Return .pbl code

if __name__ == "__main__":
    dmdx_file = sys.argv[1]
    data = parse_dmdx_file(dmdx_file)
    pebl_code = generate_pebl_script(data)

    output_file = dmdx_file.replace(".rtf", ".pbl")
    with open(output_file, 'w') as f:
        f.write(pebl_code)
```

**Effort:** 1-2 weeks for basic converter

---

## Recommendations

### For PEBL 2.2 (Immediate)

**Don't attempt DMDX integration yet**

Reasons:
- Too complex for near-term release
- Current JSON runner is sufficient for proof-of-concept
- Focus on polishing existing expbuilder tools

**Actions:**
1. ✅ Document current state (EXPBUILDER_DMDX_ANALYSIS.md) - Done
2. ✅ Document DMDX capabilities (this document) - Done
3. Add data collection to existing dmdx.pbl JSON runner
4. Create user documentation

---

### For PEBL 2.3 (Mid-term)

**Implement Phase 1: Minimal Viable DMDX Runner**

**Approach:** Runtime interpreter for DMDX .rtf files

**Scope:**
- RTF parsing
- Basic item syntax
- Text/image display
- Keyboard responses
- Simple scrambling
- Data output

**Deliverable:** `dmdx-runner.pbl` that can run simple DMDX experiments

**Estimated Effort:** 2 weeks

---

### For PEBL 3.0+ (Long-term)

**Implement Phase 2: Enhanced DMDX Runner**

**Additional Features:**
- Feedback
- Audio
- Multiple response types
- Advanced scrambling
- Basic branching

**Alternative:**
- Develop DMDX-to-PEBL converter tool (Python script)
- Provides migration path for DMDX users

**Estimated Effort:** 2-3 weeks for runtime interpreter, or 1-2 weeks for converter

---

## Conclusion

DMDX is a feature-rich experiment presentation system with extensive capabilities developed over 20+ years. While full compatibility is not realistic due to platform differences (Windows/DirectX vs. cross-platform/SDL2), **meaningful DMDX support in PEBL is more feasible than initially thought**.

### Key Implementation Insights

1. **Branching is Easy:** Frame-based playback with `while()` loops naturally supports all DMDX branching features (unconditional, conditional, call/return). No special architecture needed - just index manipulation.

2. **Expressions via Meta-Language:** DMDX expressions can be implemented using PEBL's existing `CallFunction()` mechanism. Convert expression syntax to PEBL function calls at parse time. No runtime expression evaluator needed.

3. **Most Features Already Available:** PEBL already has the primitives needed for DMDX compatibility:
   - Timing: `Wait()`, `GetTime()`
   - Display: `MakeImage()`, `EasyLabel()`, colors, positioning
   - Response: `WaitForListKeyPressWithTimeout()`
   - Audio: `LoadSound()`, `PlayForeground()`
   - Randomization: `Shuffle()`, `SeedRNG()`

### Revised Effort Estimates

**Phase 1 (2 weeks):** Core features including branching - **covers 70-80% of DMDX experiments**
**Phase 2 (2 weeks):** Enhanced features - **covers 90% of DMDX experiments**
**Phase 3 (2 weeks):** Counters/expressions via meta-language - **covers 95% of DMDX experiments**

**Total effort for comprehensive DMDX support: ~6 weeks** (much less than initially estimated)

### Recommended Strategy

1. **PEBL 2.2:** Polish existing JSON-based experiment builder (current focus)
2. **PEBL 2.3:** Implement Phase 1 + Phase 2 (4 weeks) - provides excellent DMDX compatibility
3. **PEBL 3.0:** Add Phase 3 if user demand exists (counters/expressions)

**Target Compatibility:**
- Support common DMDX paradigms (lexical decision, priming, Stroop, adaptive testing, etc.)
- Provide migration path for DMDX users
- Enable PEBL to run typical DMDX experiments with high fidelity
- **Realistic goal: 90% DMDX compatibility** (Phases 1+2)

**Alternative Approach Still Valid:**
- DMDX-to-PEBL converter tool (1-2 weeks)
- May be easier as initial proof-of-concept
- Generates editable PEBL code for manual refinement
- Could be stepping stone to full runtime interpreter

---

## Appendix A: DMDX Keyword Reference

### Complete Keyword List (By Category)

Based on DMDX documentation, here are the major keyword categories:

#### 1. Miscellaneous Parameter Line Keywords
- `<nodb>` - No database output
- `<pcf>` - Parameter continuation file
- `<asu>` - All subjects unique (randomization)

#### 2. Scramble Keywords
- `<scramble N>` - Block size for scrambling
- `<grouping N>` - Group size
- `<seed N>` - Random seed
- `<multiscramble>` - Multiple scrambling passes

#### 3. Output File Type Keywords
- `<azk>` - AZK output format (default)
- `<dtp>` - DTP binary format

#### 4. Input Type Keywords
- `<id keyboard>` - Keyboard input
- `<id mouse>` - Mouse input
- `<id touchscreen>` - Touch input
- `<id pio12>` - Parallel port button box
- `<id serialvox>` - Serial voice key
- `<id digitalvox>` - Digital voice key

#### 5. Method of Display Keywords
- `<vm NxMxB>` - Video mode (width x height x bits)
- `<vm desktop>` - Desktop resolution
- `<multimonitor>` - Multi-monitor support

#### 6. Timing Keywords
- `<fd N>` - Frame duration (in frames)
- `<msfd N>` - Millisecond frame duration
- `<delay N>` / `<d N>` - Inter-trial interval (ms)
- `<timeout N>` - Response timeout (ms)
- `<nfbt N>` - No feedback time (ms)

#### 7. Color Keywords
- `<dbc R,G,B>` - Display background color
- `<dwc R,G,B>` - Display writing color
- `<bgc R,G,B>` - Background color for text
- Hex format: `<dbc #RRGGBB>`

#### 8. Method of Input Keywords
- `<zil>` - Zillion response (multiple choice)
- `<zor>` - Zillion one response (any key)
- `<ztr>` - Zillion typed response (text entry)
- `<mpr>` - Mouse point and respond
- `<mnr>` - Mouse numeric rating

#### 9. Feedback Keywords
- `<nfb>` - No feedback
- `<clfb>` - Clear feedback (default)
- `<fbocb>` - Feedback only clears behind
- `<cfbo>` - Correct feedback only
- `<wfbo>` - Wrong feedback only
- `<cfb "text">` - Custom correct feedback
- `<wfb "text">` - Custom wrong feedback
- `<tlfb "text">` - Too late feedback

#### 10. Branching and Counter Keywords
- `<branch N>` / `<bu N>` - Unconditional branch
- `<bic N>` - Branch if correct
- `<biw N>` - Branch if wrong
- `<binr N>` - Branch if no response
- `<call N>` - Subroutine call
- `<return>` - Return from call
- `<mwb>` - Multi-way branch
- `<counter Name Value>` - Define counter
- `<inc Name>` - Increment counter
- `<dec Name>` - Decrement counter
- `<set Name Value>` - Set counter

#### 11. Drawing/Display Keywords
- `<xy X,Y>` - Absolute positioning
- `<XYJustification option>` - Text alignment
- `<bmp filename>` - Bitmap image
- `<graphic>` - Graphics display
- `<inst>` - Instruction formatting

#### 12. Audio Keywords
- `<wav filename>` - Play WAV file
- `<pan N>` - Audio panning (-10000 to 10000)
- `<volume N>` - Audio volume (0 to -10000 dB)
- `<RecordVocal>` - Record voice response
- `<DigitalVOX>` - Digital voice onset detection

#### 13. Digital Video Keywords
- `<dv filename>` - Display video file
- `<SetStartCue>` - Video start position
- `<SetEndCue>` - Video end position

#### 14. Text Formatting Keywords
- `<prose>` - Text input handling
- Font control via RTF (bold, italic, underline)
- Color control via RTF

---

## Appendix B: DMDX Example Files

### Example 1: Simple Lexical Decision

```
<scramble 4> <dbc 0,0,0> <dwc 255,255,255> <timeout 5000>
+001 "Press + for WORD, - for NONWORD" *;
+002 "HOUSE" *;
+003 "TABLE" *;
-004 "BLORK" *;
-005 "FRANE" *;
+006 "Thank you!";
```

### Example 2: Priming with Branching

```
<scramble 8> <grouping 2> <cfb "Correct"> <wfb "Wrong">
+001 "Semantic Priming Experiment" *;
+002 "DOCTOR" / * "NURSE" <bic 4>;
-003 "BREAD" / * "CHAIR" <biw 5>;
~004 "Correct!" / +006;
~005 "Please try again" / +006;
+006 "End of experiment";
```

### Example 3: Rating Task with Mouse

```
<mpr> <dbc 255,255,255> <dwc 0,0,0>
+001 "Rate the following images" *;
+002 <bmp happy.bmp> / "How happy is this face?" * "1" , "2" , "3" , "4" , "5";
+003 <bmp sad.bmp> / "How happy is this face?" * "1" , "2" , "3" , "4" , "5";
```

---

## References

- **DMDX Help Documentation:** https://psy1.psych.arizona.edu/~jforster/dmdx/help/
- **DMDX Download:** https://psy1.psych.arizona.edu/~kforster/dmdx/download.htm
- **Visual DMDX:** https://visualdmdx.com (JSON export tool)
- **PEBL Experiment Builder Analysis:** EXPBUILDER_DMDX_ANALYSIS.md
