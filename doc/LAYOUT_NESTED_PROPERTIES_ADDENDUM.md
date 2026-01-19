# Layout System: Nested Properties Integration

## Overview

This addendum describes enhancements to the Layout & Response System enabled by PEBL's nested property access implementation (completed 2025-12).

## Enhanced API Capabilities

### 1. Direct Style Modifications

All layout elements support nested property modifications:

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

### 2. Dynamic Theme Application

Create theme functions that modify layout appearance:

```pebl
define ApplyDarkTheme(layout)
{
    ## Window background
    gWin.bgcolor.red <- 40
    gWin.bgcolor.green <- 40
    gWin.bgcolor.blue <- 40

    ## All text to white
    layout.header.font.fgcolor <- MakeColor("white")
    layout.footer.font.fgcolor <- MakeColor("white")

    loop(label, layout.responseLabels)
    {
        label.font.fgcolor <- MakeColor("white")
        label.font.bgcolor.red <- 60
        label.font.bgcolor.green <- 60
        label.font.bgcolor.blue <- 60
    }

    Draw()
}

define ApplyHighContrastTheme(layout)
{
    ## Black on yellow for accessibility
    gWin.bgcolor <- MakeColor("yellow")

    layout.header.font.fgcolor <- MakeColor("black")
    layout.header.font.size <- 48              ## Larger text

    loop(label, layout.responseLabels)
    {
        label.font.fgcolor <- MakeColor("black")
        label.font.size <- 32
        label.font.bgcolor <- MakeColor("white")
    }

    Draw()
}
```

### 3. Animated Feedback

Provide visual feedback by modifying properties over time:

```pebl
define FlashCorrect(label)
{
    ## Save original colors
    origFg <- label.font.fgcolor
    origBg <- label.font.bgcolor

    ## Flash green
    label.font.bgcolor <- MakeColor("green")
    Draw()
    Wait(200)

    ## Restore
    label.font.bgcolor <- origBg
    Draw()
}

define FadeOut(label, durationMs)
{
    steps <- 20
    stepDelay <- durationMs / steps

    loop(alpha, 255, 0, -13)
    {
        label.font.fgcolor.alpha <- alpha
        label.font.bgcolor.alpha <- alpha
        Draw()
        Wait(stepDelay)
    }
}

define PulseLabel(label, count)
{
    originalSize <- label.font.size

    loop(i, 1, count)
    {
        label.font.size <- originalSize + 10
        Draw()
        Wait(150)

        label.font.size <- originalSize
        Draw()
        Wait(150)
    }
}
```

### 4. Conditional Styling

Adjust appearance based on trial conditions:

```pebl
define StyleForDifficulty(layout, difficulty)
{
    if(difficulty == "easy")
    {
        layout.stimulus.font.size <- 60
        layout.stimulus.font.fgcolor <- MakeColor("black")
        layout.responseLabels[1].font.size <- 32
        layout.responseLabels[2].font.size <- 32
    }
    elseif(difficulty == "medium")
    {
        layout.stimulus.font.size <- 48
        layout.stimulus.font.fgcolor.alpha <- 220   ## Slightly faded
        layout.responseLabels[1].font.size <- 24
        layout.responseLabels[2].font.size <- 24
    }
    else  ## hard
    {
        layout.stimulus.font.size <- 36
        layout.stimulus.font.fgcolor.alpha <- 180   ## More faded
        layout.responseLabels[1].font.size <- 18
        layout.responseLabels[2].font.size <- 18
    }

    Draw()
}
```

### 5. Accessibility Enhancements

```pebl
define ApplyAccessibilitySettings(layout, params)
{
    ## High contrast mode
    if(params.highContrast)
    {
        ApplyHighContrastTheme(layout)
    }

    ## Large text mode
    if(params.largeText)
    {
        layout.header.font.size <- layout.header.font.size * 1.5
        layout.footer.font.size <- layout.footer.font.size * 1.5

        loop(label, layout.responseLabels)
        {
            label.font.size <- label.font.size * 1.5
        }
    }

    ## Color blind mode (remove color-only cues)
    if(params.colorBlind)
    {
        ## Use patterns/shapes instead of colors
        ## Or ensure sufficient luminance contrast
        layout.stimulus.font.fgcolor <- MakeColor("black")
        gWin.bgcolor <- MakeColor("white")
    }
}
```

## Updated gLayout Object Structure

With nested properties, the gLayout object hierarchy becomes more powerful:

