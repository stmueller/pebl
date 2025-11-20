# Compiling PEBL on Windows

This guide covers multiple approaches to building PEBL on Windows, from modern automated builds to traditional IDE-based compilation.

## Prerequisites

PEBL requires the following dependencies on Windows:
- **SDL2** (2.0.x) - Core graphics and window management
- **SDL2_image** - Image loading (PNG, JPEG)
- **SDL2_ttf** - TrueType font rendering
- **SDL2_net** - Network functionality
- **SDL2_gfx** - Graphics primitives
- **SDL2_mixer** - Audio mixing (optional, for enhanced audio support)
- **C++ compiler** - GCC (MinGW) or MSVC

---

## Option 1: MSYS2/MinGW64 (Recommended - Modern)

MSYS2 provides a modern Unix-like environment for Windows with excellent package management.

### Installation

1. **Download and install MSYS2:**
   - Get the installer from https://www.msys2.org/
   - Install to default location: `C:\msys64`

2. **Update MSYS2:**
   ```bash
   # Open MSYS2 MINGW64 terminal (not MSYS2 MSYS!)
   pacman -Syu
   # Close and reopen terminal if prompted
   pacman -Su
   ```

3. **Install build tools and dependencies:**
   ```bash
   # Core build tools
   pacman -S mingw-w64-x86_64-gcc
   pacman -S mingw-w64-x86_64-make
   pacman -S make
   pacman -S bison
   pacman -S flex

   # SDL2 libraries
   pacman -S mingw-w64-x86_64-SDL2
   pacman -S mingw-w64-x86_64-SDL2_image
   pacman -S mingw-w64-x86_64-SDL2_ttf
   pacman -S mingw-w64-x86_64-SDL2_net
   pacman -S mingw-w64-x86_64-SDL2_gfx
   pacman -S mingw-w64-x86_64-SDL2_mixer

   # Git (if not already installed)
   pacman -S git
   ```

### Important: Environment Variable Setup

Before building, you need to ensure the compiler can write temporary files:

1. **Check Windows environment variables:**
   - Open "Edit the system environment variables" from Windows search
   - Click "Environment Variables"
   - Check if **System variables** (not User variables) has TMP and TEMP set to `C:\Windows\Temp`
   - If yes, **delete these system-level TMP and TEMP variables**
   - Your User-level variables should point to `C:\Users\YourUsername\AppData\Local\Temp` (keep these)

2. **Why this matters:**
   - The compiler tries to write temporary files during compilation
   - If system variables point to `C:\Windows\Temp`, you'll get "Permission denied" errors
   - User-level variables pointing to your AppData folder are writable without admin privileges

### Building PEBL

1. **Navigate to PEBL directory:**
   ```bash
   cd /c/Users/YourUsername/Documents/pebl  # Adjust path as needed
   ```

2. **Important: Use MSYS2 MinGW64 terminal**
   - Open "MSYS2 MinGW64" from Start menu (NOT "MSYS2 MSYS")
   - This ensures the correct compiler paths and environment

3. **Build native Windows executable:**
   ```bash
   make -j 10 -f Makefile-win.mak main
   ```

4. **Copy required DLLs to bin directory:**
   ```bash
   ldd bin/pebl2.exe | grep mingw | awk '{print $3}' | xargs -I {} cp {} bin/
   ```

5. **Test the build:**
   ```bash
   ./bin/pebl2.exe test.pbl
   ```

### Creating a Distributable Package

1. **Collect DLL dependencies:**
   ```bash
   # Copy required DLLs to bin/ directory
   ldd bin/pebl2.exe | grep mingw | awk '{print $3}' | xargs -I {} cp {} bin/
   ```

2. **Create installer with NSIS (optional):**
   ```bash
   pacman -S mingw-w64-x86_64-nsis
   # Create NSIS script (see below)
   makensis installer.nsi
   ```

### Makefile Adjustments for MSYS2

The main Makefile should work with minimal changes. If you encounter issues, you may need to:

```makefile
# Add to Makefile for Windows-specific settings
ifeq ($(OS),Windows_NT)
    LDFLAGS += -static-libgcc -static-libstdc++
    PEBL_EXECUTABLE = bin/pebl2.exe
else
    PEBL_EXECUTABLE = bin/pebl2
endif
```

---

## Option 2: Code::Blocks (Traditional IDE)

PEBL includes Code::Blocks project files in `codeblocks/PEBL/`.

### Setup

1. **Install Code::Blocks with MinGW:**
   - Download from http://www.codeblocks.org/downloads
   - Choose "codeblocks-XX.XX-mingw-setup.exe" (includes compiler)

2. **Install SDL2 libraries manually:**
   - Download SDL2 development libraries from https://www.libsdl.org/
   - Download SDL2_image, SDL2_ttf, SDL2_net, SDL2_gfx
   - Extract to a common directory (e.g., `C:\SDL2`)
   - Update project settings to point to include and lib directories

