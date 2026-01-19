# Macro Refactoring: PEBL_EMSCRIPTEN vs PEBL_ITERATIVE_EVAL

## Classification Key
- **PLATFORM** = `PEBL_EMSCRIPTEN` (browser-specific features)
- **EVALUATOR** = `PEBL_ITERATIVE_EVAL` (iterative vs recursive evaluator architecture)
- **MIXED** = Currently uses `PEBL_EMSCRIPTEN` but should check evaluator architecture
- **DELETE** = Dead code that should be removed

---

## src/apps/PEBL.cpp

### Lines 28-32: MIXED → SPLIT INTO TWO CHECKS
```cpp
#ifdef PEBL_EMSCRIPTEN
#include "../base/Evaluator-es.h"        // EVALUATOR concern
#include "../devices/PEventLoop-es.h"    // EVALUATOR concern
#include <emscripten.h>                  // PLATFORM concern
#include <emscripten/html5.h>            // PLATFORM concern
```

**CHANGE TO:**
```cpp
#ifdef PEBL_ITERATIVE_EVAL
#include "../base/Evaluator-es.h"
#include "../devices/PEventLoop-es.h"
#elif defined (PEBL_ITERATIVE_EVAL)  // This branch already exists but never executes!
#include "../base/Evaluator-es.h"
#include "../devices/PEventLoop-es.h"
#else
#include "../base/Evaluator.h"
#include "../devices/PEventLoop.h"
#endif

#ifdef PEBL_EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>
#endif
```

### Lines 208-210: PLATFORM (CORRECT)
```cpp
#ifdef PEBL_EMSCRIPTEN
   EM_ASM(console.log("=== PEBLInterpret() called ==="));
#endif
```
**ACTION:** Keep as-is (browser console logging)

### Lines 279-281: PLATFORM (CORRECT)
```cpp
#ifdef PEBL_EMSCRIPTEN
   files.push_back("EM.pbl");
#endif
```
**ACTION:** Keep as-is (EM.pbl contains Emscripten-specific PEBL library functions)

### Lines 587-592: MIXED → INVESTIGATE
```cpp
#if defined(PEBL_EMSCRIPTEN)
    std::cerr <<"--------o-o-o-o-o-o--\n";
    arglist->PushBack(Variant(0));
    PComplexData * pcd = new PComplexData(counted_ptr<PEBLObjectBase>(arglist));
    pList->PushBack(Variant(pcd));
#else
```
**ACTION:** INVESTIGATE - Why does Emscripten need different initialization? This looks like a bug or legacy code. The #else branch does conditional logic based on arglist->Length(). May be unrelated to either PLATFORM or EVALUATOR.

### Lines 733-738: EVALUATOR
```cpp
#ifdef PEBL_EMSCRIPTEN
    cout << "Starting evaluation with Asyncify support\n";
    // Start evaluator at head of pnode tree
    myEval->Evaluate1(head);
```
**CHANGE TO:**
```cpp
#ifdef PEBL_ITERATIVE_EVAL
    cout << "Starting evaluation with iterative evaluator\n";
    myEval->Evaluate1(head);
```

### Lines 850-852: PLATFORM (CORRECT)
```cpp
#ifdef PEBL_EMSCRIPTEN
    EM_ASM(console.log("=== main() called in PEBL.cpp ==="));
#endif
```
**ACTION:** Keep as-is (browser console)

### Lines 970-975: PLATFORM (CORRECT)
```cpp
#elif defined (PEBL_EMSCRIPTEN)
    std::string basedir = "/usr/local/share/pebl2";
```
**ACTION:** Keep as-is (virtual filesystem paths for Emscripten)

---

## src/libs/PEBLEnvironment.cpp

### Lines 43-49: EVALUATOR
```cpp
#ifdef PEBL_EMSCRIPTEN
#include "../base/Evaluator-es.h"
#include "../devices/PEventLoop-es.h"
#else
#include "../base/Evaluator.h"
#include "../devices/PEventLoop.h"
#endif
```
**CHANGE TO:**
```cpp
#ifdef PEBL_ITERATIVE_EVAL
#include "../base/Evaluator-es.h"
#include "../devices/PEventLoop-es.h"
#else
#include "../base/Evaluator.h"
#include "../devices/PEventLoop.h"
#endif
```

### Lines 2033-2071: EVALUATOR
```cpp
#ifdef PEBL_EMSCRIPTEN
    //For Emscripten iterative evaluator, schedule the function call and execute until complete
    //Create a DataNode containing the actual parameter values
    DataNode * argsDataNode = new DataNode(args, "user-generated", -1);
    //Create the PEBL_FUNCTION node with the parameter values
    OpNode * fnode = new OpNode(PEBL_FUNCTION, namenode, (PNode*)argsDataNode, "user-generated", -1);
    //Save the current node stack size to know when the function completes
    size_t nodeStackSizeBefore = myEval->GetNodeStackDepth();
    //Schedule the PEBL_FUNCTION node on myEval's node stack
    myEval->NodeStackPush(fnode);
    //Execute nodes until the function completes
    while(myEval->GetNodeStackDepth() > (int)nodeStackSizeBefore) {
        myEval->Evaluate1();
    }
    //... cleanup code
    delete argsDataNode;
    delete fnode;
#else
```
**CHANGE TO:**
```cpp
#ifdef PEBL_ITERATIVE_EVAL
    //For iterative evaluator, schedule the function call and execute until complete
    // ... same code ...
#else
    //For recursive evaluator, create a new evaluator as before
```

