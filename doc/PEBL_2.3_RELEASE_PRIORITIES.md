# PEBL 2.3 Release Priorities

**Target Release:** Q1 2026
**Primary Goal:** Windows launcher support + expanded upload-battery coverage

---

## Critical Path Items for 2.3 Release

### 1. Windows Launcher Support
- [x] C++ launcher implemented (Jan 10, 2026)
- [ ] **Windows build/compile** (CRITICAL)
  - Configure Visual Studio project OR MinGW build
  - Link against SDL2, ImGui, libzip on Windows
  - Test on Windows 10/11
- [ ] **Windows installer integration** (HIGH PRIORITY)
  - NSIS installer script updates
  - Include pebl-launcher.exe in installer
  - Desktop shortcuts for both pebl2.exe and launcher
  - First-run workspace setup (Documents\pebl-exp.2.3\)
- [ ] **Cross-platform testing**
  - Verify launcher on Linux, Windows, macOS
  - Test snapshot import/export across platforms
  - Verify file path handling (Windows backslash vs Unix forward slash)

### 2. Upload-Battery Expansion (10+ Tests Minimum)
**Current status:** Only 47 tests in upload-battery/, need at least 10 more from priority list

---

## Priority Test Analysis

### Migration Status Summary

**Total priority tests:** 35
**Currently in upload-battery:** 1 (tapping)
**Need to migrate:** 34
**Not found in battery/:** 1 (spatialcueing - does not exist)

### Category Breakdown

| Category | Count | Can Migrate Now | Needs Assets |
|----------|-------|-----------------|--------------|
| **Visual/Motor Tasks** | 28 | 28 | 0 |
| **Verbal/Memory Tasks** | 6 | 0 | 6 |
| **Total** | 34 | 28 | 6 |

---

## Test-by-Test Analysis

### ✅ Tier 1: Visual/Motor Tasks (Can Migrate Immediately)

These tests use programmatically generated stimuli or bundled media files. **No asset upload API needed.**

| Test | In Platform Library? | Response Type | Complexity | Priority | Notes |
|------|---------------------|---------------|-----------|----------|-------|
| **dexterity** | No | Mouse clicks | Low | HIGH | Finger dexterity/tapping speed |
| **donkey** | No | Keyboard | Medium | MEDIUM | Donkey/Mule discrimination |
| **mspan** | No | Keyboard | Medium | HIGH | Memory span (numeric) |
| **dualtask** | No | Keyboard | High | MEDIUM | Dual-task interference |
| **generation** | No | Keyboard | Medium | MEDIUM | Number generation task |
| **hicks** | No | Keyboard | Low | HIGH | Hicks' Law choice RT |
| **matchtosample** | No | Mouse/Click | Medium | HIGH | Visual matching task |
| **linejudgment** | No | Mouse/Click | Low | MEDIUM | Line length judgment |
| **mullerlyer** | No | Keyboard | Low | MEDIUM | Müller-Lyer illusion |
| **nback** | No | Keyboard | Medium | HIGH | N-back working memory |
| **PASAT** | No | Keyboard | High | LOW | Paced Auditory Serial Addition |
| **partial-report** | No | Keyboard | Medium | MEDIUM | Partial report paradigm |
| **plusminus** | No | Keyboard | Low | MEDIUM | Plus-minus task switching |
| **probedigit** | No | Keyboard | Medium | MEDIUM | Probe digit recognition |
| **probrev** | No | Keyboard | Medium | MEDIUM | Probability reversal learning |
| **ptracker** | No | Mouse | High | LOW | Pursuit tracking |
| **satest** | No | Mouse/Click | Medium | MEDIUM | Situation awareness test |
| **randomgeneration** | No | Keyboard | Low | MEDIUM | Random number generation |
| **tapping** | ✅ Yes | Keyboard/Mouse | Low | ✅ DONE | Already in upload-battery |
| **timetap** | No | Keyboard | Low | HIGH | Temporal tapping/estimation |
| **twocoladd** | No | Keyboard | Medium | MEDIUM | Two-column addition |
| **tsp** | No | Mouse | High | LOW | Traveling salesperson |
| **tracking** | No | Mouse | High | MEDIUM | Visual tracking task |
| **movetotarget** | No | Mouse | Low | HIGH | Move-to-target task |
| **vigilance** | No | Keyboard | Medium | HIGH | Sustained attention/vigilance |
| **vsearch** | No | Keyboard/Mouse | Medium | HIGH | Visual search task |
| **wft** | No | Keyboard | Medium | MEDIUM | Word fragment test |
| **inspection** | No | Keyboard | Medium | MEDIUM | Inspection time task |

