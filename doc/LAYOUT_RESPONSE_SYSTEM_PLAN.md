# PEBL Layout & Response System Implementation Plan

## Overview

This document describes the implementation of a unified layout and response system for PEBL battery tests. The system leverages PEBL's nested property access (implemented 2025-12) to provide powerful, flexible layout management with dynamic styling capabilities.

The system provides:

1. **Standard zone-based layouts** with automatic scaling
2. **Platform-aware response modes** (keyboard/mouse/touch)
3. **Single-function API** (`CreateLayout()`) with intelligent defaults
4. **Dynamic styling** via nested property modifications
5. **Theme support** with accessibility features
6. **Animation capabilities** for visual feedback
7. **Optional customization** via JSON configuration files
8. **Backwards compatibility** with existing tests

## Goals

- ✅ Solve Sticky Keys problem in browser (avoid shift keys in Emscripten)
- ✅ Support mobile/tablet devices with touch targets
- ✅ Auto-scale to any screen size (phones, tablets, HiDPI displays)
- ✅ Reduce boilerplate code in battery tests
- ✅ Enable dynamic styling via nested properties
- ✅ Provide visual feedback animations
- ✅ Support accessibility features (high contrast, large text)
- ✅ Maintain backwards compatibility with existing tests
- ✅ Allow experimenter customization via parameters

---

## File Structure

```
pebl/
├── media/
│   └── settings/                          # NEW: System configuration files
│       ├── default-layout.json            # Default layout configuration
│       └── response-modes.json            # Response mode definitions
│
├── pebl-lib/
│   └── Layout.pbl                         # NEW: Layout system functions
│
├── battery/
│   └── testname/
│       ├── testname.pbl
│       ├── params/
│       │   └── testname.pbl.schema        # Add responsemode parameter
│       └── layout/                        # OPTIONAL: Custom layouts
│           └── testname.pbl.layout.json   # Test-specific overrides
```

---

## API Design

### Single Function Call

```pebl
CreateLayout(testName, params:0)
  → Creates global gLayout object
  → Returns gLayout for convenience
  → Defined in pebl-lib/Layout.pbl
```

### Global Layout Object: `gLayout`

With nested property support, all layout elements can be modified directly:

```pebl
gLayout
├── .stimulusRegion (Object with nested properties)
│   ├── .x, .y (Integers) - Top-left corner
│   ├── .width, .height (Integers) - Dimensions
│   ├── .centerX, .centerY (Integers) - Center point for stimuli
│   ├── .bgcolor (PColor) - Background color
│   │   └── .red, .green, .blue, .alpha (0-255) - Color components
│   └── .visible (Boolean) - Show/hide region
│
├── .header (PLabel - full label properties)
│   ├── .text (String) - Label text
│   ├── .visible (Boolean) - Show/hide
│   ├── .font (PFont) - Font object
│   │   ├── .size (Integer) - Font size
│   │   ├── .fgcolor (PColor) - Foreground color
│   │   │   └── .red, .green, .blue, .alpha (0-255)
│   │   └── .bgcolor (PColor) - Background color
│   │       └── .red, .green, .blue, .alpha (0-255)
│   ├── .x, .y (Integers) - Position
│   └── .rotation (Number) - Rotation angle
│
├── .subheader (PLabel - same as header)
├── .footer (PLabel - same as header)
│
├── .responseLabels (List of PLabel objects)
│   └── [1], [2], ... - Each supports all nested properties
│
├── .responseMode (Object)
│   ├── .type (String) - "keyboard", "mousebutton", etc.
│   ├── .keys / .buttons (List) - Input codes
│   ├── .labels (List) - Display labels
│   └── .semantic (List) - Semantic names ("left", "right")
│
├── .scale (Number) - Screen scale factor (1.0 = 800x600)
└── .config (Object) - Full configuration object
```

### Response Waiting Function