### Lines 2129-2134: EVALUATOR
```cpp
#ifdef PEBL_EMSCRIPTEN
    eval->Evaluate1(node);
    eval->Evaluate1();
#else
    eval->Evaluate(node);
#endif
```
**CHANGE TO:**
```cpp
#ifdef PEBL_ITERATIVE_EVAL
    eval->Evaluate1(node);
    eval->Evaluate1();
#else
    eval->Evaluate(node);
#endif
```

---

## src/devices/PEventLoop.cpp

### Lines 35-42: MIXED → SPLIT
```cpp
#ifdef PEBL_EMSCRIPTEN
#include "PEventLoop-es.h"              // EVALUATOR concern
#include "../base/Evaluator-es.h"       // EVALUATOR concern
#include "emscripten.h"                 // PLATFORM concern
#else
#include "PEventLoop.h"
#include "../base/Evaluator.h"
#endif
```
**CHANGE TO:**
```cpp
#ifdef PEBL_ITERATIVE_EVAL
#include "PEventLoop-es.h"
#include "../base/Evaluator-es.h"
#else
#include "PEventLoop.h"
#include "../base/Evaluator.h"
#endif

#ifdef PEBL_EMSCRIPTEN
#include "emscripten.h"
#endif
```

### Lines 451-476: PLATFORM (CORRECT)
```cpp
#if defined(PEBL_UNIX)
    // nanosleep code
#endif

#if defined(PEBL_WIN32)
    // SDL_Delay code
#endif
```
**ACTION:** Keep as-is. Note: Should add PEBL_EMSCRIPTEN case here with emscripten_sleep() if this file is used for Emscripten.

---

## src/devices/PEventLoop-es.cpp

### Lines 35-42: MIXED → SPLIT
```cpp
#ifdef PEBL_EMSCRIPTEN
#include "PEventLoop-es.h"
//#include "../base/Evaluator-es.h"      // COMMENTED OUT
#include "../base/Evaluator.h"          // Uses recursive evaluator header???
#include "emscripten.h"
#else
```
**ISSUE:** This is already confusing! It includes Evaluator.h (recursive) not Evaluator-es.h (iterative). This might be a recent change or a bug.

**CHANGE TO:**
```cpp
// No includes needed here based on PEBL_EMSCRIPTEN
// The includes should be based on PEBL_ITERATIVE_EVAL at the top level

#ifdef PEBL_EMSCRIPTEN
#include "emscripten.h"
#endif
```

### Lines 230-251: MIXED
```cpp
#ifdef PEBL_EMSCRIPTEN
    //If Loop1() scheduled a callback, execute it now
    if(mCallbackScheduled) {
        // ... iterative evaluator node stack management ...
        while(!myEval->mNodeStack.empty() &&
              myEval->mNodeStack.size() > targetNodeStackSize) {
            myEval->Evaluate1();  // Iterative evaluator method
        }
    }
    //Yield to browser to allow browser events to be processed
    emscripten_sleep(10);        // Platform-specific browser yielding
#endif
```
**CHANGE TO:** Split into two checks
```cpp
#ifdef PEBL_ITERATIVE_EVAL
    //If Loop1() scheduled a callback, execute it now
    if(mCallbackScheduled) {
        size_t targetNodeStackSize = mCallbackNodeStackSize;
        while(!myEval->mNodeStack.empty() &&
              myEval->mNodeStack.size() > targetNodeStackSize) {
            myEval->Evaluate1();
        }
    }
#endif

#ifdef PEBL_EMSCRIPTEN
    //Yield to browser to allow browser events to be processed
    emscripten_sleep(10);
#elif defined(PEBL_UNIX)
    // nanosleep code here
#elif defined(PEBL_WIN32)
    // SDL_Delay code here
#endif
```

---

## src/base/Evaluator-es.cpp

### Lines 57-59: PLATFORM → DELETE (unused!)
```cpp
#ifdef PEBL_EMSCRIPTEN
#include "emscripten.h"
#endif
```
**ACTION:** DELETE - The header is included but never used anywhere in the file!

