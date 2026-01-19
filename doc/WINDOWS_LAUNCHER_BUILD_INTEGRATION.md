# Windows Launcher Build Integration Guide

**Created:** January 12, 2026
**Purpose:** Integrate pebl-launcher into Windows MinGW build system
**Target:** PEBL 2.3 Release

---

## Current Windows Build System Analysis

### Build Environment

**Platform:** Windows with MSYS64/MinGW64
**Location:** `/c/msys64/mingw64/bin/`

**Compilers:**
```makefile
CL = /c/msys64/mingw64/bin/gcc
CLXX = /c/msys64/mingw64/bin/g++
WINDRES = /c/msys64/mingw64/bin/windres
```

**Build Script:** `build-win.sh`
```bash
export PATH="/c/msys64/mingw64/bin:$PATH"
/c/msys64/usr/bin/make.exe -j 10 -f Makefile-win.mak main
```

### Current Windows Build for PEBL Interpreter

**Makefile:** `Makefile-win.mak`

**SDL2 Configuration:**
```makefile
BASE_SDL_FLAGS = -I/c/msys64/mingw64/include/SDL2 -I/c/msys64/mingw64/include -D_REENTRANT
```

**Link Libraries:**
```makefile
-L/c/msys64/mingw64/lib \
-lmingw32 -lSDL2main -lSDL2 -lpthread -lSDL2_image -lSDL2_net -lSDL2_ttf -lSDL2_gfx \
-lpng $(LINKOPTS)  # Includes -lcurl -lSDL2_mixer
```

**Resource Compilation:**
```makefile
resource.o: resource.rc installer/pebl2.ico
	$(WINDRES) resource.rc -O coff -o resource.o
```

**Object Directory:** `obj-native` (configurable via `OBJ_DIR`)

**Output:** `bin/pebl2.exe`

---

## Launcher Requirements Analysis

### Current Linux Launcher Build

**Makefile:** `src/apps/launcher/Makefile`

**Dependencies:**
- SDL2 (via `sdl2-config`)
- SDL2_image
- libzip
- ImGui (bundled in `libs/imgui/`)
- nlohmann::json (header-only, `libs/json.hpp`)

**Source Files:**
```
Launcher Sources:
  - PEBLLauncher.cpp
  - LauncherUI.cpp
  - LauncherConfig.cpp
  - ExperimentRunner.cpp
  - Study.cpp
  - Chain.cpp
  - WorkspaceManager.cpp
  - SnapshotManager.cpp
  - ZipExtractor.cpp
  - ../../utility/BinReloc.cpp

ImGui Sources:
  - libs/imgui/imgui.cpp
  - libs/imgui/imgui_draw.cpp
  - libs/imgui/imgui_widgets.cpp
  - libs/imgui/imgui_tables.cpp
  - libs/imgui/backends/imgui_impl_sdl2.cpp
  - libs/imgui/backends/imgui_impl_sdlrenderer2.cpp
  - libs/TextEditor.cpp
```

**Compiler Flags:**
```makefile
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O0
CXXFLAGS += -I../../../libs/imgui
CXXFLAGS += -I../../../libs/imgui/backends
CXXFLAGS += -I../../../libs
CXXFLAGS += -I../../utility
CXXFLAGS += -DENABLE_BINRELOC
CXXFLAGS += -DPEBL_VERSION=\"$(PEBL_VERSION)\"
CXXFLAGS += -DPREFIX=\"/usr/local\"
```

**Link Flags:**
```makefile
LIBS = $(SDL2_LIBS) -lzip
```

---

## Integration Strategy

### Approach 1: Add Target to Makefile-win.mak (RECOMMENDED)

**Benefits:**
- Centralized Windows build system
- Reuses existing Windows SDK paths and configuration
- Consistent with current `build-win.sh` workflow

**Implementation:**

1. Add launcher-specific variables to `Makefile-win.mak`
2. Define launcher source files
3. Create launcher object file rules
4. Add `pebl-launcher` target
5. Update `build-win.sh` to include launcher build

### Approach 2: Create Separate Makefile-launcher-win.mak

**Benefits:**
- Isolated launcher build configuration
- Easier to maintain separately

**Drawbacks:**
- Requires separate build script
- Duplicates Windows SDK configuration

**Recommendation:** Use Approach 1 for consistency

---

## Step-by-Step Integration (Approach 1)

### Step 1: Install Windows Dependencies

