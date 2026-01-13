# Phase 1 Migration Plan: 10 Priority Tests for PEBL 2.3 Release

**Created:** January 12, 2026
**Target Release:** PEBL 2.3 (March 2026)
**Goal:** Migrate 10 high-priority visual/motor tasks to upload-battery with Layout & Response System

---

## Overview

This plan details the migration of 10 priority tests from the native battery to upload-battery for the PEBL 2.3 release. All tests are visual/motor tasks that can be migrated immediately without requiring the asset upload API.

**Reference Documents:**
- `doc/PEBL_2.3_RELEASE_PRIORITIES.md` - Overall release plan
- `doc/BATTERY_MIGRATION_GUIDE.md` - Migration process (9 phases)
- `doc/LAYOUT_MIGRATION_GUIDE.md` - Layout & Response System details

---

## Test Selection Criteria

**Why these 10 tests:**
- ✅ Use programmatically generated stimuli (no external CSV/TXT files)
- ✅ Well-validated cognitive/motor tasks
- ✅ Cover major domains: attention, memory, perception, motor
- ✅ Compatible with Layout & Response System
- ✅ High research usage and impact

**Migration approach:**
- All tests will include Layout & Response System
- All tests will support "auto" and "userselect" modes (minimum)
- All tests will have multiple response mode parameter files
- All tests will be tested on web browsers before deployment

---

## Phase 1 Priority Tests (10 Tests)

### Test 1: timetap
**Description:** Temporal tapping and time estimation task
**Domain:** Timing, motor control
**Response type:** Keyboard (single key - spacebar)
**Complexity:** LOW
**Estimated time:** 2-3 hours

**Current status:**
- Location: `battery/timetap/`
- Check if already in upload-battery: `ls upload-battery/timetap/ 2>/dev/null`

**Migration requirements:**
- Layout & Response System: **YES** (single-key response mode)
- Response modes needed: spacebar, leftclick, touchscreen, clicktarget
- Translations to update: Check `ls battery/timetap/translations/`
- Parameter schema: Likely exists, needs responsemode added

**Special considerations:**
- Timing-critical task - verify browser timing is acceptable (~16ms precision)
- May need to document timing limitations in about.txt

---

### Test 2: vigilance
**Description:** Sustained attention/vigilance task
**Domain:** Attention
**Response type:** Keyboard (likely single key or 2AFC)
**Complexity:** MEDIUM
**Estimated time:** 3-4 hours

**Current status:**
- Location: `battery/vigilance/`
- Check if already in upload-battery: `ls upload-battery/vigilance/ 2>/dev/null`

**Migration requirements:**
- Layout & Response System: **YES** (determine if 2AFC or single-key)
- Response modes needed: TBD after code review
- Translations to update: Check `ls battery/vigilance/translations/`
- Parameter schema: Check/create

**Special considerations:**
- Long duration task - ensure no browser timeout issues
- Sustained attention requires consistent timing

---

### Test 3: vsearch
**Description:** Visual search task
**Domain:** Visual attention, search
**Response type:** Keyboard or mouse (likely 2AFC: present/absent)
**Complexity:** MEDIUM
**Estimated time:** 3-4 hours

**Current status:**
- Location: `battery/vsearch/`
- Check if already in upload-battery: `ls upload-battery/vsearch/ 2>/dev/null`

**Migration requirements:**
- Layout & Response System: **YES** (2AFC likely)
- Response modes needed: auto, userselect, keyboardSafe, arrowLR, mousetarget, mousebutton, touchtarget
- Translations to update: Check `ls battery/vsearch/translations/`
- Parameter schema: Check/create

**Special considerations:**
- Visual complexity - ensure stimuli render correctly in browsers
- May have multiple search array sizes - test layout scaling

---

### Test 4: movetotarget
**Description:** Move-to-target motor task
**Domain:** Motor control, pointing
**Response type:** Mouse movement and clicks
**Complexity:** MEDIUM
**Estimated time:** 3-4 hours

**Current status:**
- Location: `battery/movetotarget/`
- Check if already in upload-battery: `ls upload-battery/movetotarget/ 2>/dev/null`