### Lines 129-135: PLATFORM (but investigate)
```cpp
#ifdef PEBL_EMSCRIPTEN
    if(node == NULL) PError::ExitQuietly("Trying to evaluate null node\n");
#else
    if(node == NULL) PError::SignalFatalError("Trying to evaluate null node\n");
#endif
```
**ACTION:** Investigate ExitQuietly vs SignalFatalError. Is this truly platform-specific (browser can't show fatal error dialogs?) or should it be consistent?

### Lines 186-196: DELETE (identical code!)
```cpp
#ifdef PEBL_EMSCRIPTEN
    Evaluate1(node);
    //cout << "rescheduling eval1\n";
    //Now, reschedule next one:
    //emscripten_async_call(Eval1,NULL,10);
    return true;
#else
    return Evaluate1(node);
#endif
```
**ACTION:** DELETE the conditional - the emscripten_async_call is commented out, so both branches do the same thing!

**CHANGE TO:**
```cpp
return Evaluate1(node);
```

---

## Makefile

### Line 83: WRONG - Conflates platform and evaluator
```makefile
CXXFLAGS_EMSCRIPTEN = -DPEBL_EMSCRIPTEN -DPEBL_HTTP -DPEBL_ITERATIVE_EVAL ...
```
**CHANGE TO:**
```makefile
CXXFLAGS_EMSCRIPTEN = -DPEBL_EMSCRIPTEN -DPEBL_HTTP ...
# Don't automatically define PEBL_ITERATIVE_EVAL!
```

### Lines 257-285: Source file selection
```makefile
PEBLBASE_SRCXX = $(BASE_DIR)/Evaluator.cpp ... $(DEVICES_DIR)/PEventLoop.cpp
EMBASE_SRCXX = $(BASE_DIR)/Evaluator-es.cpp ... $(DEVICES_DIR)/PEventLoop-es.cpp
```
**ISSUE:** This hardcodes which evaluator to use based on target (main vs em-opt).

**PROPOSED SOLUTION:** Create new build targets:
```makefile
# Native recursive (default)
main: PEBL_EVAL_SRC = $(PEBLBASE_SRCXX)
main: ...

# Native iterative (for testing iterative evaluator)
main-iterative: CXXFLAGS += -DPEBL_ITERATIVE_EVAL
main-iterative: PEBL_EVAL_SRC = $(EMBASE_SRCXX)
main-iterative: ...

# Emscripten recursive (test browser stack limits)
em-recursive: CXXFLAGS = $(CXXFLAGSX) $(CXXFLAGS_EMSCRIPTEN)
em-recursive: PEBL_EVAL_SRC = $(PEBLBASE_SRCXX)
em-recursive: ...

# Emscripten iterative (current default)
em-opt: CXXFLAGS = $(CXXFLAGSX) $(CXXFLAGS_EMSCRIPTEN) -DPEBL_ITERATIVE_EVAL
em-opt: PEBL_EVAL_SRC = $(EMBASE_SRCXX)
em-opt: ...
```

---

## Summary of Changes Needed

| File | Lines | Current Macro | Should Be | Reason |
|------|-------|---------------|-----------|--------|
| PEBL.cpp | 28-32 | PEBL_EMSCRIPTEN | Split: PEBL_ITERATIVE_EVAL + PEBL_EMSCRIPTEN | Mixed concern |
| PEBL.cpp | 733-738 | PEBL_EMSCRIPTEN | PEBL_ITERATIVE_EVAL | Evaluator architecture |
| PEBLEnvironment.cpp | 43-49 | PEBL_EMSCRIPTEN | PEBL_ITERATIVE_EVAL | Evaluator architecture |
| PEBLEnvironment.cpp | 2033-2071 | PEBL_EMSCRIPTEN | PEBL_ITERATIVE_EVAL | Evaluator architecture |
| PEBLEnvironment.cpp | 2129-2134 | PEBL_EMSCRIPTEN | PEBL_ITERATIVE_EVAL | Evaluator architecture |
| PEventLoop.cpp | 35-42 | PEBL_EMSCRIPTEN | Split: PEBL_ITERATIVE_EVAL + PEBL_EMSCRIPTEN | Mixed concern |
| PEventLoop-es.cpp | 230-251 | PEBL_EMSCRIPTEN | Split: PEBL_ITERATIVE_EVAL + PEBL_EMSCRIPTEN | Mixed concern |
| Evaluator-es.cpp | 57-59 | PEBL_EMSCRIPTEN | DELETE | Unused include |
| Evaluator-es.cpp | 186-196 | PEBL_EMSCRIPTEN | DELETE | Identical code |
| Makefile | 83 | Always sets both | Remove PEBL_ITERATIVE_EVAL | Conflation |

## Test Plan After Changes

1. **Native recursive** (current default): `make main`
   - Should work exactly as before

2. **Native iterative** (NEW): `make main-iterative`
   - Tests iterative evaluator on native platform
   - Useful for debugging iterative evaluator without browser

3. **Emscripten recursive** (NEW - the experiment!): `make em-recursive`
   - Tests if recursive evaluator works in browser
   - Will reveal if browser stack limits are a real problem

4. **Emscripten iterative** (current): `make em-opt`
   - Should work exactly as before (but with corrected macros)

## Expected Benefits

1. **Clearer code** - Platform and architecture concerns separated
2. **More flexible** - Can test any evaluator on any platform
3. **Easier to unify** - If recursive evaluator works on Emscripten, we can delete Evaluator-es.cpp entirely
4. **Better debugging** - Can test iterative evaluator natively without browser
