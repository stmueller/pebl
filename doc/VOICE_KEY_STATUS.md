# Voice Key Status and Known Issues

**Last Updated:** 2025-11-19
**PEBL Version:** 2.x (pre-SDL3 migration)

## Summary

PEBL's voice key functionality (`GetVocalResponseTime()`) **works reliably with manual threshold specification**. An experimental ROC-based automatic calibration system was implemented but has unresolved heap corruption issues and should not be used or documented until SDL3 migration.

---

## Working Functionality

### Basic Voice Key (STABLE - Use This)

The core voice key works correctly when you provide a threshold manually:

```pebl
buffer <- MakeAudioInputBuffer(3000)  # 3-second buffer
rt <- GetVocalResponseTime(buffer, 0.019, 100)
# Returns: [onset_time, offset_time, detected]
```

**Parameters:**
- `buffer`: Audio buffer created with `MakeAudioInputBuffer(milliseconds)`
- `threshold`: Energy threshold (typical range: 0.01 to 0.05)
- `sustain`: Sustain time in ms (typical: 100ms)

**How to find a good threshold:**
1. Run pilot trials with a fixed threshold (e.g., 0.019)
2. Save audio files using `SaveAudioToWaveFile()`
3. Examine false positives/negatives
4. Adjust threshold up (fewer false positives) or down (fewer misses)

**Example test:** `demo/testaudioin.pbl`

---

## Experimental Calibration System (UNSTABLE - Do Not Use)

### What Was Implemented

An ROC (Receiver Operating Characteristic) curve-based automatic calibration system:

**Files:**
- `demo/voicekey-calibration.pbl` - Full calibration with silence/speech collection
- `demo/voicekey-simple-test.pbl` - Manual threshold testing
- `src/platforms/sdl/PlatformAudioIn.cpp` - Audio monitoring implementation
- `src/libs/PEBLObjects.cpp` - `StartAudioMonitor()`, `GetAudioStats()`, `StopAudioMonitor()`

**Features:**
1. **Audio Monitor** - Continuous ring buffer for real-time audio statistics
2. **Silence Collection** - Records baseline noise characteristics
3. **Speech Collection** - Records speech onset characteristics
4. **ROC Analysis** - Finds optimal threshold by minimizing false positives/negatives
5. **Real-time Stats** - `GetAudioStats(monitor, milliseconds)` returns `[energy, power, rmssd, signs, directions]`

**New PEBL Functions:**
```pebl
monitor <- StartAudioMonitor(bufferSize)  # Ring buffer for continuous monitoring
stats <- GetAudioStats(monitor, 10)       # Get last N milliseconds of stats
StopAudioMonitor(monitor)                 # Stop and close monitor
```

### Known Issues - HEAP CORRUPTION CRASH

**Symptom:**
```
malloc(): invalid size (unsorted)
Thread 1 "pebl2" received signal SIGABRT, Aborted.
```

**When it crashes:**
- After running full calibration (silence + speech collection)
- When attempting to use voice key with the calibrated threshold
- Specifically at line creating `counted_ptr<PEBLObjectBase>` in `VoiceKey()` return value

**Root cause:** Heap corruption from audio buffer lifecycle issues

**Investigation findings:**

1. **AudioInfo buffer lifecycle problem:**
   - `AudioInfo` struct contains malloc'd audio buffer (`Uint8 *audio`)
   - Multiple `counted_ptr<AudioInfo>` can reference same struct
   - Buffer may be freed while still in use by SDL subsystem
   - Attempted fix: Added `AudioInfo` destructor to free buffer
   - Result: Made problem worse - destructor called while buffer still needed

2. **Reference counting complexity:**
   - Audio monitor creates AudioInfo with ring buffer
   - `GetAudioStats()` reads from it (needs thread-safe access)
   - Voice key creates separate AudioInfo with fill buffer
   - Both use global `gAudioBuffer` pointer for SDL callbacks
   - Buffer ownership unclear when monitor stopped but PEBL script holds reference

3. **Thread safety issues:**
   - SDL audio callbacks run in separate thread
   - Added `SDL_LockAudioDevice()` / `SDL_UnlockAudioDevice()` to `GetRecentAudioStats()`
   - Added thread-safe cleanup in `CloseAudio()`:
     ```cpp
     SDL_LockAudioDevice(mAudioDevice);
     SDL_PauseAudioDevice(mAudioDevice, 1);
     gAudioBuffer = NULL;
     SDL_UnlockAudioDevice(mAudioDevice);
     SDL_Delay(200);  // Wait for callbacks to drain
     SDL_CloseAudioDevice(mAudioDevice);
     ```
   - Still crashes - heap already corrupted before `CloseAudio()`

4. **When voice key works vs. crashes:**
   - ✓ Works: Skip calibration (`gSkipCalibration = 1`), use manual threshold
   - ✗ Crashes: Run full calibration, then use voice key
   - Indicates: Audio monitor leaves corrupted state even after `StopAudioMonitor()`

**Attempted fixes:**
1. Thread-safe buffer access with SDL locks ❌
2. Longer delay (200ms) for callback drain ❌
3. AudioInfo destructor to free malloc'd buffer ❌ (made worse)
4. Remove destructor free() to allow leak ❌ (still crashes)
5. Explicit buffer cleanup in `CloseAudio()` ❌

**Current workaround:**
- AudioInfo destructor does NOT free the audio buffer
- Accepts ~264KB memory leak per calibration run
- **Still crashes anyway** - heap corruption happens before buffer free

