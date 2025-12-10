# PEBL Compile Flags Analysis

## Current State (Conflated)

### Makefile Line 83:
```makefile
CXXFLAGS_EMSCRIPTEN = -DPEBL_EMSCRIPTEN -DPEBL_HTTP -DPEBL_ITERATIVE_EVAL ...
```

**Problem**: `PEBL_EMSCRIPTEN` always defines `PEBL_ITERATIVE_EVAL` alongside it.

### What the flags SHOULD mean:

| Flag | Meaning | Purpose |
|------|---------|---------|
| `PEBL_EMSCRIPTEN` | **Platform**: Compiling for WebAssembly/browser | Controls: emscripten headers, `emscripten_sleep()`, IDBFS, browser-specific code |
| `PEBL_ITERATIVE_EVAL` | **Evaluator Architecture**: Use iterative evaluator | Controls: Evaluator-es.cpp vs Evaluator.cpp, PEventLoop-es.cpp vs PEventLoop.cpp |

### What they CURRENTLY mean:

| Flag | Actual Behavior |
|------|-----------------|
| `PEBL_EMSCRIPTEN` | Platform=Emscripten **AND** Evaluator=Iterative (always both!) |
| `PEBL_ITERATIVE_EVAL` | Evaluator=Iterative on native (for testing?) - **never actually used** |

## Evidence from Source Code

### PEBL.cpp lines 28-44:
```cpp
#ifdef PEBL_EMSCRIPTEN
    #include "../base/Evaluator-es.h"      // Iterative
    #include "../devices/PEventLoop-es.h"  // Iterative
    #include <emscripten.h>                // Platform-specific
    #include <emscripten/html5.h>          // Platform-specific

#elif defined (PEBL_ITERATIVE_EVAL)       // This NEVER triggers!
    #include "../base/Evaluator-es.h"      // Iterative (native testing?)
    #include "../devices/PEventLoop-es.h"  // Iterative
#else
    #include "../base/Evaluator.h"         // Recursive (default native)
    #include "../devices/PEventLoop.h"     // Recursive
#endif
```

**Analysis**: The `#elif` branch suggests there WAS intent to allow native testing with iterative evaluator, but since Makefile always sets both flags together for Emscripten, this path is never taken.

### PEBL.cpp lines 706-741:
```cpp
#ifdef PEBL_EMSCRIPTEN
    // Emscripten uses iterative evaluator
    myEval->Evaluate1(head);
    while(myEval->GetNodeStackDepth() > 0) {
        myEval->Evaluate1();
    }
#else
    #ifdef PEBL_ITERATIVE_EVAL
        // Native iterative evaluator (for testing?)
        myEval->Evaluate1(head);
        while(myEval->GetNodeStackDepth()>0) {
            myEval->Evaluate1();
        }
    #else
        // Traditional recursive evaluator
        ::myEval->Evaluate(head);
    #endif
#endif
```

**Analysis**: Again, nested conditionals suggest intent to separate platform from evaluator choice, but currently Emscripten always uses iterative.

### Makefile Lines 257-285:
```makefile
# Native build sources
PEBLBASE_SRCXX = ... $(BASE_DIR)/Evaluator.cpp ... $(DEVICES_DIR)/PEventLoop.cpp

# Emscripten build sources
EMBASE_SRCXX = ... $(BASE_DIR)/Evaluator-es.cpp ... $(DEVICES_DIR)/PEventLoop-es.cpp
```

**Problem**: Build system FORCES different source files, regardless of flags!

## Other Flag Usage Patterns

### Platform-Specific (Correct Usage):

```cpp
#ifdef PEBL_EMSCRIPTEN
    emscripten_sleep(10);  // Browser yielding
#else
    nanosleep(&a, &b);     // Unix sleep
#endif
```

This is **correct** - truly platform-specific behavior.

### Mixed Usage (Sloppy):