```pebl
gLayout
├── .stimulusRegion (Object with nested properties)
│   ├── .x, .y (Integers)
│   ├── .width, .height (Integers)
│   ├── .centerX, .centerY (Integers)
│   ├── .bgcolor (PColor - supports .red, .green, .blue, .alpha)
│   └── .visible (Boolean)
│
├── .header (PLabel - supports all label properties)
│   ├── .text (String)
│   ├── .visible (Boolean)
│   ├── .font (PFont)
│   │   ├── .size (Integer)
│   │   ├── .fgcolor (PColor)
│   │   │   ├── .red, .green, .blue, .alpha
│   │   └── .bgcolor (PColor)
│   │       ├── .red, .green, .blue, .alpha
│   ├── .x, .y (Position)
│   └── .rotation (Angle)
│
├── .subheader (PLabel - same as header)
├── .footer (PLabel - same as header)
│
├── .responseLabels (List of PLabel objects)
│   └── [1], [2], ... (each supports all nested properties)
│
├── .responseMode (Object)
│   ├── .type (String)
│   ├── .keys / .buttons (List)
│   ├── .labels (List)
│   └── .semantic (List)
│
├── .scale (Number)
└── .config (Object)
```

## Best Practices

### 1. Cache Original Values

When modifying properties temporarily, save originals:

```pebl
define HighlightResponse(label, durationMs)
{
    ## Cache
    origBgColor <- label.font.bgcolor
    origFgColor <- label.font.fgcolor

    ## Modify
    label.font.bgcolor <- MakeColor("yellow")
    label.font.fgcolor <- MakeColor("black")
    Draw()
    Wait(durationMs)

    ## Restore
    label.font.bgcolor <- origBgColor
    label.font.fgcolor <- origFgColor
    Draw()
}
```

### 2. Use Helper Functions

Create reusable styling functions:

```pebl
define SetLabelColor(label, colorName)
{
    label.font.fgcolor <- MakeColor(colorName)
    Draw()
}

define SetLabelBackground(label, red, green, blue, alpha)
{
    label.font.bgcolor.red <- red
    label.font.bgcolor.green <- green
    label.font.bgcolor.blue <- blue
    label.font.bgcolor.alpha <- alpha
    Draw()
}

define ScaleFont(label, scaleFactor)
{
    label.font.size <- Round(label.font.size * scaleFactor)
    Draw()
}
```

### 3. Batch Updates

Modify multiple properties before calling Draw():

```pebl
## GOOD: Single Draw() call
label.font.fgcolor.red <- 255
label.font.fgcolor.green <- 0
label.font.fgcolor.blue <- 0
label.font.size <- 48
Draw()

## LESS EFFICIENT: Multiple Draw() calls
label.font.fgcolor.red <- 255
Draw()
label.font.fgcolor.green <- 0
Draw()
label.font.fgcolor.blue <- 0
Draw()
```

## Migration Guide

### Old Approach (Pre-Nested Properties)

```pebl
## Would need setter functions
SetHeaderColor(gLayout, MakeColor("red"))
SetHeaderSize(gLayout, 36)
SetHeaderBackground(gLayout, MakeColor("white"))
```

### New Approach (With Nested Properties)

```pebl
## Direct property modification
gLayout.header.font.fgcolor <- MakeColor("red")
gLayout.header.font.size <- 36
gLayout.header.font.bgcolor <- MakeColor("white")
Draw()
```

## Compatibility Notes

- All existing code continues to work unchanged
- Nested property access is purely additive
- No breaking changes to existing layout API
- GetProperty/SetProperty still work for programmatic access

## Implementation Checklist for Layout System

When implementing CreateLayout(), ensure:

- [x] All UI elements (header, footer, labels) stored as actual PEBL objects
- [x] Objects stored in property system for nested access
- [x] No duplicate color/font storage (use property system only)
- [x] Change detection works (labels re-render when properties change)
- [ ] Documentation includes nested property examples
- [ ] Example tests demonstrate property modifications
- [ ] Theme helper functions provided in Utility.pbl

## Examples

See:
- `test-font-transparency.pbl` - Font background alpha transparency
- `test-canvas-bgcolor.pbl` - Canvas background color modification
- `test-window-bgcolor.pbl` - Window background color changes
- `test-textbox-color.pbl` - TextBox color modifications

---

**Document Version:** 1.0
**Date:** 2025-12-03
**Related:** LAYOUT_RESPONSE_SYSTEM_PLAN.md
**Status:** Implementation Complete (Nested Properties)
