# PEBL Return Statement Limitation

## Summary

**PEBL does NOT support early returns from functions.** The `return()` statement can only appear at the very end of a function body, not inside if/else blocks or other control structures.

## The Problem

This pattern does **NOT** work in PEBL:

```pebl
define MyFunction(x)
{
  if(x > 10)
  {
    return("large")    ## SYNTAX ERROR
  }

  return("small")
}
```

Error: `syntax error` at the line containing `return("large")`

## Why This Happens

### Grammar Structure

In `src/base/grammar.y`, the PEBL grammar defines two types of function bodies:

**1. functionblock with implicit return** (lines 207-212):
```yacc
block {
   /*When no return value is provided, return 1 (true)*/
   DataNode* retval  = new DataNode (Variant(1), sourcefilename, yylineno);
   OpNode *tmpReturn = new OpNode(PEBL_RETURN, retval, NULL, sourcefilename, yylineno);
   $$ = new OpNode(PEBL_STATEMENTS,$1,tmpReturn,sourcefilename,yylineno);
}
```

**2. functionblock with explicit return** (line 216):
```yacc
PEBL_LBRACE nlornone functionsequence PEBL_RBRACE
```

### The Distinction

- **`block`**: Contains a `sequence` of `statement`s. Statements do NOT include `return`.
- **`functionsequence`**: Contains a `sequence` followed by a `returnstatement`

**Line 220-221**:
```yacc
functionsequence:   returnstatement  nlornone
    |    sequence nlornone returnstatement  nlornone
```

So `returnstatement` can only appear:
1. As the ONLY thing in the function body, OR
2. At the END of a sequence of statements

**Line 321**:
```yacc
returnstatement: PEBL_RETURN statement
```

### What This Means

- `return()` is NOT a statement that can appear in a `sequence`
- `return()` can ONLY appear at the top level of a function body
- `return()` CANNOT appear inside if/else blocks, loops, or any other control structure

## The PEBL Way: Implicit Returns

### Instead of Early Returns

**Don't do this** (won't compile):
```pebl
define GetCategory(x)
{
  if(x > 10)
  {
    return("large")
  }
  elseif(x > 5)
  {
    return("medium")
  }
  else
  {
    return("small")
  }
}
```

**Do this** (PEBL style):
```pebl
define GetCategory(x)
{
  result <- ""

  if(x > 10)
  {
    result <- "large"
  }
  elseif(x > 5)
  {
    result <- "medium"
  }
  else
  {
    result <- "small"
  }

  return(result)
}
```

Or even simpler (let implicit return handle it):
```pebl
define GetCategory(x)
{
  if(x > 10)
  {
    result <- "large"
  }
  elseif(x > 5)
  {
    result <- "medium"
  }
  else
  {
    result <- "small"
  }

  result  ## Last expression is implicitly returned
}
```

## Why PEBL Works This Way

PEBL functions have an **implicit return value**: The last expression evaluated in the function is automatically returned. This means:

1. You rarely need explicit `return()` statements
2. Every function always returns a value (even if it's just `1` for true)
3. The grammar is simpler without needing to track early exits

### Examples from PEBL Library

Looking at `pebl-lib/*.pbl`, you'll see functions typically:

1. **Set a result variable and let it return implicitly**:
```pebl
define MyFunction(x)
{
  result <- DoSomething(x)
  result  ## Implicitly returned
}
```

2. **Use the last expression**:
```pebl
define Add(a, b)
{
  a + b  ## Result of expression is returned
}
```

3. **Use explicit return ONLY at the end**:
```pebl
define MyFunction(x)
{
  result <- ProcessData(x)
  return(result)
}
```

## Practical Impact

### What You CAN Do

✅ Return at the end of function:
```pebl
define MyFunction(x)
{
  result <- x * 2
  return(result)
}
```

✅ Implicit return:
```pebl
define MyFunction(x)
{
  x * 2  ## Implicitly returned
}
```

✅ Set variable in branches, return at end:
```pebl
define MyFunction(x)
{
  if(x > 0)
  {
    result <- "positive"
  }
  else
  {
    result <- "non-positive"
  }
  return(result)
}
```

### What You CANNOT Do

❌ Early return from if block:
```pebl
define MyFunction(x)
{
  if(x > 10)
  {
    return("early exit")  ## SYNTAX ERROR
  }
  ## more code...
}
```

❌ Return from loop:
```pebl
define FindItem(list)
{
  loop(item, list)
  {
    if(item == target)
    {
      return(item)  ## SYNTAX ERROR
    }
  }
}
```

❌ Multiple returns:
```pebl
define MyFunction(x)
{
  if(x > 0)
  {
    return("positive")  ## SYNTAX ERROR
  }
  return("non-positive")  ## SYNTAX ERROR
}
```

## Workarounds

### Pattern 1: Use Flag Variables

Instead of early return, use a flag and check it:

```pebl
define FindItem(list, target)
{
  found <- 0
  result <- ""

  loop(item, list)
  {
    if(not found and item == target)
    {
      result <- item
      found <- 1
    }
  }

  return(result)
}
```

### Pattern 2: Nest Conditions

Structure your logic so you only need one return:

```pebl
define Validate(x)
{
  valid <- 0

  if(x > 0)
  {
    if(x < 100)
    {
      valid <- 1
    }
  }

  return(valid)
}
```

## Comparison to Other Languages

This is different from most modern languages:

**C/JavaScript/Python**:
```javascript
function getCategory(x) {
  if (x > 10) return "large";
  if (x > 5) return "medium";
  return "small";
}
```

**PEBL**:
```pebl
define GetCategory(x)
{
  if(x > 10)
  {
    result <- "large"
  }
  elseif(x > 5)
  {
    result <- "medium"
  }
  else
  {
    result <- "small"
  }

  result  ## or return(result)
}
```

## Conclusion

The return statement limitation in PEBL is a **fundamental design choice**, not a bug. It simplifies the grammar and evaluator by:

1. Ensuring all functions have a single exit point
2. Making control flow more explicit
3. Avoiding the complexity of tracking return values through nested scopes

When writing PEBL code:
- Use variables to accumulate results
- Structure your logic to have a single return point
- Rely on implicit returns when possible
- Remember that the last expression is always returned

This is not related to the else/elseif grammar issue - it's a separate (and intentional) language design decision.
