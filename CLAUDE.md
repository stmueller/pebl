# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## CRITICAL: Read Notes_for_Claude_on_Programming_PEBL.txt First

**Before writing any PEBL code, you MUST read `Notes_for_Claude_on_Programming_PEBL.txt`.**

This file contains essential information about:
- Common PEBL programming mistakes and how to avoid them
- Emscripten-specific implementation details (EM.pbl overrides)
- How to find and use PEBL functions (built-in vs library)
- Complete workflow for creating new battery tasks
- Comprehensive function reference organized by category

The notes file is the authoritative reference for PEBL development patterns and will prevent most common errors.

## Project Overview

PEBL (Psychology Experiment Building Language) is a cross-platform psychological experiment development system. It includes:
- A custom programming language (PEBL) with interpreter
- Native Linux build (C++ with SDL2)
- WebAssembly/Emscripten build for browser deployment
- 100+ pre-built psychological test battery tasks
- Standard library of experimental psychology functions

## Build System

### Main Build Targets

**Native Linux Build:**
```bash
make main              # Build native Linux executable (bin/pebl2)
```

**WebAssembly/Emscripten Build:**
```bash
make em                # Compile to WebAssembly (bin/pebl2.html, pebl2.js, pebl2.wasm)
make fp                # Repackage data files after library changes (emscripten only)
```

**Other Targets:**
```bash
make parse             # Regenerate parser from grammar.y and Pebl.l (requires bison/flex)
make install           # Install to /usr/local/ (or PREFIX)
make clean             # Remove object files
make doc               # Build PDF manual from LaTeX source
```

### Important Build Notes

- After modifying files in `pebl-lib/`, you must edit BOTH:
  - `pebl-lib/` (authoritative source)
  - `emscripten/pebl-lib/` (packaged into pebl2.data)
  - Then run `make fp` to rebuild the data package
- The `fp` target uses `file_packager.py` to bundle files into pebl2.data
- Parser changes require bison and flex installed

## Architecture

### Core Language Implementation