```pebl
WaitForLayoutResponse(layout, timeout:-1)
  → Returns semantic response: "left", "right", "up", "down", etc.
  → Returns "<timeout>" if timeout exceeded
  → Automatically uses correct input method based on layout.responseMode
  → Defined in pebl-lib/Layout.pbl
```

### Theme Functions

```pebl
ApplyDarkTheme(layout)
  → Applies dark mode (black background, white text)

ApplyHighContrastTheme(layout)
  → Applies high contrast (black on yellow)

ApplyAccessibilitySettings(layout, params)
  → Applies accessibility settings based on params
  → Supports: highContrast, largeText, colorBlind
```

### Animation Functions

```pebl
FlashCorrect(label, durationMs:500)
  → Flashes label green to indicate correct response

FlashIncorrect(label, durationMs:500)
  → Flashes label red to indicate incorrect response

FadeOut(label, durationMs:1000)
  → Fades label out by animating alpha

PulseLabel(label, count:3, pulseSize:10)
  → Pulses label by changing font size

HighlightResponse(label, durationMs:1000)
  → Highlights label with yellow background
```

All functions defined in `pebl-lib/Layout.pbl`

---

## Nested Property Capabilities

The layout system fully leverages PEBL's nested property access for powerful dynamic styling:

### Direct Style Modifications

```pebl
CreateLayout("test")

## Header styling
gLayout.header.font.size <- 36
gLayout.header.font.fgcolor.red <- 255
gLayout.header.font.bgcolor.alpha <- 0        ## Transparent

## Footer styling
gLayout.footer.font.fgcolor <- MakeColor("blue")
gLayout.footer.font.size <- 18

## Response label styling
loop(i, 1, Length(gLayout.responseLabels))
{
    label <- Nth(gLayout.responseLabels, i)
    label.font.bgcolor.red <- 200             ## Light red background
    label.font.bgcolor.alpha <- 150           ## Semi-transparent
}
```

### Conditional Styling

```pebl
## Adjust appearance based on trial conditions
if(trialDifficulty == "hard")
{
    gLayout.header.font.size <- 36            ## Smaller text
    gLayout.header.font.fgcolor.alpha <- 180  ## Faded
}
else
{
    gLayout.header.font.size <- 60
    gLayout.header.font.fgcolor.alpha <- 255  ## Fully opaque
}
```

### Animated Feedback

```pebl
## Highlight correct response after trial
correctLabel <- Nth(gLayout.responseLabels, correctIndex)

## Flash green
correctLabel.font.bgcolor <- MakeColor("green")
Draw()
Wait(500)

## Or use animation helper
FlashCorrect(correctLabel)

## Fade out incorrect response
incorrectLabel <- Nth(gLayout.responseLabels, incorrectIndex)
FadeOut(incorrectLabel, 1000)
```

### Dynamic Theme Switching

```pebl
## Apply themes based on user preference
if(gParams.theme == "dark")
{
    ApplyDarkTheme(gLayout)
}
elseif(gParams.theme == "highcontrast")
{
    ApplyHighContrastTheme(gLayout)
}

## Or apply accessibility settings
ApplyAccessibilitySettings(gLayout, gParams)
```

---

## Default Layout Configuration

### File: `media/settings/default-layout.json`

```json
{
  "name": "PEBL Default Layout",
  "version": "1.0",
  "description": "Standard two-alternative layout with header, stimulus region, and footer",

  "baseline": {
    "width": 800,
    "height": 600,
    "comment": "Design baseline - everything scales from this"
  },

  "minimums": {
    "width": 320,
    "height": 480,
    "comment": "Minimum phone size - warn if smaller"
  },

  "zones": {
    "header": {
      "height": 60,
      "fontSize": 28,
      "visible": false,
      "comment": "Test title area"
    },
    "subheader": {
      "height": 30,
      "fontSize": 18,
      "visible": false,
      "comment": "Trial counter, status info"
    },
    "stimulus": {
      "flexGrow": true,
      "fontSize": 24,
      "comment": "Main content area - takes remaining space"
    },
    "response": {
      "height": 60,
      "fontSize": 20,
      "visible": true,
      "comment": "Response key labels"
    },
    "footer": {
      "height": 80,
      "fontSize": 16,
      "visible": false,
      "comment": "Instructions, prompts"
    }
  },

  "margins": {
    "x": 50,
    "y": 20
  },

  "responseDefaults": {
    "type": "auto",
    "numAlternatives": 2,
    "comment": "Default to platform-aware two-alternative responses"
  }
}
```

