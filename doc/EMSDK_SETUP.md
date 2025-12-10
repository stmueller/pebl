# Emscripten SDK Setup

The Emscripten SDK (`libs/emsdk/`) is **not included** in this repository due to its large size (1.7GB). You need to install it separately to build the emscripten targets.

## Installation

### Option 1: Clone into libs/emsdk/

```bash
cd libs/
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

### Option 2: Install system-wide and update Makefile

If you have Emscripten installed system-wide, update the Makefile variables:

```makefile
# Change these lines in Makefile (around line 54-56):
EMCC = emcc
EMCXX = em++
FP = file_packager.py
```

Instead of:
```makefile
EMCC = libs/emsdk/upstream/emscripten/emcc
EMCXX = libs/emsdk/upstream/emscripten/em++
FP = libs/emsdk/upstream/emscripten/tools/file_packager.py
```

## Verification

After installation, verify the setup:

```bash
# If using libs/emsdk/:
source libs/emsdk/emsdk_env.sh
emcc --version

# If using system-wide install:
emcc --version
```

You should see output like:
```
emcc (Emscripten gcc/clang-like replacement + linker emulating GNU ld) 3.x.x
```

## Building with Emscripten

Once emsdk is set up:

```bash
# Debug build:
make em -j 10

# Optimized build:
make em-opt -j 10
```

## Required Version

PEBL has been tested with:
- Emscripten 3.1.x and later

Older versions may work but are not officially supported.