**Migration requirements:**
- Layout & Response System: **MAYBE** (mouse-only task, may need layout-only)
- Response modes needed: mouseOnly (no response system integration)
- Translations to update: Check `ls battery/movetotarget/translations/`
- Parameter schema: Check/create

**Special considerations:**
- Mouse tracking task - ensure tracking works in browsers
- May need layout zones but not response system
- Touch compatibility uncertain - test on tablets

---

### Test 5: hicks
**Description:** Hicks' Law choice reaction time
**Domain:** Reaction time, decision making
**Response type:** Keyboard (multiple keys based on number of choices)
**Complexity:** MEDIUM
**Estimated time:** 3-5 hours

**Current status:**
- Location: `battery/hicks/`
- Check if already in upload-battery: `ls upload-battery/hicks/ 2>/dev/null`

**Migration requirements:**
- Layout & Response System: **NEEDS EXTENSION** (variable number of response options: 2, 4, 8)
- Response modes needed: May need custom mode for 4+ options
- Translations to update: Check `ls battery/hicks/translations/`
- Parameter schema: Check/create

**Special considerations:**
- **BLOCKER**: May need 4-way or 8-way response mode extension
- Consider deferring if response mode extension not ready
- Alternative: Implement with fixed 2-choice version first

---

### Test 6: mspan
**Description:** Memory span (numeric)
**Domain:** Working memory
**Response type:** Keyboard (number entry)
**Complexity:** MEDIUM
**Estimated time:** 3-4 hours

**Current status:**
- Location: `battery/mspan/`
- Check if already in upload-battery: `ls upload-battery/mspan/ 2>/dev/null`

**Migration requirements:**
- Layout & Response System: **NO** (text entry task, not compatible with response system)
- Response modes needed: N/A (uses keyboard text input)
- Translations to update: Check `ls battery/mspan/translations/`
- Parameter schema: Check/create

**Special considerations:**
- Uses text entry, not button responses
- Layout-only migration (header/footer/stimulus zones)
- Ensure text input works in browsers

---

### Test 7: matchtosample
**Description:** Visual matching task
**Domain:** Visual memory, matching
**Response type:** Mouse/keyboard (likely 2AFC or multi-choice)
**Complexity:** MEDIUM
**Estimated time:** 3-4 hours

**Current status:**
- Location: `battery/matchtosample/`
- Check if already in upload-battery: `ls upload-battery/matchtosample/ 2>/dev/null`

**Migration requirements:**
- Layout & Response System: **CHECK** (depends on response format)
- Response modes needed: TBD after code review
- Translations to update: Check `ls battery/matchtosample/translations/`
- Parameter schema: Check/create

**Special considerations:**
- May involve clicking on stimuli (positional response)
- Might not be compatible with standard response modes
- Review code to determine migration approach

---

### Test 8: nback
**Description:** N-back working memory task
**Domain:** Working memory
**Response type:** Keyboard (2AFC: match/no-match)
**Complexity:** MEDIUM
**Estimated time:** 3-4 hours

**Current status:**
- Location: `battery/nback/`
- Check if already in upload-battery: `ls upload-battery/nback/ 2>/dev/null`

**Migration requirements:**
- Layout & Response System: **YES** (2AFC match/no-match)
- Response modes needed: auto, userselect, keyboardSafe, arrowLR, mousetarget, mousebutton, touchtarget
- Translations to update: Check `ls battery/nback/translations/`
- Parameter schema: Check/create

**Special considerations:**
- Popular task with high research usage
- May have audio version - check for sound files
- Timing-critical (presentation rate) - verify browser timing

---

### Test 9: dexterity
**Description:** Finger dexterity/tapping speed
**Domain:** Motor control
**Response type:** Mouse clicks or keyboard (rapid tapping)
**Complexity:** LOW
**Estimated time:** 2-3 hours

**Current status:**
- Location: `battery/dexterity/`
- Check if already in upload-battery: `ls upload-battery/dexterity/ 2>/dev/null`

**Migration requirements:**
- Layout & Response System: **MAYBE** (depends on implementation)
- Response modes needed: TBD after code review
- Translations to update: Check `ls battery/dexterity/translations/`
- Parameter schema: Check/create