**Note:** Colors and detailed styling are now handled via nested properties in code rather than JSON, making the configuration simpler and more flexible.

### File: `media/settings/response-modes.json`

```json
{
  "modes": {
    "auto": {
      "description": "Platform-aware: shift keys on native, safe keys in browser",
      "native": "keyboard-shift",
      "emscripten": "keyboard-safe"
    },

    "keyboard-shift": {
      "type": "keyboard",
      "keys": ["<lshift>", "<rshift>"],
      "labels": ["LEFT-SHIFT", "RIGHT-SHIFT"],
      "semantic": ["left", "right"],
      "platforms": ["native"],
      "comment": "Traditional - works on native, causes Sticky Keys in browser"
    },

    "keyboard-safe": {
      "type": "keyboard",
      "keys": ["z", "/"],
      "labels": ["Z", "/"],
      "semantic": ["left", "right"],
      "platforms": ["all"],
      "comment": "Browser-safe - no modifier keys"
    },

    "mousebutton": {
      "type": "mousebutton",
      "buttons": [1, 3],
      "labels": ["LEFT-CLICK", "RIGHT-CLICK"],
      "semantic": ["left", "right"],
      "platforms": ["all"],
      "comment": "Click anywhere with left/right mouse buttons"
    },

    "mousetarget": {
      "type": "mousetarget",
      "labels": ["Click LEFT", "Click RIGHT"],
      "semantic": ["left", "right"],
      "platforms": ["all"],
      "targetSize": {"desktop": 100, "tablet": 150, "phone": 200},
      "comment": "Click on visual targets (auto-sized by device)"
    },

    "arrow": {
      "type": "keyboard",
      "keys": ["<left>", "<right>", "<up>", "<down>"],
      "labels": ["←", "→", "↑", "↓"],
      "semantic": ["left", "right", "up", "down"],
      "platforms": ["all"],
      "comment": "Arrow keys for navigation tasks"
    },

    "number": {
      "type": "keyboard",
      "keys": ["1", "2", "3", "4", "5", "6", "7", "8", "9"],
      "labels": ["1", "2", "3", "4", "5", "6", "7", "8", "9"],
      "semantic": ["1", "2", "3", "4", "5", "6", "7", "8", "9"],
      "platforms": ["all"],
      "comment": "Number keys for multi-alternative tasks"
    },

    "mouse-only": {
      "type": "mouse-only",
      "platforms": ["all"],
      "comment": "Task handles mouse directly (e.g., drawing, clicking objects)"
    }
  }
}
```

---

## Complete Usage Example

### Simple Two-Alternative Test

