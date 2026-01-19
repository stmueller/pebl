# PEBL Development Master Plan

This document tracks major improvements and architectural changes for PEBL.

## Memory Management & Performance

### High Priority

- [x] **Fix counted_ptr reference counting system** ✅ **COMPLETE (Dec 4, 2025)**
  - **Fixed in commit 4ece1c1**: Eliminated unnecessary copies causing excessive reference counting
  - Changed PComplexData, VariableMap, Evaluator to pass by const reference
  - Objects now properly freed when references cleared
  - Related files: `src/utility/rc_ptrs.h`, `src/base/Variant.cpp`, `src/base/PComplexData.cpp`

- [ ] **Implement font caching/factory pattern** (CRITICAL for markdown rendering)
  - Currently every `EasyLabel()` creates a new font object, even with identical parameters
  - 100 labels with same font = 100 separate TTF file loads (~3.7MB leaked)
  - **Key insight**: SDL_TTF fonts are color-agnostic! Colors applied at render time, not stored in TTF_Font
  - Multiple PEBL fonts with same (filename, style, size) but different colors can share one TTF_Font
  - **Implementation**:
    - Font factory caches by key: {filename, style, size} (excludes color!)
    - Reference counting: TTF_Font freed when last PFont using it is destroyed
    - Create `src/utility/FontCache.h/.cpp` with singleton `FontCache` class
    - Modify `PlatformFont` constructor/destructor to use cache
    - Location: `src/platforms/sdl/PlatformFont.cpp`
  - **Prerequisites**: counted_ptr issues fixed ✅, safe to implement now
  - **Impact**: 80-90% memory reduction for color-variant fonts, essential for markdown rendering
  - **Timeline**: 1 week
  - Related docs: `doc/MARKDOWN_FONT_CACHE_PLAN.md`

### Medium Priority

- [x] **Investigate sporadic font color rendering bug** ✅ **COMPLETE (Dec 9-10, 2025)**
  - **Fixed in commit 4ed0e7f**: Fixed segfault when assigning color properties via nested syntax
  - **Fixed in commit 017b3ef**: Refactored PFont to use property-only color storage
  - Eliminated double ownership bugs and memory corruption from color assignment
  - Related files: `src/objects/PFont.cpp`, `doc/FONT_COLOR_PROPERTY_BUG_FIX.md`

## Text Rendering & Formatting

### High Priority

- [ ] **Implement markdown and rich text rendering in textboxes**
  - **Phase 1: Inline formatting** (2-3 days) - Simple tag parser for basic formatting
    - Support tags: `<b>bold</b>`, `<i>italic</i>`, `<c=color>text</c>`, `<size=N>text</size>`
    - Implementation:
      - Create `FormatParser` class in `src/utility/FormatParser.h/.cpp`
      - Extend `PlatformFont::RenderFormattedText()` to render tagged segments
      - Modify `PlatformTextBox::RenderText()` to handle formatted text
      - Add `FORMATTED` property to enable (disabled by default for backward compatibility)
    - PEBL usage: `SetProperty(textbox, "FORMATTED", 1)`
    - Works with existing architecture, no external dependencies
    - Cross-platform: native, Emscripten, Windows
  - **Phase 2: Full markdown support** (1-2 weeks) - CommonMark compliant rendering
    - Integrate md4c library (pure C, very fast, one .c + one .h file)
    - Support: Headers (# H1), **bold**, *italic*, lists, links, `code blocks`
    - Implementation:
      - Add `src/utility/md4c.c` and `md4c.h` to project
      - Create `MarkdownRenderer` class in `src/utility/MarkdownRenderer.h/.cpp`
      - Implement md4c parser callbacks to generate formatted segments
      - Add `MARKDOWN` property to PTextBox
    - PEBL usage: `SetProperty(textbox, "MARKDOWN", 1)`
    - Use cases: Experiment instructions, help text, formatted reports
  - **Phase 3: Markdown to HTML export** (1 week) - Generate HTML files from markdown
    - New PEBL function: `MarkdownToHTML(markdown_text, output_file, css_file:"")`
    - Converts markdown to HTML for documentation, results reporting
    - Implementation:
      - Create `HTMLGenerator` class to convert md4c AST to HTML
      - Register `MarkdownToHTML()` in `src/libs/Functions.h`
      - Provide default CSS stylesheet in `media/markdown.css`
      - Integrate with existing `pebl-lib/HTML.pbl` library
    - Use cases: Protocol documentation, printable instructions, archival
  - **Prerequisites**:
    - Font cache MUST be implemented first (prevents memory bloat from styled text)
    - Markdown with inline colors creates many font variants - cache makes this efficient
  - **Backward compatibility**: All features opt-in, existing plain text unchanged
  - **Testing**: Plain text (no regression), formatted text, markdown, line wrapping, Emscripten
  - Related docs: `doc/SDL_RTF_INVESTIGATION.md`, `doc/MARKDOWN_FONT_CACHE_PLAN.md`

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

### Pending

- [ ] Revise `GetSubNum()` and `GetNewDataFile()` for online deployment
- [ ] Use MD5-string as natural unique subject number
- [ ] Handle multiple tests of same kind in test chains

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

## Documentation

- [ ] Document font factory architecture once implemented
- [ ] Add memory management best practices guide
- [ ] Update PEBL manual with object lifecycle information