3. **Open and build:**
   - Open `codeblocks/PEBL/PEBL.cbp`
   - Build → Build (F9)
   - Executable will be in `bin/pebl2.exe`

### Library Configuration

In Code::Blocks project settings (Project → Build Options):

**Compiler settings → Search directories → Compiler:**
- `C:\SDL2\include\SDL2`

**Linker settings → Search directories → Linker:**
- `C:\SDL2\lib`

**Linker settings → Link libraries:**
- `mingw32`
- `SDL2main`
- `SDL2`
- `SDL2_image`
- `SDL2_ttf`
- `SDL2_net`
- `SDL2_gfx`

---
## Option 4: Visual Studio (Professional/Advanced)

For developers familiar with Visual Studio, you can create a new project:

### Using CMake (Recommended if using Visual Studio)

Create a `CMakeLists.txt` in the project root:

```cmake
cmake_minimum_required(VERSION 3.10)
project(PEBL)

set(CMAKE_CXX_STANDARD 11)

# Find SDL2
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_net REQUIRED)

# Add source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(pebl2 ${SOURCES})

target_link_libraries(pebl2
    SDL2::SDL2
    SDL2::SDL2_image
    SDL2::SDL2_ttf
    SDL2::SDL2_net
)
```

Then use CMake GUI or command line:
```cmd
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Install SDL2 via vcpkg

```cmd
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install
.\vcpkg install sdl2:x64-windows
.\vcpkg install sdl2-image:x64-windows
.\vcpkg install sdl2-ttf:x64-windows
.\vcpkg install sdl2-net:x64-windows
.\vcpkg install sdl2-gfx:x64-windows
```

---

## Option 5: Cross-Compilation from Linux

If you have access to a Linux machine, you can cross-compile for Windows:

```bash
# Install MinGW cross-compiler
sudo apt-get install mingw-w64

# Install Windows SDL2 libraries
# Download Windows SDL2 development libraries and extract

# Cross-compile
make CC=x86_64-w64-mingw32-gcc \
     CXX=x86_64-w64-mingw32-g++ \
     LDFLAGS="-L/path/to/SDL2/lib" \
     CXXFLAGS="-I/path/to/SDL2/include"
```

---

## Creating an Installer

### Using NSIS (Nullsoft Scriptable Install System)

1. **Install NSIS:**
   ```bash
   # In MSYS2:
   pacman -S mingw-w64-x86_64-nsis

   # Or download from: https://nsis.sourceforge.io/
   ```

2. **Create installer script** (`installer.nsi`):

```nsis
; PEBL Installer Script
!define APP_NAME "PEBL"
!define APP_VERSION "2.2"
!define PUBLISHER "Shane Mueller"
!define WEB_SITE "http://pebl.sourceforge.net"

Name "${APP_NAME} ${APP_VERSION}"
OutFile "PEBL-${APP_VERSION}-Setup.exe"
InstallDir "$PROGRAMFILES\PEBL"

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"

  ; Copy executable and DLLs
  File "bin\pebl2.exe"
  File "bin\*.dll"

  ; Copy battery tests
  SetOutPath "$INSTDIR\battery"
  File /r "battery\*.*"

  ; Copy libraries
  SetOutPath "$INSTDIR\pebl-lib"
  File /r "pebl-lib\*.*"

  ; Copy media files
  SetOutPath "$INSTDIR\media"
  File /r "media\*.*"

  ; Create shortcuts
  CreateDirectory "$SMPROGRAMS\PEBL"
  CreateShortCut "$SMPROGRAMS\PEBL\PEBL.lnk" "$INSTDIR\bin\pebl2.exe"
  CreateShortCut "$DESKTOP\PEBL.lnk" "$INSTDIR\bin\pebl2.exe"
SectionEnd

Section "Uninstall"
  Delete "$INSTDIR\*.*"
  Delete "$SMPROGRAMS\PEBL\*.*"
  Delete "$DESKTOP\PEBL.lnk"
  RMDir /r "$INSTDIR"
  RMDir "$SMPROGRAMS\PEBL"