```pebl
define Start(p)
{
  gWin <- MakeWindow()

  ## Add responsemode to parameters
  parpairs <- [["reps", 10],
               ["responsemode", "auto"],
               ["theme", "default"]]
  gParams <- CreateParameters(parpairs, gParamFile)

  ## Create layout - ONE CALL
  CreateLayout("simon")

  ## Configure UI using nested properties
  gLayout.header.text <- "Simon Task"
  gLayout.header.visible <- 1
  gLayout.header.font.fgcolor <- MakeColor("navy")

  gLayout.footer.text <- "Respond to color, ignore position"
  gLayout.footer.visible <- 1
  gLayout.footer.font.size <- 18

  ## Apply theme if requested
  if(gParams.theme == "dark")
  {
      ApplyDarkTheme(gLayout)
  }

  ## Place stimulus in correct region
  stim <- Circle(gLayout.stimulusRegion.centerX,
                 gLayout.stimulusRegion.centerY,
                 50, MakeColor("red"), 1)
  AddObject(stim, gWin)

  Draw()

  ## Wait for response
  resp <- WaitForLayoutResponse(gLayout, 5000)

  ## Semantic checking
  if(resp == "left")
  {
    ## Left response - flash correct or incorrect
    FlashCorrect(Nth(gLayout.responseLabels, 1))
  }
  elseif(resp == "right")
  {
    ## Right response
    FlashCorrect(Nth(gLayout.responseLabels, 2))
  }
  elseif(resp == "<timeout>")
  {
    ## Timeout
    gLayout.footer.text <- "Too slow!"
    gLayout.footer.font.fgcolor <- MakeColor("red")
    Draw()
  }
}
```

---

## Best Practices

### 1. Use Nested Properties for Dynamic Changes

**✅ GOOD: Direct property modification**
```pebl
gLayout.header.font.size <- 48
gLayout.header.font.fgcolor.red <- 255
```

**❌ AVOID: Unnecessary wrapper functions**
```pebl
SetHeaderFontSize(gLayout, 48)  ## Don't create wrappers
SetHeaderColor(gLayout, MakeColor("red"))
```

### 2. Cache Original Values When Modifying Temporarily

```pebl
## Save original
origColor <- label.font.fgcolor
origSize <- label.font.size

## Modify
label.font.fgcolor <- MakeColor("red")
label.font.size <- 60

## ... do something ...

## Restore
label.font.fgcolor <- origColor
label.font.size <- origSize
```

### 3. Batch Property Changes Before Draw()

**✅ GOOD: Modify all, then Draw() once**
```pebl
label.font.fgcolor.red <- 255
label.font.fgcolor.green <- 0
label.font.fgcolor.blue <- 0
label.font.size <- 48
label.visible <- 1
Draw()
```

**❌ BAD: Draw() after each change**
```pebl
label.font.fgcolor.red <- 255
Draw()
label.font.size <- 48
Draw()
```

### 4. Use Helper Functions for Complex Styling

```pebl
## Define reusable styling function
define HighlightLabel(label, color, durationMs)
{
    origBg <- label.font.bgcolor

    label.font.bgcolor <- MakeColor(color)
    Draw()
    Wait(durationMs)

    label.font.bgcolor <- origBg
    Draw()
}

## Use it
HighlightLabel(gLayout.header, "yellow", 1000)
```

---

## Implementation Steps

### Phase 1: Core Infrastructure (Week 1)

#### 1.1 Create Configuration Files

**Files to create:**
- `media/settings/default-layout.json` (from design above)
- `media/settings/response-modes.json` (from design above)

**Actions:**
- Create files with JSON content
- Add to version control
- Test JSON parsing with existing functions

#### 1.2 Create Layout.pbl Library

**File:** `pebl-lib/Layout.pbl`

**Status:** ✅ COMPLETE - File created with all functions

**Functions implemented:**
- `CreateLayout(testName, params:0)` - Main entry point
- `WaitForLayoutResponse(layout, timeout)` - Unified response waiting
- Theme functions (ApplyDarkTheme, ApplyHighContrastTheme, ApplyAccessibilitySettings)
- Animation functions (FlashCorrect, FadeOut, PulseLabel, etc.)
- Helper functions (SetLabelColor, SetLabelBackground, StyleForDifficulty)
- Internal helpers (LoadLayoutConfig, DetermineResponseMode, CalculateLayoutZones, etc.)

#### 1.3 Copy to Emscripten

**Actions:**
```bash
cp pebl-lib/Layout.pbl emscripten/pebl-lib/Layout.pbl
make fp  ## Rebuild data package
```