**Tier 1 Subtotal: 28 tests** (excluding already-migrated tapping)

---

### ⚠️ Tier 2: Verbal/Memory Tasks (Need Asset Upload API)

These tests load word lists, stimulus text from CSV/TXT files. **Cannot migrate until asset upload is implemented.**

| Test | Assets Needed | Language Issue | Can Defer? |
|------|---------------|----------------|------------|
| **freerecall** | Word lists (CSV/TXT) | Yes - requires translated word lists | Yes |
| **DRM** | Word lists (Deese-Roediger-McDermott) | Yes - semantic associations | Yes |
| **pairedassociates** | Word pairs (CSV) | Yes - translation needed | Yes |
| **RAT** | Remote Associates problems (CSV) | Yes - linguistic/cultural | Yes |
| **lexicaldecision** | Word/nonword lists (CSV) | Yes - language-specific lexicon | Yes |
| **letterdigit** | Stimuli (CSV, possibly) | Minimal - uses letters/digits | Maybe |

**Tier 2 Subtotal: 6 tests (defer to post-2.3)**

**Asset API Requirements:**
- Upload CSV/TXT files to test directory
- Override default stimulus files
- Version control for modified assets
- Download original assets button (reset to default)

---

## Recommended Migration Plan for 2.3

### Phase 1: High-Priority Visual Tasks (Target: 10 tests)
**Timeline: 2-3 weeks**

Migrate these first (sorted by priority and simplicity):

1. **timetap** - Temporal tapping (simple keyboard, high value)
2. **vigilance** - Sustained attention (proven online compatibility)
3. **vsearch** - Visual search (well-tested paradigm)
4. **movetotarget** - Move-to-target (simple mouse task)
5. **hicks** - Hicks' Law (classic RT paradigm)
6. **mspan** - Memory span (numeric stimuli, no text files)
7. **matchtosample** - Visual matching (good cognitive measure)
8. **nback** - N-back (popular WM task)
9. **dexterity** - Finger dexterity (motor assessment)
10. **linejudgment** - Line judgment (simple perceptual task)

**Rationale:**
- All use programmatic stimuli or bundled media
- Cover major cognitive domains (attention, memory, perception, motor)
- Well-validated tasks with research usage
- Compatible with Layout & Response System

### Phase 2: Medium-Priority Tasks (Stretch Goal: +5 tests)
**Timeline: 1-2 weeks (if time permits)**

11. **mullerlyer** - Müller-Lyer illusion
12. **probedigit** - Probe digit recognition
13. **plusminus** - Task switching
14. **inspection** - Inspection time
15. **randomgeneration** - Random generation

### Phase 3: Complex/Lower-Priority Tasks (Post-2.3)
**Defer to 2.4 or later:**

- dualtask (complex dual-task design)
- generation (number generation)
- partial-report (needs careful timing validation)
- probrev (probability reversal learning)
- ptracker (pursuit tracking - mouse intensive)
- satest (situation awareness - complex)
- tsp (traveling salesperson - computationally complex)
- tracking (visual tracking - mouse intensive)
- twocoladd (two-column addition)
- PASAT (paced audio serial addition - complex timing)
- wft (word fragment test)
- donkey (donkey/mule discrimination)

---

## Migration Checklist Per Test

For each test migrated to upload-battery:

### Pre-Migration Assessment
- [ ] Review test code in `battery/{test}/{test}.pbl`
- [ ] Check for external file dependencies (CSV, TXT, images beyond media/)
- [ ] Identify response mode (keyboard/mouse/both)
- [ ] Check translation files (`translations/{test}.pbl-*.json`)
- [ ] Review parameter schema (`params/{test}.pbl.schema`)

### Layout & Response Migration
- [ ] Add Layout & Response System
  - [ ] `CreateLayout()` call
  - [ ] Identify appropriate response mode (keyboard, mouse, touch, etc.)
  - [ ] Update response handling to use semantic names
- [ ] Add `responsemode` parameter to schema
- [ ] Add `responselabels` to translations (if needed)
- [ ] Test with multiple response modes (keyboard-safe, mousebutton, touchtarget)

### Upload Integration
- [ ] Add `responsesemantics` to test code
- [ ] Ensure data upload compatibility (`Transfer.pbl` integration)
- [ ] Verify `upload.json` configuration
- [ ] Test upload to PEBLOnlinePlatform

