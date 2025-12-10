# Early Return Implementation Analysis for PEBL

## Executive Summary

Implementing early returns (return statements inside if/else/loop blocks) in PEBL would require:
1. **Grammar changes** (moderate complexity)
2. **Evaluator changes** in BOTH recursive and iterative evaluators (high complexity)
3. **Stack signal propagation** mechanism (moderate complexity)
4. **Scope/context cleanup** handling (moderate complexity)

**Estimated effort**: Medium-large refactor. Not trivial, but architecturally feasible.

**Key challenge**: The iterative evaluator (Evaluator-es.cpp) uses manual stack management and would need careful handling to ensure early returns properly unwind through nested contexts.

## Current Architecture

### How Returns Work Now

**Recursive evaluator (Evaluator.cpp line 1252-1277):**
```cpp
case PEBL_RETURN:
{
    // Used ONLY at the very end of a function.
    // Pushes STACK_RETURN_DUMMY onto stack, then evaluates expression.
    // Consequently, stack depth will be 2 instead of 1.
    // CallFunction() extracts top of stack and puts on caller's stack.

    Push(Variant(STACK_RETURN_DUMMY));
    const PNode * node1 = node->GetLeft();
    Evaluate(node1);
}
break;
```

**Iterative evaluator (Evaluator-es.cpp line 1844-1857):**
```cpp
case PEBL_RETURN:
{
    // Return keyword used ONLY at very end of function.
    // Left node is value to return; right node should be NULL.
    // Evaluate left node; stack will contain return value.

    const PNode * node1 = node->GetLeft();
    mNodeStack.push(node1);
}
break;
```

**Function call handling (Evaluator.cpp line 1475-1491):**
```cpp
// In CallFunction():
if(myEval.GetStackDepth() == 1)
{
    // No return - push default value 1
    Push(1);
}
else if (myEval.GetStackDepth() == 2)
{
    // Has return - pop value from function scope
    Variant v1 = myEval.Pop();
    Push(v1);
}
```

**Key observation**: Returns currently work by:
1. Leaving extra value on stack (depth 2 vs 1)
2. Caller detects this and extracts return value
3. Only works at END of function because depth check happens after full function execution

## Grammar Changes Required

### Current Grammar (grammar.y)

**Lines 207-216: functionblock**
```yacc
functionblock:
    block {
        /* No return - implicit return 1 */
        DataNode* retval = new DataNode(Variant(1), sourcefilename, yylineno);
        OpNode *tmpReturn = new OpNode(PEBL_RETURN, retval, NULL, sourcefilename, yylineno);
        $$ = new OpNode(PEBL_STATEMENTS,$1,tmpReturn,sourcefilename,yylineno);
    }
    | PEBL_LBRACE nlornone functionsequence PEBL_RBRACE   {$$ = $3;}
;
```

**Lines 220-224: functionsequence**
```yacc
functionsequence:
      returnstatement  nlornone          { $$ = $1;}
    | sequence nlornone returnstatement  nlornone { $$ = new OpNode(PEBL_STATEMENTS, $1, $3, sourcefilename, yylineno);}
    | endreturnstatement                       { $$ = $1;}
    | sequence nlornone endreturnstatement     { $$ = new OpNode(PEBL_STATEMENTS, $1, $3, sourcefilename, yylineno);}
;
```

**Lines 227-232: block**
```yacc
block:
      PEBL_LBRACE nlornone sequence nlornone PEBL_RBRACE   { $$ = $3;}
    | PEBL_LBRACE nlornone endstatement {$$ = $3;}
    | PEBL_LBRACE nlornone sequence nlornone endstatement { $$  = new OpNode(PEBL_STATEMENTS, $3, $5, sourcefilename, yylineno);}
    | PEBL_LBRACE nlornone PEBL_RBRACE { $$ = new DataNode (Variant(0), sourcefilename, yylineno);}
;
```

### Required Changes

**1. Allow returnstatement in regular blocks**