#### 1.4 Update Search Path (if needed)

Verify `media/settings/` is in PEBL search path. If not, add to:
- `src/utility/PEBLPath.cpp` (native)
- Emscripten preload manifest

### Phase 2: Example Conversion (Week 2)

#### 2.1 Convert `simon.pbl` as Reference Example

**Add to parameter schema:**
```
responsemode|auto|Response mode: auto, keyboard-shift, keyboard-safe, mousebutton
theme|default|Theme: default, dark, highcontrast
```

**Update code:**
```pebl
## After parameters
CreateLayout("simon")

## Style with nested properties
gLayout.header.text <- gStrings.title
gLayout.header.visible <- 1
gLayout.footer.text <- gStrings.instructions
gLayout.footer.visible <- 1

## Apply theme
if(gParams.theme == "dark")
{
    ApplyDarkTheme(gLayout)
}

## In trial loop
resp <- WaitForLayoutResponse(gLayout, 5000)

if(resp == "left")
{
    ## Handle left response
    FlashCorrect(Nth(gLayout.responseLabels, 1))
}
```

#### 2.2 Test Across Platforms

- **Native Linux:** Should use shift keys by default
- **Emscripten:** Should use z/slash by default
- **With `responsemode=mousebutton`:** Should accept left/right clicks
- **With `theme=dark`:** Should apply dark theme

### Phase 3: Documentation (Week 2-3)

#### 3.1 Create Developer Guide

**File:** `doc/LAYOUT_SYSTEM_GUIDE.md`

Contents:
- Quick start examples
- API reference (link to Layout.pbl)
- Nested property examples
- Theme customization
- Animation examples
- Accessibility features
- Troubleshooting

#### 3.2 Update Test Template

**File:** `battery/template/template.pbl`

Update to use new layout system as default example with nested property styling.

#### 3.3 Add to PEBL Manual

Update relevant sections:
- Test development workflow
- Response collection
- Screen layout
- Platform compatibility
- Nested property usage

### Phase 4: Gradual Migration (Ongoing)

#### 4.1 Priority Tests for Conversion

**High Priority** (rapid responses, Sticky Keys risk):
1. simon
2. flanker
3. gonogo
4. oddball
5. choiceRT
6. stroop-number

**Medium Priority** (2-AFC, standard layout):
7. sternberg
8. matchtosample
9. dotjudgment
10. luckvogel

**Low Priority** (special layouts, mouse-only):
- Tests with custom UI
- Tests requiring specific keys (maze, navigation)
- Tests that are already mouse-only

#### 4.2 Conversion Checklist

For each test:
- [ ] Add `responsemode` to parameter schema
- [ ] Add `CreateLayout()` call at start
- [ ] Replace manual UI creation with `gLayout.*` properties
- [ ] Use nested properties for any custom styling
- [ ] Replace `WaitForListKeyPress()` with `WaitForLayoutResponse()`
- [ ] Update response checking to use semantic names
- [ ] Test on native and Emscripten
- [ ] Document in commit message

---

## Testing Plan

### Unit Tests

Create test scripts for each function:

1. **CreateLayout()**
   - Creates all UI elements
   - Zones don't overlap
   - Elements fit on screen
   - Nested properties accessible

2. **Nested Property Access**
   - Color component modification works
   - Font size changes work
   - Visibility toggles work
   - Changes trigger re-render

3. **Theme Functions**
   - ApplyDarkTheme modifies all elements
   - ApplyHighContrastTheme works correctly
   - ApplyAccessibilitySettings applies all modes

4. **Animation Functions**
   - FlashCorrect shows green flash
   - FadeOut animates alpha correctly
   - PulseLabel changes size smoothly

5. **WaitForLayoutResponse()**
   - Keyboard mode returns correct semantic
   - Mouse button mode works
   - Timeout works correctly

### Integration Tests

**Test complete workflows:**