### Verification
- [ ] Copy to `upload-battery/{test}/`
- [ ] Add to `PEBLOnlinePlatform/config/library-tests.json`
- [ ] Test on native build
- [ ] Test on Emscripten/browser build
- [ ] Verify data upload to platform
- [ ] Check cross-browser compatibility (Chrome, Firefox, Safari)
- [ ] Test on mobile/tablet (if applicable)

### Documentation
- [ ] Update `doc/LAYOUT_MIGRATION_GUIDE.md`
- [ ] Add entry to migration status table
- [ ] Document any special considerations
- [ ] Update `CONVERTED_FILES_SUMMARY.md` in upload-battery

---

## Windows Installer Requirements

### Installer Components

**Executables:**
- pebl2.exe (PEBL interpreter)
- pebl-launcher.exe (study management UI)
- pebl-validator.exe (syntax validator)

**Installation Locations:**
```
C:\Program Files (x86)\PEBL2\
├── bin\
│   ├── pebl2.exe
│   ├── pebl-launcher.exe
│   └── pebl-validator.exe
├── battery\              # 100+ battery tests (read-only)
├── pebl-lib\             # Standard library
├── media\                # Fonts, sounds, images
├── demo\                 # Demo files
├── tutorial\             # Tutorial files
└── doc\
    └── manual.pdf
```

**User Workspace (auto-created on first run):**
```
%USERPROFILE%\Documents\pebl-exp.2.3\
├── my_studies\           # User's working studies
├── snapshots\            # Exported study snapshots
├── data\                 # Legacy data directory
├── demo\                 # Copied from installation
├── tutorial\             # Copied from installation
├── documentation\        # Copied from installation
├── manual.pdf            # Copied from installation
└── README.txt            # Welcome/getting started
```

### Desktop Shortcuts
- **PEBL Launcher** → pebl-launcher.exe (primary user interface)
- **PEBL Interpreter** → pebl2.exe (advanced users/scripting)

### Registry Entries
- File association: `.pbl` files → pebl2.exe
- Uninstall information
- Installation path

### NSIS Script Updates Needed
- Add pebl-launcher.exe to installation
- Add pebl-validator.exe to installation
- Create two desktop shortcuts (launcher + interpreter)
- First-run detection and workspace initialization
- Update file associations
- Include all necessary DLLs:
  - SDL2.dll
  - SDL2_image.dll
  - SDL2_ttf.dll
  - SDL2_gfx.dll
  - SDL2_mixer.dll (if USE_MIXER=1)
  - libzip DLL (for launcher snapshot import)
  - ImGui (statically linked, no DLL needed)

---

## Testing Plan for 2.3 Release

### Platform Testing Matrix

| Platform | PEBL Interpreter | Launcher | Validator | Upload Tests |
|----------|------------------|----------|-----------|--------------|
| **Linux (Ubuntu 22.04)** | ✅ Tested | ✅ Tested | ✅ Tested | ✅ Tested |
| **Windows 10** | ❌ Need test | ❌ Need test | ❌ Need test | ❌ Need test |
| **Windows 11** | ❌ Need test | ❌ Need test | ❌ Need test | ❌ Need test |
| **macOS** | ⏸️ Defer | ⏸️ Defer | ⏸️ Defer | ⏸️ Defer |

### Test Scenarios

**1. Fresh Installation (Windows)**
- [ ] Run installer on clean Windows 10/11
- [ ] Verify all executables installed correctly
- [ ] Launch pebl-launcher.exe from desktop shortcut
- [ ] Verify first-run workspace creation
- [ ] Check Documents\pebl-exp.2.3\ structure
- [ ] Verify demo/tutorial files copied
- [ ] Open manual.pdf

**2. Study Management (Windows Launcher)**
- [ ] Create new study
- [ ] Add tests from battery
- [ ] Create chain with instruction/consent/test/completion
- [ ] Run chain with subject ID
- [ ] Verify data saved to correct location
- [ ] Export snapshot
- [ ] Import snapshot
- [ ] Verify ZIP import works

**3. Upload Workflow**
- [ ] Configure upload.json
- [ ] Run test chain
- [ ] Verify data uploads to PEBLOnlinePlatform
- [ ] Check data appears on platform
- [ ] Download data from platform
- [ ] Verify data integrity

**4. Browser Compatibility (WebAssembly)**
- [ ] Test migrated tests on Chrome
- [ ] Test on Firefox
- [ ] Test on Safari
- [ ] Test on mobile Safari (iOS)
- [ ] Test on mobile Chrome (Android)
- [ ] Verify no Sticky Keys dialogs
- [ ] Check touch target sizes on mobile

---

