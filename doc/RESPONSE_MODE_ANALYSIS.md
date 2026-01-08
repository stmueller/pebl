# Upload-Battery Response Mode Implementation Analysis

Analysis of all tests in upload-battery categorized by their response mode implementation status.

**Date:** 2026-01-08
**Status:** Work in progress

---

## ✅ 2AFC (Two-Alternative Forced Choice) - IMPLEMENTED & TESTED

All use `responsesemantics <- "2afc"` with Layout & Response System:

1. **ANT** - Attentional Network Test (arrow directions)
2. **crt** (ChoiceRT) - Choice Reaction Time
3. **dotjudgment** - Dot numerosity judgment
4. **evenodd** - Even/odd number judgment
5. **flanker** - Eriksen Flanker Task
6. **globallocal** - Global/local visual processing
7. **luckvogel** - Same/different visual comparison
8. **manikin** - Spatial compatibility task
9. **simon** - Simon effect task
10. **TNT** - Toulouse N-back (NO/YES responses)

---

## ✅ GO/NOGO - IMPLEMENTED

Uses `responsesemantics <- "gonogo"`:

11. **gonogo** - Go/No-Go task

---

## ⚠️ GO/NOGO - MISSING responsesemantics SETTING

These use CreateLayout + WaitForLayoutResponse with single-key "spacebar" mode but haven't set `responsesemantics <- "gonogo"`:

12. **clocktest** - Mackworth Clock vigilance task (responsemode: spacebar)
13. **pcpt** - PEBL Continuous Performance Test (responsemode: spacebar)
14. **pcpt-ax** - CPT A-X variant (responsemode: spacebar)
15. **ppvt** - Psychomotor Vigilance Task (responsemode: spacebar)

**Action needed:** Add `gParams.responsesemantics <- "gonogo"` to these tasks.

---

## 🔧 USES LAYOUT SYSTEM - NEEDS INVESTIGATION

These use `CreateLayout()` for zones but have specialized response patterns (need to determine appropriate responsesemantics):

16. **BST** - Brief Symptom Test (survey/rating scale)
17. **corsi** - Corsi block tapping (clicks on sequences of boxes)
18. **maze** - Maze navigation (4-way directional: WaitForClickOnTarget with arrow keys)
19. **matrixrotation** - Mental rotation (likely multichoice)
20. **oddball** - Oddball detection task
21. **ptrails** - PEBL Trail Making (mouse/touch tracking paths)
22. **stroop-color** - Color Stroop
23. **stroop-number** - Number Stroop
24. **stroop-vic** - Victoria Stroop
25. **wpt** - Wisconsin Card Sorting analog

---

## 🚧 NOT USING LAYOUT/RESPONSE SYSTEM (Old-style)

These don't use CreateLayout - use traditional PEBL response functions (need full migration):

### Cognitive/RT Tasks
26. **antisaccade** - Antisaccade task
27. **bcst** - Berg Card Sorting Test
28. **bcst-64** - BCST 64-card version
29. **BNT** - Boston Naming Test
30. **connections** - Connections puzzle
31. **dspan** - Digit span
32. **fitts** - Fitts' Law task
33. **fourchoice** - 4-choice RT (uses WaitForListKeyPress)
34. **iowa** - Iowa Gambling Task
35. **pursuitrotor** - Pursuit rotor tracking
36. **spanvariants** - Span task variants
37. **srt** - Simple Reaction Time
38. **switcher** - Task switching
39. **tapping** - Finger tapping
40. **toav** - Test of Attentional Vigilance
41. **toh** - Tower of Hanoi
42. **tol** - Tower of London
43. **urns** - Probability learning (urn task)

### Specialized Tasks
44. **BART** - Balloon Analog Risk Task (balloon inflation clicks)
45. **timewall** - Time wall estimation

---

## 📋 SURVEY/QUESTIONNAIRE TOOLS

These are survey instruments - different response architecture:

46. **bigfive** - Big Five personality survey
47. **SSSQ** - Short Stress State Questionnaire
48. **SUS** - System Usability Scale
49. **tiredness** - Tiredness assessment
50. **TLX** - NASA Task Load Index
51. **VAScales** - Visual Analog Scales

---

## 🧪 UTILITY/TEST FILES

52. **test-simple** - Simple test file

---

## Summary Statistics

- **2AFC implemented & tested**: 10 tasks ✓
- **GO/NOGO implemented & tested**: 1 task ✓
- **GO/NOGO missing responsesemantics**: 4 tasks (clocktest, pcpt, pcpt-ax, ppvt)
- **Uses layout, needs investigation**: 10 tasks
- **Old-style (not migrated)**: 20 cognitive/RT tasks
- **Survey/questionnaire tools**: 6 tasks
- **Utility**: 1 file

**Total:** 52 test directories

---

## Next Steps

### Priority 1: Add responsesemantics to existing Layout tasks
- [ ] clocktest - add `gParams.responsesemantics <- "gonogo"`
- [ ] pcpt - add `gParams.responsesemantics <- "gonogo"`
- [ ] pcpt-ax - add `gParams.responsesemantics <- "gonogo"`
- [ ] ppvt - add `gParams.responsesemantics <- "gonogo"`

### Priority 2: Investigate Layout tasks with custom responses
Determine appropriate responsesemantics for:
- corsi (sequential clicking)
- maze (4-directional navigation)
- ptrails (path tracing)
- stroop variants (color naming)
- wpt/matrixrotation (multichoice?)
- oddball (needs investigation)

### Priority 3: Migrate old-style tasks
20 tasks need full migration to Layout & Response System

### Priority 4: Survey tools
6 survey/questionnaire tools may need specialized survey response architecture

---

## Notes

- All Layout & Response System tasks should explicitly set `responsesemantics`
- The responsesemantics parameter determines semantic response labels
- Standard semantics: "2afc" (left/right), "gonogo" (go/nogo), "4afc", etc.
- Tasks can override footer text for mode-specific instructions while still using responsesemantics