**Required Packages in MSYS64/MinGW64:**
```bash
pacman -S mingw-w64-x86_64-libzip
pacman -S mingw-w64-x86_64-zlib
pacman -S mingw-w64-x86_64-SDL2
pacman -S mingw-w64-x86_64-SDL2_image
```

**Verify Installation:**
```bash
ls /c/msys64/mingw64/lib/libzip.a
ls /c/msys64/mingw64/include/zip.h
```

### Step 2: Add Launcher Variables to Makefile-win.mak

**Insert after line 167 (TEST_DIR definition):**

```makefile
# Launcher directories (added for PEBL 2.3)
LAUNCHER_DIR = src/apps/launcher
IMGUI_DIR = libs/imgui
IMGUI_BACKEND_DIR = libs/imgui/backends
```

### Step 3: Define Launcher Source Files

**Insert after line 337 (PEBLMAIN_EXTRA definition):**

```makefile
# Launcher source files (added for PEBL 2.3)
LAUNCHER_SRCXX = \
	$(LAUNCHER_DIR)/PEBLLauncher.cpp \
	$(LAUNCHER_DIR)/LauncherUI.cpp \
	$(LAUNCHER_DIR)/LauncherConfig.cpp \
	$(LAUNCHER_DIR)/ExperimentRunner.cpp \
	$(LAUNCHER_DIR)/Study.cpp \
	$(LAUNCHER_DIR)/Chain.cpp \
	$(LAUNCHER_DIR)/WorkspaceManager.cpp \
	$(LAUNCHER_DIR)/SnapshotManager.cpp \
	$(LAUNCHER_DIR)/ZipExtractor.cpp \
	$(UTIL_DIR)/BinReloc.cpp

IMGUI_SRCXX = \
	$(IMGUI_DIR)/imgui.cpp \
	$(IMGUI_DIR)/imgui_draw.cpp \
	$(IMGUI_DIR)/imgui_widgets.cpp \
	$(IMGUI_DIR)/imgui_tables.cpp \
	$(IMGUI_BACKEND_DIR)/imgui_impl_sdl2.cpp \
	$(IMGUI_BACKEND_DIR)/imgui_impl_sdlrenderer2.cpp \
	libs/TextEditor.cpp

LAUNCHER_ALL_SRCXX = $(LAUNCHER_SRCXX) $(IMGUI_SRCXX)
LAUNCHER_OBJXX = $(patsubst %.cpp, %.o, $(LAUNCHER_ALL_SRCXX))
```

### Step 4: Add Launcher Object Directory

**Insert after line 354 (DIRS definition):**

```makefile
LAUNCHER_DIRS = \
	$(OBJ_DIR)/$(LAUNCHER_DIR) \
	$(OBJ_DIR)/$(IMGUI_DIR) \
	$(OBJ_DIR)/$(IMGUI_BACKEND_DIR) \
	$(OBJ_DIR)/libs
```

### Step 5: Create Launcher Build Target

**Insert after line 385 (main-real target), before line 387 (doc target):**

```makefile
# Launcher target for Windows (added for PEBL 2.3)
pebl-launcher-win:
	$(MAKE) -f Makefile-win.mak OBJ_DIR=obj-launcher-win CC=$(CL) CXX=$(CLXX) pebl-launcher-win-real

pebl-launcher-win-real: CC=$(CL)
pebl-launcher-win-real: CXX=$(CLXX)
pebl-launcher-win-real: CXXFLAGS = $(CXXFLAGS0) $(CXXFLAGS_WIN32) \
	-std=c++17 -Wall \
	-I$(IMGUI_DIR) \
	-I$(IMGUI_BACKEND_DIR) \
	-Ilibs \
	-I$(UTIL_DIR) \
	-DENABLE_BINRELOC \
	-DPEBL_VERSION=\"$(PEBL_VERSION)\" \
	-DPREFIX=\"/usr/local\"

pebl-launcher-win-real: $(DIRS) $(LAUNCHER_DIRS) $(LAUNCHER_OBJXX)
	@echo "Linking pebl-launcher for Windows..."
	$(CXX) $(CXXFLAGS) -Wall -mwindows -Wl,-rpath -Wl,LIBDIR $(DEBUGFLAGS) \
	-Wno-write-strings \
	$(SDL_FLAGS) -g	\
	-o $(BIN_DIR)/pebl-launcher \
	$(patsubst %.o, $(OBJ_DIR)/%.o, $(LAUNCHER_OBJXX)) \
	-L/c/msys64/mingw64/lib \
	-lmingw32 -lSDL2main -lSDL2 -lpthread -lSDL2_image -lzip -lz
	@echo "✓ pebl-launcher.exe built successfully"
```