**Parser/Lexer (src/base/):**
- `grammar.y` - Bison grammar defining PEBL syntax
- `Pebl.l` - Flex lexer specification
- `grammar.tab.cpp` - Generated parser (don't edit directly)
- `lex.yy.c` - Generated lexer (don't edit directly)
- `Evaluator.cpp` - AST evaluator that executes parsed code
- `PComplexData.cpp`, `PNode.cpp` - Parse tree node representations

**Function Libraries (src/libs/):**
- `Functions.h` - Master function registration table mapping PEBL names to C++ implementations
- `PEBLMath.cpp/h` - Math functions (trigonometry, random, rounding)
- `PEBLList.cpp/h` - List operations (shuffle, sort, access)
- `PEBLString.cpp/h` - String manipulation
- `PEBLEnvironment.cpp/h` - System interaction (keyboard, mouse, timing, files)
- `PEBLStream.cpp/h` - File I/O, network, HTTP
- `PEBLObjects.cpp/h` - Graphics objects (windows, shapes, images, audio)

**Graphics Objects (src/objects/):**
- `PWindow.cpp/h` - Display window management
- `PCanvas.cpp/h` - Pixel-level drawing surface
- `PImageBox.cpp/h` - Image display
- `PTextBox.cpp/h` - Text input widget
- `PLabel.cpp/h` - Text display
- `PDrawObject.cpp/h` - Base for geometric shapes
- `PWidget.cpp/h` - Base for interactive UI elements
- `PColor.cpp/h` - Color representation
- `PFont.cpp/h` - Font management

### PEBL Standard Library (pebl-lib/)

Auto-loaded `.pbl` files providing high-level functions:
- `Utility.pbl` - Common helpers (parameters, data files, UI dialogs, translations)
- `Design.pbl` - Experimental design (counterbalancing, sampling, randomization)
- `Graphics.pbl` - Advanced graphics (layouts, shapes, geometry)
- `Math.pbl` - Statistics and distributions
- `UI.pbl` - Complex widgets (buttons, menus, text entry, scrollboxes)
- `EM.pbl` - Event management wrappers for input handling (critical for Emscripten)
- `HTML.pbl` - HTML generation utilities
- `Transfer.pbl` - Network file transfer

**Critical EM.pbl Note:**
This file contains PEBL-interpreted reimplementations of C++ event loop functions that have issues in WebAssembly. Functions like `WaitForMouseButton`, `WaitForKeyPress` are overridden here using the `RegisterEvent`/`StartEventLoop` pattern. These interpreted versions must always include explicit `return()` statements.

### Battery Tasks (battery/)

Each task directory follows this structure:
```
battery/taskname/
├── taskname.pbl                    # Main experiment script
├── taskname.pbl.about.txt          # Task description
├── params/
│   └── taskname.pbl.schema         # Parameter definitions (param|default|description)
├── translations/
│   └── taskname.pbl-lang.json      # Localized strings (keys are UPPERCASE)
└── data/                            # Output directory (created at runtime)
```

## PEBL Programming Language

### Key Characteristics

- **Interpreted language** with C-style syntax
- **Dynamically typed** - variables don't need type declarations
- **Case-insensitive** function names (though convention uses mixed case)
- **List-based** - primary data structure is the list `[1, 2, 3]`
- **No ternary operator** - use full if/else blocks
- **No newline escapes** - use `CR(1)` for line breaks instead of `\n`
- **Explicit returns required** - `return(value)` not just `value`

### Common PEBL Patterns

**Variable Naming (SYNTAX REQUIREMENT):**
- **All variables MUST start with a lowercase letter**
- Variables starting with `g` are global (e.g., `gWin`, `gParams`, `gSubNum`)
- This is not a convention - it is part of the PEBL syntax
- Global variables are automatically available across all functions
- Local variables start with any other lowercase letter (e.g., `trial`, `response`, `xPos`)

**Parameter Management:**
```pebl
parpairs <- [["param1", default1], ["param2", default2]]
gParams <- CreateParameters(parpairs, gParamFile)
## Access via gParams.param1, gParams.param2
```

**Translation Strings:**
```pebl
GetStrings(gLanguage)  ## Loads from translations/taskname.pbl-en.json
inst <- gStrings.instructions  ## Keys are case-insensitive
```

**Experimental Design:**
```pebl
conditions <- DesignFullCounterbalance(factorList)
conditions <- Shuffle(conditionList)
loop(trial, conditions) {
    ## trial logic
}
```

**Data Files:**
```pebl
gFileOut <- GetNewDataFile(gSubNum, gWin, "taskname", "csv", "sub,trial,rt,acc")
FilePrint(gFileOut, gSubNum + "," + trial + "," + rt + "," + accuracy)
```

## Finding Function Definitions

**Built-in C++ functions:**
- Check `src/libs/Functions.h` for function table entries
- Format: `{(char*)"FUNCTIONNAME", FunctionPointer, minArgs, maxArgs}`

**Standard library functions:**
- Search `pebl-lib/*.pbl` for `define FunctionName(`
- Parameters with colons have defaults: `define Func(param, optional:default)`

**Usage examples:**
- Search battery tasks: `grep -r "FunctionName\(" battery/`
- Most tasks demonstrate common patterns

## Testing PEBL Code

**Run a PEBL script:**
```bash
bin/pebl2 path/to/script.pbl                    # Native
bin/pebl2 path/to/script.pbl -v subnum=123      # With subject number
```

**Common test files in root:**
- `test.pbl`, `test-simple.pbl` - Minimal test cases
- `choiceRT.pbl`, `dot_numerosity.pbl` - Example experiments
- Battery tasks in `battery/*/` provide full examples

## File Locations Reference

**Preload paths for Emscripten:**
- Standard library: `@/usr/local/share/pebl2/pebl-lib`
- Media files: `@/usr/local/share/pebl2/media`
- These virtual paths are what PEBL scripts reference

**Media assets:**
- `media/fonts/` - TrueType fonts (DejaVu, FreeSans, etc.)
- `media/images/` - Stock images
- `media/sounds/` - Audio files
- `media/text/` - Word lists and text resources

## Development Workflow

**When modifying C++ built-in functions:**
1. Edit source in `src/libs/PEBL*.cpp`
2. Rebuild: `make clean && make main`
3. Test with native binary
4. For Emscripten: `make clean && make em`

**When modifying standard library:**
1. Edit both `pebl-lib/*.pbl` AND `emscripten/pebl-lib/*.pbl`
2. For native: just run `bin/pebl2 test.pbl`
3. For Emscripten: run `make fp` to repackage data files

**When creating a new battery task:**
1. Copy similar existing task from `battery/`
2. Follow directory structure (main .pbl, params/, translations/)
3. Update parameter schema and translation JSON
4. Test with `bin/pebl2 battery/taskname/taskname.pbl`

## Debugging

- Enable debugging in Makefile: `USE_DEBUG = 1`
- Check compilation flags in `CXXFLAGS_LINUX` or `CXXFLAGS_EMSCRIPTEN`
- For parser issues: `make parse-debug` generates verbose output
- For Emscripten: Check browser console for runtime errors
