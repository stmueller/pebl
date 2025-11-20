# Windows Relocatability Analysis

## Current State

### Path Resolution (Already Relocatable!)

**Good news**: Windows path resolution is already exe-relative in `src/apps/PEBL.cpp:1062`:

```cpp
string basedir = PEBLUtility::StripFile(argv[0]) + "..\\";
```

This works exactly like the new Linux implementation:
- `argv[0]` = `bin\pebl2.exe`
- `StripFile()` removes filename → `bin\`
- Add `..\\` → goes up to base directory

**Result**: Windows can already run from any location (portable package, Program Files, custom install, etc.)

### The Real Problem: gUseReloc and Batch File Workaround

The `gUseReloc` flag doesn't control path relocatability—it controls **how the launcher executes child processes**.

## Two Execution Modes

### Mode 1: gUseReloc = 0 (Normal/Installed Mode)

**Used for**: Traditional installations in `C:\Program Files (x86)\PEBL\`

**Execution method** (`bin/launcher.pbl:1727-1738`):
```pebl
pid <- SystemCallUpdate(callstring,"")
RegisterEvent("<TIMER>", 1, GetTime(),"<GEQ>","MONITORCHILDPROCESS", [pid])
StartEventLoop()
```

- Launches child process asynchronously
- Monitors process with event loop
- Captures stdout/stderr via file redirection in command string
- Works fine in normal mode

### Mode 2: gUseReloc = 1 (Portable Mode)

**Used for**: Portable packages (USB drive, Dropbox sync, etc.)

**Execution method** (`bin/launcher.pbl:1720-1726`):
```pebl
runfile <- FileOpenOverWrite("tmp.bat")
FilePrint(runfile,callstring)
FileClose(runfile)
SystemCall("tmp.bat","")
```

- Writes command to temporary batch file
- Executes batch file with blocking SystemCall
- Batch file handles stdout/stderr redirection
- **Why this workaround is needed**: See below

## Why Batch Files Are Used

### The Windows Console Window Problem

Windows `CreateProcess()` with `CREATE_NO_WINDOW` flag (`src/utility/PEBLUtility.cpp:916`):

```cpp
if (CreateProcess(NULL,callstring, NULL, NULL, TRUE,
                  ABOVE_NORMAL_PRIORITY_CLASS|CREATE_NO_WINDOW,
                  NULL, NULL, &info, &processInfo))