SectionEnd
```

3. **Build installer:**
   ```bash
   makensis installer.nsi
   ```

---

## Creating a Portable Package

For a portable version (no installer):

1. **Create package directory:**
   ```bash
   mkdir PEBL-Portable
   cd PEBL-Portable
   ```

2. **Copy files:**
   ```bash
   cp ../bin/pebl2.exe .
   cp ../bin/*.dll .
   cp -r ../battery .
   cp -r ../pebl-lib .
   cp -r ../media .
   cp ../README.md .
   cp ../LICENSE .
   ```

3. **Create launcher script** (`PEBL.bat`):
   ```batch
   @echo off
   start pebl2.exe bin\launcher.pbl
   ```

4. **Package as ZIP:**
   ```bash
   zip -r PEBL-2.1-Portable-Windows.zip PEBL-Portable/
   ```

---

## Troubleshooting

### Common Build Errors

**Error: SDL.h not found**
- Verify SDL2 is installed: `pacman -Q mingw-w64-x86_64-SDL2`
- Check include paths in Makefile or IDE settings

**Error: undefined reference to SDL_main**
- Make sure you're linking against `SDL2main` before `SDL2`
- Link order matters: `-lmingw32 -lSDL2main -lSDL2`

**Error: Missing DLL when running**
- Copy DLLs to same directory as pebl2.exe: `ldd bin/pebl2.exe | grep mingw | awk '{print $3}' | xargs -I {} cp {} bin/`
- Or add MinGW bin directory to PATH: `C:\msys64\mingw64\bin`

**Error: Cannot create temporary file in C:\Windows\: Permission denied**
- This occurs during compilation when system TMP/TEMP variables point to `C:\Windows\Temp`
- Solution: Delete the **system-level** TMP and TEMP environment variables (keep user-level ones)
- After deleting, close all terminals and open a fresh MSYS2 MinGW64 terminal
- See "Environment Variable Setup" section above

**Error: undefined reference to `mmap`**
- The `mman.c` file should be included in `Makefile-win.mak` under `PUTILITIES_SRC`
- Verify it's listed: `grep mman.c Makefile-win.mak`

**Error: undefined reference to `WinMain`**
- Ensure linking order is correct: `-lmingw32 -lSDL2main -lSDL2` (in that order)
- `SDL2main` library provides the WinMain entry point wrapper
- Do NOT compile `src/apps/SDL_win32_main.c` (it's for SDL 1.x, not SDL2)

**Parser compilation fails**
- Install bison and flex: `pacman -S bison flex`
- Run `make parse` to regenerate parser files

**Compiler fails silently with no error messages**
- Usually indicates the compiler backend (cc1plus.exe) can't run
- Check if MinGW64 toolchain is fully installed: `pacman -S mingw-w64-x86_64-toolchain`
- Verify required DLLs exist: `pacman -S mingw-w64-x86_64-isl mingw-w64-x86_64-mpc mingw-w64-x86_64-mpfr`
- Ensure `/c/msys64/mingw64/bin` is in your PATH

### Runtime Issues

**Fonts not loading**
- Ensure `media/fonts/` directory is present
- Check that DejaVu fonts exist in media/fonts/

**Tests not found**
- Ensure `battery/` and `pebl-lib/` directories are in the same directory as pebl2.exe
- Or set PEBL_LANGUAGE_DIR environment variable

**Network upload fails**
- Check firewall settings
- Ensure SDL2_net.dll is present

---

## GitHub Actions for Automated Builds

For automated Windows builds on every commit, create `.github/workflows/build-windows.yml`:

```yaml
name: Build Windows

on: [push, pull_request]

jobs:
  build-windows:
    runs-on: windows-latest
    defaults:
      run:
        shell: msys2 {0}

    steps:
    - uses: actions/checkout@v3

    - uses: msys2/setup-msys2@v2
      with:
        msystem: MINGW64
        update: true
        install: >-
          mingw-w64-x86_64-gcc
          mingw-w64-x86_64-SDL2
          mingw-w64-x86_64-SDL2_image
          mingw-w64-x86_64-SDL2_ttf
          mingw-w64-x86_64-SDL2_net
          mingw-w64-x86_64-SDL2_gfx
          mingw-w64-x86_64-SDL2_mixer
          make
          bison
          flex

    - name: Build PEBL
      run: make main

    - name: Package Release
      run: |
        mkdir PEBL-Windows
        cp bin/pebl2.exe PEBL-Windows/
        ldd bin/pebl2.exe | grep mingw | awk '{print $3}' | xargs -I {} cp {} PEBL-Windows/
        cp -r battery pebl-lib media PEBL-Windows/

    - name: Upload Artifact
      uses: actions/upload-artifact@v3
      with:
        name: PEBL-Windows
        path: PEBL-Windows/
```

This will automatically build Windows executables and make them available for download from the GitHub Actions tab.

---

## Additional Resources

- **PEBL Documentation**: See `doc/` directory
- **SDL2 Documentation**: https://wiki.libsdl.org/
- **MSYS2 Documentation**: https://www.msys2.org/docs/
- **MinGW-w64**: https://www.mingw-w64.org/

## Getting Help

- **Email**: pebl.exp@gmail.com
- **GitHub Issues**: https://github.com/stmueller/pebl/issues
- **PEBL Mailing List**: pebl-list@lists.sourceforge.net

---

**Note:** This document will be updated as build processes evolve. If you encounter issues or have improvements to suggest, please submit an issue or pull request on GitHub.