Currently, `block` only allows `sequence` (which contains `statement`s). Returns are only in `functionsequence`.

**Change block to:**
```yacc
block:
      PEBL_LBRACE nlornone blocksequence nlornone PEBL_RBRACE   { $$ = $3;}
    | PEBL_LBRACE nlornone PEBL_RBRACE { $$ = new DataNode (Variant(0), sourcefilename, yylineno);}
;

blocksequence:
      statement              { $$ = $1; }
    | blocksequence nlornone statement        { $$ = new OpNode(PEBL_STATEMENTS, $1, $3, sourcefilename, yylineno);}
    | returnstatement  nlornone          { $$ = $1;}
    | blocksequence nlornone returnstatement  nlornone { $$ = new OpNode(PEBL_STATEMENTS, $1, $3, sourcefilename, yylineno);}
    | endreturnstatement                       { $$ = $1;}
    | blocksequence nlornone endreturnstatement     { $$ = new OpNode(PEBL_STATEMENTS, $1, $3, sourcefilename, yylineno);}
;
```

**Impact**: This allows return anywhere in a block, not just at function level.

**2. Update ustatement to include return?**

Alternative approach - make return a valid ustatement:

```yacc
ustatement:
      exp                   {$$ = $1;}
    | PEBL_BREAK            {$$ = new OpNode(PEBL_BREAK, NULL, NULL, sourcefilename, yylineno);}
    | PEBL_RETURN exp       {$$ = new OpNode(PEBL_RETURN, $2, NULL, sourcefilename, yylineno);}  // NEW
    | PEBL_LOCALVAR PEBL_ASSIGN exp  { ... }
    | ...
;
```

**Problem**: This conflicts with how `returnstatement` is currently structured. Would need to reconcile.

**Recommendation**: Use blocksequence approach. Cleaner separation.

## Evaluator Changes Required

### Challenge: Early Return Mechanism

Currently, PEBL has a precedent for early exit: **STACK_BREAK** for break statements.

**How STACK_BREAK works (Evaluator.cpp line 1233-1250):**

```cpp
case PEBL_BREAK:
{
    Variant v1 = Variant(STACK_BREAK);
    Push(v1);
    Push(v1);  // Push twice!
}
```

**How loops check for STACK_BREAK (line 816-822):**
```cpp
// In PEBL_WHILE:
results = Pop();
if(results.GetDataType() == P_DATA_STACK_SIGNAL &&
   results == Variant(STACK_BREAK))
{
    results = Pop();  // Pop the extra copy
    break;  // Exit C++ loop
}
```

**How STATEMENTS propagate STACK_BREAK (line 1215-1223):**
```cpp
case PEBL_STATEMENTS:
{
    Variant v1 = Pop();
    if(v1.GetDataType() == P_DATA_STACK_SIGNAL &&
       v1 == Variant(STACK_BREAK))
    {
        // Don't execute right node - just propagate break
        Push(Variant(STACK_BREAK));
        break;
    }
}
```

### Proposed Early Return Mechanism: STACK_EARLY_RETURN

**1. Create new stack signal type**

In Variant.h (or wherever stack signals are defined):
```cpp
enum StackSignal {
    STACK_LIST_HEAD,
    STACK_RETURN_DUMMY,
    STACK_BREAK,
    STACK_EARLY_RETURN  // NEW
};
```

**2. Modify PEBL_RETURN handling**

**Recursive evaluator:**
```cpp
case PEBL_RETURN:
{
    // Evaluate the return expression
    const PNode * node1 = node->GetLeft();
    Evaluate(node1);

    // Get the return value
    Variant returnValue = Pop();

    // Push signal AND value (like STACK_BREAK does)
    Push(Variant(STACK_EARLY_RETURN));
    Push(returnValue);
}
break;
```

