# PEBL 2.4 Release Checklist

**Version:** 2.4  
**Date:** 2026-04-17  
**Status:** In Progress

---

## Legend

- `[x]` Done
- `[ ]` Not yet done
- `[~]` Partially done / in progress
- `[s]` Skipped / deferred

---

## 1. Build & Version

- [x] `PEBL_VERSION` updated to 2.4 in `Makefile`
- [x] `PEBL_VERSION` comment updated in `src/apps/launcher/LauncherConfig.h`
- [x] Workspace path updated to `pebl-exp.2.4` in launcher source and comments
- [x] Windows NSIS installer script created: `installer/PEBL_2.4.nsi`
- [x] `UI.pbl` `GetFullLineBreaks()` crash fixed (empty `tmpbreaks` guard) — both `pebl-lib/` and `emscripten/pebl-lib/`
- [x] `PEBL.cpp` Linux no-argument behavior: prints usage and exits instead of running `launcher.pbl`
- [x] Native Linux build passes (`make main`) — no regressions
- [ ] Emscripten/WASM build passes (`make em-opt`) — no regressions
- [ ] Windows build compiles and links (MSVC or MinGW)
- [x] Copyright year updated to 2026 (`scripts/update_copyright_years.py 2026`)

---

## 2. Native Launcher (C++/ImGui)

### Build
- [x] Linux AppImage builds and starts launcher on double-click
- [x] Linux `pebl-launcher` symlink works after system install
- [x] Windows `pebl-launcher.exe` builds and launches

### Installed Mode
- [x] First-run workspace created at `~/Documents/pebl-exp.2.4/`
- [x] Demo, tutorial, and documentation copied to workspace on first run
- [x] Battery path auto-detected correctly

### Portable Mode
- [ ] `STANDALONE.txt` marker file triggers portable mode
- [ ] `PEBL_PORTABLE=1` environment variable triggers portable mode
- [ ] Workspace is CWD in portable mode

### Manage Studies Tab
- [x] Create new study
- [NA] Delete study
- [x] Study list persists between launcher restarts

### Tests Sub-tab
- [x] Battery task browser lists all tests
- [x] Search by name works
- [x] Test description and parameters displayed

### Chains Sub-tab
- [x] Create new chain
- [x] Add Test item to chain
- [x] Add Scale/Survey item to chain
- [x] Add Instruction item to chain
- [x] Add Consent item to chain (declining terminates chain)
- [x] Consent .osd decline terminates chain.
- [x] Add Completion item to chain
- [NO] Reorder items by drag
- [x] Reorder items with arrows
- [x] Delete item from chain
- [x] Participant code prefix/counter/session configurable
- [x] Counter increments after each successful chain run
- [x] Edit Participant Code dialog works
- [x] Language code setting works (ISO 639-1)
- [x] Run Chain executes all items in sequence
- [x] Chain randomization works
- [x] Chain advances automatically after each item completes
- [x] lsl connects registers and records to LabRecorder for enabled task

### Run Sub-tab
- [x] Fullscreen toggle works
- [x] stdout panel shows Print() output from chain run
- [x] stderr panel shows status/error messages
- [x] Output cleared/replaced on new run

### Quick Launch Tab
- [x] File browser shows only `.pbl` files and directories
- [x] Run button launches selected `.pbl` with correct subject code and settings
- [x] Fullscreen and language settings respected

### Scales/Surveys (Scale Builder)
- [x] Scale Info sub-tab: form editable
- [x] Questions sub-tab: add/edit/reorder items
- [x] Likert, multiple choice, multicheck, text entry response formats work
- [x] Dimensions & Scoring sub-tab functional
- [x] Translations sub-tab functional
- [x] Sections sub-tab functional
- [x] Parameters sub-tab functional
- [x] Save to `.osd.json` in study directory
- [x] Saved scale runnable from chain

### Settings Dialog
- [x] Language code, fullscreen default, font size (General panel)
- [x] Battery path, PEBL executable path, data output directory (File Paths panel)
- [ ] Upload server URL and authentication token (Upload panel)
- [x] Settings persisted to config file and restored on next launch

### Snapshots
- [x] Create Snapshot archives study to `snapshots/`
- [x] Import Snapshot unpacks into `my_studies/` as new study
- [ ] Snapshot round-trip works cross-platform (Linux ↔ Windows)

### Menu
- [x] File | New Study
- [x] File | Open Study
- [x] File | Save
- [x] File | Import Snapshot
- [x] File | Settings
- [x] File | Exit (saves config)
- [x] Help | Manual (opens PDF)
- [x] Help | Website (opens browser)
- [x] Help | About

---