### Step 6: Add Compilation Rules for Launcher Sources

**Insert after line 421 (%.c compilation rule):**

```makefile
# Launcher-specific compilation rules (added for PEBL 2.3)
$(LAUNCHER_DIR)/%.o: $(LAUNCHER_DIR)/%.cpp | $(LAUNCHER_DIRS)
	$(CXX) $(CXXFLAGS) -g -c $< -o $(OBJ_DIR)/$@ $(SDL_FLAGS)

$(IMGUI_DIR)/%.o: $(IMGUI_DIR)/%.cpp | $(LAUNCHER_DIRS)
	$(CXX) $(CXXFLAGS) -g -c $< -o $(OBJ_DIR)/$@ $(SDL_FLAGS)

$(IMGUI_BACKEND_DIR)/%.o: $(IMGUI_BACKEND_DIR)/%.cpp | $(LAUNCHER_DIRS)
	$(CXX) $(CXXFLAGS) -g -c $< -o $(OBJ_DIR)/$@ $(SDL_FLAGS)

libs/%.o: libs/%.cpp | $(LAUNCHER_DIRS)
	$(CXX) $(CXXFLAGS) -g -c $< -o $(OBJ_DIR)/$@ $(SDL_FLAGS)
```

### Step 7: Update DIRS Target

**Modify line 431-432 to include launcher directories:**

```makefile
$(DIRS) $(LAUNCHER_DIRS):
	@mkdir -p $@
```

### Step 8: Update Clean Target

**Modify line 446-449 to clean launcher objects:**

```makefile
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	rm -rf obj-native obj-em obj obj-launcher-win
	rm -f resource.o
	@echo "✓ Build artifacts cleaned"
```

### Step 9: Create Build Script for Launcher

**Create new file:** `build-win-launcher.sh`

```bash
#!/bin/bash
export PATH="/c/msys64/mingw64/bin:$PATH"
export TMPDIR="C:/Users/shanem/AppData/Local/Temp"
export TMP="C:/Users/shanem/AppData/Local/Temp"
export TEMP="C:/Users/shanem/AppData/Local/Temp"
cd /c/Users/shanem/Documents/dev/pebl
/c/msys64/usr/bin/make.exe -j 10 -f Makefile-win.mak pebl-launcher-win
```

**Make executable:**
```bash
chmod +x build-win-launcher.sh
```

### Step 10: Build Both Interpreter and Launcher

**Create unified build script:** `build-win-all.sh`

```bash
#!/bin/bash
export PATH="/c/msys64/mingw64/bin:$PATH"
export TMPDIR="C:/Users/shanem/AppData/Local/Temp"
export TMP="C:/Users/shanem/AppData/Local/Temp"
export TEMP="C:/Users/shanem/AppData/Local/Temp"
cd /c/Users/shanem/Documents/dev/pebl

echo "Building PEBL Interpreter..."
/c/msys64/usr/bin/make.exe -j 10 -f Makefile-win.mak main

echo ""
echo "Building PEBL Launcher..."
/c/msys64/usr/bin/make.exe -j 10 -f Makefile-win.mak pebl-launcher-win

echo ""
echo "✓ Build complete:"
ls -lh bin/pebl2.exe bin/pebl-launcher.exe
```

**Make executable:**
```bash
chmod +x build-win-all.sh
```

---

## Testing the Integration

### Test Build on Windows

**From MSYS64 terminal:**

```bash
cd /c/Users/shanem/Documents/dev/pebl

# Clean previous builds
make -f Makefile-win.mak clean

# Build launcher only
./build-win-launcher.sh

# Verify output
ls -lh bin/pebl-launcher.exe

# Test run
bin/pebl-launcher.exe
```

### Test Combined Build

```bash
# Clean
make -f Makefile-win.mak clean

# Build both
./build-win-all.sh

# Verify
ls -lh bin/pebl2.exe bin/pebl-launcher.exe
```

### Check Dependencies

**Verify DLL dependencies with `ldd` (from MSYS64):**

```bash
ldd bin/pebl-launcher.exe
```

**Expected dependencies:**
- SDL2.dll
- SDL2_image.dll
- libzip.dll
- zlib1.dll
- libgcc_s_seh-1.dll
- libstdc++-6.dll
- libwinpthread-1.dll