**Iterative evaluator:**
```cpp
case PEBL_RETURN:
{
    // Evaluate the return expression
    const PNode * node1 = node->GetLeft();
    mNodeStack.push(new OpNode(PEBL_RETURN_TAIL, NULL, NULL, ...));
    mNodeStack.push(node1);
}
break;

case PEBL_RETURN_TAIL:
{
    // Value is on top of stack
    Variant returnValue = Pop();

    // Push signal AND value
    Push(Variant(STACK_EARLY_RETURN));
    Push(returnValue);
}
break;
```

**3. Modify all control structures to check for STACK_EARLY_RETURN**

Need to update:
- **PEBL_IF** / **PEBL_IFELSE** / **PEBL_ELSE**
- **PEBL_WHILE**
- **PEBL_LOOP**
- **PEBL_STATEMENTS**

Pattern (like STACK_BREAK):
```cpp
// After executing a block:
Variant result = Pop();
if(result.GetDataType() == P_DATA_STACK_SIGNAL &&
   result == Variant(STACK_EARLY_RETURN))
{
    // Propagate early return upward
    Variant returnValue = Pop();  // Get the actual return value
    Push(Variant(STACK_EARLY_RETURN));  // Re-push signal
    Push(returnValue);  // Re-push value
    // Don't execute any more statements
    return;  // or break, depending on evaluator structure
}
```

**4. Modify PEBL_LAMBDAFUNCTION to extract early return**

Currently checks stack depth:
```cpp
if(myEval.GetStackDepth() == 1)
{
    Push(1);  // Default return
}
else if (myEval.GetStackDepth() == 2)
{
    Variant v1 = myEval.Pop();
    Push(v1);  // Explicit return
}
```

**Change to:**
```cpp
if(myEval.GetStackDepth() == 1)
{
    Push(1);  // Default return
}
else
{
    // Could be explicit return OR early return
    Variant possibleSignal = myEval.Pop();

    if(possibleSignal.IsStackSignal() &&
       possibleSignal == Variant(STACK_EARLY_RETURN))
    {
        // Early return
        Variant returnValue = myEval.Pop();
        Push(returnValue);
    }
    else
    {
        // Explicit return at end
        Push(possibleSignal);
    }
}
```

**Better approach**: Check for signal first:
```cpp
if(myEval.GetStackDepth() >= 2)
{
    Variant top = myEval.Peek();
    if(top.IsStackSignal() && top == Variant(STACK_EARLY_RETURN))
    {
        myEval.Pop();  // Remove signal
        Variant returnValue = myEval.Pop();
        Push(returnValue);
    }
    else
    {
        // Normal explicit return
        Variant v1 = myEval.Pop();
        Push(v1);
    }
}
else
{
    Push(1);  // Default
}
```

## Iterative Evaluator Complications

The iterative evaluator (Evaluator-es.cpp) is MORE complex because it uses manual stack management.

**Key challenges:**

### 1. Node Stack Unwinding

When early return happens, need to clear remaining nodes from mNodeStack:

```cpp
case PEBL_RETURN_TAIL:
{
    Variant returnValue = Pop();

    // Signal early return
    Push(Variant(STACK_EARLY_RETURN));
    Push(returnValue);

    // CRITICAL: Clear any pending nodes for current function
    // This is tricky - how do we know where function scope ends?
    // Need to track scope boundaries on node stack somehow
}
```

**Problem**: mNodeStack doesn't track scope boundaries. When a function calls another function, nodes from both are intermixed.

**Solution**: Add scope markers to node stack?

```cpp
// When entering PEBL_LAMBDAFUNCTION:
const OpNode * scopeStart = new OpNode(PEBL_SCOPE_START, ...);
mNodeStack.push(scopeStart);

// When early return occurs:
while(!mNodeStack.empty())
{
    const PNode* node = mNodeStack.top();
    if(node->GetOp() == PEBL_SCOPE_START)
    {
        mNodeStack.pop();  // Remove scope marker
        break;  // Stop unwinding
    }
    mNodeStack.pop();  // Remove node
}
```