In **PEBLEnvironment.cpp line 2033**:
```cpp
#ifdef PEBL_EMSCRIPTEN
    // Uses iterative evaluator features (NodeStackPush, Evaluate1)
#endif
```

This mixes platform and evaluator architecture concerns!

### What's Actually Platform-Specific:
- `#include <emscripten.h>` and `<emscripten/html5.h>`
- `emscripten_sleep()` calls
- IDBFS filesystem operations
- Browser console logging via `EM_ASM`
- Asyncify configuration

### What's Actually Evaluator-Architecture-Specific:
- `Evaluator.cpp` vs `Evaluator-es.cpp`
- `PEventLoop.cpp` vs `PEventLoop-es.cpp`
- `myEval->Evaluate()` vs `myEval->Evaluate1()` in loop
- Node stack management (`NodeStackPush`, `GetNodeStackDepth`)
- Variable map stack management

## Proposed Separation Strategy

### Option 1: Clean Separation (Ideal)
Make flags orthogonal:
- `PEBL_EMSCRIPTEN` = platform only
- `PEBL_ITERATIVE_EVAL` = evaluator architecture only
- Allow any combination (with Makefile targets for each)

### Option 2: Remove Iterative Evaluator (Simplest)
If recursive evaluator works with Emscripten + Asyncify:
- Remove `PEBL_ITERATIVE_EVAL` entirely
- Remove `Evaluator-es.cpp` and `PEventLoop-es.cpp`
- Keep only `PEBL_EMSCRIPTEN` for platform-specific code
- Unify to single evaluator architecture

### Option 3: Current State (Status Quo)
- `PEBL_EMSCRIPTEN` always implies iterative evaluator
- Document this conflation explicitly
- Accept that we can't test iterative evaluator on native

## Recommendation for Testing Recursive Evaluator

To test if recursive evaluator works with Emscripten:

**Makefile changes:**
```makefile
# Line 83: Remove PEBL_ITERATIVE_EVAL from Emscripten flags
CXXFLAGS_EMSCRIPTEN = -DPEBL_EMSCRIPTEN -DPEBL_HTTP -DHTTP_LIB=3 ...

# Lines 273-283: Change Emscripten source files
EMBASE_SRCXX = $(BASE_DIR)/Evaluator.cpp \      # Not Evaluator-es.cpp!
               ...
               $(DEVICES_DIR)/PEventLoop.cpp    # Not PEventLoop-es.cpp!
```

**Code changes needed:**
1. `PEBL.cpp`: Logic already handles this (will use recursive paths)
2. `PEBLEnvironment.cpp`: Check all `#ifdef PEBL_EMSCRIPTEN` blocks - separate platform vs evaluator concerns
3. `PEventLoop.cpp`: Verify `emscripten_sleep()` is guarded by `PEBL_EMSCRIPTEN` not `PEBL_ITERATIVE_EVAL`

## Risk Assessment

**Low Risk** (platform-specific code):
- Emscripten headers and `emscripten_sleep()` - these are truly platform features
- IDBFS operations - browser filesystem abstraction

**High Risk** (mixed concerns):
- `CallFunction` in PEBLEnvironment.cpp - currently uses different code for Emscripten
- Any code that uses `NodeStackPush`, `Evaluate1()`, or node stack depth checking
- These should be gated by evaluator architecture, not platform

## Next Steps

1. **Audit**: Search for all `#ifdef PEBL_EMSCRIPTEN` and classify as:
   - Pure platform (keep as-is)
   - Evaluator architecture (change to `PEBL_ITERATIVE_EVAL`)
   - Mixed (needs refactoring)

2. **Test**: Try building Emscripten with recursive evaluator
   - Modify Makefile as shown above
   - Build and test simple PEBL script
   - Check for stack overflow errors

3. **Decide**: Based on test results:
   - If recursive works → remove iterative evaluator entirely (Option 2)
   - If stack issues → keep both but separate cleanly (Option 1)
   - If unsure → document current state better (Option 3)
