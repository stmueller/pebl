# PEBL Development Master Plan

This document tracks major improvements and architectural changes for PEBL.

## Memory Management & Performance

### High Priority

- [x] **Fix counted_ptr reference counting system** ✅ **COMPLETE (Dec 4, 2025)**
  - **Fixed in commit 4ece1c1**: Eliminated unnecessary copies causing excessive reference counting
  - Changed PComplexData, VariableMap, Evaluator to pass by const reference
  - Objects now properly freed when references cleared
  - Related files: `src/utility/rc_ptrs.h`, `src/base/Variant.cpp`, `src/base/PComplexData.cpp`

- [x] **Implement font caching/factory pattern** ✅ **COMPLETE (Jan 24, 2026)**
  - **Problem solved**: Every `EasyLabel()` created separate font objects for same parameters
  - **Key insight**: SDL_TTF fonts are color-agnostic! Colors applied at render time
  - **Implementation completed**:
    - Created `src/utility/FontCache.h/.cpp` with singleton FontCacheManager class
    - Cache key: {filename, style, size} - excludes color for maximum sharing
    - Reference counting: TTF_Font automatically freed when ref_count reaches 0
    - Modified `PlatformFont` to use FontCache in constructor/destructor
    - Simplified approach: Direct `TTF_OpenFont()` instead of RWops/buffer
    - Original RWops approach caused heap corruption from 757KB buffer allocations
  - **Memory savings achieved**: 80-90% reduction for color-variant fonts
    - Example: 10 fonts with same properties but different colors
    - Without cache: 10 separate TTF_Font objects (~330KB total)
    - With cache: 1 shared TTF_Font object (~33KB total)
  - **Helper function added**: `MakeFontFamily()` in `pebl-lib/Utility.pbl`
    - Pre-creates all 4 style variants (normal, bold, italic, bold+italic)
    - Returns custom object with properties: family.normal, family.bold, etc.
    - Essential for markdown rendering - eliminates create/destroy overhead
    - Usage: `family <- MakeFontFamily("DejaVuSans.ttf", 24, fgColor, bgColor, 1)`
  - **Testing**: Verified with 3-font and 10-font tests, all passing
  - Related files:
    - `src/utility/FontCache.h/.cpp` (cache implementation)
    - `src/platforms/sdl/PlatformFont.cpp` (integration)
    - `pebl-lib/Utility.pbl` (MakeFontFamily helper)
    - `doc/FONTCACHE_ISSUE.md` (implementation notes and debugging history)

### Medium Priority

- [x] **Investigate sporadic font color rendering bug** ✅ **COMPLETE (Dec 9-10, 2025)**
  - **Fixed in commit 4ed0e7f**: Fixed segfault when assigning color properties via nested syntax
  - **Fixed in commit 017b3ef**: Refactored PFont to use property-only color storage
  - Eliminated double ownership bugs and memory corruption from color assignment
  - Related files: `src/objects/PFont.cpp`, `doc/FONT_COLOR_PROPERTY_BUG_FIX.md`

## Text Rendering & Formatting

### High Priority

