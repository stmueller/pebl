# PEBL Function Inventory

Cross-reference of all PEBL functions across documentation and source code.

**Generated:** 2025-11-12
**Updated with compiled functions:** 2025-11-12
**Updated with PEBL library functions:** 2025-11-12

## Critical Distinction: Compiled vs PEBL Functions

**Compiled Functions** are implemented in C++ and registered in `src/libs/Functions.h`. Their parameters are **never explicitly written down** in the function signature - they all use the generic signature `Variant FunctionName(Variant v)`. This means parameter names, types, and counts must be carefully extracted from the function implementation itself.

**PEBL Functions** are implemented in PEBL script (.pbl files in pebl-lib directory) and have **explicit parameter lists** including optional parameters with default values (using the `:` operator). These are easier to document since parameters are clearly visible in the source code.

## Summary Statistics

| Source | Count | Description |
|--------|-------|-------------|
| **TOTAL UNIQUE FUNCTIONS** | 727 | All unique functions across all sources |
| | | |
| **reference.tex** | 361 | Functions documented in main reference chapter |
| **chap5.tex** | 1 | Functions mentioned in tutorial chapter (Wait) |
| | | |
| **Functions.h (COMPILED)** | 271 | **Compiled C++ functions registered in Functions.h** |
| **PEBL Libraries (.pbl)** | 202 | **PEBL functions in pebl-lib/*.pbl files** |
| **Source code (C++)** | 277 | Functions implemented in src/libs/*.cpp |
| | | |
| **In BOTH reference & source** | 209 | Documented AND implemented ✅ |
| **ONLY in reference** | 152 | Documented but not found in source |
| **ONLY in source** | 68 | Implemented but NOT documented ⚠️ |
| | | |
| **🔴 COMPILED but NOT documented** | 221 | **Critical: Compiled functions missing from docs** |
| **✅ COMPILED and documented** | 50 | Compiled functions properly documented |
| | | |
| **🟡 PEBL FUNCTION undocumented** | 93 | **PEBL library functions missing from docs** |
| **✅ PEBL FUNCTION documented** | 103 | PEBL library functions properly documented |

## Compiled Functions Analysis (Functions.h)

### 🔴 CRITICAL: 221 Compiled Functions NOT Documented

Functions registered in `src/libs/Functions.h` are **compiled C++ functions**. Their parameters are **never explicitly written** in the code (all use `Variant FuncName(Variant v)`), so documentation must be extracted carefully from the implementation.

**276 total compiled functions** organized by namespace:

| Namespace | Count | Purpose |
|-----------|-------|---------|
| **PEBLMath** | 43 | Mathematical operations, trigonometry, random numbers |
| **PEBLStream** | 44 | File I/O, networking, HTTP, JSON, serial/parallel ports |
| **PEBLObjects** | 63 | Graphics, widgets, sound, custom objects, eye tracking |
| **PEBLEnvironment** | 86 | Timing, keyboard/mouse/joystick input, system info, file system |
| **PEBLList** | 30 | List manipulation, sorting, merging |
| **PEBLString** | 10 | String operations |

Of these **276 compiled functions**:
- ✅ **50** are documented in reference.tex
- 🔴 **221** are NOT documented (80% undocumented!)

### Examples of Undocumented Compiled Functions

**High Priority (commonly useful):**
- `GetTimeHP` - High-precision timing
- `GetObjectTime` - Object-specific timing
- `GetExecutableName`, `GetHomeDirectory`, `GetWorkingDirectory`, `SetWorkingDirectory`
- `GetMyIPAddress`, `GetHTTPFile`, `GetHTTPText`, `PostHTTP`, `PostHTTPFile`
- `ParseJSON` - JSON parsing
- `GetJoystick`, `OpenJoystick` and all joystick functions
- `ConnectEyeTracker`, `GetEyeObject`, `SetEyeTrackerHandler`
- `MakeChirp`, `MakeSawtoothWave`, `MakeSquareWave` - Audio wave generation
- `ResizeWindow`, `RotoZoom`
- `CallFunction` - Dynamic function calls
- `RegisterEvent`, `StartEventLoop`, `ClearEventLoop` - Event system
- `TranslateString`, `TranslateKeyCode`
- `GetLineBreaks`, `GetTextBoxCursorFromClick`

**File Operations:**
- All file operations use compiled functions but many are documented
- `CopyFile` is undocumented

**Type Checking Functions (all undocumented):**
- `IsText`, `IsNumber`, `IsInteger`, `IsFloat`, `IsString`, `IsList`
- `IsTextBox`, `IsCanvas`, `IsImage`, `IsLabel`, `IsAudioOut`, `IsFont`, `IsColor`
- `IsFileStream`, `IsWidget`, `IsWindow`, `IsShape`, `IsCustomObject`

**System Functions:**
- `GetCurrentScreenResolution`, `GetVideoModes`, `GetDrivers`
- `GetPEBLVersion`, `GetSystemType`
- `LaunchFile`, `SystemCall`, `SystemCallUpdate`
- `VariableExists`, `ExitQuietly`

## PEBL Library Functions Analysis (pebl-lib/*.pbl)

### 🟡 IMPORTANT: 93 PEBL Functions NOT Documented

Functions in the PEBL library files are written in PEBL itself and are **available to all PEBL programs**. Unlike compiled functions, their parameters are explicitly declared, making documentation easier.

**202 total PEBL library functions** organized by file:

| Library File | Count | Purpose |
|--------------|-------|---------|
| **Utility.pbl** | 61 | Utilities, demographics, text input, file operations |
| **UI.pbl** | 52 | Buttons, menus, scrollboxes, pulldowns, custom widgets |
| **Graphics.pbl** | 35 | Graphics helpers (point rotation, Gabor, Attneave shapes) |
| **Design.pbl** | 24 | Experimental design (Latin squares, sampling, shuffling) |
| **Math.pbl** | 21 | Statistics (mean, median, correlation, t-tests) |
| **HTML.pbl** | 12 | HTML generation helpers |

Of these **202 PEBL library functions**:
- ✅ **103** are documented in reference.tex (51%)
- 🟡 **93** are NOT documented (46%)
- 🔄 **6** wrap compiled functions (e.g., Append, Merge, IsMember)

### Examples of Undocumented PEBL Library Functions

**UI Functions (UI.pbl):**
- `MakeButton(label, x, y, win, width)` - Create button widget
- `MakeScrollBox(listx, x, y, width, height, win, size, fgColor, bgColor)` - Create scrollbox
- `MakePulldown(label, options, x, y, win, fontname, fgColor, bgColor)` - Create dropdown menu
- `ClickOn(objects, win)` - Wait for click on one of multiple objects
- `DrawButton(button)`, `HideButton(button)`, `ShowButton(button)` - Button manipulation

**Graphics Functions (Graphics.pbl):**
- `MakeGabor(size, freq, sd, angle, phase, bg)` - Create Gabor patch
- `MakeAttneave(n, size)` - Create Attneave random polygon
- `KaniszaSquare(x, y, len, wid, angle, grating)` - Kanisza illusion square
- `RotatePoints(pts, theta)`, `ReflectPoints(pts)`, `ZoomPoints(pts, xZoom, yZoom)` - Point transformations

**Math/Statistics (Math.pbl):**
- `Mean(list)`, `Median(list)`, `StdDev(list)` - Basic statistics
- `Quantile(list, prob)` - Quantile calculation
- `Correlation(a, b)`, `CorrelationPValue(a, b)` - Correlation analysis
- `TTest(a, b)`, `TTestPaired(a, b)` - T-tests
- `ZTest(a, b)`, `ChiSquare(obs, exp)` - Statistical tests

**Design Functions (Design.pbl):**
- `DesignLatinSquare(list1, list2)`, `LatinSquare(list)` - Latin square designs
- `DesignGrecoLatinSquare(a, b, c)` - Greco-Latin square
- `DesignBalancedSampling(list, number)` - Balanced sampling
- `ShuffleRepeat(list, n)` - Shuffle with no adjacent repeats
- `ShuffleWithoutAdjacents(list, tries:1)` - Advanced shuffling (note optional parameter)

**Utility Functions (Utility.pbl):**
- `GetNIMHDemographics(code, window, file)` - NIMH-style demographics collection
- `GetInput(text, x, y, win)` - Text input dialog
- `MessageBox(text, win)` - Display message box
- `PopUpEntryBox(text, x, y, win)` - Pop-up text entry
- `FileOpenRead(filename)` (wrapper) - File operations

## Key Findings

### ⚠️ Priority: Undocumented Functions (68)
These functions exist in the source code but are NOT documented in reference.tex:

```
AcceptNetworkConnection    ConnectEyeTracker         GetIPAddress
CallFunction               Fifth                     GetJoystick
ComPortGetByte            Fourth                    GetMyIPAddress
ComPortSendByte           Gabor                     GetObjectTime
GetExecutableName          GetTimeHP                 ParseJSON
GetEyeObject              GetTimeOfDay              PostHTTP
GetHomeDirectory          GetWorkingDirectory       PostHTTPFile
GetHTTPFile               IsJoystick                Recurse
GetHTTPText               MakeChirp                 Recurse2
GetInput0                 MakeSawtoothWave          Remove
                          MakeSquareWave            ResizeWindow
OpenComPort               SetNetworkPort            RotoZoom
Second                    SetWorkingDirectory       Strip
SetEyeTrackerHandler      SystemCallUpdate          Third
SetPoint                  ToInt                     TranslateString
WaitForKeyDown            WaitForKeyRelease         WaitForPPortData
WaitForPPortStatus
```

**Note:** Some of these may be:
- Internal/private functions not meant for public use
- Recently added functions
- Functions with different naming in docs vs code

### Functions Documented but Not in Source (152)

These are documented in reference.tex but weren't found in the C++ source scan. They may be:
- PEBL library functions (implemented in .pbl files)
- Aliases or wrapper functions
- Operators and keywords
- Functions with different internal names

**Operators & Keywords (13):**
`<>`, `+`, `-`, `/`, `*`, `^`, `;`, `#`, `<-`, `and`, `or`, `not`, `break`, `define`, `if`, `if...elseif...else`, `loop`, `return`, `while`

**Library/Helper Functions:**
```
Append                    GetAngle                  MakeMenu
BlockE                    GetAngle3                 MakeMenuItem
ChooseN                   GetEasyChoice             MakeNGonPoints
ClickCheckbox             GetEasyInput              MakePulldown
Clickon                   GetInput                  MakeScrollBox
ClickOnMenu               GetMouseCursorPosition    MakeScrollingTextBox
ClickOnScrollbox          GetNewDataFile            MakeStarPoints
ConcatenateList           GetNIMHDemographics       MakeTextList
ConvertIPString           GetProperty               Match
ConvexHull                GetSubNum                 Max
Countdown                 Insert                    Mean
CR                        Inside                    Median
CumNormInv                InsideTB                  Merge
CumSum                    IsMember                  MessageBox
DesignBalancedSampling    KaniszaPolygon            Min
DesignFullCounterbalance  KaniszaSquare             MoveCenter
DesignGrecoLatinSquare    LatinSquare               MoveCorner
DesignLatinSquare         LayoutGrid                MoveObject
Dist                      Length                    NonOverlapLayout
DrawObject                Levels                    NormalDensity
DrawPulldown              ListBy                    Nth
DrawScrollbox             ListToHumanText           OpenCOMPort
EasyLabel                 Lookup                    OpenJoystick
EasyTextBox               MakeAttneave              OpenSubMenus
Enquote                   MakeButton                Order
ExtractListItems          MakeCheckbox              Plus
FilePrintList             MakeGabor                 PopUpEntryBox
Filter                    MakeGraph                 PopUpMessageBox
Flatten                                             PrintList
FlattenN                                            PropertyExists
FoldList                                            (continued...)
```

### Functions in Both Reference & Source (209) ✅

These are properly documented and implemented:
```
Abs                       GetTime                   Random
AbsFloor                  GetVideoModes             RandomBernoulli
ACos                      GetVocalResponseTime      RandomBinomial
AddObject                 Hide                      RandomExponential
AppendFile                IsAnyKeyDown              RandomLogNormal
ASin                      IsAudioOut                RandomLogistic
ATan                      IsCanvas                  RandomNormal
Bezier                    IsColor                   RandomUniform
Ceiling                   IsCustomObject            RandomizeTimer
CheckForNetworkConnection IsDirectory               ReadCSV
Circle                    IsFileStream              Rectangle
ClearEventLoop            IsFloat                   RegisterEvent
CloseNetworkConnection    IsFont                    RemoveObject
ConnectToHost             IsImage                   (and 176 more...)
ConnectToIP               IsInteger
...
```

## Categorization Needed

To create proper Sphinx documentation, functions should be organized into categories such as:

### Suggested Categories

1. **Graphics & Display**
   - Canvas functions (MakeCanvas, DrawLine, etc.)
   - Shape drawing
   - Color management
   - Images

2. **Input/Output**
   - Keyboard input
   - Mouse input
   - Joystick
   - Touch input

3. **File Operations**
   - File reading/writing
   - Directory operations
   - CSV handling

4. **Network & Communication**
   - HTTP functions
   - Network connections
   - Serial/parallel ports
   - Eye tracking

5. **Audio**
   - Sound loading/playing
   - Audio input
   - Wave generation

6. **Mathematical**
   - Basic math
   - Statistics
   - Random numbers
   - Trigonometry

7. **List Operations**
   - List manipulation
   - Sorting/filtering
   - Statistical functions

8. **String Operations**
   - String manipulation
   - Parsing
   - Conversion

9. **UI Widgets**
   - Buttons, menus
   - Text boxes
   - Scrollboxes
   - Custom objects

10. **System & Environment**
    - Timing
    - System information
    - Process control

11. **Experimental Design**
    - Counterbalancing
    - Randomization
    - Trial management

12. **Data Collection**
    - Response collection
    - Data saving
    - Demographics

## Next Steps

### Immediate Priorities

1. **🔴 HIGHEST PRIORITY: Document the 221 undocumented COMPILED functions**
   - These are in Functions.h and are core C++ functions
   - Parameter information must be extracted from implementation (not in signature)
   - Many are high-value functions (JSON, HTTP, joystick, eye tracking, etc.)
   - Determine which are public API vs internal/experimental
   - Start with high-priority commonly-useful functions

2. **🟡 HIGH PRIORITY: Document the 93 undocumented PEBL library functions**
   - These are in pebl-lib/*.pbl files
   - **Easier to document** because parameters are explicit in source code
   - Many are high-value UI and graphics functions (buttons, scrollboxes, Gabor patches)
   - Includes important statistics functions (TTest, Correlation, etc.)
   - Start with UI.pbl (52 functions) and Graphics.pbl (35 functions)

3. **Document the 68 undocumented source functions**
   - Some overlap with compiled functions above
   - Determine which are public API vs internal
   - Add documentation for public functions
   - Mark private functions appropriately

4. **Verify the 152 "documented but not in source"**
   - Many are now identified as PEBL library functions
   - Some are operators/keywords
   - Identify remaining aliases and wrappers
   - Update or remove obsolete documentation

5. **Categorize all 727 functions**
   - Create logical groupings (use namespace/library structure as starting point)
   - Organize Sphinx documentation by category
   - Add cross-references between related functions

### Documentation Migration Plan

1. Start with well-documented, core functions (the 209 in both)
2. Add missing documentation for the 68 undocumented functions
3. Review and update the 152 potentially obsolete entries
4. Convert to Sphinx format category by category
5. Generate both HTML and LaTeX versions
6. Deploy HTML to web, integrate LaTeX into manual

## Files Generated

- `/tmp/reference_functions.txt` - All 361 functions from reference.tex
- `/tmp/chap5_functions.txt` - 1 function from chap5.tex
- `/tmp/source_functions_methods.txt` - All 277 functions from source code
- `/tmp/only_in_reference.txt` - 152 functions only in documentation
- `/tmp/only_in_source.txt` - 68 functions only in source
- `/tmp/compiled_functions.txt` - All 271 compiled functions from Functions.h
- `/tmp/extract_compiled_functions.py` - Python script to analyze compiled functions
- `/tmp/pebl_library_functions.txt` - All 202 PEBL library functions from .pbl files
- `/tmp/pebl_library_functions.csv` - PEBL functions with parameters (CSV format)
- `/tmp/extract_pebl_functions.py` - Python script to extract PEBL functions
- `/tmp/create_comprehensive_function_list.py` - Comprehensive analysis script
- `~/Dropbox/Research/pebl/pebl/doc/MASTER_FUNCTION_LIST.csv` - **Master list of all 727 functions**
- `~/Dropbox/Research/pebl/pebl/doc/FUNCTION_INVENTORY.md` - This file

## Extraction Methods

**reference.tex:** Searched for `\item[Name/Symbol]` entries

**chap5.tex:** Searched for `\texttt{FunctionName}` and function call patterns

**Source code:** Extracted from `Variant ClassName::FunctionName` patterns in `src/libs/*.cpp`

**Functions.h (Compiled Functions):** Extracted from `FunctionTable[]` entries in each namespace
- Pattern: `{(char*)"FUNCTIONNAME", FunctionName, minArgs, maxArgs}`
- Organized by namespace: PEBLMath, PEBLStream, PEBLObjects, PEBLEnvironment, PEBLList, PEBLString
- **Critical:** These compiled functions all use generic `Variant FuncName(Variant v)` signature
- Parameter details must be extracted from implementation, not from function signature

**PEBL Library Files (.pbl):** Extracted from `define FunctionName(params)` patterns
- Files scanned: Design.pbl, HTML.pbl, Graphics.pbl, UI.pbl, Utility.pbl, Math.pbl
- Pattern: `define FunctionName(param1, param2, param3:default_value)`
- Parameters with `:` have default values (optional parameters)
- **Advantage:** Parameters are explicitly named in source, making documentation easier

**Note:** Source code scan may have missed some functions if they use different naming patterns or are in other directories.
