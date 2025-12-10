# AdaptiveTextBox Implementation

## Overview

`AdaptiveTextBox` is a smart textbox that automatically adapts to fit its content using two different strategies: scaling the box or scaling the font.

## Location

Currently in `test-adaptive-textbox.pbl` for testing. Ready to be moved to `pebl-lib/UI.pbl` when approved.

## Usage

```pebl
AdaptiveTextBox(text, x, y, window, fontsize, width, height, adaptive, maxlines:30)
```

### Parameters

- `text`: The text to display
- `x, y`: Position coordinates
- `window`: The parent window
- `fontsize`: Initial font size (may be reduced if adaptive="scalefont")
- `width, height`: Target box dimensions
- `adaptive`: Adaptation strategy - `0`, `"scalebox"`, or `"scalefont"`
- `maxlines`: Maximum number of lines allowed (default: 30)

### Adaptation Strategies

#### 1. No Adaptation (`adaptive=0`)
Creates a standard `EasyTextBox` with no adaptation. Text may overflow if too long.

#### 2. Scale Box (`adaptive="scalebox"`)
- **How it works**: Creates a larger textbox that fits all text (maintaining aspect ratio), then scales it down equally using `zoomX` and `zoomY` to fit the original dimensions
- **Algorithm**:
  1. Checks if text fits at original size using `textComplete` property
  2. If not, iteratively increases scale factor by 10% (starting at 1.0)
  3. Expands both width and height by the same scale factor (preserves aspect ratio)
  4. Stops when `textComplete == 1` or reaches max scale (4.0x) or max iterations (10)
  5. Scales down using equal zoom for both dimensions (`zoom = width / tempBox.width`)

**Pros:**
- Maintains original font size and readability
- Good for preserving text clarity
- High-quality rendering with SDL2 anisotropic filtering (`SDL_ScaleModeBest`)
- Competitive quality with ScaleFont approach

**Cons:**
- Creates larger internal box (uses more memory)
- Requires texture filtering support (enabled in SDL2 build)

#### 3. Scale Font (`adaptive="scalefont"`)
- **How it works**: Iteratively reduces font size until text fits within the box dimensions
- **Algorithm**:
  1. Checks if text already fits (if yes, done)
  2. Calculates ratio of `heightNeeded / height`
  3. Reduces font size based on how far off we are:
     - Ratio > 2.0: Reduce by 20% (very far off)
     - Ratio > 1.5: Reduce by 15% (moderately far)
     - Ratio ≤ 1.5: Reduce by 10% (close)
  4. Minimum font size: 8pt
  5. Maximum iterations: 20

**Pros:**
- Maintains box dimensions exactly
- Text remains sharp (no scaling artifacts)
- More memory efficient

**Cons:**
- Reduces text size (may hurt readability for very long text)
- Iterative process (slightly slower, but typically <5 iterations)

## New Dependencies: `numTextLines` and `textComplete` Properties

The `AdaptiveTextBox` relies on two new properties added to `PTextBox`:

```pebl
box.numTextLines   ## Returns the number of lines rendered (that fit in the box)
box.lineHeight     ## Returns the height of each line in pixels
box.textComplete   ## Returns 1 if all text rendered, 0 if truncated
```

**Important**: `numTextLines` only counts lines that **fit in the box**, not total lines needed. The `textComplete` property is the stopping condition that tells us when all text has been successfully rendered.

## Implementation Notes

### ScaleBox Algorithm
- **Aspect ratio preservation**: Both width and height are scaled by the same factor at each iteration
- **Iterative expansion**: Increases scale by 10% per iteration (1.0 → 1.1 → 1.21 → 1.33...)
- **Stopping condition**: Uses `textComplete` property to detect when all text is rendered
- **Equal zoom**: Final zoom uses same value for both X and Y (`zoom = width / tempBox.width`)
- **Result**: Maintains exact aspect ratio of original box throughout process

### ScaleFont Algorithm Improvements
- **Adaptive reduction**: Reduces font more aggressively when ratio is high
- **Early termination**: Stops when text fits or reaches minimum font size (8pt)
- **Smart iteration**: Typically converges in 3-5 iterations

### Rendering Quality Improvements

**SDL2 Texture Filtering**: The implementation now uses `SDL_SetTextureScaleMode(texture, SDL_ScaleModeBest)` which enables anisotropic filtering for high-quality scaled rendering. This dramatically improves ScaleBox quality, making it competitive with ScaleFont.

**Files modified for filtering**:
- `src/platforms/sdl/PlatformTextBox.cpp` - Added `SDL_ScaleModeBest` after texture creation
- `src/platforms/sdl/PlatformLabel.cpp` - Added `SDL_ScaleModeBest` for consistency

### Known Limitations

1. **MaxLines truncation**: When `maxlines` is exceeded, text is truncated. Consider warning users or providing scrolling for very long text.

2. **Test 5 issue**: At very small font sizes (when maxlines=5 with longText), readability becomes poor. This is expected behavior but highlights the tradeoff.

## Test Results

The test suite (`test-adaptive-textbox.pbl`) demonstrates:

- **TEST 1**: Short text - no adaptation needed (all strategies produce same result)
- **TEST 2**: Medium text - both strategies adapt successfully, scalefont preferred for clarity
- **TEST 3**: Long text in tiny box - extreme adaptation, scalefont handles better
- **TEST 4**: Narrow boxes - tests wrapping, both strategies work
- **TEST 5**: MaxLines limit - shows truncation behavior

## Recommendations

### When to use ScaleBox:
- Font size must be preserved (e.g., accessibility requirements)
- High-quality rendering is available (SDL2 with anisotropic filtering)
- Memory is not a constraint
- Now competitive with ScaleFont for quality

### When to use ScaleFont:
- Box dimensions must be exact
- Memory efficiency matters
- Text length is moderate (font won't get too small)
- Slightly faster (no iterative scaling and zoom)

### When to use neither (adaptive=0):
- You control text length precisely
- Overflow is acceptable/handled elsewhere
- Maximum performance needed

## Future Enhancements

Possible improvements:
1. Add scrolling support for very long text
2. Hybrid approach: try scalefont first, fall back to scalebox if font gets too small
3. Add `minFontSize` as a parameter
4. Return metadata about what adaptation was performed
5. Support for canvas-based approaches with better rendering

## Files Modified

### Property System:
- `src/objects/PTextBox.cpp` - Added `NUMTEXTLINES` and `TEXTCOMPLETE` property initialization
- `src/platforms/sdl/PlatformTextBox.h` - Added `GetProperty()` declaration
- `src/platforms/sdl/PlatformTextBox.cpp` - Implemented `numTextLines` and `textComplete` properties in FindBreaks()

### Rendering Quality:
- `src/platforms/sdl/PlatformTextBox.cpp` - Added `SDL_SetTextureScaleMode(mTexture, SDL_ScaleModeBest)` for anisotropic filtering
- `src/platforms/sdl/PlatformLabel.cpp` - Added `SDL_SetTextureScaleMode(mTexture, SDL_ScaleModeBest)` for consistency

### Implementation & Documentation:
- `test-adaptive-textbox.pbl` - Implementation and test suite
- `ADAPTIVE_TEXTBOX_README.md` - This documentation

## Ready for Integration

The `AdaptiveTextBox` function and its helpers are ready to be moved to `pebl-lib/UI.pbl` or `pebl-lib/Utility.pbl` when approved.
