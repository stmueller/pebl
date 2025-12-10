# Native Iterative Build

## Purpose

The `native-iterative` Makefile target compiles PEBL for native Linux/Mac using the **iterative evaluator** (`Evaluator-es.cpp`) and **iterative event loop** (`PEventLoop-es.cpp`) instead of the regular recursive versions.

This makes debugging much easier than using the Emscripten build, while still testing the exact same code paths that run in the browser.

## Usage

### Compile

```bash
make native-iterative
```

This creates: `bin/pebl2-iterative`

### Run

```bash
bin/pebl2-iterative path/to/script.pbl
```

Example:
```bash
bin/pebl2-iterative demo/tests/test-callstack-digitspan.pbl
```

### Debug with GDB

```bash
gdb --args bin/pebl2-iterative demo/tests/test-callstack-digitspan.pbl
(gdb) break PError::SignalFatalError
(gdb) run
```

## Key Differences from Regular Build

| Feature | `make main` (regular) | `make native-iterative` |
|---------|----------------------|-------------------------|
| Binary name | `bin/pebl2` | `bin/pebl2-iterative` |
| Evaluator | `Evaluator.cpp` (recursive) | `Evaluator-es.cpp` (iterative) |
| Event loop | `PEventLoop.cpp` (blocking) | `PEventLoop-es.cpp` (non-blocking) |
| Code path | Native-only | **Same as Emscripten/Web** |
| PEBL_ITERATIVE_EVAL | Not defined | **Defined** |
| Use case | Production | **Debugging web issues** |

## Why Use This?

### Problem
When debugging issues in the Emscripten/WebAssembly build:
- Slow compile times (1-2 minutes)
- Browser-based debugging is limited
- Console output is harder to work with
- Can't use native debugging tools like gdb/lldb

### Solution
The `native-iterative` build gives you:
- ✅ **Same code** as Emscripten (Evaluator-es.cpp, PEventLoop-es.cpp)
- ✅ **Fast compile times** (10-20 seconds)
- ✅ **Native debugging** with gdb/lldb
- ✅ **Direct terminal output** (no browser console)
- ✅ **Add print statements** and recompile quickly

## Use Cases

### 1. Call Stack Bug Debugging

Test the call stack issue found in digitspan.pbl:

```bash
make native-iterative
bin/pebl2-iterative demo/tests/test-callstack-digitspan.pbl
```

If it crashes with "gCallStack is empty", you can debug:

```bash
gdb --args bin/pebl2-iterative demo/tests/test-callstack-digitspan.pbl
(gdb) break Evaluator::Evaluate1
(gdb) run
```

### 2. Event Loop Timing Issues

Test event loop behavior (e.g., the gonogo timeout bug):

```bash
make native-iterative
bin/pebl2-iterative upload-battery/gonogo/gonogo.pbl -s 001
```

### 3. Verifying Emscripten Fixes

Before deploying to production, test your Evaluator-es.cpp changes natively:

```bash
# 1. Make changes to src/base/Evaluator-es.cpp
# 2. Test natively (fast)
make native-iterative
bin/pebl2-iterative test.pbl

# 3. If it works, build for web (slow)
make em-opt
```

## Technical Details

### Compiler Flags

The `native-iterative` target uses:
- `-DPEBL_ITERATIVE_EVAL` - Enables iterative evaluator macros
- `-DPEBL_LINUX` - Linux platform flags
- Native SDL2 libraries (not Emscripten's)
- Standard GCC/G++ (not emcc/em++)

### Source Files

Uses `EMBASE_SRCXX` which includes:
- `src/base/Evaluator-es.cpp` (iterative)
- `src/devices/PEventLoop-es.cpp` (non-blocking)

Plus all the standard native files:
- Native device support (`PDEVICES_SRC`)
- Native utilities (`PUTILITIES_SRC`)
- SDL2 platform code

### Object Files

Object files are stored in `obj/` with the same structure as the regular build. Clean build:

```bash
make clean
make native-iterative
```

## Comparison: Three Ways to Test

| Method | Compile Time | Debugging | Code Path | Use When |
|--------|-------------|-----------|-----------|----------|
| `make main` | Fast | ✅ Native gdb | Recursive evaluator | Production native |
| `make native-iterative` | Fast | ✅ Native gdb | **Iterative (web)** | **Debugging web issues** |
| `make em-test` | Slow | ⚠️ Browser | Iterative (web) | Final web testing |

## See Also

- [CALLSTACK_BUG_TESTS.md](../demo/tests/CALLSTACK_BUG_TESTS.md) - Test cases for call stack debugging
- [Evaluator-es.cpp](../src/base/Evaluator-es.cpp) - Iterative evaluator source
- [PEventLoop-es.cpp](../src/devices/PEventLoop-es.cpp) - Non-blocking event loop source
