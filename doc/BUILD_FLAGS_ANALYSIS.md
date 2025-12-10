# Build Flags Analysis: USE_NETWORK, USE_PORTS, USE_WAAVE

**Date:** 2025-11-18
**Purpose:** Analyze optional build flags to determine if separate native/Emscripten handling is needed

---

## Current Status

### Build Flags (Makefile lines 37-42)

```makefile
#USE_WAAVE=1       ##Optional; comment out to turn off waave multimedia library
#USE_AUDIOIN=1     ##Optional; comment out to turn off sdl_audioin library
#USE_NETWORK=1      ##Optional; comment out to turn off sdl_net library.
#USE_PORTS=1        ##lpt, serial port, etc.
USE_HTTP=1         ##Optional; turn on/off for http get/set
USE_MIXER=1        ##Optional; uses sdl mixer for better audio+ogg/mp3/etc.
```

**All optional flags are currently DISABLED (commented out)** except:
- `USE_HTTP=1` - Always enabled for both native and Emscripten
- `USE_MIXER=1` - Always enabled for both native and Emscripten

---

## Flag-by-Flag Analysis

### 1. USE_WAAVE (Line 37)

**What it does:**
- Defines: `-DPEBL_MOVIES`
- Links: `-lwaave`
- Purpose: Video playback via WAAVE library

**Source code impact:**
- `src/libs/PEBLEnvironment.cpp` - Movie functions guarded by `#ifdef PEBL_MOVIES`
- `src/libs/PEBLObjects.cpp` - Movie object creation
- `src/platforms/sdl/PlatformMovie.cpp` - Entire file guarded
- `src/apps/PEBL.cpp` - Movie initialization code

**Current Status:** ❌ **DISABLED**

**User notes:**
> "USE_WAAVE=1 is really an old library that I never got working in SDL2, and probably can be replaced."

**Recommendation:**
- **REMOVE this flag entirely** - it's non-functional legacy code
- Clean up the `#ifdef PEBL_MOVIES` guards in source files if/when movie support is reimplemented
- For now, leave disabled - no platform-specific handling needed

---

### 2. USE_NETWORK (Line 39)

**What it does:**
- Defines: `-DPEBL_NETWORK`
- Links: `-lSDL2_net` (native only)
- Purpose: TCP/UDP networking via SDL_net

**Source code impact:**
- `src/libs/PEBLStream.cpp` - Network functions (`GetTCPServer`, `GetUDPConnection`, etc.)
- `src/platforms/sdl/PlatformNetwork.cpp` - SDL_net wrapper implementation
- When disabled: stub implementations provided (line 122 in PEBLStream.cpp)

**Emscripten support:**
- Emscripten **ALWAYS includes** `-sUSE_SDL_NET=2` (line 90)
- SDL2_net is available in Emscripten's port system
- TCP/UDP work via WebSocket proxying

**Current Status:** ❌ **DISABLED for native**, ✅ **ALWAYS ENABLED for Emscripten**

**Inconsistency Detected:**
- Native builds: `USE_NETWORK` commented out → stub functions only
- Emscripten builds: `-sUSE_SDL_NET=2` always included → full networking available
- This means Emscripten has network capabilities that native builds lack!

**User notes:**
> "I'm not sure if it matters, but ports probably does not work in emscripten, but sdl_net could work, as is exposes TCP and UDP comms."

**Recommendation:**
```makefile
# Enable USE_NETWORK by default for both platforms
USE_NETWORK=1

# Native build already handles this via:
ifdef USE_NETWORK
    CXXFLAGS3 = -DPEBL_NETWORK
    LINKOPTS3 = -lSDL2_net
endif

# Emscripten already has -sUSE_SDL_NET=2 in CXXFLAGS_EMSCRIPTEN
```

**Action:** Enable `USE_NETWORK=1` by uncommenting line 39. This gives feature parity between native and Emscripten builds.

---

### 3. USE_PORTS (Line 40)