### 2. Variable Map Stack

The iterative evaluator maintains variable scope using `mVariableMapStack`:

```cpp
// Entering function (line 1088):
mVariableMapStack.push(mLocalVariableMap);
mScope = funcname.GetFunctionName();

// Exiting function (line 1143):
mLocalVariableMap = mVariableMapStack.top();
mVariableMapStack.pop();
```

**Early return must trigger this cleanup!**

```cpp
case PEBL_RETURN_TAIL:
{
    Variant returnValue = Pop();

    // Unwind node stack to function boundary
    while(!mNodeStack.empty())
    {
        const PNode* node = mNodeStack.top();
        if(node->GetOp() == PEBL_SCOPE_START)
        {
            mNodeStack.pop();
            break;
        }
        mNodeStack.pop();
    }

    // Restore variable scope (like PEBL_FUNCTION_TAIL_LIBFUNCTION does)
    mScope = mScopeStack.top();
    mScopeStack.pop();
    mLocalVariableMap = mVariableMapStack.top();
    mVariableMapStack.pop();

    // Pop call stack for error reporting
    gCallStack.Pop();

    // Signal early return to caller
    Push(Variant(STACK_EARLY_RETURN));
    Push(returnValue);
}
```

**Problem**: This duplicates cleanup logic from PEBL_FUNCTION_TAIL_LIBFUNCTION. Should be refactored into shared method.

### 3. Handling in All Control Structures

EVERY control structure in iterative evaluator needs update:

- PEBL_IF_TAIL
- PEBL_IF_TAIL2
- PEBL_ELSE_TAIL
- PEBL_WHILE_TAIL
- PEBL_WHILE_TAIL2
- PEBL_LOOP_TAIL1
- PEBL_LOOP_TAIL2
- PEBL_STATEMENTS_TAIL1

Each must check for STACK_EARLY_RETURN and propagate it instead of continuing execution.

**Example for PEBL_STATEMENTS_TAIL1 (line 1797-1817):**

```cpp
case PEBL_STATEMENTS_TAIL1:
{
    Variant v1 = Pop();

    // Check for BREAK (existing)
    if(v1.GetDataType() == P_DATA_STACK_SIGNAL &&
       v1 == Variant(STACK_BREAK))
    {
        mNodeStack.pop();  // Remove right statement
        Push(Variant(STACK_BREAK));
        break;
    }

    // Check for EARLY_RETURN (NEW)
    if(v1.GetDataType() == P_DATA_STACK_SIGNAL &&
       v1 == Variant(STACK_EARLY_RETURN))
    {
        Variant returnValue = Pop();  // Get return value
        mNodeStack.pop();  // Remove right statement from execution
        Push(Variant(STACK_EARLY_RETURN));  // Re-push signal
        Push(returnValue);  // Re-push value
        break;
    }
}
break;
```

## Implementation Steps

### Phase 1: Grammar Changes (Low Risk)

1. Add `blocksequence` production to allow returns in blocks
2. Update `block` to use `blocksequence`
3. Test that existing code still compiles and runs
4. Create test cases for early returns (will fail until evaluator updated)

**Estimated time**: 4-8 hours

### Phase 2: Stack Signal Infrastructure (Low Risk)

1. Add `STACK_EARLY_RETURN` to stack signal enum
2. Update Variant comparison operators if needed
3. Add debug printing for new signal type

**Estimated time**: 2-4 hours

### Phase 3: Recursive Evaluator Changes (Medium Risk)

1. Update `PEBL_RETURN` to use STACK_EARLY_RETURN
2. Add early return checks to:
   - PEBL_IF (line 431-453)
   - PEBL_ELSE (line 469-487)
   - PEBL_WHILE (line 783-827)
   - PEBL_LOOP (line 830-913)
   - PEBL_STATEMENTS (line 1180-1230)
3. Update `CallFunction()` to handle early returns
4. Test thoroughly with nested cases