- [x] **Phase 1: HTML-Lite Markup System** ✅ **COMPLETE (Jan 26, 2026)**
  - **Features implemented**:
    - Created `FormatParser` class in `src/utility/FormatParser.h/.cpp`
    - Modified `PlatformTextBox::RenderText()` with two-pass baseline-aligned rendering
    - Added `FORMATTED` property (disabled by default for backward compatibility)
    - PEBL usage: `SetProperty(textbox, "FORMATTED", 1)`
  - **Inline formatting tags**:
    - `<b>bold</b>`, `<i>italic</i>`, `<u>underline</u>` - Text styling
    - `<c=color>text</c>` - Colors (752 X11 names + hex `#RRGGBB` or `#RGB`)
    - `<size=N>text</size>` - Font sizes (8-200pt)
    - `<br>` - Line breaks
  - **Block-level tags**:
    - `<h1>` through `<h6>` - Headers (32pt, 28pt, 24pt, 20pt, 18pt, 16pt bold)
    - `<h1=center>`, `<h2=right>`, etc. - Headers with justification
    - `<p=left|center|right>` - Text justification (persists until newline)
    - `<hr>` - Horizontal rules
    - `<li>` - Bullet list items (auto-bullet, indent, newline)
    - `<indent>`, `<indent=N>` - Horizontal positioning by N character widths
  - **Color support**:
    - All 752 X11 color names (red, darkgreen, cornflowerblue, mediumseagreen, etc.)
    - Hex color codes: `#RRGGBB` and `#RGB` formats
    - Uses existing `PColor` system via `RGBColorNames.h`
  - **Rendering features**:
    - Baseline alignment for mixed font sizes (typographically correct)
    - **Line height fix**: Tracks maximum font height per line using `TTF_FontHeight()`
    - Prevents header overlap when mixing different font sizes on same line
    - FontCache integration for memory efficiency
    - PColor architecture throughout (no SDL_Color mixing)
    - Cross-platform: native, Emscripten, Windows
  - **Editing behavior**:
    - Raw tag editing: tags become visible when `GetInput()` is called
    - FORMATTED mode automatically disabled for editing
    - Tags preserved and editable as literal text
  - **Testing**: Multiple test files created (test-format.pbl, test-baseline-alignment.pbl, test-br-quick.pbl, test-colors-full.pbl, test-htmllite-markup.pbl)
  - **Screenshot generation**:
    - Script: `../PEBLOnlinePlatform/scripts/generate-markup-screenshots.pbl`
    - Generates 7 documentation screenshots demonstrating all features:
      1. `markup-inline-formatting.png` - Bold, italic, underline
      2. `markup-colors-sizes.png` - Colors and font sizes
      3. `markup-headers.png` - All 6 header levels with proper spacing
      4. `markup-justification.png` - Left/center/right alignment
      5. `markup-lists-indents.png` - Bullet lists and indentation
      6. `markup-horizontal-rules.png` - Horizontal rule separators
      7. `markup-example-instructions.png` - Complete psychology experiment example
    - Screenshots stored in: `../PEBLOnlinePlatform/help/images/markup/`
  - **Documentation**:
    - Technical guide: `doc/FORMATTED_TEXT.md`
    - User guide: `../PEBLOnlinePlatform/help/html-lite-markup.md`
    - Quick reference table with all tags and examples
    - Example templates for common use cases (instructions, surveys, results, warnings)
    - Labels vs TextBoxes support comparison
  - **Use cases**: Experiment instructions, on-screen feedback, formatted stimuli, results displays
  - Related files:
    - `src/utility/FormatParser.h/.cpp` (parser implementation)
    - `src/platforms/sdl/PlatformTextBox.cpp` (rendering with line height fix, lines 307-540)
    - `doc/FORMATTED_TEXT.md` (technical documentation)
    - `../PEBLOnlinePlatform/help/html-lite-markup.md` (user documentation)
    - `../PEBLOnlinePlatform/scripts/generate-markup-screenshots.pbl` (screenshot generator)