**What it does:**
- Defines: `-DPEBL_USEPORTS`
- Links: None (uses inline C code)
- Purpose: Serial port (COM) and parallel port (LPT) access for hardware interfacing

**Source code impact:**
- `src/devices/PComPort.cpp/h` - Entire files guarded by `#ifdef PEBL_USEPORTS`
- `src/devices/PParallelPort.cpp/h` - Parallel port support
- `src/libs/Functions.h` - Port functions registered conditionally
- Uses `src/utility/rs232.c` - cross-platform serial library

**Emscripten compatibility:**
- **NOT AVAILABLE** - Web browsers cannot access serial/parallel ports
- Browser security model forbids direct hardware access
- WebSerial API exists but requires different implementation

**Current Status:** ❌ **DISABLED**

**Platform differences:**
- Native Linux: ✅ Can access `/dev/ttyS*`, `/dev/ttyUSB*` (requires permissions)
- Emscripten: ❌ Cannot access hardware ports (browser sandbox)

**User notes:**
> "ports probably does not work in emscripten"

**Recommendation:**
```makefile
# Only enable for native builds
# Leave disabled by default (requires hardware and permissions)
# Users can uncomment if they need port access
#USE_PORTS=1

# If we want to enable it by default for native only:
ifeq ($(findstring em,$(MAKECMDGOALS)),)
    # Not an Emscripten target
    USE_PORTS=1
endif
```

**Action:**
- **Option A (conservative):** Leave disabled by default (current state). Users who need port access can uncomment manually.
- **Option B (convenient):** Enable for native builds only using conditional logic above.

Since port access requires hardware and special permissions, **Option A is recommended** - keep it opt-in.

---

### 4. USE_AUDIOIN (Line 38)

**What it does:**
- Defines: `-DPEBL_AUDIOIN`
- Links: None currently (commented out: `-lsdl_audioin -lsndio`)
- Purpose: Audio input/recording

**Source code impact:**
- Minimal - appears to be incomplete implementation
- Comment says "audio in now supported with baseline SDL supposedly" (line 106)

**Current Status:** ❌ **DISABLED**

**Recommendation:** Leave disabled - appears to be unfinished/superseded.

---

## Summary and Recommendations

| Flag | Current State | Native | Emscripten | Recommendation |
|------|--------------|--------|------------|----------------|
| USE_WAAVE | Disabled | N/A | N/A | **Remove** - non-functional legacy |
| USE_NETWORK | Disabled | Could work | Always enabled | **Enable for native** - achieve parity |
| USE_PORTS | Disabled | Could work | Cannot work | **Keep disabled** - opt-in only |
| USE_AUDIOIN | Disabled | Incomplete | Unknown | **Keep disabled** - unfinished |
| USE_HTTP | Enabled | ✅ Works | ✅ Works | **Keep enabled** - already good |
| USE_MIXER | Enabled | ✅ Works | ✅ Works | **Keep enabled** - already good |

---

## Proposed Makefile Changes

### Minimal Change (Recommended)

Simply enable USE_NETWORK for feature parity:

```makefile
# Line 39 - uncomment:
USE_NETWORK=1      ##Optional; comment out to turn off sdl_net library.
```

**Impact:**
- Native builds gain TCP/UDP networking (requires `libsdl2-net-dev`)
- Matches Emscripten capability
- No code changes needed - just flip the flag

---

### Advanced Change (Platform-Specific)

If you want explicit platform separation:

```makefile
#USE_WAAVE=1       ##Optional; comment out to turn off waave multimedia library
#USE_AUDIOIN=1     ##Optional; comment out to turn off sdl_audioin library

# Network support - enable for both platforms
USE_NETWORK=1      ##TCP/UDP via SDL_net (works on native and Emscripten)

# Serial/parallel ports - native only (not available in browsers)
# Uncomment if you need hardware port access on Linux
#USE_PORTS=1

USE_HTTP=1         ##Optional; turn on/off for http get/set
USE_MIXER=1        ##Optional; uses sdl mixer for better audio+ogg/mp3/etc.
```

**No conditional logic needed** - the existing `ifdef USE_NETWORK` already works correctly for both platforms.