**Special considerations:**
- Rapid tapping task - ensure browser can handle high event rate
- May measure clicks in specific regions (positional)
- Check if compatible with response system or needs custom approach

---

### Test 10: linejudgment
**Description:** Line length judgment task
**Domain:** Perception
**Response type:** Keyboard (2AFC: longer/shorter or left/right)
**Complexity:** LOW
**Estimated time:** 2-3 hours

**Current status:**
- Location: `battery/linejudgment/`
- Check if already in upload-battery: `ls upload-battery/linejudgment/ 2>/dev/null`

**Migration requirements:**
- Layout & Response System: **YES** (2AFC)
- Response modes needed: auto, userselect, keyboardSafe, arrowLR, mousetarget, mousebutton, touchtarget
- Translations to update: Check `ls battery/linejudgment/translations/`
- Parameter schema: Check/create

**Special considerations:**
- Simple perceptual task
- Visual stimuli (lines) should scale well with layout system
- Good candidate for early migration (low complexity)

---

## Migration Process Summary

### For Each Test (9 Phases)

**Phase 1: Pre-Migration Assessment**
1. Check if test already in upload-battery
2. Review test code to identify response type
3. Check for external file dependencies
4. Identify translation files
5. Review parameter schema

**Phase 2: Code Modifications**

**2.1: Data Upload**
- Add `InitializeUpload()` at start
- Add `UploadFile(gSubNum, datafile, "/upload.json")` at end

**2.2: Instruction Cleanup**
- Remove "experimenter" references
- Update to online-appropriate language

**2.3: Layout & Response System** (if applicable)
- Add `responsemode` parameter to schema (default: "auto")
- Set `responsesemantics` in code (NOT schema)
- Create layout with `CreateLayout()`
- Update MessageBox to fit stimulus region
- Replace `gVideoWidth/2` with `gLayout.centerX`
- Replace `WaitForListKeyPress` with `WaitForLayoutResponse`

**2.4: Translation Updates**
- Update ALL language files
- Remove hardcoded key names
- Use generic "response key" language

**Phase 3: Parameter Schema**
- Create/update `.pbl.schema.json` file
- Add responsemode parameter
- Ensure all parameters documented

**Phase 4: Screen Size**
- Use layout zones for positioning
- Test at multiple resolutions

**Phase 5: Browser Testing**
- Test in Chrome, Firefox, Safari, Edge
- Check console for errors
- Verify timing acceptable

**Phase 6: Performance**
- Verify timing precision (~16ms)
- Document limitations

**Phase 7: Testing & Validation**
- Complete test run
- Compare data output to native version
- Test multiple response modes

**Phase 8: Catalog Update**
- Add to `PEBLOnlinePlatform/config/available_tests.json`
- Include screenshots, description, references

**Phase 9: Documentation**
- Update `.about.txt`
- Create `MIGRATION_NOTES.txt`

---

## Required Parameter Files (Per Test)

**MINIMUM (REQUIRED):**
```bash
battery/testname/params/testname-auto.par.json
battery/testname/params/testname-userselect.par.json
```

**RECOMMENDED (for 2AFC tests):**
```bash
battery/testname/params/testname-keyboardSafe.par.json
battery/testname/params/testname-arrowLR.par.json
battery/testname/params/testname-mousetarget.par.json
battery/testname/params/testname-mousebutton.par.json
battery/testname/params/testname-touchtarget.par.json
```

**RECOMMENDED (for single-key tests):**
```bash
battery/testname/params/testname-spacebar.par.json
battery/testname/params/testname-leftclick.par.json
battery/testname/params/testname-touchscreen.par.json
battery/testname/params/testname-clicktarget.par.json
```

---

## Migration Timeline

### Week 1-2: Assessment & Planning (Jan 13-26, 2026)
**Goal:** Assess all 10 tests, identify blockers

**Tasks:**
- [ ] Check which tests already in upload-battery
- [ ] Review code for each test to determine:
  - Response type (2AFC, single-key, multi-choice, mouse-only, text entry)
  - Layout & Response System compatibility
  - Translation files available
  - External dependencies