### Upload (UploadLine)
- [ ] `UploadLine()` appends single CSV row to server file
- [ ] No-op when `gUpload` is false
- [ ] Settings file resolution order: explicit arg → `gUploadFile` → `upload.json`
- [ ] `pooled:1` writes to shared task file; `pooled:0` writes per-participant



## 6. Documentation

### LaTeX Manual (pman/)
- [x] `intro.tex` — Linux installation updated for AppImage workflow
- [x] `intro.tex` — Command-line options updated (all flags in PEBL.cpp documented including `--lsl`)
- [x] `launcher.tex` — Chapter completely rewritten for native C++/ImGui launcher
- [x] `launcher.tex` — Screenshot figures added (`\begin{figure}` blocks)
- [x] Screenshots captured and saved to `pman/images/`:
- [x] `reference.tex` regenerated from Sphinx (`make latex-chapter-with-styles`)
- [x] `pdflatex main.tex` completes without errors (run twice for cross-refs)
- [x] PDF manual reviewed for formatting issues

### Sphinx / HTML Reference
- [x] `design.rst` — `Union`, `Intersect`, `SetDifference` added
- [x] `peblstring.rst` — `RegexMatch` added
- [x] `peblstream.rst` — `UploadLine` added
- [x] `utility.rst` — `IsDefined`, `Indent`, `MakeFontFamily`, `JoinStrings`, `ConvertNewlinesToCR`, `InitializeLSL`, `LSLMarker`, `FinalizeLSL` added
- [x] `peblenvironment.rst` — `IsPEBLObject` added
- [x] `lsl.rst` — new file: `CreateLSLOutlet`, `SendLSLMarker`, `CloseLSLOutlet`, `LSLHasConsumers`, `LSLLocalClock`
- [x] `layout.rst` — new file: all 16 Layout library functions
- [x] `index.rst` — `lsl` and `layout` added to toctrees
- [ ] `make html` builds without Sphinx warnings
- [ ] `make latex-chapter-with-styles` rebuilds cleanly
- [ ] HTML output spot-checked in browser (new entries visible and linked)
- [ ] Update PEBL web function reference from sphinx
- [ ] Update pebl doxygen reference

---

## 7. Platform Testing Matrix

| Component | Linux (AppImage) | Windows 10 | Windows 11 |
|-----------|:---:|:---:|:---:|
| `pebl2` interpreter | [ ] | [ ] | [ ] |
| `pebl-launcher` | [ ] | [ ] | [ ] |
| `pebl-validator` | [ ] | [ ] | [ ] |
| Chain run (3+ items) | [ ] | [ ] | [ ] |
| Upload workflow | [ ] | [ ] | [ ] |
| Snapshot import/export | [ ] | [ ] | [ ] |

### WebAssembly / Browser Testing
| Browser | Basic task | Layout response | Upload |
|---------|:---:|:---:|:---:|
| Chrome (desktop) | [ ] | [ ] | [ ] |
| Firefox (desktop) | [ ] | [ ] | [ ] |
| Safari (desktop) | [ ] | [ ] | [ ] |

---

## 8. Windows Installer

- [ ] NSIS script `installer/PEBL_2.4.nsi` tested on clean Windows 10/11 VM
- [ ] All executables installed: `pebl2.exe`, `pebl-launcher.exe`, `pebl-validator.exe`
- [ ] All required DLLs present: SDL2.dll, SDL2_image.dll, SDL2_ttf.dll, SDL2_gfx.dll, SDL2_mixer.dll, libzip DLL
- [ ] Battery, pebl-lib, media, demo, tutorial installed to `C:\Program Files\PEBL2\`
- [ ] `pebl-exp.2.4\` workspace auto-created in `My Documents` on first launch
- [ ] Start Menu shortcuts created for Launcher and Interpreter
- [ ] Desktop shortcut for Launcher created
- [ ] `.pbl` file association registered
- [ ] Uninstaller works cleanly
- [ ] Manual PDF installed and linked from Start Menu

---

## 9. Release Artifacts

- [ ] Linux AppImage: `PEBL-2.4-x86_64.AppImage`
- [ ] Windows installer: `PEBL_setup.2.4.exe`
- [ ] PDF manual: `PEBLManual2.4.pdf`
- [ ] Release notes written
- [ ] Git tag `v2.4` created on release commit
- [ ] SourceForge / website updated
- [ ] Announcement posted
- [ ] Contact labstreaminglayer

---

## Known Issues / Deferred to 2.5

- macOS launcher build (deferred; use peblhub.online on macOS)
- Asset Upload API for verbal/memory tasks (DRM, freerecall, pairedassociates, RAT, lexicaldecision)
- Joystick/gamepad response mode
- Voice key fixes (SDL3 migration pending)
