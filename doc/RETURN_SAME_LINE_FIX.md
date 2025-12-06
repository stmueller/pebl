# Grammar Fix for return(x) } Same-Line Pattern

## Problem

The PEBL parser was rejecting `return(value) }` when the closing brace appeared on the same line as the return statement (at function level).

### Example that failed:

```pebl
define TestSameLine(x)
{
  result <- x * 2
  return(result) }    ## SYNTAX ERROR - line 4
```

Error: `syntax error` at line 4

### Pattern that worked:

```pebl
define TestNextLine(x)
{
  result <- x * 2
  return(result)      ## Works fine - newline before }
}
```

## Root Cause

In `src/base/grammar.y`, the grammar rules were:

**Line 216 - functionblock with explicit return:**
```yacc
PEBL_LBRACE nlornone functionsequence PEBL_RBRACE   {$$ = $3;}
```

**Lines 220-221 - functionsequence:**
```yacc
functionsequence:   returnstatement  nlornone          { $$ = $1;}
	|	    sequence nlornone returnstatement  nlornone { $$ = new OpNode(PEBL_STATEMENTS, $1, $3, sourcefilename, yylineno);}
```

**Line 323 - returnstatement:**
```yacc
returnstatement: PEBL_RETURN statement    {$$ = new OpNode(PEBL_RETURN, $2, NULL, sourcefilename, yylineno);}
```

**Line 244 - statement:**
```yacc
statement: ustatement PEBL_NEWLINE {$$ = $1;}
```

The problem:
- `returnstatement` = `PEBL_RETURN statement`
- `statement` = `ustatement PEBL_NEWLINE`
- This means return **requires NEWLINE** after the expression
- But `}` on same line means no newline between return expression and closing brace

## Solution

Created a new `endreturnstatement` rule that parallels `endstatement`:

**Existing pattern for regular statements:**
```yacc
/*Normal statement, ending in a newline*/
statement: ustatement PEBL_NEWLINE {$$ = $1;}

/*Allow the last statement in a block to be terminated by }*/
endstatement: ustatement PEBL_RBRACE {$$ = $1;}
```

**New pattern for return statements (lines 323-329):**
```yacc
/*returnstatement requires NEWLINE after the expression*/
returnstatement: PEBL_RETURN statement    {$$ = new OpNode(PEBL_RETURN, $2, NULL, sourcefilename, yylineno);}
	;

/*endreturnstatement allows } to terminate (no newline needed), like endstatement*/
endreturnstatement: PEBL_RETURN ustatement    {$$ = new OpNode(PEBL_RETURN, $2, NULL, sourcefilename, yylineno);}
	;
```

**Updated functionsequence to accept endreturnstatement (lines 220-224):**
```yacc
functionsequence:   returnstatement  nlornone          { $$ = $1;}
	|	    sequence nlornone returnstatement  nlornone { $$ = new OpNode(PEBL_STATEMENTS, $1, $3, sourcefilename, yylineno);}
	|           endreturnstatement                       { $$ = $1;}
	|           sequence nlornone endreturnstatement     { $$ = new OpNode(PEBL_STATEMENTS, $1, $3, sourcefilename, yylineno);}
;
```

**Also updated type declarations (line 148):**
```yacc
%type <exp>  functionsequence returnstatement endreturnstatement functionblock
```

## Key Design Details

**Why `endreturnstatement: PEBL_RETURN ustatement` and not `PEBL_RETURN endstatement`?**

- `endstatement: ustatement PEBL_RBRACE` - **consumes the `}`**
- But `functionblock: PEBL_LBRACE nlornone functionsequence PEBL_RBRACE` - **also wants to consume the `}`**
- If `endreturnstatement` used `endstatement`, both rules would try to consume `}`, causing a conflict
- By using `ustatement` directly, `endreturnstatement` doesn't consume `}`, leaving it for `functionblock`

## Testing

### Test file: `test-return-function-level.pbl`

```pebl
define TestSameLine(x)
{
  result <- x * 2
  return(result) }

define TestNextLine(x)
{
  result <- x * 2
  return(result)
}
```

**Before fix:**
- Pattern 1 (same line): SYNTAX ERROR at line 4
- Pattern 2 (next line): Works

**After fix:**
- Pattern 1 (same line): ✅ Works, returns 10
- Pattern 2 (next line): ✅ Works, returns 10

## Patterns Now Supported

At **function level** (inside functionblock):

1. ✅ `return(value) \n }` - return with newline before closing brace (already worked)
2. ✅ `return(value) }` - return with closing brace on same line (NOW WORKS)
3. ✅ `return(value)` with implicit `}` on next line (already worked)

## Important Limitation

This fix ONLY applies to returns at the **function level** (the final return in a function body).

Returns **inside control structures** (if/else/loop) are still not supported, as documented in RETURN_STATEMENT_LIMITATION.md. This is intentional design - PEBL doesn't support early returns.

**This STILL fails:**
```pebl
define TestIfSameLine(x)
{
  if(x > 10)
  {
    return("large")    ## SYNTAX ERROR - early return not supported
  }
  return("small")
}
```

The reason: `if` blocks use `block`, not `functionblock`. The `block` rule doesn't include `functionsequence` or `returnstatement` at all - only regular `sequence` and `statement`.

## Files Modified

- `src/base/grammar.y`:
  - Line 148: Added `endreturnstatement` to type declarations
  - Lines 220-224: Updated `functionsequence` rule to accept `endreturnstatement` alternatives
  - Lines 323-329: Defined `returnstatement` and new `endreturnstatement` rules
- Rebuilt parser with `make parse`
- Rebuilt binary with `make main`

## Commit Message

```
Fix return statement syntax to allow } on same line

- Add endreturnstatement rule: PEBL_RETURN ustatement
- Update functionsequence to accept endreturnstatement alternatives
- Allows return(value) } pattern at function level
- Does NOT enable early returns (still intentionally unsupported)

Before: return(x) } caused syntax error
After: return(x) } works correctly at function level

This only affects return statements at the END of function bodies.
Returns inside if/else/loop blocks remain unsupported by design.
```
