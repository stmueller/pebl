# PEBL Syntax Verification from Actual Code

This document verifies PEBL syntax rules by examining actual code in battery/ and demo/.

## Not-Equal Operators - All Three Variants Are Valid

### 1. `!=` (Most Common)
**Usage**: Modern C-style operator, frequently used
**Count**: Many occurrences

Examples:
```pebl
if ( rval != 0 )  # symmetryspan.pbl
if ( write != 0 and First(result)=="D")  # symmetryspan.pbl
if((curX != x.x) or curY != x.y)  # dexterity.pbl
```

### 2. `<>` (Traditional)
**Usage**: Classic "not equal" operator from BASIC/Pascal
**Count**: 69 occurrences in battery/

Examples:
```pebl
if ( gSymSpanHighlightPickedFaces <> 0 )  # symmetryspan.pbl
while(resp <> 10)  # mspan/buildup.pbl
while(resp <> 10)  # mspan/staircase.pbl
```

### 3. `~=` (Rare)
**Usage**: Alternative operator, rarely used
**Count**: 6 occurrences (all in testsyntax.pbl)

Examples:
```pebl
TestValue(3~=3,0,"3~=3")    # testsyntax.pbl (tests equality)
TestValue(4~=3,1,"4~=3")    # testsyntax.pbl (tests inequality)
TestValue(4~=5,1,"4~=5")    # testsyntax.pbl (tests inequality)
```

## Summary

All three operators (`!=`, `<>`, `~=`) are:
- ✅ Defined in the official lexer (`src/base/Pebl.l`)
- ✅ Used in actual PEBL code
- ✅ Included in the Pygments syntax highlighter

**Recommendation**: All three should remain in the syntax highlighter for compatibility, though `!=` and `<>` are the primary forms used in practice.
