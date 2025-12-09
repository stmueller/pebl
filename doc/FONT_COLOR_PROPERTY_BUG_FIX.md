# Font Color Property Assignment Bug Fix

## Summary

Fixed a critical segmentation fault that occurred when assigning color properties to font objects using nested property syntax (e.g., `label.font.fgcolor <- MakeColor("red")`).

## Problem

The crash occurred in `PFont::SetProperty()` when setting FGCOLOR or BGCOLOR properties. The code was calling `PEBLObjectBase::SetProperty(name, v)` which triggered a copy assignment of the property map containing Variants with counted_ptr references, causing memory corruption.

### Crash Location

```
PFont.cpp:223 - PEBLObjectBase::SetProperty(name, v)
```

### Stack Trace

```
#0  std::_Rb_tree::operator=
#1  std::map::operator=
#2  PEBLObjectBase::operator=
#3  PColor::operator=
#4  PFont::SetFontColor
#5  PlatformFont::SetFontColor
#6  PFont::SetProperty
```

## Root Cause

The color objects (mFontColor, mBackgroundColor) are stored as `counted_ptr<PColor>` and registered in the property map during construction via `InitializeProperty()`. The property map entry points to the SAME PColor object.

When `PFont::SetProperty()` was called for color properties, it was:
1. Calling `PEBLObjectBase::SetProperty(name, v)` to update the map entry
2. This triggered the compiler-generated copy assignment operator for PEBLObjectBase
3. The copy assignment tried to copy the entire mProperties map
4. Copying Variants with counted_ptr caused memory corruption in the reference counting system

## Solution

**Remove the redundant `PEBLObjectBase::SetProperty()` call** for FGCOLOR and BGCOLOR properties.

The property map entry already points to the correct PColor object (created during construction). We only need to update the color values within that object, not replace the map entry.

### Code Changes

**Before (PFont.cpp lines 220-233):**
```cpp
else if (name == "FGCOLOR")
{
    // Store the variant in property map first
    PEBLObjectBase::SetProperty(name, v);  // <-- CAUSED CRASH
    PColor newColor = *(dynamic_cast<PColor*>(v.GetComplexData()->GetObject().get()));
    SetFontColor(newColor);
}
```

**After:**
```cpp
else if (name == "FGCOLOR")
{
    // Extract the new color and update our internal color object
    // Don't call PEBLObjectBase::SetProperty - the property map already points to mFontColor
    PColor newColor = *(dynamic_cast<PColor*>(v.GetComplexData()->GetObject().get()));
    SetFontColor(newColor);
}
```

**Also updated SetFontColor/SetBackgroundColor (PFont.cpp lines 285-301):**
```cpp
void PFont::SetFontColor(const PColor color)
{
    // Update individual color components to avoid problematic copy assignment
    mFontColor->SetRed(color.GetRed());
    mFontColor->SetGreen(color.GetGreen());
    mFontColor->SetBlue(color.GetBlue());
    mFontColor->SetAlpha(color.GetAlpha());
}
```

Previously used `*mFontColor = color` which triggered the problematic copy assignment operator.

## Test Cases

Created test files to verify the fix:
- `test-font-color-bug.pbl` - Original reproduction case
- `test-global-vs-local-property-bug.pbl` - Comprehensive test
- `test-set-bgcolor.pbl` - Minimal test case

All tests now pass without crashing.

## Impact

This fix allows safe reassignment of font colors in PEBL scripts:
```pebl
label <- EasyLabel("text", x, y, win, size)
label.font.fgcolor <- MakeColor("red")     # Now works
label.font.bgcolor <- MakeColor("white")   # Now works
label.font.fgcolor <- MakeColor("blue")    # Reassignment also works
```

## Files Modified

- `src/objects/PFont.cpp` - Removed redundant SetProperty calls, updated color assignment methods
- `battery/simon/simon.pbl` - Removed workaround (redundant color assignments)

## Related Commits

- 58bbbcb - "Fix nested property access for color modifications" (introduced the bug)
- 4ece1c1 - "Fix memory leaks in counted_ptr system" (made reference counting stricter, exposing the bug)

## Date

December 9, 2025
