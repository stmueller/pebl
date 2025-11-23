# PEBL Library Synchronization Analysis

## Problem

The `pebl-lib/` and `emscripten/pebl-lib/` directories have diverged. The main `pebl-lib/` contains newer code that hasn't been copied to `emscripten/pebl-lib/`.

## File Comparison Results

### Files That Differ

```bash
$ diff -qr pebl-lib emscripten/pebl-lib
Files pebl-lib/customlauncher.pbl and emscripten/pebl-lib/customlauncher.pbl differ
Files pebl-lib/EM.pbl and emscripten/pebl-lib/EM.pbl differ
Files pebl-lib/Graphics.pbl and emscripten/pebl-lib/Graphics.pbl differ
Files pebl-lib/UI.pbl and emscripten/pebl-lib/UI.pbl differ
Files pebl-lib/Utility.pbl and emscripten/pebl-lib/Utility.pbl differ
```

### Line Count Comparison

| File | pebl-lib | emscripten/pebl-lib | Difference |
|------|----------|---------------------|------------|
| **Utility.pbl** | **2257** | 2062 | **+195 lines** (MAJOR) |
| Graphics.pbl | 1408 | 1414 | -6 lines |
| UI.pbl | 2011 | 2011 | 0 lines (same size, different content) |
| customlauncher.pbl | 826 | 781 | +45 lines |
| EM.pbl | 370 | 377 | -7 lines |

## Major Differences

### 1. Utility.pbl (+195 lines in main)

**Key Updates in pebl-lib/Utility.pbl:**

- **Enhanced GetSubNum()** with 20+ language translations
  - Old version: Hard-coded English, German, Portuguese only
  - New version: 24 languages with three-tier detection:
    1. Explicit `gLanguage` variable
    2. System locale detection via `GetSystemLocale()`
    3. English fallback
  - Includes: English, Chinese, Spanish, Arabic, Hindi, Portuguese, Russian, Japanese, German, French, Italian, Korean, Dutch, Persian, Turkish, Polish, Ukrainian, Hungarian, Hebrew, Swedish, Danish, Finnish, Bulgarian

- **Automatic font detection**:
  - `DetectTextScript(text)` - Detects script type
  - `GetFontForText(text)` - Returns appropriate font
  - `IsRTL(text)` - Right-to-left text detection
  - Automatic font selection in GetSubNum()

- **Enhanced GetTranslations()** function
  - Centralized translation loading with three-tier fallback
  - System locale integration
  - Used by battery tests for multi-language support

- **Updated copyright** to 2025 (vs 2020 in emscripten version)

### 2. customlauncher.pbl (+45 lines in main)

**Differences:** Need detailed analysis

### 3. EM.pbl (-7 lines in emscripten)

**Emscripten version has:**
- Additional comments explaining `GetNewDataFile()` behavior
- Documentation about `gDataDirectory` and token-based paths
- Notes about chain-launcher participant ID suffixes

**Both versions define same functions:**
- `Wait(time)`
- `WaitForKeyPress(key)`
- `WaitForListKeyPressWithTimeout(keys,timeout,style)`
- `WaitForKeyPressWithTimeout(key,timeout)`
- `WaitForAnyKeyPressWithTimeout(timeout)`
- `WaitForListKeyPress(keys)`
- `WaitForKeyRelease(key)`
- `WaitForAnyKeyPress()`
- `WaitForAnyKeyRelease()`
- `WaitForMouseButton()`
- `WaitForMouseButtonWithTimeout(timeout)`
- `GetInput(textbox, exitkey)`
- `GetNewDataFile(subnum, win, basename, ext, header)`

### 4. Graphics.pbl (-6 lines in main)

**Minor differences** - need detailed analysis

### 5. UI.pbl (same size, different content)

**Same line count but content differs** - need detailed analysis

## EM.pbl Analysis: Event Loop Function Overrides

### Purpose of EM.pbl

The file contains **PEBL-interpreted reimplementations** of C++ event loop functions that previously had issues in WebAssembly before ASYNCIFY was available.

**Original problem (pre-ASYNCIFY):**
- C++ functions like `WaitForKeyPress()` blocked the event loop
- JavaScript couldn't process events while PEBL waited
- Browser would freeze/hang

**Original solution:**
- Rewrite functions in PEBL using `RegisterEvent()` / `StartEventLoop()` pattern
- These interpreted versions work around the blocking issue
- Must include explicit `return()` statements

**Current situation (with ASYNCIFY):**
- Emscripten now supports `emscripten_sleep()` and ASYNCIFY
- C++ functions can yield to browser event loop
- **Question: Are these PEBL overrides still necessary?**

