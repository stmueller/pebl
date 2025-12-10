# PEBL Launcher AppImage Investigation

## Question
Will the PEBL launcher work correctly when PEBL is run from an AppImage?

## Current Launcher Behavior

### How Launcher Identifies PEBL Executable

The launcher relies on `gExecutableName`, which is automatically set by PEBL itself:

**In `src/apps/PEBL.cpp:664`:**
```cpp
myEval->gGlobalVariableMap.AddVariable("gExecutableName", argv[0]);
```

**In `bin/launcher.pbl`:**

**Linux (line 256):**
```pebl
gPEBLName <- gExecutableName
```

**Windows (lines 236, 1691, 1763, 1952):**
```pebl
gPEBLName <- gQuote + gExecutableName + gQuote
## OR when gUseReloc:
gPEBLName <- "\bin\pebl2.exe"  ## relative path
```

### How Launcher Launches Child Processes

**RunScript() function (line 1965):**
```pebl
callstring <- "cd " + gQuote+DirListToText(dirChain)+gQuote + sep  +
              gPEBLName + " "  +gQuote+filename+gQuote + " -s " + subcode
```

Then executed via:
```pebl
pid <- SystemCallUpdate(callstring,"")
```

## AppImage Behavior

### What is `argv[0]` in an AppImage?

When you run an AppImage, **`argv[0]` is the path to the AppImage file itself**, not the extracted binary.

Examples:
- `./PEBL-2.2-x86_64.AppImage` (if run from current directory)
- `/full/path/to/PEBL-2.2-x86_64.AppImage` (if run with full path)
- `PEBL-2.2-x86_64.AppImage` (if in PATH)

### AppImage Environment Variables

AppImages set these environment variables when running:
- `APPIMAGE` - Full path to the AppImage file
- `APPDIR` - Mount point of the AppImage filesystem (e.g., `/tmp/.mount_PEBL-xXXXXX`)
- `ARGV0` - Original argv[0]
- `OWD` - Original working directory

## Analysis

### ✅ **Good News: It Should Work!**

When the launcher tries to launch a child PEBL process using `gExecutableName` (which contains `argv[0]`), it will re-invoke the AppImage itself, which is the correct behavior:

**Example command constructed by launcher:**
```bash
cd "/path/to/experiment" ; ./PEBL-2.2-x86_64.AppImage experiment.pbl -s 001 --fullscreen ...
```

This **WILL work** because:
1. The AppImage is still accessible at its original path
2. Running the AppImage again will extract and run PEBL correctly
3. Each invocation is independent

### Potential Issues

#### Issue 1: AppImage Path Validity
**Problem:** If the AppImage was moved or deleted after launch, child processes won't be able to find it.

**Likelihood:** Low - users typically don't move running executables.

**Solution:** AppImages are designed for this use case. The file remains accessible while running.

#### Issue 2: Relative Path Resolution
**Problem:** If `argv[0]` is a relative path (e.g., `./PEBL.AppImage`), and the launcher changes the working directory before launching children, the relative path won't resolve.

**Analysis:** Looking at line 1965:
```pebl
callstring <- "cd " + gQuote+DirListToText(dirChain)+gQuote + sep  +
              gPEBLName + " "  +gQuote+filename+gQuote
```

The launcher DOES change directory (`cd` command), but uses `;` as separator, so each command runs in sequence. The `cd` changes the directory for the child process, not the launcher itself. The `gPEBLName` is resolved in the CURRENT shell before `cd` executes, so relative paths should work.

**However**, if `argv[0]` is relative, the shell command:
```bash
cd "/path/to/experiment" ; ./PEBL.AppImage test.pbl
```
will try to find `./PEBL.AppImage` in `/path/to/experiment`, which won't exist!

**This IS a problem!**

## Solutions

### Solution 1: Use Absolute Path for gExecutableName (RECOMMENDED)

Modify `src/apps/PEBL.cpp` to always use the absolute path for `gExecutableName`:

```cpp
#include <limits.h>  // For PATH_MAX
#include <stdlib.h>  // For realpath()

// Around line 664:
char resolved_path[PATH_MAX];
char* abs_path = realpath(argv[0], resolved_path);
if (abs_path != NULL) {
    myEval->gGlobalVariableMap.AddVariable("gExecutableName", abs_path);
} else {
    // Fallback to argv[0] if realpath fails
    myEval->gGlobalVariableMap.AddVariable("gExecutableName", argv[0]);
}
```

### Solution 2: Use APPIMAGE Environment Variable

Check for AppImage environment and use it:

```cpp
// Around line 664:
const char* appimage_path = getenv("APPIMAGE");
if (appimage_path != NULL) {
    // We're running from an AppImage - use its path
    myEval->gGlobalVariableMap.AddVariable("gExecutableName", appimage_path);
} else {
    // Not an AppImage - use argv[0]
    myEval->gGlobalVariableMap.AddVariable("gExecutableName", argv[0]);
}
```

### Solution 3: Modify Launcher to Use Absolute Path

In `bin/launcher.pbl`, resolve `gExecutableName` to absolute path before using it:

```pebl
## After line 256:
if(GetSystemType() == "LINUX" or GetSystemType() == "OSX")
{
    gPEBLName <- gExecutableName

    ## Convert to absolute path if relative
    if(SubString(gPEBLName, 1, 1) != "/")
    {
        gPEBLName <- GetWorkingDirectory() + "/" + gPEBLName
    }
}
```

## Recommendation

**Implement Solution 1 (absolute path in PEBL.cpp)** because:
1. Fixes the problem at the source
2. Works for all use cases (not just AppImage)
3. No PEBL script changes needed
4. Most robust solution

**Alternative:** Implement Solution 2 specifically for AppImage, which is cleaner and uses the intended environment variable.

**Both solutions can coexist:**
```cpp
// Priority 1: Use APPIMAGE environment variable if available
const char* appimage_path = getenv("APPIMAGE");
if (appimage_path != NULL) {
    myEval->gGlobalVariableMap.AddVariable("gExecutableName", appimage_path);
} else {
    // Priority 2: Resolve argv[0] to absolute path
    char resolved_path[PATH_MAX];
    char* abs_path = realpath(argv[0], resolved_path);
    if (abs_path != NULL) {
        myEval->gGlobalVariableMap.AddVariable("gExecutableName", abs_path);
    } else {
        // Fallback: use argv[0] as-is
        myEval->gGlobalVariableMap.AddVariable("gExecutableName", argv[0]);
    }
}
```

## Testing Needed

1. Build AppImage
2. Test launcher from different invocation methods:
   - `./PEBL-2.2-x86_64.AppImage` (relative path)
   - `/full/path/to/PEBL-2.2-x86_64.AppImage` (absolute path)
   - Add to PATH and run as `PEBL-2.2-x86_64.AppImage` (command name only)
3. Launch experiments from different directories
4. Test experiment chains
5. Test translation editor and data combiner utilities

## Conclusion

**The launcher WILL MOSTLY WORK with AppImages**, but there is a **CRITICAL BUG** when `argv[0]` is a relative path and the launcher changes directories before invoking child processes.

**Recommended fix:** Use `realpath()` or `APPIMAGE` environment variable to ensure `gExecutableName` is always an absolute path.