**Estimated time**: 16-24 hours

### Phase 4: Iterative Evaluator Changes (High Risk)

1. Add scope marker mechanism to track function boundaries
2. Update `PEBL_RETURN` / `PEBL_RETURN_TAIL`
3. Create shared cleanup function for scope unwinding
4. Add early return checks to ALL tail operations:
   - PEBL_IF_TAIL / PEBL_IF_TAIL2
   - PEBL_ELSE_TAIL
   - PEBL_WHILE_TAIL / PEBL_WHILE_TAIL2
   - PEBL_LOOP_TAIL1 / PEBL_LOOP_TAIL2
   - PEBL_STATEMENTS_TAIL1
5. Update function call handling
6. Test extensively - this is complex!

**Estimated time**: 32-48 hours

### Phase 5: Testing & Documentation (Medium Risk)

1. Create comprehensive test suite:
   - Simple early returns
   - Nested early returns
   - Early returns in loops
   - Early returns in deeply nested if/else
   - Early returns with complex expressions
   - Interaction with break statements
2. Test both native and Emscripten builds
3. Document new behavior
4. Update manual and examples

**Estimated time**: 16-24 hours

## Total Effort Estimate

**70-108 hours** (roughly 2-3 weeks of focused development)

## Risks & Considerations

### High Risk Areas

1. **Iterative evaluator scope unwinding**: Most complex part. Scope markers could interact badly with existing code.

2. **Stack depth assumptions**: Current code assumes specific stack depths for returns. Early returns break this.

3. **Emscripten async compatibility**: Iterative evaluator exists specifically for Asyncify compatibility. Changes must not break this.

4. **Interaction with STACK_BREAK**: Two different early-exit mechanisms could interact in unexpected ways.

### Alternative Approaches

**Option 1: Single unified early-exit signal**

Instead of separate STACK_BREAK and STACK_EARLY_RETURN, use one signal with a type field:

```cpp
struct EarlyExitSignal {
    enum Type { BREAK, RETURN };
    Type type;
    Variant value;  // Only used for RETURN
};
```

**Pro**: Simpler propagation logic
**Con**: Changes existing break behavior (risky)

**Option 2: Exception-like unwinding**

Instead of stack signals, use C++ exceptions:

```cpp
class PEBLEarlyReturn {
    Variant returnValue;
};

// In PEBL_RETURN:
throw PEBLEarlyReturn(returnValue);

// In CallFunction:
try {
    myEval.Evaluate(node2);
} catch(PEBLEarlyReturn& ret) {
    Push(ret.returnValue);
}
```

**Pro**: Automatic unwinding, cleaner code
**Con**: Performance impact, incompatible with Emscripten Asyncify

**Recommendation**: Stick with stack signals. Most consistent with existing architecture.

## Conclusion

**Is it feasible?** Yes, definitely feasible.

**Is it worth it?** Depends on use case:

**Benefits of early returns:**
- More natural control flow
- Reduced nesting in complex functions
- Familiar to programmers from other languages

**Costs:**
- Significant development effort (2-3 weeks)
- Risk of introducing bugs in core evaluator
- Increased complexity in already-complex iterative evaluator
- Need to maintain two different implementations (recursive + iterative)

**Alternative**: Current PEBL pattern (set result variable, single return at end) is actually considered **good practice** in some programming communities (single-exit-point principle). The limitation forces clearer code structure.

## Recommendation

Given that:
1. PEBL has worked this way for 20+ years
2. Users have adapted to the pattern
3. Implementation is complex and risky
4. Current pattern has pedagogical benefits (explicit result flow)

**I would recommend NOT implementing early returns** unless there's a compelling use case that can't be solved with current patterns.

If you do proceed, start with Phase 1 (grammar) and Phase 2 (infrastructure), then fully test Phase 3 (recursive evaluator) before attempting Phase 4 (iterative evaluator). The iterative evaluator changes are where most bugs will lurk.