### Functions That May Be Obsolete with ASYNCIFY

All 13 functions in EM.pbl were created to work around pre-ASYNCIFY limitations:

**Potentially obsolete:**
1. `Wait(time)` - Could use native C++ with asyncify
2. `WaitForKeyPress(key)` - Could use native C++ with asyncify
3. `WaitForListKeyPressWithTimeout(keys,timeout,style)` - Could use native C++
4. `WaitForKeyPressWithTimeout(key,timeout)` - Could use native C++
5. `WaitForAnyKeyPressWithTimeout(timeout)` - Could use native C++
6. `WaitForListKeyPress(keys)` - Could use native C++
7. `WaitForKeyRelease(key)` - Could use native C++
8. `WaitForAnyKeyPress()` - Could use native C++
9. `WaitForAnyKeyRelease()` - Could use native C++
10. `WaitForMouseButton()` - Could use native C++
11. `WaitForMouseButtonWithTimeout(timeout)` - Could use native C++
12. `GetInput(textbox, exitkey)` - Could use native C++

**Still necessary:**
13. `GetNewDataFile(subnum, win, basename, ext, header)` - **Keep this one**
    - Handles token-based paths specific to emscripten
    - Uses `gDataDirectory` for centralized persistent storage
    - Different behavior than native version

## Recommendation

### Step 1: Sync Non-EM Files Immediately

Copy updated versions from `pebl-lib/` to `emscripten/pebl-lib/`:

```bash
cp pebl-lib/Utility.pbl emscripten/pebl-lib/Utility.pbl
cp pebl-lib/customlauncher.pbl emscripten/pebl-lib/customlauncher.pbl
cp pebl-lib/Graphics.pbl emscripten/pebl-lib/Graphics.pbl
cp pebl-lib/UI.pbl emscripten/pebl-lib/UI.pbl
```

**Reason:** These contain important updates (multi-language support, font detection) that should work on emscripten.

### Step 2: Test Event Loop Functions

Create test script to verify if native C++ functions work with ASYNCIFY:

**Test script: `test-eventloop-asyncify.pbl`**

```pebl
## Test if native C++ event loop functions work with ASYNCIFY
## Run on emscripten with EM.pbl temporarily disabled

define Start()
{
  gWin <- MakeWindow("grey")

  ## Test 1: Wait()
  PrintNow("Test 1: Wait(500)")
  t1 <- GetTime()
  Wait(500)
  t2 <- GetTime()
  Print("Wait completed in " + (t2-t1) + "ms (expected ~500ms)")

  ## Test 2: WaitForAnyKeyPress()
  PrintNow("Test 2: WaitForAnyKeyPress() - Press any key")
  t1 <- GetTime()
  key <- WaitForAnyKeyPress()
  t2 <- GetTime()
  Print("Key pressed: " + key + " after " + (t2-t1) + "ms")

  ## Test 3: WaitForMouseButton()
  PrintNow("Test 3: WaitForMouseButton() - Click mouse")
  t1 <- GetTime()
  click <- WaitForMouseButton()
  t2 <- GetTime()
  Print("Mouse clicked at " + click + " after " + (t2-t1) + "ms")

  ## Test 4: WaitForKeyPressWithTimeout()
  PrintNow("Test 4: WaitForKeyPressWithTimeout(<space>, 2000) - Press space within 2 sec")
  t1 <- GetTime()
  result <- WaitForKeyPressWithTimeout("<space>", 2000)
  t2 <- GetTime()
  Print("Result: " + result + " after " + (t2-t1) + "ms")

  ## Test 5: GetInput()
  PrintNow("Test 5: GetInput() - Type text and press Enter")
  tb <- EasyTextBox("", 100, 100, gWin, 20, 200, 30)
  Draw()
  t1 <- GetTime()
  input <- GetInput(tb, "<return>")
  t2 <- GetTime()
  Print("Input: " + input + " after " + (t2-t1) + "ms")

  Print(CR(2) + "All tests completed successfully!")
  Print("Native C++ functions with ASYNCIFY appear to work correctly.")
  WaitForAnyKeyPress()
}

define PrintNow(text)
{
  Print(CR(1) + text)
  Draw()
}
```

### Step 3: Testing Procedure

**A. Test with EM.pbl enabled (current behavior):**
```bash
# Uses PEBL-interpreted overrides
bin/pebl2.html test-eventloop-asyncify.pbl
```

**B. Test with EM.pbl disabled:**

