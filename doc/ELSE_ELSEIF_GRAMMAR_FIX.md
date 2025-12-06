# Grammar Fix for else/elseif with Newlines

## Problem

The PEBL parser was rejecting `}  \n elseif` patterns - specifically, when there was a newline between a closing brace `}` and a subsequent `elseif` statement in a chain.

### Example that failed:
```pebl
if(x < 5)
{
  result <- "less than 5"
}
elseif(x < 10)      ## First elseif - works
{
  result <- "5 to 9"
}
elseif(x < 15)      ## Second elseif - SYNTAX ERROR
{
  result <- "10 to 14"
}
```

## Root Cause

In `src/base/grammar.y` at line 344, the recursive elseif rule was:

```yacc
|      PEBL_ELSEIF PEBL_LPAREN exp PEBL_RPAREN nlornone block  elseifseq {
```

This means: `elseif(...) block elseifseq` - the grammar expected the next `elseifseq` to follow immediately after the `block` with NO newlines allowed between them.

Compare to line 289 which correctly allows newlines:
```yacc
| PEBL_IF PEBL_LPAREN exp PEBL_RPAREN nlornone block elseifseq_or_nothing {
```

The initial if statement correctly has `nlornone` (newlines-or-none) before the optional else/elseif sequence.

## Solution

Add `nlornone` between `block` and the recursive `elseifseq`:

```yacc
|      PEBL_ELSEIF PEBL_LPAREN exp PEBL_RPAREN nlornone block nlornone elseifseq {
		/*First make the else node*/
		PNode * tmpNode = new OpNode(PEBL_ELSE, $6, $8, sourcefilename, yylineno);
		/*Put the else node in the IF node*/
		$$ = new OpNode(PEBL_IFELSE, $3, tmpNode, sourcefilename, yylineno); }
```

**Important**: The variable reference changed from `$7` to `$8` because we added a new grammar token.

## Testing

### Test file: `test-elseif-chain.pbl`
```pebl
define Start(p)
{
  x <- 12

  if(x < 5)
  {
    Print("less than 5")
  }
  elseif(x < 10)
  {
    Print("5 to 9")
  }
  elseif(x < 15)
  {
    Print("10 to 14")
  }
  else
  {
    Print("15 or more")
  }
}
```

**Before fix**: Syntax error at line 15 (second elseif)
**After fix**: Works correctly, prints "10 to 14"

## Patterns Now Supported

1. ✅ `} \n else \n {` - else on new line
2. ✅ `} \n elseif(...) \n {` - first elseif on new line
3. ✅ `} \n elseif(...) \n { } \n elseif(...) \n {` - multiple elseifs
4. ✅ `} else {` - else on same line (already worked)
5. ✅ `} elseif(...) {` - elseif on same line (already worked)

## Remaining Limitations

The comprehensive test file (`demo/tests/test-else-grammar.pbl`) still has issues:

1. **Start() parameter**: Should be `Start(p)` not `Start()`
2. **Return statements in blocks**: The pattern of having `return()` inside a regular block without a newline terminator doesn't work. Example:

```pebl
if(x > 10)
{
  return("large")    ## Syntax error - needs newline before }
}
```

This appears to be a deeper grammar issue related to how `returnstatement` vs `statement` are handled in blocks vs functionblocks. The grammar expects:
- `returnstatement` = `PEBL_RETURN statement`
- `statement` = `ustatement PEBL_NEWLINE`

So a return always needs a newline, but the `}` consumes it via the `endstatement` rule. This may require further investigation.

## Files Modified

- `src/base/grammar.y` - Added `nlornone` before recursive `elseifseq` (line 344)
- Rebuilt parser with `make parse`
- Rebuilt binary with `make main`

## Commit Message

```
Fix elseif chain syntax to allow newlines between } and elseif

- Add nlornone token between block and recursive elseifseq rule
- Update variable reference from $7 to $8 in grammar action
- Fixes syntax error when chaining multiple elseif statements with newlines
- Pattern } \n elseif(...) \n { now works correctly

Before: Only first elseif in chain could have newline before it
After: All elseifs in chain can have newlines before them
```
