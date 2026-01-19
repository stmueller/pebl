# AppImage Integration for pebl-launcher and pebl-validator

## Investigation: Adding C++ Launcher to PEBL AppImage

This document investigates the requirements and implementation for bundling pebl-launcher and pebl-validator into the PEBL Linux AppImage.

---

## Current AppImage Structure

### Existing PEBL AppImage (from build-appimage.sh)

**AppDir Layout:**
```
AppDir/
├── usr/
│   ├── bin/
│   │   └── pebl2 -> ../pebl2/bin/pebl2  (symlink)
│   ├── pebl2/
│   │   ├── bin/
│   │   │   ├── pebl2               (main executable)
│   │   │   └── launcher.pbl        (old PEBL launcher)
│   │   ├── battery/                (100+ test scripts)
│   │   ├── demo/
│   │   ├── tutorials/
│   │   ├── pebl-lib/               (PEBL standard library)
│   │   ├── media/
│   │   └── doc/
│   └── share/
│       ├── applications/PEBL2.desktop
│       └── icons/hicolor/256x256/apps/pebl2.png
```

**Entry Point:**
- Desktop file: `Exec=pebl2` (launches old launcher.pbl)
- Symlink: `/usr/bin/pebl2` → `/usr/pebl2/bin/pebl2`

### Existing Validator AppImage (from build-validator-appimage.sh)

**AppDir Layout:**
```
ValidatorAppDir/
├── usr/
│   ├── bin/
│   │   └── pebl-validator          (validator executable)
│   └── pebl-lib/                   (PEBL library for validator)
```

---

## Concern 1: Running launcher vs pebl executable vs validator

### Current Behavior
- **AppImage launches:** `pebl2` → runs old `launcher.pbl` script
- **launcher.pbl then spawns:** `pebl2` to run selected experiments

### Proposed Solution

#### Option A: Make launcher the primary entry point (RECOMMENDED)
```
AppDir/
├── usr/
│   ├── bin/
│   │   ├── pebl-launcher -> ../pebl2/bin/pebl-launcher  (symlink, NEW DEFAULT)
│   │   ├── pebl2 -> ../pebl2/bin/pebl2                  (symlink)
│   │   └── pebl-validator -> ../pebl2/bin/pebl-validator (symlink)
│   └── pebl2/
│       └── bin/
│           ├── pebl-launcher        (C++ launcher, NEW)
│           ├── pebl2                (PEBL executable)
│           ├── pebl-validator       (validator)
│           └── launcher.pbl         (old launcher, kept for compatibility)
```

**Desktop File Change:**
```ini
[Desktop Entry]
Name=PEBL 2
Exec=pebl-launcher
Icon=pebl2
```

**Benefits:**
- Modern UI immediately on launch
- All three executables available in AppImage
- Old launcher.pbl still accessible via Quick Launch tab
- Backward compatible (users can run `pebl2` directly if needed)

#### How ExperimentRunner Finds pebl2

**Current Implementation (ExperimentRunner.cpp:49-83):**
```cpp
std::string ExperimentRunner::GetPEBLExecutablePath() const
{
    // Linux: Read /proc/self/exe to find launcher location
    char exePath[1024];
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);

    if (len != -1) {
        exePath[len] = '\0';
        std::string path(exePath);
        size_t lastSlash = path.find_last_of('/');
        if (lastSlash != std::string::npos) {
            // Replace "pebl-launcher" with "pebl2"
            path = path.substr(0, lastSlash + 1) + "pebl2";
            return path;
        }
    }

    // Fallback: assume pebl2 is in PATH
    return "pebl2";
}
```

**Why This Works in AppImage:**
1. AppImage extracts to `/tmp/.mount_PEBL-XXX/`
2. Launcher runs at: `/tmp/.mount_PEBL-XXX/usr/bin/pebl-launcher`
3. /proc/self/exe resolves the symlink: `/tmp/.mount_PEBL-XXX/usr/pebl2/bin/pebl-launcher`
4. GetPEBLExecutablePath() returns: `/tmp/.mount_PEBL-XXX/usr/pebl2/bin/pebl2`
5. ✅ pebl2 executable found in same directory

**No changes needed** - the current implementation is AppImage-compatible.

---

## Concern 2: File Copying from /battery to ~/Documents/pebl-exp.2.3

### Current Behavior (WorkspaceManager.cpp:83-110)