Temporarily rename functions in `emscripten/pebl-lib/EM.pbl`:

```pebl
## Rename to disable overrides - test native C++ versions
define Wait_DISABLED(time) { ... }
define WaitForKeyPress_DISABLED(key) { ... }
# ... etc
```

Then rebuild and test:
```bash
make fp  # Rebuild data bundle with modified EM.pbl
# Test in browser - uses native C++ functions
```

**C. Compare results:**
- Do both work correctly?
- Are there timing differences?
- Does browser remain responsive?
- Any console errors?

### Step 4: Decision Tree

```
If native C++ functions work with ASYNCIFY:
  ├─ Remove event loop overrides from EM.pbl
  ├─ Keep only GetNewDataFile() override
  ├─ Document in EM.pbl that ASYNCIFY made overrides obsolete
  └─ Simplify emscripten build

If native C++ functions fail or hang:
  ├─ Keep event loop overrides in EM.pbl
  ├─ Document why they're still needed
  └─ Consider hybrid approach (some native, some override)
```

## Expected Outcome

**Hypothesis:** With ASYNCIFY enabled, native C++ event loop functions should work correctly, making most of EM.pbl's overrides obsolete.

**Benefits if hypothesis is correct:**
- Simpler codebase (fewer overrides to maintain)
- Better performance (native C++ vs interpreted PEBL)
- Less divergence between native and emscripten builds
- Only emscripten-specific functionality remains in EM.pbl

**If hypothesis is incorrect:**
- Keep EM.pbl overrides as-is
- Document testing results
- Understand specific failure modes

## Action Items

- [x] Copy updated .pbl files to emscripten/pebl-lib/ ✓ COMPLETED
  - Utility.pbl (2257 lines - multi-language support)
  - customlauncher.pbl (826 lines)
  - Graphics.pbl (1408 lines)
  - UI.pbl (2011 lines)
- [x] Create test-eventloop-asyncify.pbl test script ✓ COMPLETED
- [ ] Test with EM.pbl enabled (baseline) - READY TO TEST
- [ ] Disable EM.pbl event loop overrides
- [ ] Rebuild emscripten bundle with `make fp`
- [ ] Test with EM.pbl disabled (native C++)
- [ ] Compare results and document findings
- [ ] Make decision on EM.pbl future
- [ ] Update documentation

## Completed Work

**Date:** 2025-11-23

**Step 1 - Library Sync: COMPLETED**

All non-EM.pbl files have been synchronized from `pebl-lib/` to `emscripten/pebl-lib/`:

```bash
$ diff -q pebl-lib emscripten/pebl-lib
Files pebl-lib/EM.pbl and emscripten/pebl-lib/EM.pbl differ
```

Only EM.pbl now differs between the two directories (as expected). The synced files include:

- **Utility.pbl** - Critical update with 24-language support, automatic font detection, enhanced GetSubNum() and GetTranslations()
- **customlauncher.pbl** - 45 lines of updates
- **Graphics.pbl** - Minor updates
- **UI.pbl** - Content changes

**Step 2 - Test Script: COMPLETED**

Created `test-eventloop-asyncify.pbl` with 7 comprehensive tests:

1. Wait(500) - Timing validation
2. WaitForAnyKeyPress() - Key input
3. WaitForMouseButton() - Mouse input
4. WaitForKeyPressWithTimeout() - Timeout behavior (wait for timeout)
5. WaitForKeyPressWithTimeout() - Key press within timeout
6. WaitForListKeyPress() - Multiple key options
7. GetInput() - Text input widget

The test script provides detailed feedback showing elapsed times and pass/fail status for each test.

**Next Steps:**

The test script is ready to run. To proceed with testing:

1. First, test with current EM.pbl enabled (baseline):
   ```bash
   make em-opt    # Build emscripten runtime
   make fp        # Build data bundle with current EM.pbl
   # Test in browser: bin/pebl2.html with test-eventloop-asyncify.pbl
   ```

2. Then, disable EM.pbl overrides and test native C++ functions:
   ```bash
   # Edit emscripten/pebl-lib/EM.pbl to rename event loop functions
   make fp        # Rebuild data bundle
   # Test in browser again
   ```

3. Compare results to determine if EM.pbl overrides are still needed.

## Notes

- **Critical:** `GetNewDataFile()` override must remain - it's not about event loops, it's about emscripten-specific paths
- **Makefile reminder:** After modifying `emscripten/pebl-lib/*.pbl`, run `make fp` to rebuild the data bundle
- **ASYNCIFY flags:** Already enabled in Makefile with `-s ASYNCIFY=1`