**These DLLs must be:**
1. Included in the installer
2. Placed in the same directory as pebl-launcher.exe
3. OR added to Windows PATH

---

## Installer Integration (NSIS)

### Required DLLs to Package

**Collect from `/c/msys64/mingw64/bin/`:**

```
SDL2.dll
SDL2_image.dll
SDL2_ttf.dll
SDL2_gfx.dll
SDL2_mixer.dll (if USE_MIXER=1)
SDL2_net.dll
libzip.dll
zlib1.dll
libpng16-16.dll
libfreetype-6.dll
libbz2-1.dll
libiconv-2.dll
libintl-8.dll
libgcc_s_seh-1.dll
libstdc++-6.dll
libwinpthread-1.dll
libcurl-4.dll
```

### NSIS Script Updates

**Installer file:** `installer/pebl.nsi` (or similar)

**Add launcher executable:**

```nsis
; Install PEBL Launcher
File "bin\pebl-launcher.exe"

; Create desktop shortcut for launcher (primary interface)
CreateShortCut "$DESKTOP\PEBL Launcher.lnk" "$INSTDIR\pebl-launcher.exe" "" "$INSTDIR\pebl-launcher.exe" 0

; Create desktop shortcut for interpreter (advanced users)
CreateShortCut "$DESKTOP\PEBL Interpreter.lnk" "$INSTDIR\pebl2.exe" "" "$INSTDIR\pebl2.exe" 0

; Create start menu entries
CreateDirectory "$SMPROGRAMS\PEBL2"
CreateShortCut "$SMPROGRAMS\PEBL2\PEBL Launcher.lnk" "$INSTDIR\pebl-launcher.exe"
CreateShortCut "$SMPROGRAMS\PEBL2\PEBL Interpreter.lnk" "$INSTDIR\pebl2.exe"
CreateShortCut "$SMPROGRAMS\PEBL2\Uninstall.lnk" "$INSTDIR\uninstall.exe"
```

**Add DLLs to installer:**

```nsis
; SDL2 and dependencies
File "C:\msys64\mingw64\bin\SDL2.dll"
File "C:\msys64\mingw64\bin\SDL2_image.dll"
File "C:\msys64\mingw64\bin\SDL2_ttf.dll"
File "C:\msys64\mingw64\bin\SDL2_gfx.dll"
File "C:\msys64\mingw64\bin\SDL2_mixer.dll"
File "C:\msys64\mingw64\bin\SDL2_net.dll"

; libzip and zlib
File "C:\msys64\mingw64\bin\libzip.dll"
File "C:\msys64\mingw64\bin\zlib1.dll"

; PNG and image support
File "C:\msys64\mingw64\bin\libpng16-16.dll"
File "C:\msys64\mingw64\bin\libfreetype-6.dll"

; Runtime libraries
File "C:\msys64\mingw64\bin\libgcc_s_seh-1.dll"
File "C:\msys64\mingw64\bin\libstdc++-6.dll"
File "C:\msys64\mingw64\bin\libwinpthread-1.dll"

; Networking
File "C:\msys64\mingw64\bin\libcurl-4.dll"

; Other dependencies (add as needed based on ldd output)
File "C:\msys64\mingw64\bin\libbz2-1.dll"
File "C:\msys64\mingw64\bin\libiconv-2.dll"
File "C:\msys64\mingw64\bin\libintl-8.dll"
```

**Uninstaller updates:**

```nsis
Section "Uninstall"
  ; Remove executables
  Delete "$INSTDIR\pebl2.exe"
  Delete "$INSTDIR\pebl-launcher.exe"

  ; Remove DLLs
  Delete "$INSTDIR\SDL2.dll"
  Delete "$INSTDIR\SDL2_image.dll"
  Delete "$INSTDIR\libzip.dll"
  ; ... (all other DLLs)

  ; Remove shortcuts
  Delete "$DESKTOP\PEBL Launcher.lnk"
  Delete "$DESKTOP\PEBL Interpreter.lnk"
  Delete "$SMPROGRAMS\PEBL2\*.*"
  RMDir "$SMPROGRAMS\PEBL2"

  ; ... (rest of uninstall)
SectionEnd
```

---

## Troubleshooting

### Common Issues

**1. Missing libzip.dll**

```
Error: The program can't start because libzip.dll is missing
```

