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