**Portable Mode Detection:**
```cpp
bool WorkspaceManager::IsPortableMode() const {
    // Check for PORTABLE marker file
    if (DirectoryExists("./PORTABLE") || DirectoryExists("../PORTABLE")) {
        return true;
    }

    // Check for PEBL_PORTABLE=1 environment variable
    const char* portableEnv = getenv("PEBL_PORTABLE");
    if (portableEnv && strcmp(portableEnv, "1") == 0) {
        return true;
    }

    return false;
}
```

**Workspace Initialization:**
```cpp
bool WorkspaceManager::CopyResources(const std::string& installationPath) {
    if (installationPath.empty() || IsPortableMode()) {
        return false;  // Skip copying in portable mode
    }

    // Copy documentation
    std::string docSource = installationPath + "/doc";
    std::string docDest = mWorkspacePath + "/doc";
    if (DirectoryExists(docSource)) {
        CopyDirectory(docSource, docDest, false);
    }

    // Copy demos
    std::string demoSource = installationPath + "/demo";
    std::string demoDest = mWorkspacePath + "/demo";
    if (DirectoryExists(demoSource)) {
        CopyDirectory(demoSource, demoDest, false);
    }

    // Copy tutorials
    // ...
}
```

### Problem
The launcher needs to find the installation path to copy from. Currently it's passed as a parameter, but where does it come from?

### Solution: Use BinReloc for Installation Path Discovery

The main PEBL executable already uses BinReloc (Binary Relocation) to find its installation directory. We should use the same approach.

**Current PEBL Usage (PEBL.cpp):**
```cpp
#include "BinReloc.h"

// In main():
BrInitError error;
if (br_init(&error) == 0) {
    // BinReloc failed, use fallbacks
}

// Get installation prefix
char* prefix = br_find_prefix(PREFIX);
std::string installPath = std::string(prefix) + "/pebl2";
free(prefix);
```

**Proposed Launcher Change:**

Add to LauncherUI.cpp first-run dialog:
```cpp
void LauncherUI::ShowFirstRunDialog()
{
    // ... existing code ...

    if (ImGui::Button("Continue", ImVec2(buttonWidth, 40))) {
        if (!mWorkspace->Initialize()) {
            printf("ERROR: Failed to initialize workspace\n");
        } else {
            // Use BinReloc to find installation path
            std::string installPath;

            #ifdef ENABLE_BINRELOC
            char* prefix = br_find_prefix(PREFIX);
            if (prefix) {
                installPath = std::string(prefix) + "/pebl2";
                free(prefix);
            }
            #endif

            // Fallback for portable/development mode
            if (installPath.empty()) {
                char exePath[1024];
                ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
                if (len != -1) {
                    exePath[len] = '\0';
                    std::string path(exePath);
                    size_t lastSlash = path.find_last_of('/');
                    if (lastSlash != std::string::npos) {
                        installPath = path.substr(0, lastSlash);
                        // Go up to find pebl2 directory
                        if (installPath.find("/bin") != std::string::npos) {
                            lastSlash = installPath.find_last_of('/');
                            installPath = installPath.substr(0, lastSlash);
                        }
                    }
                }
            }

            printf("Installation path: %s\n", installPath.c_str());
            mWorkspace->CopyResources(installPath);
        }
        mShowFirstRunDialog = false;
    }
}
```

**Why This Works in AppImage:**
1. AppImage mounts at `/tmp/.mount_PEBL-XXX/`
2. BinReloc resolves prefix: `/tmp/.mount_PEBL-XXX/usr`
3. Installation path: `/tmp/.mount_PEBL-XXX/usr/pebl2`
4. Battery directory: `/tmp/.mount_PEBL-XXX/usr/pebl2/battery/`
5. CopyDirectory copies battery → `~/Documents/pebl-exp.2.3/battery/`

**Copying Battery Tests:**

Current behavior: Only copies doc/, demo/, tutorials/
Proposed: Also copy battery/ on first run

**Update WorkspaceManager::CopyResources():**
```cpp
bool WorkspaceManager::CopyResources(const std::string& installationPath) {
    // ... existing doc/demo/tutorial copying ...

    // Copy battery tests (excluding data directories)
    std::string batterySource = installationPath + "/battery";
    std::string batteryDest = mWorkspacePath + "/battery";
    if (DirectoryExists(batterySource)) {
        CopyDirectory(batterySource, batteryDest, true);  // excludeData=true
    }

    return true;
}
```