```

**Problem**: This combination doesn't properly handle shell redirection operators (`>`, `2>`):
- `CREATE_NO_WINDOW` prevents console window from flashing
- But Windows shell redirection requires a console to exist
- Result: Redirection silently fails or command fails to execute

**Batch file workaround**:
- `.bat` files are executed by `cmd.exe` automatically
- `cmd.exe` creates its own console context
- Redirection works properly within that context
- Original PEBL launcher window remains visible

## Current Installation Scenarios

### Scenario A: Program Files Installation

**Path**: `C:\Program Files (x86)\PEBL\`

**Why `C:\Program Files\` doesn't work**:
- Requires 64-bit build
- Current Windows build is 32-bit (targets wider compatibility)
- 32-bit apps go to `Program Files (x86)` by convention
- This is NOT a relocatability issue—just Windows convention

**Setup**: Users typically install via installer, shortcut points to `bin\pebl2.exe`

**Launcher setting**: `gUseReloc <- 0`

### Scenario B: Portable Package

**Path**: Any location (USB drive, `D:\PEBL\`, Dropbox, etc.)

**Setup**:
- Root batch file `PEBL.bat` launches `bin\pebl2.exe`
- Allows users to double-click without navigating to `bin\`
- More user-friendly for portable deployment

**Launcher setting**: `gUseReloc <- 1` (must be manually edited before packaging)

## Proposed Improvements

### Option 1: Fix Windows Redirection (Recommended)

**Create proper redirection in C++ without batch files**

Implement `SystemCallWithRedirect()` that properly sets up Windows pipes:

```cpp
PROCESS_INFORMATION PEBLUtility::SystemCallWithRedirect(
    std::string call,
    std::string args,
    std::string stdout_file,
    std::string stderr_file)
{
    // Create pipes for stdout/stderr
    SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};
    HANDLE hStdout, hStderr;

    // Open files for redirection
    hStdout = CreateFile(stdout_file.c_str(), GENERIC_WRITE, 0, &sa,
                         CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    hStderr = CreateFile(stderr_file.c_str(), GENERIC_WRITE, 0, &sa,
                         CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // Set up STARTUPINFO with redirected handles
    STARTUPINFO info = {sizeof(info)};
    info.dwFlags = STARTF_USESTDHANDLES;
    info.hStdOutput = hStdout;
    info.hStdError = hStderr;
    info.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

    PROCESS_INFORMATION processInfo;
    std::string tmp = "cmd.exe /c " + call + " " + args;
    char* callstring = const_cast<char *>(tmp.c_str());

    if (CreateProcess(NULL, callstring, NULL, NULL, TRUE,
                      ABOVE_NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW,
                      NULL, NULL, &info, &processInfo))
    {
        // Close our copies of the handles
        CloseHandle(hStdout);
        CloseHandle(hStderr);

        return processInfo;
    }

    // Error handling
    CloseHandle(hStdout);
    CloseHandle(hStderr);
    return processInfo;
}
```

**Benefits**:
- Eliminates batch file workaround
- Works from any location
- Proper async process monitoring
- Cleaner, more robust solution

**Implementation**:
1. Add `SystemCallWithRedirect()` to `PEBLUtility.cpp/h`
2. Expose as PEBL built-in function `SystemCallUpdate2()` or similar
3. Update launcher.pbl to use new function instead of batch files
4. Remove `gUseReloc` flag entirely

### Option 2: Keep Batch Files, Simplify gUseReloc

**Make gUseReloc automatic based on environment**

Instead of manual editing, detect at runtime:

```pebl
## Auto-detect portable vs installed mode
define DetectPortableMode()
{
    ## Check if we're in a "typical" installation path
    exePath <- GetExecutableName()
    if(SubString(ToUpper(exePath), 1, 16) == "C:\PROGRAM FILES")
    {
        return 0  ## Installed mode
    } else {
        return 1  ## Portable mode
    }
}

gUseReloc <- DetectPortableMode()
```

**Benefits**:
- No manual editing of launcher
- Single package for both scenarios
- Still uses batch file workaround

**Drawbacks**:
- Batch files still needed
- Detection heuristic might be wrong
- Doesn't solve underlying problem

### Option 3: Eliminate Portable/Install Distinction

**Always use same execution method**

Accept console window flash OR use Option 1's proper redirection:

```pebl
## Remove all gUseReloc checks
## Always use async SystemCallUpdate
pid <- SystemCallUpdate(callstring,"")
RegisterEvent("<TIMER>", 1, GetTime(),"<GEQ>","MONITORCHILDPROCESS", [pid])
StartEventLoop()
```

**Benefits**:
- Simplest code path
- No mode distinction
- Works from any location

**Drawbacks**:
- Console window may flash briefly (depends on execution environment)
- If redirection doesn't work, launcher can't monitor progress

## Comparison with Linux Implementation

**Linux**: No such distinction needed because:
- Unix fork/exec model doesn't have console window issues
- Shell redirection works reliably via `system()`
- No need for gUseReloc at all

**Windows**: Console subsystem complications require workarounds

## Recommendation

**Implement Option 1** (proper Windows redirection in C++):

### Phase 1: Implement SystemCallWithRedirect
1. Add function to `PEBLUtility.cpp/h`
2. Test thoroughly on Windows 10/11
3. Verify stdout/stderr capture works correctly

### Phase 2: Update Launcher
1. Replace batch file code with new function
2. Remove all `if(gUseReloc)` branches
3. Use single code path for all Windows scenarios

### Phase 3: Cleanup
1. Remove `gUseReloc` variable entirely
2. Update documentation
3. Test both Program Files and portable scenarios

### Phase 4: Distribution
1. Single Windows package works everywhere
2. No manual configuration needed
3. Matches Linux/macOS simplicity

## Testing Checklist

When testing Windows relocatability:

- [ ] Run from `C:\Program Files (x86)\PEBL\bin\pebl2.exe`
- [ ] Run from `C:\Users\Username\Desktop\PEBL\bin\pebl2.exe`
- [ ] Run from USB drive `E:\PEBL\bin\pebl2.exe`
- [ ] Run from network share `\\server\share\PEBL\bin\pebl2.exe`
- [ ] Run from path with spaces `C:\My Programs\PEBL\bin\pebl2.exe`
- [ ] Verify launcher can execute battery tests
- [ ] Verify stdout/stderr are captured to log files
- [ ] Verify no console windows flash during test execution
- [ ] Verify process monitoring works (launcher shows progress)

## Code Locations Reference

**Path resolution**:
- `src/apps/PEBL.cpp:1053-1072` - Windows basedir calculation

**SystemCall implementations**:
- `src/utility/PEBLUtility.cpp:895-935` - Blocking SystemCall
- `src/utility/PEBLUtility.cpp:942-972` - Async SystemCallAndReturn

**Launcher execution logic**:
- `bin/launcher.pbl:33` - gUseReloc flag definition
- `bin/launcher.pbl:1720-1738` - Batch file execution (portable)
- `bin/launcher.pbl:2028-2056` - Main experiment execution with both modes

**Batch file creation**:
- `bin/launcher.pbl:1722-1725` - tmp.bat creation and execution
- `bin/launcher.pbl:2031-2035` - Same for experiment launching

## Conclusion

Windows PEBL is already fully relocatable in terms of **path resolution**. The `gUseReloc` complexity is about **process execution**, not relocatability.

The batch file workaround is functional but inelegant. Implementing proper Windows handle redirection (Option 1) would:
- Eliminate batch files
- Remove gUseReloc complexity
- Make Windows match Linux simplicity
- Provide more robust process management

This should be the long-term solution.
