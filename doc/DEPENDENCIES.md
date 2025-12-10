# PEBL External Dependencies

This document lists external libraries that are **not included** in the git repository and must be installed separately.

## Required for All Builds

### SDL2 and Extensions
Install via your system package manager:

**Ubuntu/Debian:**
```bash
sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-net-dev libsdl2-mixer-dev
```

**macOS (Homebrew):**
```bash
brew install sdl2 sdl2_image sdl2_ttf sdl2_net sdl2_mixer
```

**Fedora/RHEL:**
```bash
sudo dnf install SDL2-devel SDL2_image-devel SDL2_ttf-devel SDL2_net-devel SDL2_mixer-devel
```

### SDL2_gfx
**Location:** `libs/SDL2_gfx-1.0.4/` (excluded from git)

**Download and build:**
```bash
cd libs/
wget https://sourceforge.net/projects/sdl2gfx/files/SDL2_gfx-1.0.4.tar.gz
tar xzf SDL2_gfx-1.0.4.tar.gz
cd SDL2_gfx-1.0.4

# For native build:
mkdir build-linux
cd build-linux
../configure
make

# For emscripten build:
mkdir build-em
cd build-em
emconfigure ../configure
emmake make
```

## Required for Emscripten Builds

### Emscripten SDK
**Location:** `libs/emsdk/` (excluded from git)

See [EMSDK_SETUP.md](EMSDK_SETUP.md) for detailed installation instructions.

**Quick setup:**
```bash
cd libs/
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

## Optional Dependencies (Not Currently Used)

### Lab Streaming Layer (LSL)
**Location:** `libs/labstreaminglayer/` (excluded from git)
**Status:** Code exists in `src/utility/PLabStreamingLayer.cpp` but not actively used
**Size:** 118MB

If needed in the future:
```bash
cd libs/
git clone https://github.com/sccn/labstreaminglayer.git
```

### XID Device Library
**Location:** `libs/xid_device_library-master/` (excluded from git)
**Status:** Not referenced in source code
**Size:** 19MB

## Libraries Included in Repository

### utfcpp
**Location:** `libs/utfcpp/`
**Status:** ✓ Included in git repository
**Size:** 284KB
**Used by:** `src/objects/PTextBox.cpp`, `src/platforms/sdl/PlatformTextBox.cpp`

This is the only library tracked in git due to its small size and direct integration.

## Libraries Already in Source Tree

The following libraries are already included in `src/utility/` and do not need separate installation:

- **jsmn** - JSON parser (`src/utility/jsmn.h`)
- **happyhttp** - HTTP client (`src/utility/happyhttp.cpp`)

## Development Tools

### Bison and Flex
Required for parsing PEBL grammar:

**Ubuntu/Debian:**
```bash
sudo apt-get install bison flex
```

**macOS:**
```bash
brew install bison flex
```

### Build Tools
```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake

# macOS
xcode-select --install
brew install cmake
```

## Verification

After installing dependencies, verify the build:

```bash
# Native build
make main -j $(nproc)

# Emscripten build (after sourcing emsdk_env.sh)
make em -j $(nproc)
```

## Notes

- The `libs/` directory contains several unused libraries that are kept locally but excluded from git
- These include: kiss_sdl, OpticianSans, jsmn (duplicate), simplehttp
- Downloaded archives (.zip, .tar.gz) in libs/ should not be committed to git
