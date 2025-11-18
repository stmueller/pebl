# Upload-Battery Requirements Analysis Summary

Generated: 2025-10-21

## Overview

Analyzed 21 tests and generated `params/requirements.json` for each test.

## Summary Table

| Test | Keyboard | Mouse | Audio | Color | Notes |
|------|----------|-------|-------|-------|-------|
| BART | ✓ | ✓ | Rec | ✓ | Balloon colors: orange, gold, purple |
| bcst | ✓ | ✓ | - | ✓ | Card sorting by color/shape |
| bcst-64 | ✓ | ✓ | - | ✓ | 64-card version |
| BST | ✓ | ✓ | Rec | - | Brief stimulus test |
| clocktest | ✓ | - | - | ✓ | Clock drawing |
| corsi | ✓ | ✓ | Rec | - | Block tapping, optional audio |
| dspan | ✓ | ✓ | Rec | ✓ | Visual or auditory digit span |
| face-ratings | ✓ | ✓ | - | - | Face recognition |
| flanker | ✓ | - | - | - | Arrow flanker task |
| gonogo | ✓ | ✓ | - | - | Go/No-Go task |
| iowa | ✓ | ✓ | Rec | ✓ | IGT with colored decks |
| manikin | ✓ | - | - | ✓ | Red/blue directions |
| oddball | ✓ | - | - | ✓ | Red circles/squares |
| ptrails | ✓ | ✓ | - | ✓ | Pink circles, yellow squares |
| spatialgrid | - | ✓ | Rec | ✓ | Mouse-only, spatial memory |
| stroop-color | ✓ | - | - | - | Word reading (ironically no color!) |
| stroop-number | ✓ | - | - | - | Number counting |
| stroop-vic | ✓ | - | - | ✓ | Victoria Stroop with colors |
| switcher | ✓ | ✓ | - | ✓ | Task switching with colors |
| test-simple | ✓ | - | - | - | Simple test template |
| toav | ✓ | - | - | - | Test of Variables of Attention |

**Legend:**
- ✓ = Required
- Rec = Recommended (optional)
- - = Not used

## Key Findings

### Input Patterns

**Keyboard Required (18 tests):**
- Most tests use keyboard for responses
- Only exceptions: spatialgrid (mouse-only)

**Mouse Required (12 tests):**
- BART, bcst, bcst-64, BST, corsi, dspan, face-ratings, gonogo, iowa, ptrails, spatialgrid, switcher

**Keyboard-Only (6 tests):**
- clocktest, flanker, manikin, oddball, stroop-color, stroop-number, stroop-vic, test-simple, toav

### Audio Usage

**Audio Recommended (7 tests):**
- BART: Balloon pop sounds
- BST: Stimulus presentation sounds
- corsi: Block tap sounds (optional)
- dspan: Can use auditory presentation
- iowa: Feedback sounds
- spatialgrid: Stimulus sounds

**No Audio (14 tests):**
- All others can run silently

### Color Discrimination

**Color Required (13 tests):**
- BART: Orange, gold, purple balloons
- bcst/bcst-64: Colored cards
- clocktest: Clock colors
- dspan: Colored feedback
- iowa: Red/blue/green/yellow decks
- manikin: Red/blue directions
- oddball: Red stimuli
- ptrails: Pink circles, yellow squares
- spatialgrid: Colored grid patterns
- stroop-vic: Colored words
- switcher: Colored shapes

**Important for Accessibility:**
Tests with color discrimination should warn colorblind participants or offer alternative versions.

### Screen Size Recommendations

**All tests currently default to:**
- Minimum: 1024x768
- Recommended: 1280x800

**Needs Review:**
Some tests may require larger screens based on layout:
- bcst-64: Uses 450px offset (may need 1100+ width)
- ptrails: Field width configurable (may need adjustment)
- spatialgrid: Image-based, may need larger screens

## Next Steps

### 1. Review Screen Sizes
Manually test each test on smaller screens to find true minimums:
```bash
# Test on 1024x768, 1280x800, 1366x768, 1920x1080
# Document where overlaps or cut-off occurs
```

### 2. Add Test-Specific Notes

Examples to add:

**BART:**
```json
"color": {
  "discrimination_required": true,
  "notes": "Three balloon types: orange (life=8), gold (life=16), purple (life=128)"
}
```

**dspan:**
```json
"audio": {
  "required": false,
  "recommended": true,
  "notes": "Default is visual presentation. Researchers can enable auditory mode via parameters."
}
```

**ptrails:**
```json
"color": {
  "discrimination_required": true,
  "notes": "Pink circles (numbers), yellow squares (letters). May be challenging for red-green colorblind users."
}
```

### 3. Implement Frontend Display

- [ ] Load requirements.json in launch.php
- [ ] Display requirements with icons
- [ ] Add client-side validation in public-launcher.html
- [ ] Warn users with screens below minimum
- [ ] Show colorblind warning when relevant

### 4. Enable Researcher Overrides

For logged-in researchers creating studies:
- Override audio requirements (force required/optional)
- Override minimum screen size
- Add custom warnings
- Store overrides in study configuration

## Files Generated

All tests now have:
```
upload-battery/[TEST_NAME]/params/requirements.json
```

Each file contains:
- display: minimum/recommended screen sizes
- input: keyboard/mouse/movement requirements
- audio: required/recommended flags
- color: discrimination requirements
- browser: fullscreen and version requirements

All files marked as DRAFT and need manual review.

## Analysis Tool

Script: `/home/home/smueller/Dropbox/Research/pebl/PEBLOnlinePlatform/scripts/analyze-test-requirements.sh`

Re-run anytime to update specific tests:
```bash
./scripts/analyze-test-requirements.sh /path/to/upload-battery
```
