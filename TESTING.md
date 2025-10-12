# Testing Guide for Asyncify Implementation

## Build Status
✅ Build completed successfully with Asyncify flags enabled
✅ test.pbl includes comprehensive diagnostic tests
✅ All source files modified and rebuilt

## How to Test

### 1. Start Local Web Server
```bash
cd bin
python -m http.server 8000
```

### 2. Open in Browser
Navigate to: http://localhost:8000/pebl2.html

### 3. Test Sequence
The test.pbl script will automatically run through 5 diagnostic stages:

#### Test 1: Basic Drawing (Auto)
- **Expected**: Label appears saying "Test 1: If you see this, drawing works"
- **Expected**: Wait 1 second, then automatically proceed
- **Console**: Should show "Wait(1000) completed"

#### Test 2: WaitForAnyKeyPress (Manual)
- **Expected**: Label appears saying "Test 2: Press any key..."
- **Action**: Press ANY key on keyboard
- **Expected**: Should immediately detect the key and proceed
- **Console**: Should show "WaitForAnyKeyPress() returned: [KEY]"

#### Test 3: Multiple Rapid Keys (Manual)
- **Expected**: Label says "Test 3: Press 5 keys rapidly..."
- **Action**: Press 5 keys quickly, one after another
- **Expected**: Counter updates after each keypress showing "Keys pressed: N (last: KEY)"
- **Console**: Should show all 5 keys being detected
- **CRITICAL**: This tests the original problem - all 5 keys should be captured

#### Test 4: GetInput (Manual)
- **Expected**: Label says "Test 4: Type text, press RETURN"
- **Expected**: Text box appears
- **Action**: Type some text, then press RETURN/ENTER
- **Expected**: Each character should appear as you type (no delays)
- **Console**: Should show "GetInput() returned: [YOUR TEXT]"

#### Test 5: Results (Manual)
- **Expected**: Shows "You typed: [YOUR TEXT]"
- **Expected**: Shows "All tests complete! Press any key to exit."
- **Action**: Press any key
- **Expected**: Program exits cleanly
- **Console**: Should show "=== ALL TESTS COMPLETE ==="

## What to Look For

### Success Indicators:
- ✅ All 5 keys in Test 3 are detected immediately (original issue: only 1/60 worked)
- ✅ GetInput in Test 4 shows each character as typed (original issue: polling delays)
- ✅ No freezing or browser unresponsiveness
- ✅ Console shows all Print() statements in order

### Failure Indicators:
- ❌ Browser freezes/becomes unresponsive
- ❌ Keys are missed in Test 3
- ❌ GetInput doesn't respond to typing
- ❌ Program stops between tests
- ❌ Console shows errors

## Console Output
Open browser Developer Tools (F12) and check Console tab for diagnostic output.

## Architecture Changes
This implementation uses Emscripten Asyncify to enable proper blocking behavior:
- `emscripten_sleep(10)` in Loop1() yields to browser while waiting for events
- Evaluator runs with periodic `emscripten_sleep(0)` yields every 100 steps
- SDL_PumpEvents() is called via gEventQueue->Prime() in the event loop
- No more polling workarounds in EM.pbl

## If Tests Fail
Report which test number fails and include:
1. Console output up to the failure point
2. Whether browser froze or just stopped progressing
3. Any JavaScript errors in console