---

## Testing Plan

After enabling USE_NETWORK:

### Native Build
```bash
# Ensure SDL2_net is installed
sudo apt-get install libsdl2-net-dev  # Debian/Ubuntu
# or
sudo dnf install SDL2_net-devel       # Fedora/RHEL

# Build and verify
make clean && make main

# Test network functions (if you have test scripts)
bin/pebl2 test-network.pbl
```

### Emscripten Build
No changes needed - already enabled via `-sUSE_SDL_NET=2`

```bash
make clean && make em-test
```

**Expected behavior:**
- Both builds should have identical network function availability
- Functions like `OpenTCPServer()`, `OpenUDPConnection()` should be available
- No stub implementations

---

## Dependencies

### Native Linux
```bash
# Required for USE_NETWORK=1
sudo apt-get install libsdl2-net-dev

# Already required (no change)
libsdl2-dev
libsdl2-image-dev
libsdl2-ttf-dev
libsdl2-mixer-dev
libsdl2-gfx-dev
libcurl4-openssl-dev
```

### Emscripten
No changes - SDL2_net port downloads automatically during build

---

## Conclusion

**✅ IMPLEMENTED (2025-11-18)**

### Changes Made

1. **✅ Enabled USE_NETWORK=1** (Makefile line 38)
   - Gives native builds TCP/UDP networking via SDL2_net
   - Achieves feature parity with Emscripten builds

2. **✅ Enabled USE_PORTS=1** (Makefile line 39)
   - Enables serial/parallel port access on native Linux
   - Not available in Emscripten (browser security limitations)
   - Fixed API compatibility: Updated `RS232_OpenComport()` call in `PComPort.cpp` to include 4th parameter (flowctrl=0)

3. **✅ Fixed Makefile Build System**
   - Added special rule for `lex.yy.c` to compile with `-Wno-register` flag (suppresses deprecated keyword warnings)
   - Pure C files (rs232.c) compile with C compiler (`clang -std=gnu99`)
   - Lexer file (lex.yy.c) compiles with C++ compiler due to C++ header dependencies
   - Removed obsolete `lex.yy.cpp` file that was shadowing the current `lex.yy.c`

4. **✅ Updated PEBLBASE_SRC** (Makefile line 282)
   - Changed `lex.yy.c` to `$(BASE_DIR)/lex.yy.c` for proper path resolution

### Build Result

**Status:** ✅ Build successful with **ZERO warnings**
**Binary:** `bin/pebl2` (7.4 MB)
**Build flags active:**
- `-DPEBL_MIXER` (audio mixing)
- `-DPEBL_NETWORK` (TCP/UDP via SDL2_net)
- `-DPEBL_USEPORTS` (serial/parallel ports)
- `-DPEBL_HTTP` (HTTP via libcurl)

### Platform Comparison

| Feature | Native Linux (NOW) | Emscripten | Notes |
|---------|-------------------|------------|-------|
| TCP/UDP Networking | ✅ SDL2_net | ✅ SDL2_net (WebSocket proxy) | Feature parity achieved |
| Serial Ports | ✅ rs232.c | ❌ Not available | Hardware access, native only |
| Parallel Ports | ✅ Native I/O | ❌ Not available | Hardware access, native only |
| HTTP | ✅ libcurl | ✅ Fetch API | Different backends |
| Audio Mixing | ✅ SDL2_mixer | ✅ SDL2_mixer | Both platforms |

### Files Modified

1. `Makefile`
   - Lines 38-39: Enabled USE_NETWORK and USE_PORTS
   - Line 282: Fixed PEBLBASE_SRC path
   - Lines 609-613: Added special compilation rules for C files

2. `src/devices/PComPort.cpp`
   - Line 77: Updated `RS232_OpenComport()` call to include 4th parameter (flowctrl=0)

3. Cleanup
   - Removed obsolete `src/base/lex.yy.cpp` file

**No separate native/Emscripten logic needed** - the existing Makefile structure already handles platform differences correctly.