1. **Two-alternative test (simon)**
   - Native: shift keys work
   - Emscripten: z/slash work
   - Mouse button mode works
   - Dark theme applies correctly

2. **Phone screen (360x640)**
   - Layout scales correctly
   - All elements visible
   - Touch targets large enough
   - Fonts readable

3. **HiDPI screen (2560x1440)**
   - Fonts scale up correctly
   - No pixelation
   - UI looks crisp
   - Nested properties work at scale

4. **Custom layout JSON**
   - Custom config loads
   - Overrides defaults
   - Invalid JSON handled gracefully

### Platform Testing

- **Native Linux:** Default shift keys, themes work
- **Emscripten Chrome:** Default z/slash, animations smooth
- **Emscripten Firefox:** Same as Chrome
- **Mobile Safari (iOS):** Touch mode, scaling correct
- **Mobile Chrome (Android):** Touch mode, animations work

---

## Custom Layout Example

### File: `battery/stroop/layout/stroop.pbl.layout.json`

```json
{
  "zones": {
    "header": {
      "height": 100,
      "fontSize": 36,
      "visible": true
    },
    "stimulus": {
      "fontSize": 48,
      "comment": "Large text for word/color"
    },
    "response": {
      "height": 80,
      "fontSize": 24
    }
  }
}
```

**Then style in code:**
```pebl
CreateLayout("stroop")

## Custom color styling via nested properties
gLayout.header.font.fgcolor <- MakeColor("red")
gLayout.responseLabels[1].font.bgcolor.red <- 255
gLayout.responseLabels[2].font.bgcolor.green <- 255
gLayout.responseLabels[3].font.bgcolor.blue <- 255
gLayout.responseLabels[4].font.bgcolor.red <- 255
gLayout.responseLabels[4].font.bgcolor.green <- 255
```

---

## Migration Examples

### Example 1: From Manual UI to Layout System

**Before:**
```pebl
foot1 <- EasyLabel("LEFT-SHIFT                    RIGHT-SHIFT",
                   gVideoWidth/2, gVideoHeight/2+200, gWin, 22)

resp <- WaitForListKeyPress(["<lshift>","<rshift>"])

if(resp == "<lshift>") {
  corr <- (side == "left")
}
```

**After:**
```pebl
CreateLayout("test")

resp <- WaitForLayoutResponse(gLayout)

if(resp == "left") {  ## Semantic!
  corr <- (side == "left")
  FlashCorrect(Nth(gLayout.responseLabels, 1))
}
```

### Example 2: Adding Dynamic Styling

**Before (static):**
```pebl
header <- EasyLabel("Title", gVideoWidth/2, 50, gWin, 28)
```

**After (dynamic with nested properties):**
```pebl
CreateLayout("test")

gLayout.header.text <- "Title"
gLayout.header.visible <- 1

## Modify based on conditions
if(difficulty == "hard")
{
    gLayout.header.font.size <- 20
    gLayout.header.font.fgcolor.alpha <- 180  ## Faded
}

## Animate feedback
if(correct)
{
    gLayout.header.font.bgcolor <- MakeColor("green")
    Draw()
    Wait(500)
}
```

---

## Benefits Summary

### For Users
- ✅ Works on phones/tablets (auto-scaling)
- ✅ No Sticky Keys interruptions (browser-safe mode)
- ✅ Consistent UI across tests
- ✅ Better accessibility (themes, large text)
- ✅ Visual feedback animations

### For Developers
- ✅ Less boilerplate code (one CreateLayout() call)
- ✅ Auto-scaling to any screen size
- ✅ Semantic response names ("left" vs "<lshift>")
- ✅ Direct property modification (no setter functions needed)
- ✅ Easy theming and styling
- ✅ Animation helpers included

### For Experimenters
- ✅ Configure response mode via parameters
- ✅ Choose themes (default, dark, high contrast)
- ✅ Customize layout without code changes
- ✅ Platform detection automatic
- ✅ Accessibility options available