## Success Criteria for 2.3 Release

### Must-Have (Blocking Release)
- ✅ Layout & Response System implemented and documented
- ✅ C++ Native Launcher implemented
- ✅ PEBL Validator implemented
- [ ] **Windows launcher build working**
- [ ] **Windows installer includes all components**
- [ ] **At least 10 new tests in upload-battery** (from priority list)
- [ ] All new upload-battery tests verified on platform
- [ ] No regression in existing tests
- [ ] Documentation updated (manual, migration guides)

### Nice-to-Have (Can Defer)
- [ ] macOS launcher build
- [ ] Asset upload API (for verbal/memory tasks)
- [ ] 15+ tests in upload-battery (stretch goal)
- [ ] Mobile-optimized touch interface polish
- [ ] Additional response modes (gamepad/joystick)

---

## Timeline Estimate

**Week 1-2: Windows Launcher Build**
- Configure Windows build environment
- Resolve linking issues (SDL2, ImGui, libzip)
- Test on Windows 10/11
- Fix Windows-specific bugs (path separators, etc.)

**Week 3-5: Test Migration (Phase 1: 10 tests)**
- 2 tests per week
- Layout & Response System integration
- Upload verification
- Cross-browser testing

**Week 6: Windows Installer**
- Update NSIS script
- Test installation/uninstallation
- Verify first-run initialization
- Create desktop shortcuts

**Week 7: Testing & Polish**
- Full platform testing matrix
- Bug fixes
- Documentation updates
- Release candidate builds

**Week 8: Release**
- Final builds for Linux and Windows
- Release notes
- Website updates
- Announcement

**Total: ~8 weeks to 2.3 release**

---

## Post-2.3 Roadmap

### Version 2.4 (Future)
- **Asset Upload API** for PEBLOnlinePlatform
- Migrate Tier 2 verbal/memory tasks (freerecall, DRM, pairedassociates, RAT, lexicaldecision, letterdigit)
- macOS launcher support
- Joystick/gamepad response mode
- Font caching (prerequisite for markdown rendering)
- Markdown rendering in textboxes

### Version 2.5 (Future)
- Lab Streaming Layer (LSL) integration
- Voice key fixes (SDL3 migration)
- Advanced analytics dashboard
- Collaborative study editing

---

## Dependencies

### External Libraries (Windows Build)
- **SDL2** (2.28+) - Window management, input, audio
- **SDL2_image** - Image loading (PNG, JPEG)
- **SDL2_ttf** - TrueType font rendering
- **SDL2_gfx** - Graphics primitives
- **SDL2_mixer** - Audio playback (OGG, MP3, WAV)
- **ImGui** (1.89+) - Launcher UI (Dear ImGui)
- **libzip** - ZIP file handling (snapshot import)
- **nlohmann::json** - JSON parsing (header-only)

### Build Tools (Windows)
- **Option A:** Visual Studio 2019/2022 (MSVC compiler)
- **Option B:** MinGW-w64 (GCC for Windows)
- **Option C:** Cross-compile from Linux (mingw-w64)

---

## Risk Assessment

### High Risk
- **Windows launcher build complexity** - Never built before, may encounter unexpected issues
  - Mitigation: Allocate 2 weeks, have fallback to defer launcher to 2.3.1
- **DLL dependency management** - Ensuring all DLLs included in installer
  - Mitigation: Use dependency walker, test on clean Windows VM

### Medium Risk
- **Test migration time** - 10 tests may take longer than estimated
  - Mitigation: Start with simplest tests, parallelize if possible
- **Cross-browser compatibility** - New tests may have browser-specific issues
  - Mitigation: Test early and often, use Layout & Response System

### Low Risk
- **Installer script updates** - NSIS is well-documented
- **Documentation** - Templates exist from previous migrations

---

## Questions to Resolve

1. **Asset Upload API Priority:**
   - Can we defer Tier 2 verbal/memory tasks to 2.4?
   - **Answer:** Yes, 10 visual/motor tasks sufficient for 2.3

2. **macOS Support:**
   - Include macOS launcher in 2.3 or defer?
   - **Recommendation:** Defer to 2.4, focus on Windows first

3. **Testing Resources:**
   - Do we have Windows 10/11 test machines available?
   - Can we get beta testers for Windows builds?

4. **Release Timing:**
   - Target date for 2.3 release?
   - **Recommendation:** March 2026 (8 weeks from mid-January)

---

**Document Version:** 1.0
**Date:** January 12, 2026
**Status:** Planning Phase
**Next Review:** After Windows launcher build attempt