- [ ] Identify blockers (tests needing response mode extensions)
- [ ] Prioritize migration order based on complexity

**Estimated time:** 10-15 hours (1-1.5 hours per test)

### Week 3-5: Core Migrations (Jan 27 - Feb 16, 2026)
**Goal:** Migrate 6-8 tests (simple to moderate complexity)

**Prioritized order (simplest first):**
1. **linejudgment** (2AFC, simple) - 2-3 hours
2. **dexterity** (simple motor) - 2-3 hours
3. **timetap** (single-key) - 2-3 hours
4. **nback** (2AFC, popular) - 3-4 hours
5. **vsearch** (2AFC, visual) - 3-4 hours
6. **vigilance** (attention) - 3-4 hours
7. **mspan** (text entry, layout-only) - 3-4 hours
8. **movetotarget** (mouse, layout-only) - 3-4 hours

**Estimated time:** 22-30 hours (spread over 3 weeks)

### Week 6: Complex/Blocked Tests (Feb 17-23, 2026)
**Goal:** Migrate remaining tests or defer if blocked

**Tests:**
- **matchtosample** - TBD based on code review
- **hicks** - May need 4-way response mode (BLOCKER?)

**Fallback plan if blocked:**
- Implement simplified 2-choice version of hicks
- Defer multi-choice hicks to 2.4 (after response mode extension)
- Choose alternative test from Phase 2 list

**Estimated time:** 6-10 hours

### Week 7: Testing & Polish (Feb 24 - Mar 2, 2026)
**Goal:** Comprehensive testing, bug fixes

**Tasks:**
- [ ] Test all migrated tests in Chrome, Firefox, Safari, Edge
- [ ] Test auto and userselect modes for each test
- [ ] Verify data upload to PEBLOnlinePlatform
- [ ] Compare data output to native versions
- [ ] Fix any bugs discovered during testing
- [ ] Update documentation

**Estimated time:** 10-15 hours

### Week 8: Final Review & Deployment (Mar 3-9, 2026)
**Goal:** Deploy to PEBLOnlinePlatform, final verification

**Tasks:**
- [ ] Deploy tests to PEBLOnlinePlatform
- [ ] Update test library catalog
- [ ] Create screenshots for each test
- [ ] Verify tests appear in platform UI
- [ ] Final smoke tests
- [ ] Update PEBL_2.3_RELEASE_PRIORITIES.md with completion status

**Estimated time:** 5-10 hours

---

## Total Time Estimate

**Assessment:** 10-15 hours
**Core migrations:** 22-30 hours
**Complex tests:** 6-10 hours
**Testing & polish:** 10-15 hours
**Deployment:** 5-10 hours

**TOTAL: 53-80 hours (6.5-10 work days)**

**Timeline:** 8 weeks (mid-January to early March 2026)

---

## Success Criteria

**Must complete for PEBL 2.3 release:**
- ✅ At least 8 of 10 tests successfully migrated
- ✅ All migrated tests work in major browsers (Chrome, Firefox, Safari, Edge)
- ✅ All migrated tests support auto and userselect modes
- ✅ Data upload verified for all migrated tests
- ✅ Tests appear in PEBLOnlinePlatform catalog
- ✅ No regressions in existing tests

**Nice to have:**
- ✅ All 10 tests migrated
- ✅ Multiple response modes tested for each test
- ✅ Screenshots captured for catalog
- ✅ Migration notes documented for each test

---

## Risk Assessment

### High Risk Items

**1. Response mode extensions needed**
- **Risk:** hicks may need 4-way or 8-way response mode
- **Mitigation:** Implement simplified 2-choice version, defer full version to 2.4
- **Fallback:** Select alternative test from Phase 2 list

**2. Browser timing limitations**
- **Risk:** timetap, vigilance may require precise timing
- **Mitigation:** Test early, document limitations, verify acceptable precision
- **Fallback:** Add warnings about browser timing limitations

**3. Mouse tracking compatibility**
- **Risk:** movetotarget, dexterity may have issues in browsers
- **Mitigation:** Test early with browser mouse tracking
- **Fallback:** Mark as desktop-only if browser issues unfixable

