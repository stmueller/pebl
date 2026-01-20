# Upload-Battery Tests: Response Mode Categorization

Tests analyzed for Layout & Response System integration and `userselect` support.

## Category 1: Two-Alternative Forced Choice (2AFC)
**Semantic**: `["left", "right"]` or similar binary choice
**Recommended**: `gParams.responsesemantics <- "2afc"`
**Action**: Add `"userselect"` to options, add responsesemantics line

### Tests in this category:
1. **ANT** - Attention Network Test (left/right arrow direction)
   - Current: `"auto"` default, has options
   - Add: `"userselect"` to options

2. **BST** - Brief Smell Test (left/right nostril)
   - Current: `"keyboardShift"` default
   - Add: `"userselect"` to options

3. **crt** - Choice Reaction Time (left/right)
   - Current: `"auto"` default
   - Add: `"userselect"` to options

4. **dotjudgment** - Dot comparison (left/right has more)
   - Current: `"auto"` default
   - Add: `"userselect"` to options

5. **evenodd** - Even/odd number judgment
   - Current: `"auto"` default
   - Add: `"userselect"` to options
   - Note: Semantically "even"/"odd" but maps to left/right

6. **flanker** - Eriksen Flanker Task (left/right direction)
   - Current: `"auto"` default
   - Add: `"userselect"` to options

7. **globallocal** - Navon figures (identify global/local shape)
   - Current: `"auto"` default
   - Add: `"userselect"` to options

8. **luckvogel** - Luck-Vogel visual working memory (same/different)
   - Current: `"auto"` default
   - Add: `"userselect"` to options

9. **manikin** - Mental rotation (left/right hand identification)
   - Current: `"auto"` default
   - Add: `"userselect"` to options

10. **matrixrotation** - Mental rotation of 3D matrices
    - Current: `"keyboardShift"` default
    - Add: `"userselect"` to options
    - Note: Uses non-standard casing "mouseTarget" (should be "mousetarget")

11. **oddball** - Auditory/visual oddball (target/standard)
    - Current: `"auto"` default
    - Add: `"userselect"` to options

12. **simon** - Simon task (left/right spatial compatibility)
    - Current: `"auto"` default, already has extensive options
    - Add: `"userselect"` to options

13. **TNT** - Think/No-Think memory suppression
    - Current: `"auto"` default
    - Add: `"userselect"` to options

14. **wpt** - Wiener Planungstest (planning test)
    - Current: `"auto"` default
    - Add: `"userselect"` to options

15. **linejudgment** - Line length comparison (which is longer?)
    - Current: `"auto"` default, already has extensive options
    - Status: **MIGRATED** Jan 12, 2026
    - Add: `"userselect"` to options
    - Note: Migrated to PEBL 2.3 Layout & Response System with semantic labels

## Category 2: Go/No-Go (Single Key Response)
**Semantic**: `["response"]`
**Recommended**: `gParams.responsesemantics <- "gonogo"` or `"singlekey"`
**Action**: Add `"userselect"` as option (for accessibility), add responsesemantics line

### Tests in this category:
1. **clocktest** - Clock drawing test (advance/continue)
   - Current: `"spacebar"` default, has options for click/touch
   - Add: `"userselect"` to options
   - Note: Single key to advance through prompts

2. **gonogo** - Go/No-Go task
   - Current: Has params for various single-key modes
   - Add: `"userselect"` to options
   - Note: Already well-configured for multiple input types

3. **pcpt** - Penn Continuous Performance Test
   - Current: `"spacebar"` default
   - Add: `"userselect"` to options

4. **pcpt-ax** - Continuous Performance Test (AX variant)
   - Current: `"spacebar"` default
   - Add: `"userselect"` to options

5. **ppvt** - Psychomotor Vigilance Task (go/no-go RT task)
   - Current: `"spacebar"` default, already migrated to Layout & Response System
   - Add: `"userselect"` to options
   - Note: NOT the Peabody Picture test - this is vigilance/RT task

## Category 3: Multi-Alternative (4+ choices) - MIGRATION CANDIDATES
**Semantic**: Custom based on number of choices
**Action**: These tests currently use `responsemode <- "none"` and manual response handling. They COULD be migrated to use the Layout & Response System with number keys.

### Tests in this category:
1. **stroop-color** - Color Stroop (4 color choices: 1-4 keys)
   - Current: `responsemode <- "none"`, uses `WaitForListKeyPressWithTimeout` with keys 1-4
   - Status: **MIGRATION CANDIDATE** - could use Layout & Response System
   - Semantic: `["1", "2", "3", "4"]` (maps to red/blue/green/yellow)
   - Action: Could migrate to use `number` response mode + userselect + responselabels
   - Note: Requires maintaining keyboard AND vocal response options

2. **stroop-number** - Number Stroop (number key responses 1-9)
   - Current: `responsemode <- "none"`, manual response handling
   - Status: **MIGRATION CANDIDATE** - could use `"numbers"` canned semantic
   - Semantic: `"numbers"` canned label
   - Action: Could migrate to use `number` response mode + userselect

3. **stroop-vic** - Victoria Stroop
   - Current: Uses CreateLayout
   - Status: **Needs investigation** - response pattern unclear
   - Action: Check if similar to other stroop tests

## Category 4: Special/Custom Response (Not Using Standard Layout Response)
**Action**: Review individually - may not need changes

### Tests in this category:
1. **corsi** - Corsi Block Tapping (spatial memory)
   - Uses: `WaitForClickOnTarget` directly (not WaitForLayoutResponse)
   - Note: Clicks on block sequence - custom response handling
   - Action: **No changes needed** - uses mouse-only interaction

2. **maze** - Maze navigation
   - Uses: `WaitForClickOnTarget` with arrow directions
   - Semantic: 4-way directional (up/down/left/right)
   - Note: Arrow-based navigation
   - Action: **Review** - may benefit from `"arrow"` response mode + userselect

3. **ptrails** - Paper trails (connect-the-dots)
   - Uses: `responsemode <- "none"` (layout only, no response system)
   - Note: Custom click-on-circles interaction
   - Action: **No changes needed** - uses mouse-only interaction

## Summary Statistics
- **Category 1 (2AFC)**: 15 tests - all need `"userselect"` + `gParams.responsesemantics <- "2afc"`
  - Includes linejudgment (migrated Jan 12, 2026)
- **Category 2 (Go/No-Go)**: 5 tests - all need `"userselect"` + `gParams.responsesemantics <- "gonogo"`
  - Includes ppvt (Psychomotor Vigilance Task - already migrated)
- **Category 3 (Multi-choice)**: 3 tests - stroop variants that could be migrated to use Layout & Response System
- **Category 4 (Custom)**: 3 tests - no standard changes needed

## Implementation Priority
1. **High Priority**: Category 1 (15 tests) - straightforward 2AFC pattern (linejudgment completed Jan 12, 2026)
2. **High Priority**: Category 2 (5 tests) - single-key pattern (ppvt already done)
3. **Future Enhancement**: Category 3 (3 tests) - migration opportunity for stroop tests
4. **Skip**: Category 4 (3 tests) - custom response handling

## Next Steps
1. Start with Category 1 tests (2AFC)
2. For each test:
   - Add `"userselect"` to responsemode options in schema.json
   - Add `gParams.responsesemantics <- "2afc"` after CreateParameters
   - Test with userselect mode to verify semantic inference works
3. Then proceed to Category 2 tests (Go/No-Go)
4. Finally review Category 3 tests individually