---

## Code Changes Summary

### Modified Files

**Header:**
- `src/platforms/sdl/PlatformAudioIn.h`
  - Renamed `Stop()` → `PauseAudioMonitor()` (avoid confusion with playback Stop)
  - Added `CloseAudio()` method for explicit cleanup
  - Added `GetRecentAudioStats()` for ring buffer statistics
  - Added `AudioInfo` constructor and destructor (currently no-op free)

**Implementation:**
- `src/platforms/sdl/PlatformAudioIn.cpp`
  - `AudioInCallbackLoop()` - Ring buffer implementation for continuous monitoring
  - `GetRecentAudioStats()` - Thread-safe ring buffer read with SDL locks
  - `CloseAudio()` - Thread-safe cleanup: pause, clear pointer, wait, close
  - `PauseAudioMonitor()` - Pause recording without closing device
  - Extensive debug output (left in for future debugging)

**PEBL Interface:**
- `src/libs/PEBLObjects.cpp`
  - `StartAudioMonitor()` - Create PlatformAudioIn with type 2 (ring buffer)
  - `GetAudioStats()` - Wrapper for `GetRecentAudioStats()`
  - `StopAudioMonitor()` - Calls `CloseAudio()` to clean up
  - `RecordToBuffer()` - Updated to call `PauseAudioMonitor()`

### Debug Output Added

Comprehensive tracing for future debugging:
- Buffer creation/destruction with addresses and sizes
- AudioInfo lifecycle (constructor/destructor)
- PlatformAudioIn lifecycle
- CloseAudio() cleanup sequence
- VoiceKey() return value creation steps

---

## Recommendations

### Short Term (Current Release)

1. **Document only manual threshold voice key** - This works reliably
2. **Do not expose calibration system** - Experimental, crashes
3. **Keep calibration code** - Useful for SDL3 migration reference
4. **Test manual voice key on Windows** - Ensure basic functionality works

### Medium Term (SDL3 Migration)

SDL3 has completely redesigned audio architecture:
- New audio stream API
- Better lifecycle management
- Clearer buffer ownership
- Modern thread-safety primitives

**Migration strategy:**
1. Port basic voice key first (manual threshold)
2. Test thoroughly on Linux/Windows/macOS
3. Re-implement audio monitoring with SDL3 streams
4. Rebuild calibration system with proper buffer lifecycle

### Long Term

**Proper calibration implementation needs:**
1. Clear buffer ownership model (who frees what, when)
2. Proper notification when SDL is done with buffer
3. Maybe: Copy buffer data instead of sharing pointers
4. Maybe: Use SDL3 audio streams instead of callbacks
5. Comprehensive automated tests for buffer lifecycle

---

## Testing

### What to Test Before Release

**Manual threshold voice key (must work):**
```bash
bin/pebl2 demo/testaudioin.pbl
```
- Should record speech
- Should detect onset/offset
- Should save .wav files
- Should NOT crash

**What NOT to test:**
- `demo/voicekey-calibration.pbl` (crashes)
- Any ROC-based threshold finding
- `StartAudioMonitor()` / `StopAudioMonitor()` functions

### Platform Testing Checklist

- [ ] Linux: Manual threshold voice key works
- [ ] Windows: Manual threshold voice key works
- [ ] macOS: Manual threshold voice key works (if audio input available)

---

## For Future Developers

### If You Want to Fix This

**Read first:**
1. This document
2. Debug output from running `demo/voicekey-calibration.pbl`
3. `Notes_for_Claude_on_Programming_PEBL.txt` (general PEBL architecture)

**Key insight:**
The crash happens during basic heap allocation (`new size_t(1)` in counted_ptr constructor), which means the heap was corrupted earlier by:
- Double-free of audio buffer
- Buffer freed while SDL still using it
- Buffer overrun in audio callback
- Race condition between threads

**Debugging approach:**
1. Enable all debug output (already present)
2. Run under Valgrind: `valgrind --leak-check=full bin/pebl2 demo/voicekey-calibration.pbl`
3. Use gdb with catchpoints: `catch signal SIGABRT`
4. Examine heap corruption detection: `MALLOC_CHECK_=3`

**SDL3 migration benefits:**
- Modern audio stream API with clearer ownership
- Better documentation of buffer lifecycle
- Thread-safe buffer management built-in
- Callback vs. stream model choice

---

## References

**Working Examples:**
- `demo/testaudioin.pbl` - Simple manual threshold test
- `battery/*/` - Many battery tasks use voice key successfully

**SDL2 Documentation:**
- Audio capture: https://wiki.libsdl.org/SDL2/CategoryAudio
- Thread safety: https://wiki.libsdl.org/SDL2/SDL_LockAudioDevice

**Related Code:**
- `src/platforms/sdl/PlatformAudioOut.cpp` - Audio playback (works fine)
- `src/utility/rc_ptrs.h` - Reference counted pointers implementation

---

## Conclusion

**Current status:** Voice key with manual threshold is production-ready and should be documented. ROC-based automatic calibration is experimental, crashes due to heap corruption, and should remain undocumented until SDL3 migration provides opportunity to rebuild with proper buffer lifecycle management.

**For users:** Use manual threshold. Test with your specific hardware/environment. Typical threshold range is 0.01-0.05, with 0.019 as a good starting point.

**For developers:** The calibration code is a good reference implementation of the algorithm, but the buffer lifecycle issues need fundamental architectural changes that SDL3 migration will enable.