### For Platform
- ✅ Backwards compatible
- ✅ Solves Sticky Keys problem
- ✅ Enables mobile deployment
- ✅ Modern, maintainable code
- ✅ Leverages nested property system

---

## Implementation Checklist

### Phase 1: Core Infrastructure ✅ PARTIALLY COMPLETE
- [ ] Create `media/settings/default-layout.json`
- [ ] Create `media/settings/response-modes.json`
- [ ] Add files to emscripten preload (Makefile, file_packager)
- [x] Create `pebl-lib/Layout.pbl` with all functions
- [ ] Copy to `emscripten/pebl-lib/Layout.pbl`
- [ ] Test with simple script
- [ ] Rebuild Emscripten (`make fp`)

### Phase 2: Example Conversion
- [ ] Convert `simon.pbl` to use layout system
- [ ] Add `responsemode` and `theme` to `simon.pbl.schema`
- [ ] Test native build
- [ ] Test Emscripten build
- [ ] Verify shift keys on native
- [ ] Verify z/slash in browser
- [ ] Test theme switching
- [ ] Test animations
- [ ] Document conversion in commit

### Phase 3: Documentation
- [ ] Write `doc/LAYOUT_SYSTEM_GUIDE.md`
- [ ] Update `battery/template/template.pbl`
- [ ] Update PEBL manual sections
- [ ] Create migration examples
- [ ] Write troubleshooting guide

### Phase 4: Gradual Migration
- [ ] Convert high-priority tests
- [ ] Test each conversion thoroughly
- [ ] Document parameter schema additions
- [ ] Update test documentation

### Phase 5: Polish
- [ ] Add error handling
- [ ] Add debug mode (print layout info)
- [ ] Add validation warnings
- [ ] Performance testing
- [ ] Cross-platform verification

---

## Timeline

- **Week 1:** Core infrastructure - JSON configs, test Layout.pbl functions
- **Week 2:** Example conversion (simon) + basic documentation
- **Weeks 3-6:** Gradual migration of priority tests
- **Week 7:** Polish, testing, and release

## Success Metrics

- ✅ All converted tests work on native Linux
- ✅ All converted tests work in Emscripten
- ✅ No Sticky Keys dialogs in browser tests
- ✅ Layout scales correctly on 360x640 (phone)
- ✅ Layout scales correctly on 2560x1440 (HiDPI)
- ✅ Nested property modifications work correctly
- ✅ Theme switching works
- ✅ Animations smooth and functional
- ✅ Backwards compatibility maintained
- ✅ Documentation complete
- ✅ At least 10 tests converted

---

## Future Enhancements

- Touch gesture support (swipe, pinch)
- Gamepad/joystick response mode
- Voice input response mode
- More animation effects (rotate, scale, slide)
- Accessibility improvements (screen reader support)
- Layout animation/transitions
- Responsive layout templates beyond two-alternative
- Visual layout editor tool
- Performance profiling tools

---

## Reference Examples

### Test Files Demonstrating Nested Properties

- `test-font-transparency.pbl` - Font background alpha transparency
- `test-canvas-bgcolor.pbl` - Canvas background color modification
- `test-window-bgcolor.pbl` - Window background color changes
- `test-textbox-color.pbl` - TextBox color modifications
- `test-circle-color-nested.pbl` - DrawObject color modifications

### Library Files

- `pebl-lib/Layout.pbl` - Complete layout system implementation
- `pebl-lib/Utility.pbl` - Core utility functions
- `pebl-lib/UI.pbl` - Additional UI helpers

---

**Document Version:** 2.0
**Date:** 2025-12-03
**Status:** Ready for Implementation
**Prerequisites:** Nested property access system (implemented 2025-12)
**Related Files:** `pebl-lib/Layout.pbl`, `test-font-transparency.pbl`