---

## Concern 3: How Launcher Accesses PEBL Directories

### Quick Launch Tab

**Current Implementation (LauncherUI.cpp:62-90):**
```cpp
// Set Quick Launch to start in pebl-exp.<version>
const char* home = getenv("HOME");
if (home) {
    std::string peblExpPath = std::string(home) + "/Documents/pebl-exp." + PEBL_VERSION;
    struct stat info;
    if (stat(peblExpPath.c_str(), &info) == 0 && (info.st_mode & S_IFDIR)) {
        mQuickLaunchDirectory = peblExpPath;
        // ... scan for .pbl files ...
    } else {
        // Fall back to config directory
        mQuickLaunchDirectory = config->GetExperimentDirectory();
    }
}
```

**Why This Works:**
- Quick Launch looks in `~/Documents/pebl-exp.2.3/` (user's home directory)
- This is independent of AppImage mount point
- ✅ No changes needed

### Study System

**Current Implementation (WorkspaceManager.cpp:22):**
```cpp
mWorkspacePath = GetDocumentsPath() + "/pebl-exp.2.3";
```

**Study directories:**
- `~/Documents/pebl-exp.2.3/my_studies/`
- `~/Documents/pebl-exp.2.3/snapshots/`

**Why This Works:**
- All study data stored in user's home directory
- Independent of AppImage mount point
- ✅ No changes needed

### Translation Editor & Data Combiner

**Current Implementation (LauncherUI.cpp:2329-2409):**
```cpp
void LauncherUI::LaunchTranslationEditor() {
    // Find pebl2 executable
    char exePath[1024];
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    // ... resolves to pebl2 in same directory ...

    std::string command = peblExec + " translatetest.pbl " + scriptPath + " &";
    system(command.c_str());
}
```

**Why This Works:**
- Uses same technique as ExperimentRunner to find pebl2
- translatetest.pbl is at `/tmp/.mount_PEBL-XXX/usr/pebl2/pebl-lib/translatetest.pbl`
- PEBL executable can find its own pebl-lib via BinReloc
- ✅ No changes needed

---

## Implementation Plan

### Phase 1: Build Infrastructure

**1. Update src/apps/launcher/Makefile**

Already done - launcher builds with:
```bash
make pebl-launcher
```

**2. Update main Makefile install target**

Add launcher installation:
```makefile
install:
    # ... existing pebl2 installation ...

    # Install launcher (if built)
    if [ -f "bin/pebl-launcher" ]; then \
        cp bin/pebl-launcher $(DESTDIR)$(PREFIX)/$(PEBLNAME)/bin/; \
    fi
```

**3. Update build-appimage.sh**

```bash
# After building pebl2
make pebl-launcher

# Install both to AppDir
make install DESTDIR="${APPDIR}" PREFIX=/usr

# Verify both executables
if [ ! -f "${APPDIR}/usr/pebl2/bin/pebl2" ]; then
    echo "ERROR: pebl2 not found"
    exit 1
fi
if [ ! -f "${APPDIR}/usr/pebl2/bin/pebl-launcher" ]; then
    echo "ERROR: pebl-launcher not found"
    exit 1
fi

# Create symlinks for all executables
mkdir -p "${APPDIR}/usr/bin"
ln -sf ../pebl2/bin/pebl2 "${APPDIR}/usr/bin/pebl2"
ln -sf ../pebl2/bin/pebl-launcher "${APPDIR}/usr/bin/pebl-launcher"

# Update desktop file to launch pebl-launcher by default
cat > "${APPDIR}/usr/share/applications/PEBL2.desktop" << EOF
[Desktop Entry]
Version=1.0
Name=PEBL 2
Comment=Psychology Experiment Building Language
Exec=pebl-launcher
Terminal=false
Type=Application
Categories=Science;
Icon=pebl2
StartupNotify=true
EOF
```

### Phase 2: Code Updates

**1. Add BinReloc support to launcher**

Launcher already links with BinReloc (src/utility/BinReloc.cpp).

Update LauncherUI.cpp first-run dialog to use br_find_prefix() as shown above.

**2. Update WorkspaceManager to copy battery/**

Add battery copying to CopyResources() as shown above.

**3. Test portable mode**

Create PORTABLE marker file for AppImage portable mode:
```bash
# In AppImage root
touch usr/pebl2/PORTABLE
```

This prevents workspace initialization and runs entirely from AppImage mount point.

### Phase 3: Validator Integration (Optional)

The validator is currently a separate AppImage. To include it in main AppImage:

**Update build-appimage.sh:**
```bash
# Build validator too
make validator

# Install to AppDir
cp bin/pebl-validator "${APPDIR}/usr/pebl2/bin/"

# Create symlink
ln -sf ../pebl2/bin/pebl-validator "${APPDIR}/usr/bin/pebl-validator"
```

**Validator becomes:**
- Accessible via command line: `pebl-validator script.pbl`
- Launchable from launcher Tools menu (future enhancement)

---

## Testing Checklist

### Build Tests
- [ ] `make pebl-launcher` succeeds
- [ ] `make install` installs launcher to correct location
- [ ] `./build-appimage.sh 2.3` creates AppImage with launcher
- [ ] AppImage contains all three executables (pebl2, pebl-launcher, pebl-validator)

### Runtime Tests - Fresh Install
- [ ] Double-click AppImage → launcher opens
- [ ] First-run dialog appears
- [ ] Click "Continue" → workspace created at ~/Documents/pebl-exp.2.3/
- [ ] Battery tests copied to workspace
- [ ] Demo/doc/tutorials copied to workspace

### Runtime Tests - Quick Launch
- [ ] Quick Launch tab opens in ~/Documents/pebl-exp.2.3/
- [ ] Can navigate directories
- [ ] Can select and run .pbl files
- [ ] PEBL executable launches correctly
- [ ] stdout/stderr captured and displayed

### Runtime Tests - Study System
- [ ] Can create new study
- [ ] Can add tests from battery
- [ ] Can create parameter variants
- [ ] Can create chains
- [ ] Can run chains with subject code
- [ ] Data saved to correct test directories

### Runtime Tests - Utilities
- [ ] Translation editor launches from test
- [ ] Data combiner launches from test data directory
- [ ] Both utilities run pebl2 scripts correctly

### Runtime Tests - Portable Mode
- [ ] Create PORTABLE marker file in AppImage
- [ ] Extract AppImage: `./PEBL-2.3-x86_64.AppImage --appimage-extract`
- [ ] Create PORTABLE file: `touch squashfs-root/usr/pebl2/PORTABLE`
- [ ] Repackage or run from squashfs-root/
- [ ] Launcher runs without creating workspace
- [ ] Uses AppImage directories directly

---

## Summary

### Required Changes

**Minimal (Core Functionality):**
1. ✅ Update Makefile install target to include pebl-launcher
2. ✅ Update build-appimage.sh to build and bundle launcher
3. ✅ Update desktop file Exec= to launch pebl-launcher
4. ✅ Add BinReloc integration to first-run dialog
5. ✅ Add battery/ copying to WorkspaceManager::CopyResources()

**Optional (Enhanced):**
6. Include pebl-validator in main AppImage
7. Add "Portable Mode" marker file option
8. Add validator launch from Tools menu

### No Changes Needed

✅ **ExperimentRunner** - Already finds pebl2 via /proc/self/exe
✅ **WorkspaceManager** - Uses ~/Documents (independent of AppImage)
✅ **Quick Launch** - Uses ~/Documents (independent of AppImage)
✅ **Translation/Data Tools** - Find pebl2 same way as ExperimentRunner

### Risk Assessment

**Low Risk:**
- Most code already AppImage-compatible via relative path resolution
- BinReloc is proven technology used by main PEBL
- Portable mode provides fallback for edge cases
- Old launcher.pbl still available as fallback

**Testing Priority:**
1. First-run workspace initialization (high impact)
2. Quick Launch file access (frequently used)
3. Study chain execution (complex workflow)
4. Translation/data tools (lower frequency)

---

## Next Steps

1. Implement Makefile install target changes
2. Implement build-appimage.sh updates
3. Add BinReloc to launcher first-run dialog
4. Add battery copying to WorkspaceManager
5. Test AppImage build and run
6. Document AppImage-specific features in README
7. Create AppImage CI/CD build (optional)

---

**Document Status:** Investigation Complete
**Ready for Implementation:** Yes
**Estimated Effort:** 2-4 hours
**Risk Level:** Low
