# Migration Guide: Adapting Existing PEBL Tasks to Layout & Response System

This guide demonstrates how to migrate existing PEBL battery tasks to use the unified Layout & Response System, using the `evenodd` task as a reference implementation.

## Table of Contents

1. [Overview](#overview)
2. [Benefits of Migration](#benefits-of-migration)
3. [Step-by-Step Migration Process](#step-by-step-migration-process)
4. [Before/After Comparison](#beforeafter-comparison)
5. [Common Migration Patterns](#common-migration-patterns)
6. [Testing Checklist](#testing-checklist)
7. [Troubleshooting](#troubleshooting)

## Overview

The Layout & Response System provides:
- **Platform-aware responses**: Automatic adaptation between desktop (shift keys), web (Z/slash), and touch (click targets)
- **Automatic screen scaling**: Layout zones that adapt to any screen size
- **Consistent UI**: Standardized header/footer/stimulus regions across all battery tasks
- **Zero code changes for response mode**: Users can switch via parameters, not code

**Key principle**: Preserve 95% of your existing code. Only replace the UI/response layer.

## Benefits of Migration

### Before Migration
- Hardcoded response keys (e.g., `<lshift>`, `<rshift>`)
- Manual label positioning (e.g., `gVideoWidth/2`, `gVideoHeight-200`)
- Platform-specific issues (shift keys trigger Windows Sticky Keys, don't work on web)
- No adaptation for tablets/touch screens

### After Migration
- Semantic responses (`"left"`, `"right"`)
- Automatic layout scaling
- Platform-aware response modes
- Touch-friendly interfaces available
- Configurable via parameters

## Overall Migration Status

**Last Updated**: December 15, 2025 (ptrails Category 3 migration complete - first mouse-based task migrated)

### Summary Statistics

- **Total tests in upload-battery**: 52
- **Fully migrated** (Layout + Response): 18 tests (35%) - **All 7 Category 1 tests complete** ✅
- **Need response mode extensions**: 5 tests (10%) - requires 4-choice grid or 4-way directional modes
- **Layout-only migration**: 15 tests (29%) - includes Stroop tests (4-choice keyboard with verbal interference) and custom response tasks
- **Not suitable for migration**: 14 tests (27%)

**Available response modes:**
- **2-option modes**: auto, keyboardShift, keyboardSafe, arrowLR, mousetarget, touchtarget, mousebutton
- **Single-response modes**: spacebar, leftclick, touchscreen, clicktarget
- **Specialized modes**: mouseOnly (for drawing/clicking tasks)

**Migration achievements:**
- **Full migration** (Layout + Response): 18 tests (35%) - **All 7 Category 1 tests complete Dec 15, 2025** ✅
- **With response extensions**: 23 tests (44%) - add 5 Category 2 tests (next phase)
- **Layout-only migration**: 38 tests (73%) - add 15 Category 3 tests for UI consistency (optional phase)

## Semantic Label Migration Status

**Last Updated**: December 15, 2025

The Layout & Response System was created on **December 4, 2025** (commit 0a55b63).
The **semantic-first label architecture** (allowing task-specific labels like "SAME"/"DIFFERENT") was added on **December 9, 2025** (commit d5b40e6).

### Migration Phases

**Phase 1: Pre-semantic migrations (Dec 4-7, 2025)**
Tests migrated to Layout & Response System but using generic directional semantics:
- These tests showed: `"LEFT-SHIFT (LEFT)"` / `"RIGHT-SHIFT (RIGHT)"`
- Required updating to show task-specific meanings

**Phase 2: Semantic-first migrations (Dec 9-11, 2025)**
Tests using true semantic labels with `responselabels` parameter:
- **Completed**: luckvogel, evenodd, simon, BST, dotjudgment, flanker, manikin, gonogo
- Shows task-specific labels: `"Z (SAME)"` / `"/ (DIFFERENT)"`, `"Z (ODD)"` / `"/ (EVEN)"`, etc.

### Tests Needing Semantic Label Updates (11 tests)

**Status Key:**
- ✅ = Fully migrated with semantic labels, tested, and synced to upload-battery
- 🔄 = In progress (code complete, testing in progress)
- ⚠️ = Migrated to layout system, but needs semantic label update
- ❌ = Not yet migrated

| Test | Status | Current Labels | Should Be | Migration Date | Notes |
|------|--------|----------------|-----------|----------------|-------|
| **luckvogel** | ✅ | SAME / DIFFERENT | ✓ Correct | Dec 11, 2025 | Synced to upload-battery. arrowLR added Dec 14, 2025 |
| **evenodd** | ✅ | ODD / EVEN | ✓ Correct | Dec 11, 2025 | Synced to upload-battery. arrowLR added Dec 14, 2025 |
| **dotjudgment** | ✅ | LEFT / RIGHT | ✓ Correct (spatial) | Dec 6, 2025 | Synced to upload-battery. arrowLR added Dec 14, 2025 |
| **flanker** | ✅ | LEFT / RIGHT | ✓ Correct (directional) | Dec 6, 2025 | Synced to upload-battery |
| **manikin** | ✅ | LEFT / RIGHT | ✓ Correct (spatial) | Dec 6, 2025 | Synced to upload-battery. arrowLR added Dec 14, 2025 |
| **simon** | ✅ | RED / BLUE | ✓ Correct | Dec 11, 2025 | Tested & synced to upload-battery |
| **BST** | ✅ | CIRCLE / SQUARE | ✓ Correct | Dec 11, 2025 | Tested & synced to upload-battery |
| **gonogo** | ✅ | GO (singlekey) | ✓ Correct | Dec 11, 2025 | 4 modes: spacebar, leftclick, touchscreen, clicktarget. Synced to upload-battery |
| **oddball** | ✅ | CIRCLE / SQUARE | ✓ Correct | Dec 11, 2025 | Semantic labels, touchtarget fix. Synced to upload-battery |
| **crt** | ✅ | LEFT / RIGHT | ✓ Correct (positional) | Dec 11, 2025 | Semantic-first architecture. Synced to upload-battery |
| **wpt** | ✅ | RAIN / SUN | ✓ Correct | Dec 11, 2025 | Semantic labels with translated "Tap for" prefix for touchtarget mode. Synced to upload-battery. arrowLR added Dec 14, 2025 |
| **ANT** | ✅ | LEFT / RIGHT | ✓ Correct (directional) | Dec 13, 2025 | 8 translations updated (en, es, fr, de, it, nl, pt, br). 6 parameter files (added arrowLR Dec 14). Synced to upload-battery |
| **pcpt** | ✅ | GO (singlekey) | ✓ Correct | Dec 14, 2025 | 10 translations (de, en, es, fr, he, hr, it, nl, pt, tr). 4 modes (spacebar/leftclick/touchscreen/clicktarget). Non-blocking visual feedback. Parameterized letters. Synced to upload-battery |
| **TNT** | ✅ | NO / YES | ✓ Correct | Dec 14, 2025 | Dynamic semantic labels (NO/YES), arrowLR mode, 7 translations (de, en, es, fr, it, nl, pt). Synced to upload-battery |
| **ppvt** | ✅ | GO (singlekey) | ✓ Correct | Dec 15, 2025 | PVT (Psychomotor Vigilance Task), 4 modes (spacebar/leftclick/touchscreen/clicktarget), 11 translations (de, en, es, fr, he, hr, it, nl, pt, se, tr). Synced to upload-battery |
| **template** | ⚠️ | LEFT / RIGHT | Example labels | Dec 4, 2025 | Needs semantic update |

### How to Update Tests with Semantic Labels

Tests marked ⚠️ need the `responselabels` parameter added. See luckvogel.pbl for reference implementation.

**Step 1: Add responselabels to parameter schema**

In `params/taskname.pbl.schema`:
```pebl
["responselabels", ["LABEL1", "LABEL2"]]
```

**Step 2: Add responselabels to translation files**

Create translation keys for each language (en, es, fr, de, it, nl, pt):
```json
{
  "LABEL1": "First Choice",
  "LABEL2": "Second Choice"
}
```

**Step 3: Set responselabels in Start() function**

```pebl
gParams.responselabels <- [gStrings.label1, gStrings.label2]
```

**Step 4: For dynamic labels (like luckvogel)**

If labels need to change during execution:
```pebl
## Remove old labels
loop(label, gLayout.responseLabels) { RemoveObject(label, gWin) }
loop(border, gLayout.responseBorders) { RemoveObject(border, gWin) }

## Update semantic labels
gParams.responselabels <- [gStrings.newlabel1, gStrings.newlabel2]

## Recreate labels
gLayout <- CreateResponseLabels(gLayout, fontSize)
Draw()
```

### Upload-battery/ Sync Status

All 18 fully-migrated tests (except template) have been synced from battery/ to upload-battery/:
- Synced: ANT, BST, clocktest, crt, dotjudgment, evenodd, flanker, gonogo, luckvogel, manikin, oddball, pcpt, pcpt-ax, ppvt, simon, srt, TNT, wpt
- Last updated: Dec 15, 2025

**Note**: When semantic labels are added to battery/ tests, they must be re-synced to upload-battery/.

---

## Remaining Tests Assessment

This section provides a comprehensive assessment of all remaining upload-battery tests (41 tests) for migration feasibility. Tests are categorized by their compatibility with the Layout and Response systems.

### Legend

**Layout System Feasibility:**
- ✅ **EASY** - Standard trial structure, can use header-subheader-stimulus-response-footer zones
- ⚠️ **MODERATE** - Requires adaptation but feasible (e.g., complex layouts, multiple stimuli)
- ❌ **DIFFICULT** - Custom layout requirements that don't fit zone model

**Response System Feasibility:**
- ✅ **COMPATIBLE** - Can use existing response modes (2-alternative, single-key, etc.)
- 🔧 **NEEDS_EXTENSION** - Requires new response mode (e.g., 4-way directional, 4-choice grid)
- ❌ **INCOMPATIBLE** - Fundamentally incompatible (mouse tracking, drag-and-drop, positional clicking)

**Migration Priority:**
- 🔴 **HIGH** - Commonly used, high impact
- 🟡 **MEDIUM** - Moderately used, would benefit from migration
- 🟢 **LOW** - Rarely used or limited benefit

---

### Category 1: Full Migration Ready - Standard 2-Alternative & Single-Response Tasks

**Can migrate to both Layout AND Response systems with minimal effort**

| Test | Priority | Layout | Response | Input Type | Notes |
|------|----------|--------|----------|------------|-------|
| ~~**ANT**~~ | ✅ **COMPLETE** | ✅ EASY | ✅ COMPATIBLE | kbd | **MIGRATED Dec 13, 2025** - 2-alt (left/right), 8 translations, 5 parameter files |
| ~~**srt**~~ | ✅ **COMPLETE** | ✅ EASY | ✅ COMPATIBLE | kbd | **MIGRATED Dec 14, 2025** - Single-key response, 4 modes (spacebar/leftclick/touchscreen/clicktarget), 7 translations, trial counter, visual feedback |
| ~~**pcpt**~~ | ✅ **COMPLETE** | ✅ EASY | ✅ COMPATIBLE | kbd | **MIGRATED Dec 14, 2025** - Single-key CPT, 4 modes, 10 translations, non-blocking visual feedback, parameterized letters |
| ~~**pcpt-ax**~~ | ✅ **COMPLETE** | ✅ EASY | ✅ COMPATIBLE | kbd | **MIGRATED Dec 14, 2025** - AX-CPT variant, single-key (4 modes), 10 translations, non-blocking visual feedback, larger instruction textbox |
| ~~**TNT**~~ | ✅ **COMPLETE** | ✅ EASY | ✅ COMPATIBLE | kbd | **MIGRATED Dec 14, 2025** - 2-alt think/no-think, arrowLR mode, dynamic semantic labels, 7 translations |
| ~~**clocktest**~~ | ✅ **COMPLETE** | ⚠️ MODERATE | ✅ COMPATIBLE | kbd/mouse | **MIGRATED Dec 14, 2025** - Mackworth Clock vigilance task, 3 modes (spacebar/leftclick/touchscreen), custom zones (500px stimulus), 7 translations, clock scaling with 240px max radius |
| ~~**ppvt**~~ | ✅ **COMPLETE** | ✅ EASY | ✅ COMPATIBLE | kbd/mouse | **MIGRATED Dec 15, 2025** - PVT (Psychomotor Vigilance Task), 4 modes (spacebar/leftclick/touchscreen/clicktarget), 11 translations, platform-independent instructions |

**Migration Notes:**
- All use standard trial structure with central stimulus
- All keyboard-based with 1-2 response options
- Can use existing layout zones and response modes
- **2-alternative tasks**: ~~ANT (COMPLETE)~~, ~~TNT (COMPLETE)~~
- **Single-response tasks**: ~~srt (COMPLETE)~~, ~~pcpt (COMPLETE)~~, ~~pcpt-ax (COMPLETE)~~, ~~clocktest (COMPLETE)~~, ~~ppvt (COMPLETE)~~ - use single-key mode like gonogo (spacebar, leftclick, touchscreen, or clicktarget)

**Estimated effort**: 2-3 hours per test (3-4 hours for clocktest due to canvas scaling)
**Note**: ppvt incorrectly listed as "Peabody Picture Vocabulary Test" in earlier versions - it's actually PVT (Psychomotor Vigilance Task), a simple single-response RT task

**Category 1 Completed Migrations** (All 7 tests complete as of Dec 15, 2025) ✅:
- ANT (Dec 13, 2025) - 2-alt task with 8 translations and 5 parameter files. arrowLR added Dec 14, 2025
- srt (Dec 14, 2025) - Single-key task with 4 response modes, 7 translations, trial counter, visual feedback
- pcpt (Dec 14, 2025) - Single-key CPT with 4 modes, 10 translations, non-blocking visual feedback, parameterized target/foil letters
- pcpt-ax (Dec 14, 2025) - AX-CPT variant with 4 modes, 10 translations, non-blocking visual feedback, larger instruction textbox (800×600)
- TNT (Dec 14, 2025) - 2-alt think/no-think with arrowLR mode, dynamic semantic labels (NO/YES), 7 translations
- clocktest (Dec 14, 2025) - Mackworth Clock vigilance task with 3 modes (spacebar/leftclick/touchscreen), custom zones maximizing clock space (500px stimulus on 800x600), 7 translations, adaptive clock radius (240px max, scales down proportionally for small screens)
- ppvt (Dec 15, 2025) - PVT (Psychomotor Vigilance Task) with 4 modes (spacebar/leftclick/touchscreen/clicktarget), 11 translations (de, en, es, fr, he, hr, it, nl, pt, se, tr), platform-independent instructions

---

### Category 2: Layout Migration + Response Mode Extension Needed

**Can migrate to Layout system, but requires new response modes to be implemented first**

| Test | Priority | Layout | Response | Input Type | Response Mode Needed | Notes |
|------|----------|--------|----------|------------|---------------------|-------|
| **bcst** | 🔴 HIGH | ✅ EASY | 🔧 NEEDS_EXTENSION | kbd (1-4 keys) | 4-choice grid | Card sorting - HAS keyboard variant with 4-choice response |
| **iowa** | 🟡 MEDIUM | ✅ EASY | 🔧 NEEDS_EXTENSION | kbd (1-4 keys) | 4-choice grid | Gambling task - HAS keyboard variant with 4-choice response |
| **maze** | 🔴 HIGH | ✅ EASY | 🔧 NEEDS_EXTENSION | kbd (arrows) | 4-way directional | Needs up/down/left/right arrows + on-screen buttons for touch |
| **antisaccade** | 🔴 HIGH | ✅ EASY | 🔧 NEEDS_EXTENSION | kbd (arrows) | 4-way directional | Uses 3-4 arrow keys for directional judgments, timing critical |
| **fourchoice** | 🟡 MEDIUM | ✅ EASY | 🔧 NEEDS_EXTENSION | kbd (4 keys) | 4-choice grid | 4-alternative forced choice |

**Migration Notes:**
- **bcst** and **iowa**: Verified to have keyboard variants using `WaitForListKeyPress(["1","2","3","4"])` - good migration candidates once 4-choice mode implemented
- **maze** and **antisaccade**: Require 4-way directional response mode (arrow keys + on-screen directional buttons)
- **antisaccade**: Verified to use `["<left>","<right>","<up>","<down>"]` or 3-way variant - cannot use 2-alternative response system
- **fourchoice**: Standard 4-alternative forced choice
- All require new response mode implementations (see "Response Mode Extensions Needed" section below)

**Estimated effort**:
- Response mode development: 8-10 hours per mode (one-time, reusable)
- Per-test migration: 2-3 hours each after mode implemented

---

### Category 3: Layout-Only Migration - Response System Incompatible

**Can migrate to Layout system for header/footer zones, but must keep custom response handling**

| Test | Priority | Layout | Response | Input Type | Notes |
|------|----------|--------|----------|------------|-------|
| ~~**stroop-color**~~ | ✅ **COMPLETE** | ✅ EASY | ❌ INCOMPATIBLE | kbd (4 keys) | **MIGRATED Dec 14, 2025** - Layout-only migration, custom 4-choice response handling retained |
| ~~**stroop-number**~~ | ✅ **COMPLETE** | ✅ EASY | ❌ INCOMPATIBLE | kbd (4 keys) | **MIGRATED Dec 14, 2025** - Layout-only migration, custom 4-choice response handling retained |
| ~~**ptrails**~~ | ✅ **COMPLETE** | ✅ EASY | ❌ INCOMPATIBLE | mouse clicks | **MIGRATED Dec 15, 2025** - Trailmaking test with header-only layout (responseMode=0), 700×500 field maintained, 8 duplicate functions removed (VecTimes/VecSum/Dist→Math.pbl, RemoveSubset/Insert/Rest→Design.pbl, WaitForDownClick→Utility.pbl, GetMinDist→Graphics.pbl), ArgMin/ArgMax bug fixed |
| **stroop-vic** | 🟡 MEDIUM | ✅ EASY | ❌ INCOMPATIBLE | kbd (4 keys) | 4-alt Stroop variant - requires verbal interference, keyboard-only (not suitable for mouse/touch) |
| **switcher** | 🟡 MEDIUM | ✅ EASY | ❌ INCOMPATIBLE | mouse clicks | Task switching - requires mouse click response |
| **corsi** | 🟡 MEDIUM | ⚠️ MODERATE | ❌ INCOMPATIBLE | mouse clicks | Spatial span with click sequence |
| **dspan** | 🟡 MEDIUM | ✅ EASY | ❌ INCOMPATIBLE | kbd (digits) | Digit span - needs numeric keypad input (10 keys) |
| **toh** | 🟡 MEDIUM | ⚠️ MODERATE | ❌ INCOMPATIBLE | mouse clicks | Tower of Hanoi - drag/click disks |
| **tol** | 🟡 MEDIUM | ⚠️ MODERATE | ❌ INCOMPATIBLE | mouse clicks | Tower of London - similar to ToH |
| **bcst-64** | 🟡 MEDIUM | ⚠️ MODERATE | ❌ INCOMPATIBLE | mouse clicks | 64-card variant (no keyboard mode) |
| **BART** | 🟡 MEDIUM | ⚠️ MODERATE | ❌ INCOMPATIBLE | mouse clicks | Balloon analog risk task - click to inflate |
| **urns** | 🟢 LOW | ⚠️ MODERATE | ❌ INCOMPATIBLE | mouse clicks | Probabilistic choice task |
| **tapping** | 🟡 MEDIUM | ✅ EASY | 🔧 NEEDS_EXTENSION | kbd (spacebar) | Rapid tapping task - could use single-key mode |
| **timewall** | 🟡 MEDIUM | ✅ EASY | ❌ INCOMPATIBLE | kbd (spacebar holds) | Time estimation - requires precise timing of holds |

**Migration Notes:**
- **Stroop tests**: Require 4-choice keyboard input with verbal interference - NOT suitable for mouse/touch response modes
- **Switcher**: Requires mouse click response - keep custom handling
- **Other tasks**: Complex spatial layouts, positional clicking requirements, or specialized input
- Layout system can provide header/footer but stimulus region may have limited utility for some tasks
- Response system generally not applicable (positional clicking, complex sequences, specialized input, or verbal interference requirements)
- **Recommendation**: Migrate to layout system for UI consistency, but keep custom response handling

#### Important: Layout System Elements vs Custom Task Elements

**CRITICAL DISTINCTION**: The Layout system creates elements as **properties of the gLayout object**, NOT as standalone global variables.

**Layout system elements** (created by Layout system):
- `gLayout.header` - Header label (available in all Layout modes)
- `gLayout.subheader` - Subheader label (available in all Layout modes)
- `gLayout.footer` - Footer label (available even with responseMode=0)
- `gLayout.responseLabels` - Response label list (only with Response System, responseMode > 0)
- `gLayout.responseBorders` - Response border list (only with Response System, responseMode > 0)

**Custom task elements** (ONLY create when Layout doesn't provide them):
- Task-specific stimulus labels in response zone (when not using Response System)
- Specialized UI elements unique to your task
- **Do NOT create**: `gFooter1`, `gFooter2`, `gHeader` - these duplicate Layout functionality

**Key points:**

1. **When using `CreateLayout(taskname, gWin, 0)` (responseMode=0 for Category 3 migrations):**
   - The Layout system DOES create `gLayout.header`, `gLayout.subheader`, and `gLayout.footer`
   - The Layout system does NOT create `gLayout.responseLabels` (requires Response System)
   - **You SHOULD use `gLayout.footer.text` for footer content** - don't create custom footer labels

2. **Custom task variables should ONLY be created when Layout doesn't provide them:**
   - Don't create `gFooter1`, `gFooter2`, `gHeader` - these duplicate Layout functionality
   - DO create custom elements when Layout doesn't provide them (e.g., response instruction labels in response zone)
   - Position custom elements using Layout zones: `gLayout.zones.response.y`, etc.

3. **For Category 3 layout-only migrations:**
   - Call `CreateLayout(taskname, gWin, 0)` to get responsive zones
   - Use `gLayout.centerX`, `gLayout.centerY` for positioning
   - **Use `gLayout.footer.text` for footer content** (don't create custom footers)
   - Only create custom elements when absolutely necessary (Layout doesn't provide them)

**Example (Stroop tasks - CORRECTED):**
```pebl
## Category 3 migration: Layout zones only, no Response System
CreateLayout("stroop-number", gWin, 0)  ## responseMode=0

## Use Layout zones for responsive positioning
Move(gStimLabel, gLayout.centerX, gLayout.centerY)

## CORRECT: Use gLayout.footer for footer content (it DOES exist even with responseMode=0)
gLayout.footer.text <- "Identify QUANTITY"
Show(gLayout.footer)

## Create custom labels ONLY when Layout doesn't provide them
## For response instructions (Layout doesn't provide this without Response System)
responseInstructionFont <- MakeFont(gPEBLBaseFont, 0, Round(20 * gLayout.scale), fg, gBG, 1)
gResponseInstruction <- MakeLabel("", responseInstructionFont)
AddObject(gResponseInstruction, gWin)
Move(gResponseInstruction, gLayout.centerX, gLayout.zones.response.y + (gLayout.zones.response.height / 2))
```

**Common error:**
```pebl
## WRONG - creating custom footer when Layout provides one
gFooter1 <- MakeLabel("footer text", gHeaderFont)
Move(gFooter1, gLayout.centerX, gVideoHeight - 100)
AddObject(gFooter1, gWin)

## CORRECT - use Layout's footer
gLayout.footer.text <- "footer text"
Show(gLayout.footer)

## The Layout system provides gLayout.footer even with responseMode=0
## Don't create gFooter1, gFooter2, etc. - use gLayout.footer instead
```

#### CRITICAL LESSON LEARNED: Stroop Task Migrations (Dec 14, 2025)

**During the stroop-color and stroop-number migrations, a fundamental misunderstanding of Category 3 migrations occurred that wasted significant time and effort.**

**THE MISTAKE:**

Initially, the Stroop tasks were migrated by:
1. Calling `CreateLayout(taskname, gWin, 0)` to get responsive zones
2. Creating **custom footer labels** (gFooter1, gFooter2, gFooter1a) positioned using Layout zones
3. Treating the migration as "use Layout zones for positioning, but create all UI elements yourself"

**This completely defeated the purpose of the Layout system migration.**

**THE CORRECT APPROACH:**

Even with `responseMode=0` (Category 3, no Response System), the Layout system **still provides gLayout.footer**. The correct migration pattern is:

1. Call `CreateLayout(taskname, gWin, 0)` to get responsive zones
2. **USE gLayout.footer for footer text** - set via `gLayout.footer.text <- "your text"`
3. Only create custom elements when the Layout system doesn't provide them
4. For the Stroop tasks, this meant:
   - **Use gLayout.footer** for secondary info (e.g., "Identify QUANTITY")
   - **Create custom gResponseInstruction label** for response zone (Layout doesn't provide this without Response System)
   - Position custom labels using Layout zones: `gLayout.zones.response.y`, `gLayout.zones.response.height`

**KEY PRINCIPLE:**

**"Don't create custom UI elements when the Layout system provides them"**

The Layout system creates these elements as `gLayout` properties:
- `gLayout.header` - Available in all Layout modes
- `gLayout.subheader` - Available in all Layout modes
- `gLayout.footer` - **Available even with responseMode=0**
- `gLayout.responseLabels` - Only with Response System (responseMode > 0)

**Category 3 migrations should:**
- ✅ Use `gLayout.header.text` for header content
- ✅ Use `gLayout.subheader.text` for subheader content
- ✅ Use `gLayout.footer.text` for footer content
- ✅ Use Layout zones for positioning: `gLayout.centerX`, `gLayout.centerY`, `gLayout.zones.*`
- ✅ Create custom elements ONLY when Layout doesn't provide them
- ❌ Do NOT create gFooter1, gFooter2, or other custom UI elements that duplicate Layout functionality

**This lesson applies to all Category 3 migrations going forward.**

---

### Category 4: Not Suitable for Migration - Fundamentally Incompatible

**Tasks that should NOT be migrated due to fundamental incompatibilities**

#### Mouse/Touch Tracking Required

| Test | Priority | Layout | Response | Input Type | Notes |
|------|----------|--------|----------|------------|-------|
| **pursuitrotor** | 🟡 MEDIUM | ❌ DIFFICULT | ❌ INCOMPATIBLE | mouse tracking | Requires continuous mouse tracking of moving target |
| **fitts** | 🟡 MEDIUM | ❌ DIFFICULT | ❌ INCOMPATIBLE | mouse movement | Fitts' law - requires precise mouse movement measurement |
| **toav** | 🟢 LOW | ❌ DIFFICULT | ❌ INCOMPATIBLE | mouse movement | Time-on-task, mouse-based |

#### Survey/Form-Based (Not Trial-Based)

| Test | Priority | Layout | Response | Input Type | Notes |
|------|----------|--------|----------|------------|-------|
| **bigfive** | 🟢 LOW | N/A | N/A | mouse (forms) | Personality questionnaire |
| **SSSQ** | 🟢 LOW | N/A | N/A | mouse (forms) | Survey - state self-esteem |
| **SUS** | 🟢 LOW | N/A | N/A | mouse (forms) | System usability scale |
| **tiredness** | 🟢 LOW | N/A | N/A | mouse (forms) | Fatigue questionnaire |
| **TLX** | 🟢 LOW | N/A | N/A | mouse (forms) | NASA Task Load Index |
| **VAScales** | 🟢 LOW | N/A | N/A | mouse (slider) | Visual analog scales |

#### Specialized/Unique Requirements

| Test | Priority | Layout | Response | Input Type | Notes |
|------|----------|--------|----------|------------|-------|
| **connections** | 🟡 MEDIUM | ⚠️ MODERATE | ❌ INCOMPATIBLE | mouse clicks | Matrix connections task - complex grid layout, works fine as-is |
| **BNT** | 🟢 LOW | ⚠️ MODERATE | N/A | verbal/text | Boston Naming Test - requires verbal/typed responses |
| **test-simple** | N/A | N/A | N/A | N/A | Test/demo file, not a real task |

**Migration Notes:**
- **Mouse tracking tasks**: Cannot be adapted to keyboard-only; layout system provides minimal benefit
- **Surveys/questionnaires**: Use form/slider interfaces, not trial-based structure; incompatible with Layout/Response paradigm
- **Specialized tasks**: Have unique requirements (verbal responses, text entry, complex custom layouts)
- **connections**: Works fine with current implementation, complex grid layout makes migration unnecessary
- **Recommendation**: Do NOT migrate these tests

---

### Response Mode Extensions Needed

To migrate Category 2 tests, the following response modes need to be implemented:

#### 1. Four-Choice Grid Mode

**For**: bcst, iowa, fourchoice

**Implementation**:
- **Keyboard**: Number keys 1-4 or letter keys (Q/W/E/R)
- **Mouse**: 2x2 grid of click targets in response zone
- **Touch**: Same as mouse
- **Semantic responses**: "choice1", "choice2", "choice3", "choice4" (or semantic labels like "deck1", "deck2", etc.)

**Visual layout**: 2x2 grid of clickable targets in response zone

**Estimated development**: 8-10 hours

#### 2. Four-Way Directional Mode

**For**: maze, antisaccade

**Implementation**:
- **Keyboard**: Arrow keys (up/down/left/right)
- **Mouse**: 4 directional click targets in cross/compass pattern
- **Touch**: Same as mouse
- **Semantic responses**: "up", "down", "left", "right"

**Visual layout**: Cross or compass pattern in response zone

**Estimated development**: 8-10 hours

#### 3. Numeric Keypad Mode (Optional, Low Priority)

**For**: dspan (if migration desired)

**Implementation**:
- **Keyboard**: Number keys 0-9
- **Mouse**: On-screen numeric keypad (3x4 grid)
- **Touch**: Same as mouse
- **Semantic responses**: "0" through "9"

**Visual layout**: 3x4 numeric keypad in response zone

**Estimated development**: 6-8 hours

---

### Migration Recommendations & Priorities

#### ✅ Category 1 Tests - All 7 Complete (Dec 15, 2025) ✅

**All tests completed**:
1. ✅ **ANT** - COMPLETE (Dec 13, 2025) - 2-alt attention network test with 8 translations. arrowLR added Dec 14, 2025
2. ✅ **srt** - COMPLETE (Dec 14, 2025) - Simple RT with 4 response modes, 7 translations, trial counter, visual feedback
3. ✅ **pcpt** - COMPLETE (Dec 14, 2025) - Continuous performance task with 4 modes, 10 translations, non-blocking visual feedback
4. ✅ **pcpt-ax** - COMPLETE (Dec 14, 2025) - AX-CPT variant with 4 modes, 10 translations, non-blocking visual feedback, larger instruction textbox
5. ✅ **TNT** - COMPLETE (Dec 14, 2025) - 2-alt think/no-think with arrowLR mode, dynamic semantic labels (NO/YES), 7 translations
6. ✅ **clocktest** - COMPLETE (Dec 14, 2025) - Mackworth Clock vigilance task with 3 modes (spacebar/leftclick/touchscreen), custom zones (500px stimulus on 800x600), 7 translations, adaptive clock radius (240px max)
7. ✅ **ppvt** - COMPLETE (Dec 15, 2025) - PVT (Psychomotor Vigilance Task) with 4 modes (spacebar/leftclick/touchscreen/clicktarget), 11 translations, platform-independent instructions

#### Medium Priority: Category 2 Tests (5 tests - after response modes implemented)

**Requires 4-choice grid mode first**:
1. **bcst** - Card sorting (keyboard variant exists)
2. **iowa** - Gambling task (keyboard variant exists)
3. **fourchoice** - Standard 4-alternative choice

**Requires 4-way directional mode first**:
4. **maze** - Spatial navigation
5. **antisaccade** - Directional judgment (timing critical, widely used)

**Estimated total effort**:
- Response mode development: 16-20 hours (both modes)
- Test migrations: 10-15 hours (5 tests)
- **Total**: 26-35 hours

#### Lower Priority: Category 3 Tests (15 tests - layout-only)

These tests can be migrated to use layout zones for UI consistency, but must keep custom response handling:

**High priority (keyboard-only, 4-choice with verbal interference):**
- **stroop-color**, **stroop-number**, **stroop-vic** - Require custom 4-choice keyboard handling, not suitable for response system

**Medium priority (custom response requirements):**
- **switcher**, **ptrails**, **connections**, **corsi**, **dspan**, **toh**, **tol**, **bcst-64**, **BART**, **urns**, **tapping**, **timewall**

**Recommendation**: Defer until Category 1 and 2 are complete. Benefit is primarily UI consistency. Stroop tests are high priority due to widespread use.

**Estimated effort**: 3-4 hours per test (layout adaptation only)

#### Not Recommended: Category 4 Tests (14 tests)

These tests should NOT be migrated:
- Mouse tracking: pursuitrotor, fitts, toav
- Surveys: bigfive, SSSQ, SUS, tiredness, TLX, VAScales
- Specialized: BNT, test-simple

---

### arrowLR Response Mode

**Added**: December 14, 2025

A new global response mode `arrowLR` was added to support 2-option tasks using left/right arrow keys. This mode is suitable for tasks where arrow keys provide a more intuitive directional response than shift keys.

**Mode definition** (`media/settings/response-modes.json`):
```json
"arrowLR": {
  "type": "keyboard",
  "keys": ["<left>", "<right>"],
  "labels": ["←", "→"],
  "semantic": ["left", "right"],
  "platforms": ["all"],
  "comment": "Left/right arrow keys for two-option tasks"
}
```

**Label formatting**: For 2-option keyboard modes (including arrowLR), labels are displayed symmetrically with keys on the inside:
- Left label: `"semantic key"` (e.g., "NO ←", "ODD ←", "left ←")
- Right label: `"key semantic"` (e.g., "→ YES", "→ EVEN", "→ right")

This creates visual symmetry where the arrow keys point toward the center of the screen.

**Tasks with arrowLR support** (as of Dec 14, 2025):
- **ANT** - Attention Network Test (directional cues)
- **BST** - Binary Search Task (spatial targets)
- **crt** - Choice Reaction Time (positional stimuli)
- **dotjudgment** - Dot Judgment (spatial comparison)
- **evenodd** - Even/Odd Judgment (with semantic labels ODD/EVEN)
- **flanker** - Eriksen Flanker Task (directional responses)
- **luckvogel** - Luck & Vogel Visual Working Memory (same/different)
- **manikin** - Manikin Test (spatial orientation)
- **oddball** - Oddball Task (target detection)
- **simon** - Simon Task (spatial congruency)
- **TNT** - Think/No-Think Task (with semantic labels NO/YES)
- **wpt** - Weather Prediction Task (rain/sun)

**When to use arrowLR**:
- Tasks with inherently directional or spatial responses (left/right, up/down concepts)
- Tasks where arrow keys are more intuitive than alphabetic keys
- As an alternative to shift keys for users who prefer arrow navigation
- Tasks with semantic meanings that map well to left/right (odd/even, no/yes, same/different when combined with responselabels)

**How to add arrowLR to a task**:

1. Add `"arrowLR"` to the responsemode options in `params/taskname.pbl.schema.json`:
```json
{
  "name": "responsemode",
  "type": "string",
  "default": "auto",
  "options": ["auto", "keyboardShift", "keyboardSafe", "arrowLR", "mousetarget", "mousebutton"],
  "description": "Response method: auto (platform-aware), keyboardShift (L/R shift), keyboardSafe (Z/slash for web), arrowLR (left/right arrows), mousetarget (click targets), mousebutton (L/R mouse buttons)"
}
```

2. Create a parameter preset file `params/taskname-arrowLR.par.json`:
```json
{
  "responsemode": "arrowLR"
}
```

3. Test the task with arrowLR mode:
```bash
bin/pebl2 battery/taskname/taskname.pbl -s 1 --pfile params/taskname-arrowLR.par.json
```

**Note**: No changes to the main .pbl file are needed. The Layout & Response System automatically handles arrowLR mode when it's specified as the responsemode parameter.

---

### Summary Statistics

**Total upload-battery tests**: 52

**Migration status breakdown:**
- ✅ **Already migrated**: 21 tests (40%) - **All 7 Category 1 tests + 3 Category 3 tests complete** ✅
- 🟡 **Category 2** (needs response extensions): 5 tests (10%)
- 🟠 **Category 3** (layout-only): 11 tests remaining (21%)
- 🔴 **Category 4** (not suitable): 15 tests (29%)

**Realistic migration targets:**
- **Full migration** (Layout + Response): 18 tests (35% of battery) ✅ **COMPLETE**
  - All 7 Category 1 tests complete (35%)
- **With response mode extensions**: 23 tests (44% of battery)
  - Add 5 Category 2 tests after implementing new modes (4-choice grid, 4-way directional)
- **Layout-only** (UI consistency): 32 tests (62% of battery) - **3 of 14 complete** 🔄
  - Completed: stroop-color, stroop-number, ptrails (Dec 14-15, 2025)
  - Remaining: 11 Category 3 tests with custom response handling

**Not recommended for migration**: 15 tests (29% of battery)

---

### Next Steps

**Phase 1: Complete Category 1 migrations - ✅ COMPLETE (All 7 tests migrated)**
1. ✅ ANT - COMPLETE (Dec 13, 2025)
2. ✅ srt - COMPLETE (Dec 14, 2025)
3. ✅ pcpt - COMPLETE (Dec 14, 2025)
4. ✅ pcpt-ax - COMPLETE (Dec 14, 2025)
5. ✅ TNT - COMPLETE (Dec 14, 2025)
6. ✅ clocktest - COMPLETE (Dec 14, 2025)
7. ✅ ppvt - COMPLETE (Dec 15, 2025)
8. **Status**: All 18 Category 1 tests complete (100%) ✅
9. **Achievement**: Full multi-platform support for ALL standard 2-choice and single-response tasks

**Phase 2: Implement response mode extensions (Medium-term)**
1. Develop 4-choice grid response mode
2. Develop 4-way directional response mode
3. Estimated: 16-20 hours total
4. **Impact**: Enables migration of 5 additional high-value tests

**Phase 3: Migrate Category 2 tests (After Phase 2)**
1. Migrate bcst, iowa, fourchoice, maze, antisaccade
2. Estimated: 10-15 hours total
3. **Impact**: Brings total to 23 fully-migrated tests (44% of battery)

**Phase 4: Consider Category 3 layout-only migrations (Optional)**
1. Migrate Stroop tests (high priority - widely used), switcher, ptrails, corsi, dspan, etc. for UI consistency
2. Keep custom response handling (Stroop tests require 4-choice keyboard with verbal interference)
3. Estimated: 45-60 hours total (15 tests)
4. **Impact**: UI consistency across 73% of battery, including high-value Stroop tests

**Key Principle**: Focus migration efforts on tests that provide clear benefits (platform compatibility, user configurability, consistent UX). Accept that ~27% of battery tests have specialized requirements that justify custom implementations.

---

## Step-by-Step Migration Process

### Step 1: Add responsemode Parameter

**File**: `params/taskname.pbl.schema.json`

Add a new parameter to your schema:

```json
{
  "name": "responsemode",
  "type": "string",
  "default": "auto",
  "options": ["auto", "keyboardShift", "keyboardSafe", "mousetarget", "mousebutton"],
  "label": "Response Mode",
  "description": "Response method: auto (platform-aware), keyboardShift (L/R shift), keyboardSafe (Z/slash for web), arrowLR (left/right arrows), mousetarget (click targets), mousebutton (L/R mouse buttons)"
}
```

**File**: Main `.pbl` script (in `Start()` function)

Add to parameter pairs:

```pebl
## BEFORE
parPairs <- [["reps", 10],
             ["isi", 500]]

## AFTER
parPairs <- [["reps", 10],
             ["isi", 500],
             ["responsemode", "auto"]]  ## NEW
```

### Step 2: Create Layout in Start()

**Location**: In `Start()` function, after creating window and loading parameters, but before first Draw()

**BEFORE:**
```pebl
gWin <- MakeWindow("black")
gParams <- CreateParameters(parpairs, gParamFile)
GetStrings(gLanguage)

## Get subject number...
## Create data file...

Draw()
MessageBox(gStrings.inst1, gWin)
```

**AFTER:**
```pebl
gWin <- MakeWindow("black")
gParams <- CreateParameters(parpairs, gParamFile)
GetStrings(gLanguage)

## Get subject number...
## Create data file...

## NEW: Create layout with response system
gLayout <- CreateLayout("taskname", gWin, gParams)

## NEW: Configure layout zones
gLayout.header.text <- gStrings.header
gLayout.subheader.visible <- 0  ## Hide if not needed
## Footer is set automatically by layout system

Draw()
MessageBox(gStrings.inst1, gWin)
```

**Key points:**
- Call `CreateLayout()` AFTER parameters are loaded (it reads `gParams.responsemode`)
- Call BEFORE first `Draw()`
- Set header text from translations
- Hide subheader if your task doesn't use it
- Footer is automatically generated based on response mode

### Step 3: Update MessageBox to Fit Stimulus Region

**IMPORTANT**: When using `MessageBox()` or `EasyTextBox()` for instructions/breaks, these need to fit within the stimulus region to avoid overlapping with the layout's header and footer.

**Option 1: Use AdaptiveTextBox (Recommended)**

`AdaptiveTextBox()` automatically adapts to fit content within the stimulus region, preventing overflow and overlap with layout zones. It's especially useful for long instruction text that might not fit on smaller screens.

```pebl
AdaptiveTextBox(text, x, y, window, fontsize, width, height, adaptive, maxlines)
```

**Adaptation strategies:**
- `"scalefont"` - Reduces font size to fit text (preserves box dimensions)
- `"scalebox"` - Expands box maintaining aspect ratio, then scales down using zoom (preserves font size)
- `0` - No adaptation (creates standard textbox)

**BEFORE:**
```pebl
define MessageKeyBox(message)
{
  tb <- EasyTextBox(message, 100, 100, gWin, 28, gVideoWidth-200, gVideoHeight-200)
  Draw()
  WaitForAnyKeyPress()
  RemoveObject(tb, gWin)
}
```

**AFTER:**
```pebl
define MessageKeyBox(message)
{
  ## Use AdaptiveTextBox to fit within stimulus region
  ## Automatically reduces font or scales box if text doesn't fit
  tb <- AdaptiveTextBox(message,
                        gLayout.stimulusRegion.x + 20,
                        gLayout.stimulusRegion.y + 20,
                        gWin, 56,
                        gLayout.stimulusRegion.width - 40,
                        gLayout.stimulusRegion.height - 40,
                        "scalefont")  ## or "scalebox" to preserve font size
  Draw()
  WaitForAnyKeyPress()
  RemoveObject(tb, gWin)
}
```

**Benefits:**
- Automatically handles text overflow
- Adapts to different screen sizes
- Prevents overlap with header/footer
- No need to manually calculate gutters
- Choice of adaptation strategy (scale font vs scale box)

**Option 2: Use MessageBox with custom gutters**

`MessageBox()` has gutter parameters that can be overridden:

```pebl
MessageBox(text, win, fontsize:20, xleftgutter:100, xrightgutter:100,
           bottomgutter:200, useBG:1, ack:"<OK>")
```

**BEFORE:**
```pebl
MessageBox(gStrings.interblock, gWin)  ## Uses default gutters
```

**AFTER:**
```pebl
## Calculate gutters to fit stimulus region
leftgutter <- gLayout.stimulusRegion.x + 20
rightgutter <- gVideoWidth - (gLayout.stimulusRegion.x + gLayout.stimulusRegion.width) + 20
bottomgutter <- gVideoHeight - (gLayout.stimulusRegion.y + gLayout.stimulusRegion.height) + 20

MessageBox(gStrings.interblock, gWin, 20, leftgutter, rightgutter, bottomgutter)
```

**Option 3: Create custom MessageKeyBox with EasyTextBox**

For tasks that use a custom message function without adaptive features:

**BEFORE:**
```pebl
define MessageKeyBox(message)
{
  tb <- EasyTextBox(message, 100, 100, gWin, 28, gVideoWidth-200, gVideoHeight-200)
  Draw()
  WaitForAnyKeyPress()
  RemoveObject(tb, gWin)
}
```

**AFTER:**
```pebl
define MessageKeyBox(message)
{
  ## Use layout stimulus region to avoid overlap with header/footer
  ## Textbox positioned by upper-left corner
  tb <- EasyTextBox(message,
                    gLayout.stimulusRegion.x + 20,
                    gLayout.stimulusRegion.y + 20,
                    gWin, 28,
                    gLayout.stimulusRegion.width - 40,
                    gLayout.stimulusRegion.height - 40)
  Draw()
  WaitForAnyKeyPress()
  RemoveObject(tb, gWin)
}
```

**Note**: This option may still overflow if text is too long. Use AdaptiveTextBox (Option 1) if overflow is a concern.

**Option 4: Temporarily hide header/footer**

If you prefer to keep using default MessageBox parameters:

```pebl
## Hide layout zones during MessageBox
gLayout.header.visible <- 0
gLayout.footer.visible <- 0
Draw()
MessageBox(message, gWin)
gLayout.header.visible <- 1
gLayout.footer.visible <- 1
Draw()
```

### Step 4: Replace Manual Labels with Layout Zones

**CRITICAL: Replace ALL instances of `gVideoWidth/2` and `gVideoHeight/2` with layout positioning**

This is a common migration mistake that will cause positioning errors. The layout system creates a stimulus region that excludes the header, subheader, response zone, and footer. Using `gVideoHeight/2` will position elements in the absolute center of the screen, which may be covered by the footer or response labels.

**Common locations to check:**
- Trial() function - stimulus positioning
- DoInstructions() - demo stimuli and cue positioning
- Any function that creates fixation crosses, feedback, or other centered elements
- Any function that positions elements relative to screen dimensions

**BEFORE (typical Trial() function):**
```pebl
define Trial(stimulus)
{
  ## Create labels manually
  header <- EasyLabel(gStrings.header, gVideoWidth/2, 100, gWin, 40)
  stim <- EasyLabel(stimulus, gVideoWidth/2, gVideoHeight/2, gWin, 80)
  footer <- EasyLabel(gStrings.footer, gVideoWidth/2, gVideoHeight-200, gWin, 30)

  ## Cues and fixations also used old positioning
  fixation <- EasyLabel("+", gVideoWidth/2, gVideoHeight/2, gWin, 40)
  cueTop <- EasyLabel("*", gVideoWidth/2, gVideoHeight/2 - 50, gWin, 30)

  Draw()
  ## ... response collection ...

  ## Clean up
  Hide(header)
  Hide(stim)
  Hide(footer)
}
```

**AFTER:**
```pebl
define Trial(stimulus)
{
  ## Header and footer are already set in layout (in Start())
  ## Just create stimulus in center of stimulus region
  stim <- EasyLabel(stimulus, gLayout.centerX, gLayout.centerY, gWin, 80)

  ## ALL positioning should use layout coordinates
  fixation <- EasyLabel("+", gLayout.centerX, gLayout.centerY, gWin, 40)
  cueTop <- EasyLabel("*", gLayout.centerX, gLayout.centerY - 50, gWin, 30)

  Draw()
  ## ... response collection ...

  ## Only clean up stimulus
  Hide(stim)
}
```

**Key changes:**
- Remove header/footer label creation (set once in `Start()`)
- **Replace `gVideoWidth/2` with `gLayout.centerX`** (or `gLayout.stimulusRegion.centerX`)
- **Replace `gVideoHeight/2` with `gLayout.centerY`** (or `gLayout.stimulusRegion.centerY`)
- Replace all screen-relative positioning with layout-relative positioning
- Only hide/remove the stimulus object

**Convenience note:** `gLayout.centerX` and `gLayout.centerY` are shorthand for `gLayout.stimulusRegion.centerX/Y`. Both work identically - use whichever you prefer. The top-level properties are provided as a convenient replacement for the common pattern of `gVideoWidth/2, gVideoHeight/2`.

**Available Layout Properties:**

The layout object provides these properties for positioning elements:

**Stimulus region** (most commonly used):
- `gLayout.stimulusRegion.x` - Left edge of stimulus region
- `gLayout.stimulusRegion.y` - Top edge of stimulus region
- `gLayout.stimulusRegion.width` - Width of stimulus region
- `gLayout.stimulusRegion.height` - Height of stimulus region
- `gLayout.stimulusRegion.centerX` - Horizontal center of stimulus region
- `gLayout.stimulusRegion.centerY` - Vertical center of stimulus region

**Convenience shortcuts** (same as stimulus region centers):
- `gLayout.centerX` - Same as `gLayout.stimulusRegion.centerX`
- `gLayout.centerY` - Same as `gLayout.stimulusRegion.centerY`

**UI elements** (header, footer, etc.):
- `gLayout.header` - Header label object (`.text`, `.visible` properties)
- `gLayout.subheader` - Subheader label object (`.text`, `.visible` properties)
- `gLayout.footer` - Footer label object (`.text`, `.visible` properties)

**Zone boundaries** (advanced usage - for positioning feedback, status messages, etc.):
- `gLayout.zones.header.y` - Y position of header zone
- `gLayout.zones.header.height` - Height of header zone
- `gLayout.zones.subheader.y` - Y position of subheader zone
- `gLayout.zones.subheader.height` - Height of subheader zone
- `gLayout.zones.response.y` - Y position of response zone (between stimulus and footer)
- `gLayout.zones.response.height` - Height of response zone
- `gLayout.zones.footer.y` - Y position of footer zone
- `gLayout.zones.footer.height` - Height of footer zone

**Example usage:**
```pebl
## Position stimulus in center of stimulus region
stim <- EasyLabel("TEXT", gLayout.centerX, gLayout.centerY, gWin, 80)

## Position feedback in response zone
feedbackY <- gLayout.zones.response.y + gLayout.zones.response.height/2
feedback <- MakeLabel("Correct!", font)
Move(feedback, gVideoWidth/2, feedbackY)

## Position textbox within stimulus region
tb <- AdaptiveTextBox(message,
                      gLayout.stimulusRegion.x + 20,
                      gLayout.stimulusRegion.y + 20,
                      gWin, 56,
                      gLayout.stimulusRegion.width - 40,
                      gLayout.stimulusRegion.height - 40,
                      "scalefont")
```

### Step 4: Replace Response Collection

**BEFORE:**
```pebl
define Trial(stimulus)
{
  ## ... create stimuli ...
  Draw()

  time1 <- GetTime()
  resp <- WaitForListKeyPress(["<lshift>", "<rshift>"])
  time2 <- GetTime()
  rt <- time2 - time1

  ## Check correctness with raw key names
  if(correctResponse == "left")
  {
    correct <- (resp == "<lshift>")
  } else {
    correct <- (resp == "<rshift>")
  }

  return [stimulus, resp, correct, rt]
}
```

**AFTER:**
```pebl
define Trial(stimulus)
{
  ## ... create stimuli ...
  Draw()

  time1 <- GetTime()

  ## Returns semantic response: "left", "right", or "<timeout>"
  resp <- WaitForLayoutResponse(gLayout, 5000)

  time2 <- GetTime()
  rt <- time2 - time1

  ## Check correctness with semantic names
  correct <- (resp == correctResponse)

  return [stimulus, resp, correct, rt]
}
```

**Key changes:**
- `WaitForListKeyPress(["<lshift>", "<rshift>"])` → `WaitForLayoutResponse(gLayout, 5000)`
- Response is now semantic: `"left"`, `"right"`, or `"<timeout>"`
- Add timeout (5000ms recommended, adjust as needed)
- Correctness logic uses semantic names, not raw keys

### Step 5: Update Translation Strings

**IMPORTANT**: You must update ALL available language translations, not just English. Tasks typically have translations in multiple languages (en, es, lt, etc.).

**Files**: `translations/taskname.pbl-en.json`, `translations/taskname.pbl-es.json`, `translations/taskname.pbl-lt.json`, etc.

**Check which translations exist:**
```bash
ls battery/taskname/translations/
```

**BEFORE (English):**
```json
{
  "HEADER": "Task Name - Trial 1",
  "FOOTER": "<left shift> LEFT                    RIGHT <right shift>",
  "INSTRUCTIONS": "Press LEFT SHIFT for left, RIGHT SHIFT for right.",
  "FOOTERL": "left-shift",
  "FOOTERR": "right-shift"
}
```

**AFTER (English):**
```json
{
  "HEADER": "Task Name - Trial 1",
  "FOOTER": "LEFT                                    RIGHT",
  "INSTRUCTIONS": "Press the LEFT response key for left, the RIGHT response key for right.",
  "FOOTERL": "LEFT",
  "FOOTERR": "RIGHT"
}
```

**AFTER (Spanish example):**
```json
{
  "HEADER": "Tarea - Prueba 1",
  "FOOTER": "IZQUIERDA                                    DERECHA",
  "INSTRUCTIONS": "Presiona el botón de respuesta IZQUIERDA para izquierda, el botón de respuesta DERECHA para derecha.",
  "FOOTERL": "IZQUIERDA",
  "FOOTERR": "DERECHA"
}
```

**AFTER (Lithuanian example):**
```json
{
  "HEADER": "Užduotis - Bandymas 1",
  "FOOTER": "KAIRĖ                                    DEŠINĖ",
  "INSTRUCTIONS": "Spauskite KAIRĮJĮ atsakymo mygtuką kairėje, DEŠINĮJĮ atsakymo mygtuką dešinėje.",
  "FOOTERL": "KAIRĖ",
  "FOOTERR": "DEŠINĖ"
}
```

**Key changes for ALL languages:**
- Update `FOOTER` to show semantic mapping (e.g., "ODD ... EVEN", "LEFT ... RIGHT") WITHOUT hardcoded keys
- The layout system adds response key labels automatically below your footer text
- Update `INSTRUCTIONS` to use generic "response key" language instead of hardcoded "shift key", "left shift", "shift izquierdo", etc.
- Add or update `HEADER` string if not already present
- Update `FOOTERL` and `FOOTERR` to show semantic labels (used in instruction screens)
- Translate `INTERBLOCK` and `DEBRIEF` messages if they weren't already translated

**Example for evenodd task (English):**
```json
{
  "HEADER": "Decide whether odd or even",
  "FOOTER": "ODD                                      EVEN",
  "INSTRUCTIONS": "When a number is ODD, press the LEFT response key...",
  "FOOTERL": "ODD",
  "FOOTERR": "EVEN"
}
```

The footer shows the semantic mapping (ODD/EVEN), and the layout system automatically displays the actual response keys (shift/Z/click target) below it based on the current response mode.

**Translation workflow:**
1. Start with English (`taskname.pbl-en.json`)
2. Identify what needs to change (remove shift key references, add HEADER/FOOTER if missing)
3. Update each available language translation following the same pattern
4. Ensure semantic meanings are preserved (e.g., "LEFT"/"RIGHT", "IZQUIERDA"/"DERECHA", "KAIRĖ"/"DEŠINĖ")

### Step 6: Update Data Output (If Needed)

If your data file records the response:

**BEFORE:**
```
subnum,trial,stimulus,response,correct,rt
001,1,stimulus1,<lshift>,1,523
001,2,stimulus2,<rshift>,1,456
```

**AFTER:**
```
subnum,trial,stimulus,response,correct,rt
001,1,stimulus1,left,1,523
001,2,stimulus2,right,1,456
```

**Note**: Response values change from raw keys (`<lshift>`) to semantic names (`left`). This is EXPECTED and CORRECT. Your analysis scripts may need minor updates to handle semantic names.

## Common Issues Checklist

These issues were discovered during migrations and should be addressed proactively in every task migration:

### Issue 1: Layout Creation Timing

**Problem**: Layout footer appears before GetSubNum() dialog if layout is created too early.

**Solution**: Always create layout AFTER GetSubNum(), not before.

**Code pattern:**
```pebl
gWin <- MakeWindow("black")
Initialize()  ## This calls GetStrings()

## Get subject number FIRST
if(gSubNum+""=="0")
{
  gSubNum <- GetSubNum(gWin)
}

## THEN create layout
gLayout <- CreateLayout("taskname", gWin, gParams)
gLayout.header.text <- gStrings.header
gLayout.subheader.visible <- 1
gLayout.footer.text <- gStrings.footer
```

### Issue 2: Subheader Trial Counter

**Problem**: Tasks don't show trial progress to participants.

**Solution**: Enable subheader and update it in Trial() function.

**Code pattern:**
```pebl
## In Start() after CreateLayout():
gLayout.subheader.visible <- 1  ## Enable subheader

## In Trial() function:
define Trial(stim, practice, totalTrials)
{
  gLayout.subheader.text <- "Trial " + gTrial + " of " + totalTrials
  ## ... rest of trial code
}
```

**For tasks with practice and test phases:**
```pebl
## Calculate separate trial counts
gPracticeTrials <- Length(practrials)
gTestTrials <- Length(basetrials)

## Practice phase
gTrial <- 1
loop(trial, practrials)
{
  Trial(trial, 1, gPracticeTrials)  ## Pass practice trial count
  gTrial <- gTrial + 1
}

## Reset counter for test phase
gTrial <- 1
loop(trial, basetrials)
{
  Trial(trial, 0, gTestTrials)  ## Pass test trial count
  gTrial <- gTrial + 1
}
```

### Issue 3: Complete Translation Coverage

**Problem**: Tasks only have English translations, missing standard languages (es, lt, pt).

**Solution**: Create translations for all standard PEBL languages before finalizing migration.

**Standard languages:**
- `en` - English (always required)
- `es` - Spanish
- `lt` - Lithuanian
- `pt` - Portuguese

**Workflow:**
```bash
## Check existing translations
ls battery/taskname/translations/

## Create missing translations
## Copy en.json as template, translate all strings
cp taskname.pbl-en.json taskname.pbl-es.json
## Edit taskname.pbl-es.json with Spanish translations
```

**Key translation strings for layout migration:**
- `PRACTICE_HEADER` - "PRACTICE PHASE" / "FASE DE PRÁCTICA" / etc.
- `HEADER` - "TESTING PHASE" / "FASE DE PRUEBA" / etc.
- `FOOTER` - Semantic labels without key names
- `FOOTERL` / `FOOTERR` - Semantic response labels
- `INSTRUCTIONS` - Remove "shift key" references, use "response key"
- `INTERBLOCK` - Between-block messages
- `DEBRIEF` - End-of-task message

### Issue 4: MessageBox Overlap with Layout

**Problem**: MessageBox or EasyTextBox overlaps with layout header/footer.

**Solution**: Use AdaptiveTextBox to constrain message boxes to stimulus region and handle text overflow.

**Code pattern (Recommended):**
```pebl
define MessageKeyBox(message)
{
  ## Use AdaptiveTextBox to fit within stimulus region
  ## Automatically adapts if text doesn't fit
  tb <- AdaptiveTextBox(message,
                        gLayout.stimulusRegion.x + 20,
                        gLayout.stimulusRegion.y + 20,
                        gWin, 56,
                        gLayout.stimulusRegion.width - 40,
                        gLayout.stimulusRegion.height - 40,
                        "scalefont")  ## or "scalebox" to preserve font size
  Draw()
  WaitForAnyKeyPress()
  RemoveObject(tb, gWin)
}
```

**Alternative (without adaptation):**
```pebl
define MessageKeyBox(message)
{
  ## Use layout stimulus region to avoid overlap with header/footer
  ## Textbox positioned by upper-left corner
  tb <- EasyTextBox(message,
                    gLayout.stimulusRegion.x + 20,
                    gLayout.stimulusRegion.y + 20,
                    gWin, 56,
                    gLayout.stimulusRegion.width - 40,
                    gLayout.stimulusRegion.height - 40)
  Draw()
  WaitForAnyKeyPress()
  RemoveObject(tb, gWin)
}
```

**Note**: Use 20px margins to prevent text from touching edges. AdaptiveTextBox is preferred because it handles text overflow automatically.

### Issue 5: Manual Footer Cleanup in Instructions

**Problem**: Instruction functions create manual footer labels that aren't removed, causing double footers.

**Solution**: Remove all manual footer labels created during instructions.

**Check these functions:**
- `DoInstructions()`
- `ShowInstructions()`
- Any function that displays instructions before trials

**Code pattern:**
```pebl
define DoInstructions()
{
  tb <- EasyTextbox(gInst1text, 100, 50, gWin, 32, gVideoWidth-200, 200)

  ## Example stimuli for instructions
  stim1 <- Circle(x1, y1, 75, MakeColor("red"), 1)
  stim2 <- Square(x2, y2, 150, MakeColor("red"), 1)

  ## REMOVE manual footer - layout handles this
  ## footer <- EasyLabel(gFooterL + " ... " + gFooterR, ...)  ## DELETE THIS

  AddObject(stim1, gWin)
  AddObject(stim2, gWin)
  Draw()
  WaitForAnyKeyPress()

  ## Clean up ALL created objects
  RemoveObject(stim1, gWin)
  RemoveObject(stim2, gWin)
  ## RemoveObject(footer, gWin)  ## DELETE THIS LINE TOO
  RemoveObject(tb, gWin)
}
```

### Issue 6: Practice vs Test Phase Headers

**Problem**: Tasks with practice phases don't distinguish between practice and test.

**Solution**: Add PRACTICE_HEADER translation and switch headers between phases.

**Translation additions:**
```json
{
  "PRACTICE_HEADER": "PRACTICE PHASE",
  "HEADER": "TESTING PHASE"
}
```

**Code pattern:**
```pebl
## Before practice trials
gLayout.header.text <- gStrings.practice_header
loop(trial, practrials)
{
  ## ... practice trials
}

MessageKeyBox(gInterblock)

## Switch to test phase and restart trial numbering
gLayout.header.text <- gStrings.header
gTrial <- 1

loop(trial, basetrials)
{
  ## ... test trials
}
```

### Issue 7: Instruction Font Sizes Too Small

**Problem**: Default font sizes in instruction textboxes are too small relative to box size.

**Solution**: Double the font sizes for instruction textboxes.

**Font size guidelines:**
- **DoInstructions() textbox**: 32 (was 16)
- **MessageKeyBox() textbox**: 56 (was 28)
- **MessageBox() calls**: 40 (was 20) - if using standard MessageBox

**Code changes:**
```pebl
## In DoInstructions()
tb <- EasyTextbox(gInst1text, 100, 50, gWin, 32, gVideoWidth-200, 200)
##                                         ^^ doubled from 16

## In MessageKeyBox()
tb <- EasyTextBox(message,
                  gLayout.stimulusRegion.x + 20,
                  gLayout.stimulusRegion.y + 20,
                  gWin, 56,  ## doubled from 28
                  gLayout.stimulusRegion.width - 40,
                  gLayout.stimulusRegion.height - 40)
```

### Migration Checklist Summary

Use this checklist for every task migration:

- [ ] **Layout timing**: CreateLayout() called AFTER GetSubNum()
- [ ] **Subheader enabled**: `gLayout.subheader.visible <- 1`
- [ ] **Trial counter**: Update subheader in Trial() with current trial and total
- [ ] **Practice/test phases**: Separate headers and restart trial numbering if applicable
- [ ] **Complete translations**: All standard languages (en, es, lt, pt) updated
- [ ] **Translation keys**: PRACTICE_HEADER, HEADER, FOOTER, FOOTERL, FOOTERR, INSTRUCTIONS
- [ ] **MessageBox constrained**: MessageKeyBox uses stimulus region coordinates
- [ ] **Font sizes doubled**: Instructions 32, MessageBox 56
- [ ] **Manual footers removed**: No footer labels in DoInstructions()
- [ ] **Footer cleanup**: RemoveObject() calls removed for manual footers
- [ ] **CRITICAL: gVideoWidth/2 and gVideoHeight/2 replaced**: ALL instances replaced with gLayout.centerX/centerY (check Trial(), DoInstructions(), and any fixation/cue positioning)
- [ ] **Files copied**: All changes copied from battery/ to upload-battery/

## Before/After Comparison

### Complete evenodd Example

#### BEFORE (Original)

**evenodd.pbl:**
```pebl
define Start(p)
{
  gWin <- MakeWindow("black")
  parPairs <- [["reps",10], ["isi",500], ["minval",100], ["maxval",999]]
  gParams <- CreateParameters(parpairs, gParamFile)
  GetStrings(gLanguage)

  if(gSubNum+""=="0")
    { gSubNum <- GetSubNum(gWin) }

  header <- "subnum,trial,starttime,number,isodd,resp,corr,rt"
  gFileOut <- GetNewDataFile(gSubNum, gWin, "evenodd", "csv", header)

  Draw()
  MessageBox(gStrings.inst1, gWin)

  ## ... trial generation ...
  loop(i, trials) {
    out <- Trial(i)
    FilePrintList(gFileout, Merge([gSubNum, trial, starttime], out), ",")
  }
}

define Trial(number)
{
  header <- EasyLabel(gStrings.header, gVideoWidth/2, 100, gWin, 40)
  stim <- EasyLabel(number+"", gVideoWidth/2, gVideoHeight/2, gWin, 80)
  footer <- EasyLabel(gStrings.footer, gVideoWidth/2, gVideoHeight-200, gWin, 30)

  Draw()
  time1 <- GetTime()
  resp <- WaitForListKeyPress(["<lshift>","<rshift>"])
  time2 <- GetTime()
  rt <- (time2-time1)
  odd <- IsOdd(number)
  corr <- (odd and resp=="<lshift>") or ((not odd) and resp=="<rshift>")
  Hide(stim)
  Draw()
  return [number, odd, resp, corr, rt]
}
```

**translations/evenodd.pbl-en.json:**
```json
{
  "INST1": "...When a number is odd, press the LEFT shift key. When a number is even, press the RIGHT shift key...",
  "HEADER": "Decide whether odd or even",
  "FOOTER": "<left shift> ODD                    EVEN <right shift>"
}
```

#### AFTER (Migrated)

**evenodd.pbl:**
```pebl
define Start(p)
{
  gWin <- MakeWindow("black")
  parPairs <- [["reps",10], ["isi",500], ["minval",100], ["maxval",999],
               ["responsemode","auto"]]  ## ADDED
  gParams <- CreateParameters(parpairs, gParamFile)
  GetStrings(gLanguage)

  if(gSubNum+""=="0")
    { gSubNum <- GetSubNum(gWin) }

  header <- "subnum,trial,starttime,number,isodd,resp,corr,rt"
  gFileOut <- GetNewDataFile(gSubNum, gWin, "evenodd", "csv", header)

  ## ADDED: Create layout
  gLayout <- CreateLayout("evenodd", gWin, gParams)
  gLayout.header.text <- gStrings.header
  gLayout.subheader.visible <- 0
  gLayout.footer.text <- gStrings.footer

  Draw()
  MessageBox(gStrings.inst1, gWin)

  ## ... trial generation (UNCHANGED) ...
  loop(i, trials) {
    out <- Trial(i)
    FilePrintList(gFileout, Merge([gSubNum, trial, starttime], out), ",")
  }
}

define Trial(number)
{
  ## CHANGED: Use layout for positioning, remove header/footer
  stim <- EasyLabel(number+"", gLayout.centerX, gLayout.centerY, gWin, 80)

  Draw()
  time1 <- GetTime()

  ## CHANGED: Use layout response system
  resp <- WaitForLayoutResponse(gLayout, 5000)

  time2 <- GetTime()
  rt <- (time2-time1)
  odd <- IsOdd(number)

  ## CHANGED: Semantic response mapping
  corr <- (odd and resp=="left") or ((not odd) and resp=="right")

  Hide(stim)
  Draw()
  return [number, odd, resp, corr, rt]
}
```

**translations/evenodd.pbl-en.json:**
```json
{
  "INST1": "...When a number is ODD, press the LEFT response key. When a number is EVEN, press the RIGHT response key...",
  "HEADER": "Decide whether odd or even",
  "FOOTER": "ODD                                      EVEN"
}
```

**params/evenodd.pbl.schema.json:**
```json
{
  "parameters": [
    {
      "name": "responsemode",
      "type": "string",
      "default": "auto",
      "options": ["auto", "keyboardShift", "keyboardSafe", "arrowLR", "mousetarget", "mousebutton"],
      "description": "Response method..."
    }
  ]
}
```

## Common Migration Patterns

### Pattern 1: Two-Alternative Forced Choice

**Task Type**: Odd/Even, Left/Right, Yes/No, etc.

**Key Mapping**: Left response = Option A, Right response = Option B

**Migration**:
- Replace `WaitForListKeyPress(["<lshift>", "<rshift>"])` with `WaitForLayoutResponse(gLayout, timeout)`
- Map correctness: `correct <- (resp == "left")` or `correct <- (resp == "right")`
- Response values in data: `"left"` or `"right"` instead of `"<lshift>"` or `"<rshift>"`

### Pattern 2: Go/No-Go Tasks

**Task Type**: Press for target, withhold for non-target

**Key Mapping**: Any response = "go", timeout = "no-go"

**Migration**:
```pebl
## BEFORE
resp <- WaitForKeyPress(1000)
go <- (resp != "<timeout>")

## AFTER
resp <- WaitForLayoutResponse(gLayout, 1000)
go <- (resp != "<timeout>")
```

**Note**: For go/no-go, you might want a single-button response mode. Consider adding a parameter for this.

### Pattern 3: Multiple Choice (>2 options)

**Task Type**: 3+ response options

**Current limitation**: Layout system currently optimized for 2-alternative responses.

**Recommended approach**: Keep manual response handling for now, OR contribute a multi-alternative layout mode.

### Pattern 4: Continuous Response

**Task Type**: Drag-and-drop, mouse tracking, analog input

**Migration**: Not applicable - layout system is for discrete responses. Keep existing implementation.

## Testing Checklist

After migrating your task, verify:

### Functional Tests

- [ ] Task runs without errors
- [ ] Stimuli display correctly in center of stimulus region
- [ ] Header and footer display correctly
- [ ] Response collection works (try both left and right responses)
- [ ] Timeout handling works (wait without responding)
- [ ] Data file format matches expectations
- [ ] Summary/feedback displays correctly

### Response Mode Tests

**CRITICAL**: All migrated tasks MUST be tested with ALL response modes, especially mousebutton and mousetarget. Mouse-based modes have different event handling than keyboard modes and require thorough testing.

Run your task with each response mode:

```bash
# Default (auto - should use keyboardShift on native)
bin/pebl2 battery/taskname/taskname.pbl -s 1

# Explicit keyboard modes
bin/pebl2 battery/taskname/taskname.pbl -s 2 --pfile params/keyboardShift.json
bin/pebl2 battery/taskname/taskname.pbl -s 3 --pfile params/keyboardSafe.json

# Mouse modes (REQUIRED TESTING)
bin/pebl2 battery/taskname/taskname.pbl -s 4 --pfile params/mousetarget.json
bin/pebl2 battery/taskname/taskname.pbl -s 5 --pfile params/mousebutton.json
```

**Create preset files** (`params/*.json`):

**params/mousetarget.json:**
```json
{
  "responsemode": "mousetarget"
}
```

**params/mousebutton.json:**
```json
{
  "responsemode": "mousebutton"
}
```

**params/keyboardShift.json:**
```json
{
  "responsemode": "keyboardShift"
}
```

**params/keyboardSafe.json:**
```json
{
  "responsemode": "keyboardSafe"
}
```

### Mouse/Touch Mode Testing Checklist

For **mousebutton** mode, verify:
- [ ] Left mouse click registers as "left" response
- [ ] Right mouse click registers as "right" response
- [ ] Clicks work on instruction screens
- [ ] Clicks work during trials
- [ ] Timeout still functions when no click occurs
- [ ] Response feedback displays correctly
- [ ] Footer shows "LEFT-CLICK" and "RIGHT-CLICK" labels

For **mousetarget** mode, verify:
- [ ] Click targets appear in response zone
- [ ] Clicking left target registers as "left" response
- [ ] Clicking right target registers as "right" response
- [ ] Targets are large enough (especially for touch screens)
- [ ] Targets don't overlap with stimuli
- [ ] Footer shows clickable target labels
- [ ] Targets respond to both mouse and touch input

### Single-Response Tasks (Go-NoGo, CPT, etc.)

For tasks with single-response modes (spacebar, leftclick, touchscreen):

**Test with spacebar mode:**
```bash
bin/pebl2 battery/taskname/taskname.pbl -s 6 --pfile params/spacebar.json
```

**params/spacebar.json:**
```json
{
  "responsemode": "spacebar"
}
```

**Test with leftclick mode:**
```bash
bin/pebl2 battery/taskname/taskname.pbl -s 7 --pfile params/leftclick.json
```

**params/leftclick.json:**
```json
{
  "responsemode": "leftclick"
}
```

**Test with touchscreen mode:**
```bash
bin/pebl2 battery/taskname/taskname.pbl -s 8 --pfile params/touchscreen.json
```

**params/touchscreen.json:**
```json
{
  "responsemode": "touchscreen"
}
```

Verify for single-response modes:
- [ ] Instruction screens advance on response
- [ ] Trials register responses correctly
- [ ] No-response trials (timeouts) handled properly
- [ ] Footer shows appropriate single-response label
- [ ] Response feedback (if any) displays correctly

### Screen Size Tests

- [ ] 800x600 (baseline)
- [ ] 1024x768 (common desktop)
- [ ] 1920x1080 (HD)
- [ ] 1280x800 (laptop)
- [ ] Fullscreen mode

Verify labels don't overlap and stimuli remain centered.

### Data Validation

Compare data files from before and after migration:

**Check**:
- Same number of columns
- Same trial count
- RT values similar (should be identical)
- Accuracy similar (should be identical)
- Only response column should differ (raw keys → semantic names)

### Translation Tests

If your task has translations:

```bash
bin/pebl2 battery/taskname/taskname.pbl -s 10 --language es
bin/pebl2 battery/taskname/taskname.pbl -s 11 --language fr
```

Verify footer labels update appropriately.

## Troubleshooting

### Error: "gLayout is not defined"

**Cause**: `CreateLayout()` not called or called after it's used

**Fix**: Ensure `CreateLayout()` is called in `Start()` AFTER parameters loaded, BEFORE first `Draw()`

### Error: "No translation file exists"

**Cause**: Translation file not found

**Fix**: Run from task directory: `cd battery/taskname && ../../bin/pebl2 taskname.pbl`

### Response always returns "<timeout>"

**Cause**: Wrong response keys being pressed, or timeout too short

**Fix**:
- Check which response mode is active (look at footer labels)
- Increase timeout: `WaitForLayoutResponse(gLayout, 10000)` (10 seconds)
- Print response mode: `Print("Response mode: " + gLayout.responseMode.type)`

### Footer shows wrong key labels

**Cause**: Response mode not set correctly

**Fix**: Check `gParams.responsemode` is being loaded from parameters

**Debug**:
```pebl
Print("Response mode parameter: " + gParams.responsemode)
Print("Layout response mode: " + gLayout.responseMode.type)
```

### Stimuli not centered

**Cause**: Using old `gVideoWidth/2` instead of layout regions

**Fix**: Replace with `gLayout.stimulusRegion.centerX` and `.centerY`

### Labels overlap

**Cause**: Screen too small for layout zones

**Fix**: Layout system handles this automatically. If still an issue, check font sizes - you may want to use layout's scaled font sizes instead of hardcoded values.

### Data format changed

**Expected**: Response column now shows `"left"`/`"right"` instead of `"<lshift>"`/`"<rshift>"`

**This is correct**: Semantic names are platform-independent. Update analysis scripts:

**R example**:
```r
# OLD
data$correct <- data$response == "<lshift>"

# NEW
data$correct <- data$response == "left"
```

**Python example**:
```python
# OLD
df['correct'] = df['response'] == '<lshift>'

# NEW
df['correct'] = df['response'] == 'left'
```

## Summary

Migration to the Layout & Response System involves:

1. **Add responsemode parameter** to schema and parameter pairs
2. **Call CreateLayout()** in `Start()` after parameters, before Draw()
3. **Remove manual header/footer labels** from Trial() function
4. **Use layout regions** for stimulus positioning
5. **Replace WaitForListKeyPress()** with `WaitForLayoutResponse()`
6. **Update correctness logic** to use semantic names (`"left"`, `"right"`)
7. **Update translations** to use generic "response key" language
8. **Test thoroughly** with multiple response modes

**What stays the same**:
- Trial generation logic
- Data file structure (except response column values)
- Summary statistics
- Overall task flow

**What improves**:
- Platform compatibility (native, web, tablet)
- User configurability (no code changes needed)
- Consistent UI across battery tasks
- Automatic screen scaling

For questions or issues, consult the PEBL manual Chapter 6 (Layout & Response System) or ask on the PEBL mailing list.
