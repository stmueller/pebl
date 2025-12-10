# Call Stack Bug Test Cases

## Problem Description

Error: `gCallStack is empty in PEBL_FUNCTION_TAIL_LIBFUNCTION`

This error occurs in the Emscripten build when executing certain PEBL scripts. It was discovered when testing the digitspan.pbl test from upload-battery.

**Error location**: `src/base/Evaluator-es.cpp:1132`

**Symptom**: The error occurs when a user-defined function returns, and the evaluator tries to pop the call stack but finds it empty.

## Test Cases

Run these tests to isolate the cause of the bug:

### 1. test-callstack-working.pbl
**Status**: Should PASS
**Description**: Control test - Start() without parameter, calls user function
**Expected**: Works fine (like test-call-function.pbl which already works)

```bash
bin/pebl2 demo/tests/test-callstack-working.pbl
```

### 2. test-callstack-simple.pbl
**Status**: May FAIL
**Description**: Simplest case - Start(p) with parameter, calls user function
**Purpose**: Tests if having a parameter on Start() causes the issue

```bash
bin/pebl2 demo/tests/test-callstack-simple.pbl
```

### 3. test-callstack-no-initupload.pbl
**Status**: May FAIL
**Description**: Start(p) with parameter, NO InitializeUpload(), calls user function
**Purpose**: Tests if InitializeUpload() is part of the problem

```bash
bin/pebl2 demo/tests/test-callstack-no-initupload.pbl
```

### 4. test-callstack-bug.pbl
**Status**: May FAIL
**Description**: Start(p) with parameter, calls InitializeUpload(), then calls user function
**Purpose**: Full reproduction case with InitializeUpload()

```bash
bin/pebl2 demo/tests/test-callstack-bug.pbl
```

### 5. test-callstack-digitspan.pbl
**Status**: Should FAIL
**Description**: Exact structure of digitspan.pbl that causes the error
**Purpose**: Closest reproduction of the actual failing test

```bash
bin/pebl2 demo/tests/test-callstack-digitspan.pbl
```

## Root Cause Analysis

### Call Stack Management in Evaluator-es.cpp

The Emscripten evaluator uses manual call stack management (not automatic like native):

1. **Push**: Line 1103 in `PEBL_FUNCTION_TAIL2 -> PEBL_LAMBDAFUNCTION`
   ```cpp
   gCallStack.Push(node);
   ```

2. **Pop**: Line 1134 in `PEBL_FUNCTION_TAIL_LIBFUNCTION`
   ```cpp
   gCallStack.Pop();
   ```

### Initial Call Stack State

- Constructor (line 103): `gCallStack.Push(gEvalNode);`
- This provides 1 item on the stack at startup

### Hypothesis

The issue may be related to:

1. **Start() parameter handling**: When Start() has a parameter, the initial call may not set up the call stack correctly
2. **InitializeUpload() interaction**: This function (from Utility.pbl) may affect the call stack
3. **Nested function calls**: The sequence Start(p) → InitializeUpload() → Initialize() may create an unbalanced push/pop
4. **Asyncify unwinding**: State restoration after async operations might not restore call stack correctly

## Testing Instructions

### Native with Iterative Evaluator (RECOMMENDED for debugging)

The easiest way to test and debug the call stack issue is to use the native build with the iterative evaluator. This gives you the same code path as Emscripten but with native debugging tools.

1. Compile native binary with iterative evaluator:
   ```bash
   make native-iterative
   ```
   This creates: `bin/pebl2-iterative`

2. Run test cases:
   ```bash
   bin/pebl2-iterative demo/tests/test-callstack-working.pbl
   bin/pebl2-iterative demo/tests/test-callstack-simple.pbl
   bin/pebl2-iterative demo/tests/test-callstack-digitspan.pbl
   ```

3. Use gdb for debugging:
   ```bash
   gdb --args bin/pebl2-iterative demo/tests/test-callstack-digitspan.pbl
   (gdb) break PError::SignalFatalError
   (gdb) run
   ```

**Advantages:**
- Same Evaluator-es.cpp and PEventLoop-es.cpp code as Emscripten
- Native debugging with gdb/lldb
- Faster compile times than Emscripten
- Direct terminal output (no browser console)
- Can add print statements and recompile quickly

### Web (Emscripten)

1. Compile to WASM:
   ```bash
   make em-test
   ```

2. Copy test file to runtime directory:
   ```bash
   cp demo/tests/test-callstack-*.pbl bin/
   ```

3. Run via pebl-launcher.html with test parameter:
   ```
   http://localhost:8080/runtime/pebl-launcher.html?test=test-callstack-simple&token=TEST&participant=001
   ```

### Native Recursive (control test)

Run directly with standard pebl2 (uses Evaluator.cpp):
```bash
make main
bin/pebl2 demo/tests/test-callstack-simple.pbl
```

This should work fine since the recursive evaluator doesn't have this bug.

## Expected Outcome

If the bug is fixed, all tests should print "Success!" without errors.

## Debugging Tips

1. **Enable debug printing**: Uncomment `#define PEBL_DEBUG_PRINT 1` in Evaluator-es.cpp line 82
2. **Check call stack size**: Add logging around line 1131 to print `gCallStack.Size()`
3. **Trace function calls**: Log when PEBL_LAMBDAFUNCTION and PEBL_FUNCTION_TAIL_LIBFUNCTION execute

## Related Files

- `src/base/Evaluator-es.cpp` - Emscripten evaluator with call stack management
- `src/base/Evaluator-es.h` - Header file
- `pebl-lib/Utility.pbl` - Contains InitializeUpload() function
- `upload-battery/dspan/digitspan.pbl` - Original failing test

## Git History

- Commit `eabfad9`: "Fix missing call stack trace in Emscripten error reporting"
  - Added manual gCallStack.Push/Pop to match native behavior
  - This commit may have introduced or exposed the current bug

## Next Steps

1. Run all 5 test cases to identify which conditions trigger the bug
2. Add debug logging to track call stack push/pop operations
3. Compare execution path between working (test-call-function.pbl) and failing cases
4. Check if Start() parameter handling differs from normal function calls
5. Verify InitializeUpload() doesn't interfere with call stack