### Medium Risk Items

**1. Translation file coverage**
- **Risk:** Some tests may have incomplete translations
- **Mitigation:** Update at least English, defer other languages if needed
- **Impact:** Moderate (tests work in English)

**2. Complex visual layouts**
- **Risk:** vsearch, matchtosample may have layout challenges
- **Mitigation:** Use layout zones, test at multiple screen sizes
- **Impact:** Moderate (may need layout adjustments)

### Low Risk Items

**1. Data upload failures**
- **Risk:** Network issues during testing
- **Mitigation:** Test with stable connection, retry on failure
- **Impact:** Low (easily debuggable)

**2. Browser compatibility edge cases**
- **Risk:** Minor rendering differences across browsers
- **Mitigation:** Test early and often
- **Impact:** Low (usually CSS adjustments)

---

## Blocked/Deferred Tests

If any test cannot be completed due to technical blockers:

**Defer to PEBL 2.4:**
- Tests requiring 4+ way response modes
- Tests requiring asset upload API (none in Phase 1)
- Tests with unfixable browser compatibility issues

**Replacement candidates from Phase 2:**
- **mullerlyer** - Müller-Lyer illusion (2AFC, simple)
- **probedigit** - Probe digit recognition (2AFC)
- **plusminus** - Task switching (2AFC)
- **inspection** - Inspection time (2AFC)
- **randomgeneration** - Random generation (text entry)

---

## Daily Progress Tracking

Use this checklist to track progress:

```markdown
## Test Migration Progress

### linejudgment
- [ ] Assessment complete
- [ ] Code modified (upload, layout, translations)
- [ ] Parameter files created (auto, userselect, +5 more)
- [ ] Tested in browsers
- [ ] Deployed to upload-battery
- [ ] Added to catalog

### dexterity
- [ ] Assessment complete
- [ ] Code modified
- [ ] Parameter files created
- [ ] Tested in browsers
- [ ] Deployed to upload-battery
- [ ] Added to catalog

### timetap
- [ ] Assessment complete
- [ ] Code modified
- [ ] Parameter files created
- [ ] Tested in browsers
- [ ] Deployed to upload-battery
- [ ] Added to catalog

### nback
- [ ] Assessment complete
- [ ] Code modified
- [ ] Parameter files created
- [ ] Tested in browsers
- [ ] Deployed to upload-battery
- [ ] Added to catalog

### vsearch
- [ ] Assessment complete
- [ ] Code modified
- [ ] Parameter files created
- [ ] Tested in browsers
- [ ] Deployed to upload-battery
- [ ] Added to catalog

### vigilance
- [ ] Assessment complete
- [ ] Code modified
- [ ] Parameter files created
- [ ] Tested in browsers
- [ ] Deployed to upload-battery
- [ ] Added to catalog

### mspan
- [ ] Assessment complete
- [ ] Code modified
- [ ] Parameter files created
- [ ] Tested in browsers
- [ ] Deployed to upload-battery
- [ ] Added to catalog

### movetotarget
- [ ] Assessment complete
- [ ] Code modified
- [ ] Parameter files created
- [ ] Tested in browsers
- [ ] Deployed to upload-battery
- [ ] Added to catalog

### matchtosample
- [ ] Assessment complete
- [ ] Code modified
- [ ] Parameter files created
- [ ] Tested in browsers
- [ ] Deployed to upload-battery
- [ ] Added to catalog

### hicks
- [ ] Assessment complete
- [ ] Code modified (or deferred)
- [ ] Parameter files created
- [ ] Tested in browsers
- [ ] Deployed to upload-battery
- [ ] Added to catalog
```

---

## Next Steps (Immediate Actions)

1. **Review this plan with project lead** - Confirm priorities and timeline
2. **Start assessment phase** - Review code for all 10 tests
3. **Identify blockers early** - Especially hicks (multi-choice response)
4. **Set up testing environment** - Browser VMs, test data upload endpoint
5. **Begin with linejudgment** - Simplest test, good learning case

---

**Document Version:** 1.0
**Status:** Planning Phase
**Next Review:** After assessment phase complete (Week 2)
**Owner:** Migration team