**Solution:**
```bash
# Install libzip in MSYS64
pacman -S mingw-w64-x86_64-libzip

# Verify
ls /c/msys64/mingw64/lib/libzip.a
ls /c/msys64/mingw64/bin/libzip.dll
```

**2. SDL2 Linking Errors**

```
undefined reference to SDL_CreateRenderer
```

**Solution:**
- Ensure `-lmingw32 -lSDL2main` comes BEFORE `-lSDL2`
- Check SDL2 installation: `pacman -S mingw-w64-x86_64-SDL2`

**3. ImGui Compilation Errors**

```
imgui.cpp: error: 'SDL_Window' was not declared
```

**Solution:**
- Verify SDL2 headers: `-I/c/msys64/mingw64/include/SDL2`
- Check CXXFLAGS includes `$(SDL_FLAGS)`

**4. ZipExtractor Missing zlib**

```
undefined reference to inflate/deflate
```

**Solution:**
- Add `-lz` to link flags
- Install zlib: `pacman -S mingw-w64-x86_64-zlib`

**5. BinReloc.cpp Windows Compatibility**

```
error: ENABLE_BINRELOC requires Linux/Unix
```

**Solution:**
- BinReloc.cpp should detect Windows and disable itself
- Check for `#ifdef PEBL_WIN32` guards in BinReloc.cpp

**6. Path Separator Issues**

```
Error: Cannot find C:\Users\...\pebl-exp.2.3/study-info.json
```

**Solution:**
- Ensure WorkspaceManager.cpp uses platform-agnostic path joining
- Replace hardcoded `/` with `std::filesystem::path` or conditional separators

---

## Verification Checklist

### Pre-Build Checks
- [ ] MSYS64/MinGW64 installed
- [ ] SDL2 packages installed
- [ ] libzip installed
- [ ] zlib installed
- [ ] ImGui sources present in `libs/imgui/`
- [ ] nlohmann::json.hpp present in `libs/`

### Build Verification
- [ ] `make -f Makefile-win.mak pebl-launcher-win` succeeds
- [ ] `bin/pebl-launcher.exe` created
- [ ] File size reasonable (~2-5 MB)
- [ ] No missing DLL errors when running
- [ ] Launcher UI appears on Windows

### Functional Testing
- [ ] Launcher starts without errors
- [ ] Can create new study
- [ ] Can add tests to study
- [ ] Can create chain
- [ ] Can run simple test
- [ ] Can export snapshot
- [ ] Can import snapshot (ZIP)
- [ ] Quick Launch works
- [ ] Upload configuration saves

### Installer Testing
- [ ] All DLLs packaged
- [ ] Both shortcuts created (launcher + interpreter)
- [ ] Launcher runs from desktop shortcut
- [ ] Workspace auto-created on first run
- [ ] Uninstaller removes all files

---

## Timeline Estimate

**Week 1: Build System Integration**
- Day 1-2: Modify Makefile-win.mak (Steps 2-8)
- Day 3: Test build on Windows VM
- Day 4-5: Debug linking issues, missing DLLs

**Week 2: Installer Integration**
- Day 1-2: Update NSIS script
- Day 3: Test installer on clean Windows 10/11
- Day 4-5: Fix first-run initialization, path issues

**Total: 2 weeks for complete Windows launcher integration**

---

## Next Steps

1. **Immediate (Today):**
   - Apply Steps 2-8 to Makefile-win.mak
   - Create build scripts (build-win-launcher.sh, build-win-all.sh)

2. **Short-term (This Week):**
   - Test build on Windows machine
   - Resolve dependency issues
   - Verify launcher functionality

3. **Medium-term (Next Week):**
   - Update NSIS installer script
   - Test full installation workflow
   - Create Windows testing VM

4. **Before 2.3 Release:**
   - Test on Windows 10 and Windows 11
   - Verify all 10+ migrated upload-battery tests work
   - Update release documentation

---

## References

**Main Makefile:** `Makefile` (Linux build reference)
**Windows Makefile:** `Makefile-win.mak`
**Launcher Makefile:** `src/apps/launcher/Makefile`
**Release Plan:** `doc/PEBL_2.3_RELEASE_PRIORITIES.md`
**NSIS Documentation:** https://nsis.sourceforge.io/Docs/

---

**Document Status:** Draft - Ready for Implementation
**Last Updated:** January 12, 2026
**Author:** Claude Code
**Review Required:** Shane Mueller
