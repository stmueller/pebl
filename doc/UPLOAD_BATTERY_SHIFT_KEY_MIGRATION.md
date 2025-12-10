# Upload-Battery Shift Key Migration Analysis

## Executive Summary

**12 tasks** in `upload-battery/` use shift keys for responses and require migration to the Layout & Response System to avoid:
- Windows Sticky Keys dialog triggering during experiments
- Non-functional shift keys in web/Emscripten deployment
- Accessibility issues on tablet/touch devices

## Priority Classification

### High Priority (Two-Alternative Forced Choice)
These tasks use standard left/right shift for 2AFC responses and can be migrated using the evenodd pattern:

1. **flanker** - Eriksen Flanker Task
   - Pattern: `["<lshift>","<rshift>"]` for left/right responses
   - Timeout: configurable (gParams.timeout)
   - Correctness: `(targdir ==1 and resp == "<rshift>") or (targdir ==-1 and resp == "<lshift>")`

2. **oddball** - Oddball Detection Task
   - Pattern: `["<lshift>","<rshift>"]` for target detection
   - Timeout: 2500ms
   - Correctness: `(targID ==2 and resp == "<rshift>") or (targID ==1 and resp == "<lshift>")`

3. **dotjudgment** - Dot Numerosity Judgment
   - Pattern: `["<lshift>","<rshift>"]` for more/fewer judgments
   - No timeout (waits indefinitely)
   - Correctness: `(order ==-1 and resp == "<rshift>") or (order ==1 and resp == "<lshift>")`

4. **luckvogel** - Luck/Vogel Visual Working Memory
   - Pattern: `["<lshift>","<rshift>"]` for same/different judgments
   - No timeout
   - Correctness: `resp=="<rshift>"` (right) or `resp=="<lshift>"` (left)

5. **wpt** - Wisconsin Perceptual Test
   - Pattern: `["<lshift>","<rshift>"]` for match/no-match
   - No timeout
   - Correctness: `resp == "<lshift>"` (left) or `resp == "<rshift>"` (right)

6. **crt** - Choice Reaction Time
   - Pattern: `["<lshift>","<rshift>"]` for left/right responses
   - Timeout: configurable (gParams.timeout)
   - Correctness: `(resp =="<lshift>" and order == 1) or (resp =="<rshift>" and order == -1)`

7. **manikin** - Manikin Task (spatial compatibility)
   - Pattern: `["<LSHIFT>","<RSHIFT>"]` for left/right hand responses
   - No timeout
   - Correctness: `(resp=="<rshift>" and hand== -1) or (resp=="<lshift>" and hand==1)`
   - Note: Uses uppercase LSHIFT/RSHIFT in some places

### Medium Priority (Configurable Keys)

8. **ANT** - Attention Network Task
   - Pattern: Uses parameters `["leftresponse","<lshift>"]` and `["rightresponse","<rshift>"]`
   - Already parameterized! Easy to migrate
   - Timeout: 1700ms
   - Response via: `WaitForListKeyPressWithTimeout([gParams.LeftResponse,gParams.RightResponse],1700,1)`

### Special Cases

9. **gonogo** - Go/No-Go Task
   - Pattern: `["<lshift>","<rshift>"]` but may be go/no-go (any key vs no response)
   - Timeout: `gParams.isi-50`
   - May need single-button response mode consideration

10. **BST** - Brief Symptom Test
    - Pattern: Hybrid - supports both click targets AND keyboard
    - `WaitForClickOnTargetWithTimeout(gFeet,["<lshift>","<rshift>"],gResponseTimeLimit)` OR
    - `WaitForListKeyPressWithTimeout(["<lshift>","<rshift>"],gResponseTimeLimit,1)`
    - Already has mouse support - may only need keyboard migration

11. **simon** - Simon Task
    - Pattern: `["<lshift>","<rshift>"," "]` (includes spacebar for some phases)
    - Two different response patterns in code
    - More complex - needs careful analysis

12. **urns** -Urn Decision Task
    - Pattern: Multiple modes with different key sets:
      - `["<lshift>","<rshift>"," "]` (with spacebar)
      - `["<lshift>","<rshift>"]` (without spacebar)
      - `["<lshift>"," "]` (left + spacebar only)
      - `["<rshift>"," "]` (right + spacebar only)
    - Most complex - context-dependent key sets

## Migration Effort Estimates

### Easy (1-2 hours each)
- **flanker** - Straightforward 2AFC
- **oddball** - Straightforward 2AFC
- **dotjudgment** - Straightforward 2AFC
- **luckvogel** - Straightforward 2AFC
- **wpt** - Straightforward 2AFC
- **crt** - Straightforward 2AFC with timeout
- **manikin** - 2AFC with case sensitivity issue to fix
- **ANT** - Already parameterized!

### Medium (2-4 hours each)
- **gonogo** - Need to verify go/no-go pattern
- **BST** - Hybrid click/keyboard, may need layout integration for click targets

### Complex (4-6 hours each)
- **simon** - Multiple response phases, mixed keys
- **urns** - Context-dependent key sets, complex logic

## Recommended Migration Order

1. **ANT** - Already parameterized, easiest migration
2. **flanker** - Clean 2AFC, high usage
3. **oddball** - Clean 2AFC, high usage
4. **crt** - Choice RT is fundamental task
5. **dotjudgment** - Clean 2AFC
6. **luckvogel** - Clean 2AFC
7. **wpt** - Clean 2AFC
8. **manikin** - 2AFC with minor fixes needed
9. **gonogo** - Verify pattern first
10. **BST** - Analyze hybrid system
11. **simon** - Complex, needs design decisions
12. **urns** - Most complex, needs careful redesign

## Migration Checklist (Per Task)

For each task, follow the evenodd migration pattern:

- [ ] Add `responsemode` parameter to schema
- [ ] Add to parameter pairs in Start()
- [ ] Call `CreateLayout()` after parameters loaded
- [ ] Configure layout zones (header, footer)
- [ ] Replace manual labels with layout zones
- [ ] Replace `WaitForListKeyPress(["<lshift>","<rshift>"])` with `WaitForLayoutResponse(gLayout, timeout)`
- [ ] Update correctness logic to use semantic names (`"left"`, `"right"`)
- [ ] Update translations to remove hardcoded key references
- [ ] Test all response modes (auto, keyboardShift, keyboardSafe, mousetarget, mousebutton)
- [ ] Verify data format (response column now shows semantic names)
- [ ] Update task documentation

## Expected Benefits

After migration, all tasks will:
- ✅ Work on Windows without Sticky Keys dialog
- ✅ Work on web/Emscripten platforms
- ✅ Support tablet/touch devices (mousetarget mode)
- ✅ Allow user-configurable response modes
- ✅ Have consistent UI across battery
- ✅ Scale automatically to any screen resolution

## Data Format Changes

**Before migration:**
```csv
subnum,trial,stimulus,response,correct,rt
001,1,left,<lshift>,1,523
001,2,right,<rshift>,1,456
```

**After migration:**
```csv
subnum,trial,stimulus,response,correct,rt
001,1,left,left,1,523
001,2,right,right,1,456
```

Analysis scripts will need minor updates to handle semantic response names instead of raw key codes.

## References

- See `doc/LAYOUT_MIGRATION_GUIDE.md` for detailed migration instructions
- See `battery/evenodd/evenodd.pbl` for reference implementation
- See `doc/pman/layout.tex` (Manual Chapter 6) for Layout & Response System documentation