- [ ] **Phase 2: Full CommonMark markdown support** ⏸️ **DEFERRED - Not currently needed**
  - **Status**: HTML-lite markup (Phase 1) adequately serves all current use cases
  - **Use cases covered by HTML-lite**:
    - Experiment instructions with headers, lists, and formatting
    - On-screen feedback with colors, alignment, and styling
    - Formatted stimuli for psychology experiments
    - Results displays with indentation and rules
  - **Rationale for deferral**:
    - HTML-lite tags are more intuitive for experiment designers (`<b>bold</b>` vs `**bold**`)
    - No need for CommonMark features like nested lists, blockquotes, or links in experiments
    - Simpler parser means smaller binary size and fewer dependencies
    - Current implementation is sufficient and complete
  - **If implemented in future** (low priority):
    - Integrate md4c library (pure C, very fast, one .c + one .h file)
    - Support: Headers (# H1), **bold**, *italic*, lists, links, `code blocks`
    - Add `MARKDOWN` property to PTextBox: `SetProperty(textbox, "MARKDOWN", 1)`
    - Would coexist with HTML-lite (different FORMATTED vs MARKDOWN properties)
  - Related docs: `doc/SDL_RTF_INVESTIGATION.md`, `doc/MARKDOWN_FONT_CACHE_PLAN.md`

- [ ] **Phase 3: Markdown to HTML export** ⏸️ **DEFERRED - Not currently needed**
  - **Status**: Dependent on Phase 2 markdown implementation
  - **Alternative approaches**:
    - Use existing `pebl-lib/HTML.pbl` library for HTML generation
    - Generate HTML directly from PEBL data structures (no markdown intermediate)
    - Researchers can use external markdown tools if needed
  - **If implemented in future** (low priority):
    - New PEBL function: `MarkdownToHTML(markdown_text, output_file, css_file:"")`
    - Use cases: Protocol documentation, printable instructions, archival
  - Related docs: `doc/MARKDOWN_FONT_CACHE_PLAN.md`

- **Phase 1 prerequisites completed**:
  - ✅ Font cache implemented (prevents memory bloat from styled text)
  - ✅ HTML-lite markup complete with all inline and block-level tags
  - ✅ Line height fix for proper rendering of mixed font sizes
  - ✅ Documentation and screenshot generation complete

- **Backward compatibility**: All features opt-in, existing plain text unchanged
- **Testing**: Plain text (no regression), formatted text, line wrapping, Emscripten - all passing

## Research Integration

### High Priority

- [ ] **Text-to-speech (TTS) for instruction playback** ⏸️ **ON HOLD**
  - **Purpose**: Enable spoken instructions for participants with reading difficulties or for auditory instruction delivery
  - **Current status**: Research complete, documented in `doc/TTS_INTEGRATION_RESEARCH.md`
  - **Decision (Jan 2026)**: Project on hold - eSpeak NG quality insufficient (robotic voice)
  - **Quality assessment**:
    - eSpeak NG (formant synthesis): Robotic, not acceptable for research use
    - Piper TTS (neural): High quality but 15-35MB per voice (too large for core distribution)
    - Web Speech API: High quality but web-only, inconsistent across browsers
  - **Future direction**: Consider Piper TTS as optional developer tool
    - Not included in default PEBL distribution
    - Separate installation guide for researchers who need high-quality TTS
    - User downloads voice models separately (like lab equipment)
    - Build flag: `USE_NEURAL_TTS=1` (disabled by default)
  - **Alternative approach**: Recommend pre-recorded audio for instructions
    - Researchers record instructions in their own voice or professional voice actor
    - Standard PEBL audio playback (LoadSound, PlayForeground)
    - Better quality than any TTS system
    - Full control over prosody, emphasis, pacing
  - Related docs: `doc/TTS_INTEGRATION_RESEARCH.md`

- [ ] **Fix voice key heap corruption and test across platforms**
  - **Current status**: Manual threshold voice key works (`GetVocalResponseTime(buffer, threshold, sustain)`)
  - **Problem**: ROC-based automatic calibration crashes with heap corruption
  - **Symptom**: `malloc(): invalid size (unsorted)` after running calibration + voice key
  - **Root cause**: AudioInfo buffer lifecycle issues - buffer freed while SDL still using it
  - **Attempted fixes**: Thread-safe access, longer delays, explicit cleanup - all failed
  - **Current workaround**: Skip calibration, use manual threshold (works reliably)
  - **Implementation needs**:
    - Short term: Test manual threshold voice key on Windows/macOS
    - Medium term (SDL3 migration): Rebuild calibration with proper buffer lifecycle
    - SDL3 has redesigned audio with clearer ownership and better thread safety
    - Re-implement audio monitoring with SDL3 streams instead of callbacks
  - **Testing**:
    - `demo/testaudioin.pbl` - must work on all platforms
    - `demo/voicekey-calibration.pbl` - currently crashes, don't document
  - Related files:
    - `src/platforms/sdl/PlatformAudioIn.h/cpp` - Audio input implementation
    - `src/libs/PEBLObjects.cpp` - `StartAudioMonitor()`, `GetAudioStats()`, `StopAudioMonitor()`
    - `doc/VOICE_KEY_STATUS.md` - Detailed analysis of issues
    - `doc/VOICEKEY_CALIBRATION.md` - Calibration algorithm documentation

- [ ] **Implement Lab Streaming Layer (LSL) programming interface**
  - Integrate Lab Streaming Layer: https://github.com/sccn/labstreaminglayer
  - LSL enables time-synchronized collection of measurement data in research experiments
  - Common use cases: EEG/physiological data synchronization with PEBL task events
  - **Current status**:
    - LSL library already downloaded in `libs/labstreaminglayer/`
    - Basic C++ wrapper started: `src/utility/PLabStreamingLayer.h/cpp` (2019)
    - Implements PLSL class with `SendMessage()` method
    - No PEBL built-in functions exposed yet (not in Functions.h)
  - **Implementation needs**:
    - Complete C++ wrapper: fix syntax errors, add destructor, initialization
    - Add PEBL built-in functions to `src/libs/Functions.h`:
      - `CreateLSLOutlet(name, type)` - returns LSL outlet object
      - `SendLSLMarker(outlet, marker)` - sends string marker with timestamp
      - `CloseLSLOutlet(outlet)` - cleanup
    - Event markers for trial start/end, stimulus presentation, responses
    - Timestamp synchronization between PEBL and LSL timebases
    - Build system integration: link LSL library, add to Makefile
  - Platform considerations: Native Linux/Windows build (LSL has C/C++ library)
  - Related files:
    - Existing: `src/utility/PLabStreamingLayer.h/cpp`
    - New: `src/libs/PEBLLSL.cpp` (built-in function wrappers)
    - Modify: `src/libs/Functions.h` (function registration)

## Input & Response Systems

### High Priority

- [ ] **Add joystick/gamepad as standard response mode**
  - Integrate joystick/gamepad input into Layout & Response System
  - Common use cases:
    - Game controllers for directional responses
    - Response boxes (XID, Cedrus) that present as joysticks/HID devices
    - Multi-button input devices for research
  - Implementation needs:
    - SDL2 joystick/gamepad API integration (already available in SDL2)
    - New response mode: `"joystick"` in `media/settings/response-modes.json`
    - PEBL functions for joystick detection and configuration
    - Mapping joystick buttons/axes to semantic responses ("left", "right", "up", "down")
    - Support for multiple connected joysticks (participant selection)
  - Response mode configurations:
    - 2-button mode: Map button 0/1 to left/right responses
    - 4-button mode: Map buttons to 4-way directional responses
    - Axis mode: Map left stick axes to directional responses
    - Custom mapping: Allow button/axis configuration via parameters
  - Platform considerations:
    - Native builds: Full SDL2 joystick support
    - Web/Emscripten: Gamepad API support (may have browser limitations)
  - Related files:
    - New: `src/libs/PEBLJoystick.cpp` (joystick wrapper functions)
    - Modify: `src/libs/Functions.h` (register joystick functions)
    - Modify: `pebl-lib/Layout.pbl` (joystick response mode handling)
    - Modify: `media/settings/response-modes.json` (add joystick modes)
  - Testing requirements:
    - Test with Xbox/PlayStation controllers
    - Test with research response boxes (Cedrus, PST)
    - Verify button mapping consistency across platforms

## Online/Web Platform

### Completed

- [x] **Implement Layout & Response System** ✅ **COMPLETE (Jan 8, 2026)**
  - **Commit 868840d**: Full implementation with responsesemantics support
  - **Features implemented**:
    - Standard zone-based layouts with automatic scaling to any screen size
    - Platform-aware response modes (keyboard/mouse/touch)
    - Single-function API (`CreateLayout()`) with intelligent defaults
    - Dynamic styling via nested property modifications (leverages Dec 2025 property system)
    - Theme support (default, dark, high-contrast) with accessibility features
    - Animation capabilities for visual feedback (FlashCorrect, FadeOut, PulseLabel, etc.)
    - Semantic response names ("left"/"right" instead of "<lshift>"/"<rshift>")
    - Multi-language support via translated response labels
    - Optional JSON configuration files for test-specific layouts
  - **Migration status (as of Jan 2026)**:
    - 18 battery tests fully migrated (35%)
    - All 7 Category 1 tests complete (100%)
    - Response modes: keyboard-shift, keyboard-safe, keyboard-arrow, mousebutton, mousetarget, touchtarget, clicktarget, spacebar, none
    - Tests include: luckvogel, evenodd, dotjudgment, flanker, manikin, simon, BST, gonogo, oddball, crt, wpt, ANT, PCPT, TNT, ppvt, clocktest, stroop tasks, ptrails, corsi, matrixrotation
  - **Key benefits**:
    - Solves Sticky Keys problem in browsers (no shift key dialogs)
    - Enables mobile/tablet deployment with touch targets
    - Reduces boilerplate code in tests (one CreateLayout call vs. manual UI)
    - Auto-scales to any screen size (phones, HiDPI displays)
    - Backwards compatible with existing tests
  - Related files:
    - `pebl-lib/Layout.pbl` (68KB implementation)
    - `media/settings/response-modes.json` (5.8KB configuration)
    - `doc/LAYOUT_RESPONSE_SYSTEM_PLAN.md` (implementation plan)
    - `doc/LAYOUT_MIGRATION_GUIDE.md` (93KB migration guide)
    - `doc/SEMANTIC_LABEL_SYSTEM.md` (semantic architecture)
  - Related commits: 868840d, 3a5d457, and 100+ migration commits

- [x] **Implement C++ Native Launcher with Study Management System** ✅ **COMPLETE (Jan 10, 2026)**
  - **Commit daed98b**: Full C++ launcher with ImGui interface
  - **Features implemented**:
    - Study-based organizational structure aligned with online platform
    - Create/edit/manage studies with tests and chains
    - Study-info.json and chain JSON parsing/editing
    - Battery browser (read-only view of installed tests)
    - Chain editor with instruction/consent/test/completion pages
    - Chain execution engine with ChainPage integration
    - Snapshot creation (clean exports excluding data/)
    - Snapshot import (including ZIP support with format conversion)
    - Snapshot validation with error/warning reporting
    - Quick Launch feature (run most recent study directly)
    - Subject code tracking and duplicate detection
    - Upload configuration (study-level token and server URL)
    - Chain-level upload enable/disable checkboxes
    - File picker for upload.json (auto-populates server settings)
    - First-run workspace initialization
  - **Architecture**:
    - C++ with Dear ImGui for cross-platform UI
    - SDL2 for window management
    - Study/Chain/Test class hierarchy
    - SnapshotManager for import/export operations
    - ZipExtractor for ZIP file handling
    - JSON parsing with nlohmann::json
  - **Bidirectional workflow**:
    - Create studies natively and upload to platform
    - Download platform snapshots and use directly
    - Consistent format between native and online
    - Automatic format conversion for platform snapshots
  - **Build integration**:
    - Separate Makefile in `src/apps/launcher/`
    - Links against SDL2, ImGui, libzip
    - Builds to `bin/pebl-launcher`
    - AppImage support (combined with pebl2 and validator)
  - Related files:
    - `src/apps/launcher/` (complete launcher implementation)
    - `doc/NATIVE_LAUNCHER_STUDY_SYSTEM.md` (40KB specification)
    - `doc/APPIMAGE_LAUNCHER_INTEGRATION.md` (integration docs)
  - Related commits: daed98b (initial), ecfdfbb (UI improvements), b17aeda (Makefile), a44ea2f (AppImage)

- [x] **Implement PEBL Validator** ✅ **COMPLETE (Dec 27, 2025)**
  - **Commit e9d1985**: Command-line syntax validation tool
  - **Features implemented**:
    - Syntax validation using PEBL's parser (grammar.y, Pebl.l)
    - File existence checking
    - JSON and text output modes (--json flag)
    - Proper exit codes for scripting (0=valid, 1=invalid, 2=usage error)
    - Start() function requirement validation (commit 6861855, Dec 29, 2025)
  - **Architecture**:
    - Full PEBL parser integration (parse-only, no execution)
    - Separate build: `make validator` → `bin/pebl-validator`
    - No SDL dependencies for execution (validator platform layer)
    - Uses `obj-validator/` build directory
  - **Use cases**:
    - Pre-upload validation in online platform (PHP integration)
    - LLM-assisted PEBL code validation
    - Automated testing in CI/CD pipelines
    - Batch validation of battery tests
  - **Example usage**:
    ```bash
    bin/pebl-validator battery/corsi/corsi.pbl
    bin/pebl-validator test.pbl --json
    ```
  - **Future enhancements** (Phase 2):
    - Function validation (detect undefined function calls)
    - AST-based linting (unsafe patterns, common mistakes)
    - Variable usage analysis
  - Related files:
    - `src/apps/PEBLValidator.cpp` (main validator app)
    - `src/platforms/validator/` (validator platform layer)
    - `doc/PEBL_VALIDATOR.md` (documentation)
  - Related commits: e9d1985 (initial), 6861855 (Start validation)

- [x] **Data file handling for online deployment** ✅ **COMPLETE (Verified Jan 28, 2026)**
  - **Status**: Already implemented and working correctly
  - **How it works**:
    - `FileOpenWrite()` in `src/libs/PEBLStream.cpp` (lines 167-228) automatically prevents data loss
    - When file exists, auto-generates versioned filename (e.g., `stroop-P0011.csv`, `stroop-P0012.csv`)
    - Displays warning: "File [original] already exists. Using [versioned] instead"
    - Works identically on native and Emscripten builds
  - **Online workflow**:
    - `emscripten/pebl-lib/EM.pbl` overrides `GetNewDataFile()` for token-based paths
    - Uses `gDataDirectory` when defined (online mode): `/data/{token}/{testname}/{participant}/`
    - Falls back to local `data/` directory (desktop mode)
    - Calls `FileOpenWrite()` which handles versioning automatically
  - **Participant ID management**:
    - `InitializeUpload()` in `pebl-lib/Utility.pbl` reads `upload.json`
    - Sets `gSubNum` from `upload.json` (handles chain-launcher suffixes)
    - Chain-launcher can modify IDs: "P001" → "P001-stroop", "P001-flanker"
    - Each test gets correct participant ID with optional suffix
  - **Multi-test chain handling**:
    - Upload.json rewritten between tests with modified participant ID
    - Each participant gets isolated directory: `/data/{token}/{testname}/{participant}/`
    - Pooled files (via `FileOpenAppend()`) go to participant directory
    - No conflicts during upload
  - Related files:
    - `src/libs/PEBLStream.cpp` (FileOpenWrite auto-versioning)
    - `emscripten/pebl-lib/EM.pbl` (GetNewDataFile override)
    - `pebl-lib/Utility.pbl` (InitializeUpload, GetSubNum)

### Pending

None - all online deployment infrastructure is complete and working.

## Code Quality

### High Priority

- [x] **Implement recursive property printing for debugging** ✅ **COMPLETE (Dec 16, 2025)**
  - **Implemented as `PrintProperties()`** - Overrides compiled version with interpreted PEBL function
  - **Location**: `pebl-lib/Utility.pbl` and `emscripten/pebl-lib/Utility.pbl` (lines 2410-2469)
  - **Features implemented**:
    - Recursive traversal with configurable depth (default 5)
    - Circular reference detection using object equality (enabled by Variant.cpp bug fix)
    - Object type display using `"TYPE" + obj` pattern
    - Indented hierarchical output for clarity
  - **Prerequisites met**: Object equality bug fixed in Variant.cpp (commit from Dec 16, 2025)
  - **Key insight**: No need for GetObjectID() - objects can be compared directly via IsMember()
  - Related files: `test-printproperties.pbl` (test suite), `src/base/Variant.cpp` (equality fix)
  - **Implementation**:
    - Location: `pebl-lib/Utility.pbl` (add to standard library)
    - Function signature: `PrintPropertiesRecursive(obj, recursive:1, depth:5, _indent:0, _visited:[])`
    - Parameters:
      - `obj` - The PEBL object to inspect
      - `recursive:1` - Enable recursive traversal (1=yes, 0=no, equivalent to PrintProperties)
      - `depth:5` - Maximum recursion depth (prevents infinite loops)
      - `_indent:0` - Internal: current indentation level (not user-specified)
      - `_visited:[]` - Internal: list of visited object IDs to detect circular references
  - **Algorithm**:
    ```pebl
    define PrintPropertiesRecursive(obj, recursive:1, depth:5, _indent:0, _visited:[])
    {
      ## Get object ID to track visited objects
      objID <- GetObjectID(obj)  ## Need to implement this or use object address

      ## Check if already visited (circular reference)
      if(IsMember(objID, _visited))
      {
        Print(Indent(_indent) + "[Circular reference to " + objID + "]")
        return
      }

      ## Check depth limit
      if(_indent >= depth)
      {
        Print(Indent(_indent) + "[Max depth " + depth + " reached]")
        return
      }

      ## Add to visited list
      _visited <- Append(_visited, objID)

      ## Get all properties
      props <- GetPropertyNames(obj)  ## Need to implement or use existing method

      ## Print each property
      loop(propName, props)
      {
        value <- GetProperty(obj, propName)

        ## Check if value is a PEBLObject
        if(recursive and IsPEBLObject(value))
        {
          Print(Indent(_indent) + propName + ": <object>")
          ## Recurse into nested object
          PrintPropertiesRecursive(value, recursive, depth, _indent + 1, _visited)
        } else {
          ## Print simple value
          Print(Indent(_indent) + propName + ": " + value)
        }
      }
    }

    define Indent(level)
    {
      return RepeatString("  ", level)  ## 2 spaces per level
    }
    ```
  - **Required helper functions** (may need C++ implementation):
    - `GetObjectID(obj)` - Returns unique identifier for object (memory address or internal ID)
    - `GetPropertyNames(obj)` - Returns list of all property names for object
    - `IsPEBLObject(value)` - Returns true if value is a complex PEBL object
  - **Alternative simpler implementation** (if helpers not available):
    - Use existing `PrintProperties(obj)` for top level
    - Manually recurse into known nested properties
    - Check property type with `IsObject()` or similar
  - **Example usage**:
    ```pebl
    ## Simple (no recursion)
    PrintPropertiesRecursive(gLayout, 0)  ## Same as PrintProperties(gLayout)

    ## Recursive with default depth (5)
    PrintPropertiesRecursive(gLayout)

    ## Recursive with custom depth
    PrintPropertiesRecursive(gLayout, 1, 3)  ## Max 3 levels deep
    ```
  - **Example output**:
    ```
    name: evenodd
    centerX: 400
    centerY: 300
    header: <object>
      text: Decide whether odd or even
      x: 400
      y: 50
      font: <object>
        size: 40
        fgcolor: <object>
          red: 0
          green: 0
          blue: 128
        bgcolor: <object>
          red: 255
          green: 255
          blue: 255
    footer: <object>
      text: ODD                    EVEN
      x: 400
      y: 750
    responseLabels: <list of 2 objects>
      [1]: <object>
        text: ODD <lshift>
        x: 200
        y: 700
      [2]: <object>
        text: <rshift> EVEN
        x: 600
        y: 700
    ```
  - **Benefits**:
    - Easier debugging of complex nested objects (Layout, Response System)
    - No C++ code changes required (pure PEBL implementation)
    - Flexible depth control prevents infinite loops
    - Circular reference detection prevents crashes
  - **Testing**:
    - Test with simple object (Label)
    - Test with nested object (Layout with header/footer/responseLabels)
    - Test with circular references (object refers to parent)
    - Test depth limiting
    - Test with recursive=0 (should match PrintProperties behavior)
  - **Timeline**: 1-2 days
  - **Dependencies**: May need `GetObjectID()` and `GetPropertyNames()` C++ functions

- [ ] Document counted_ptr reference counting mechanism
- [ ] Add memory leak tests to test suite
- [ ] Profile memory usage of common battery tasks

## Survey & Questionnaire Infrastructure

### High Priority

- [ ] **Implement ScaleRunner System** 🚧 **IN PROGRESS (Feb 2026)**
  - **Purpose**: Unified platform for psychological scales and questionnaires
  - **Problem solved**: Eliminates code duplication across 7+ scale implementations (SUS, BigFive, TLX, FASCW, SSSQ, etc.)
  - **Current status**: Core infrastructure complete, ready for testing
  - **Implementation completed**:
    - Created `battery/scales/ScaleRunner.pbl` (~670 lines)
    - JSON-based scale definition system
    - Reverse coding system (coding: 1 vs coding: -1)
    - Composite scoring (sum_coded, mean_coded, weighted_sum)
    - Question filtering by dimension (do_ext, do_agr parameters)
    - Formatted report generation
    - Translation system integration
    - LSL marker support
    - Upload system integration
  - **Question types implemented**:
    - ✅ Likert scale (horizontal click rectangles) - COMPLETE
    - 🔲 VAS (Visual Analog Scale) - from tiredness.pbl, TLX.pbl
    - 🔲 Grid rating - from FASCW.pbl
    - 🔲 Text entry (short/long) - from survey.pbl
    - 🔲 Multiple choice - from survey.pbl
    - 🔲 Instructions - from survey.pbl
  - **Scale definitions created**:
    - `battery/scales/definitions/bigfive.json` - 50 items, 5 dimensions
    - Parameter schema: `battery/scales/params/ScaleRunner.pbl.schema.json`
    - Translation template: `battery/scales/translations/bigfive.pbl-en.json`
  - **Usage**:
    ```bash
    bin/pebl2 battery/scales/ScaleRunner.pbl -s P001 -v scale bigfive
    bin/pebl2 battery/scales/ScaleRunner.pbl -s P001 -v scale bigfive -v do_ext 0
    ```
  - **Next steps**:
    - Implement remaining question types (VAS, Grid, text, multi-choice)
    - Create additional scale definitions (SUS, TLX, FASCW, SSSQ, KSS)
    - Add all 50 BigFive question texts to translations
    - Test reverse coding and composite scoring
    - Validate against existing scale implementations
    - Add to online platform battery selection
  - **Benefits**:
    - Single codebase for all scales (DRY principle)
    - Easy to add new scales (just JSON, no code)
    - Consistent data format across scales
    - Automatic scoring with reverse coding
    - Formatted reports for participants/researchers
    - Argument-based selection: RunScale("bigfive", options)
  - Related files:
    - `battery/scales/ScaleRunner.pbl` (main runner)
    - `battery/scales/definitions/*.json` (scale definitions)
    - `battery/scales/translations/*.json` (multi-language support)
    - `doc/SCALERUNNER_DESIGN.md` (comprehensive design document)

### Medium Priority

- [ ] **Expand ScaleRunner Question Type Library**
  - Port VAS implementation from tiredness.pbl (lines 217-292, 450-523)
  - Port Grid implementation from FASCW.pbl (lines 47-186)
  - Port text/multi-choice from survey.pbl
  - Add pairwise comparison (for TLX weighting phase)
  - Timeline: 3-5 hours total

- [ ] **Create Scale Definition Library**
  - Convert existing scales to JSON format:
    - SUS (10 items, simple Likert)
    - TLX (6 VAS + pairwise weighting)
    - FASCW (10 items, grid layout)
    - SSSQ (24 items, Likert)
    - KSS (1 VAS item)
  - Add 10-20 common scales from research literature
  - Document licensing and citations
  - Timeline: 1 hour per scale

- [ ] **Online Survey Builder**
  - Web-based GUI for creating custom scales
  - Point-and-click question editor
  - Export to ScaleRunner JSON format
  - Integration with PEBLOnlinePlatform
  - Timeline: 4-6 weeks

### Deferred

- [ ] **Stimulus Specification System for Memory Tests**
  - Generic framework for DRM, free recall, recognition tasks
  - JSON-based stimulus definitions
  - Automatic counterbalancing
  - Lower priority - narrower use case than scales
  - Requires more research into paradigm variations

## Documentation

- [ ] Document font factory architecture once implemented
- [ ] Add memory management best practices guide
- [ ] Update PEBL manual with object lifecycle information
