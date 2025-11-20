# Installing PEBL on Linux

PEBL 2.2 uses a **self-contained, relocatable installation** structure. This means PEBL and all its resources can be installed anywhere on your system - no hardcoded paths to `/usr/local`. The executable automatically finds resources relative to its location.

## Quick Installation

### System Install (Recommended)

Install PEBL to `/usr/local/pebl2/` with a convenient symlink in your PATH:

```bash
# Build PEBL
make -j 10 main

# Install (creates /usr/local/pebl2/ and symlink at /usr/local/bin/pebl2)
sudo make install
```

After installation, you can run PEBL from anywhere:

```bash
pebl2 path/to/script.pbl
```

### Alternative Installation Locations

PEBL can be installed to any directory by setting the `PREFIX` variable:

**Install to /opt:**
```bash
sudo make install PREFIX=/opt
# Creates: /opt/pebl2/
# Symlink: /opt/bin/pebl2
```

**User installation (no sudo required):**
```bash
make install PREFIX=$HOME/.local DESTDIR=""
# Creates: ~/.local/pebl2/
# Symlink: ~/.local/bin/pebl2
# Add ~/.local/bin to your PATH if needed
```

**Custom location:**
```bash
make install PREFIX=/path/to/install DESTDIR=""
```

### Development / Portable Mode

You don't need to install PEBL to use it! Run directly from the source directory:

```bash
# Build
make -j 10 main

# Run from bin/ directory
./bin/pebl2 test.pbl
./bin/pebl2 battery/stroop/stroop.pbl
./bin/pebl2 bin/launcher.pbl
```

This is perfect for:
- Development and testing
- Portable installations (USB drive, Dropbox sync)
- Running multiple PEBL versions side-by-side

## Installation Structure

PEBL uses a self-contained directory structure:

```
<install-location>/pebl2/
├── bin/
│   └── pebl2              # Executable
├── media/
│   ├── fonts/             # TrueType fonts
│   ├── images/            # Stock images
│   ├── sounds/            # Audio files
│   └── text/              # Word lists
├── pebl-lib/              # PEBL standard library
│   ├── Design.pbl
│   ├── Utility.pbl
│   ├── Graphics.pbl
│   └── ...
├── battery/               # Pre-built psychological tests
│   ├── stroop/
│   ├── flanker/
│   └── ...
├── demo/                  # Example experiments
├── tutorials/             # Tutorial scripts
└── doc/                   # Documentation
```

## How Path Resolution Works

PEBL uses **BinReloc** (Binary Relocation) to automatically find resources:

1. PEBL determines its executable location (e.g., `/usr/local/pebl2/bin/pebl2`)
2. Goes up one directory to find the base (`/usr/local/pebl2/`)
3. Searches for resources relative to base:
   - `../media/fonts/`
   - `../pebl-lib/`
   - etc.

This works regardless of where PEBL is installed. No environment variables or configuration files needed!

## Desktop Integration

The `make install` command automatically creates a desktop entry at:
```
/usr/local/share/applications/PEBL2.desktop
```

This adds PEBL to your application menu with the proper icon and executable path.

## AppImage (Portable Single-File)

PEBL can also be built as an AppImage for maximum portability:

```bash
make appimage
```

This creates a single executable file that includes everything:
```bash
./bin/PEBL-2.2-x86_64.AppImage
```

No installation needed - just download and run! Perfect for:
- Users without admin privileges
- Running on university/lab computers
- Quick deployment to multiple machines

## Uninstalling

```bash
sudo make uninstall
```

Or manually:
```bash
sudo rm -rf /usr/local/pebl2
sudo rm /usr/local/bin/pebl2
sudo rm /usr/local/share/applications/PEBL2.desktop
```

## First Run

On first run, PEBL creates a user workspace at:
```
~/Documents/pebl-exp.2.2/
```

This directory contains:
- Copies of the battery tests (for customization)
- Experiment data files
- Configuration files
- Tutorial files

This workspace is separate from the PEBL installation and is safe to modify.

## Dependencies

PEBL requires these system libraries:

- **SDL2** - Core graphics and input
- **SDL2_image** - Image loading (PNG, JPEG)
- **SDL2_ttf** - TrueType font rendering
- **SDL2_net** - Network support
- **SDL2_gfx** - Graphics primitives
- **SDL2_mixer** (optional) - Audio mixing
- **libcurl** - HTTP support

### Ubuntu/Debian:
```bash
sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev \
                     libsdl2-net-dev libsdl2-gfx-dev libsdl2-mixer-dev \
                     libcurl4-openssl-dev
```

### Fedora/RHEL:
```bash
sudo dnf install SDL2-devel SDL2_image-devel SDL2_ttf-devel \
                 SDL2_net-devel SDL2_gfx-devel SDL2_mixer-devel \
                 libcurl-devel
```

### Arch Linux:
```bash
sudo pacman -S sdl2 sdl2_image sdl2_ttf sdl2_net sdl2_gfx sdl2_mixer curl
```

## Build from Source

```bash
# Clone the repository
git clone https://github.com/your-repo/pebl.git
cd pebl

# Install dependencies (Ubuntu example)
sudo apt-get install build-essential clang libsdl2-dev libsdl2-image-dev \
                     libsdl2-ttf-dev libsdl2-net-dev libsdl2-gfx-dev \
                     libsdl2-mixer-dev libcurl4-openssl-dev

# Build (parallel compilation with 10 jobs)
make -j 10 main

# Test
./bin/pebl2 test-simple.pbl

# Install
sudo make install
```

## Troubleshooting

**"Cannot find media files"**
- PEBL should automatically find resources relative to the executable
- Check that `media/`, `pebl-lib/` exist in the parent directory of the `bin/` folder
- Verify BinReloc is working by checking the console output on startup

**"Failed to open font"**
- Ensure `media/fonts/` contains `.ttf` files
- PEBL looks for fonts in `<base>/media/fonts/`

**"Library files not found"**
- PEBL automatically loads standard library files from `<base>/pebl-lib/`
- Check that `Design.pbl`, `Utility.pbl`, etc. exist in that directory

**Permission denied when installing**
- Use `sudo make install` for system-wide installation
- Or use user installation: `make install PREFIX=$HOME/.local DESTDIR=""`

## Migration from PEBL 0.14 or Earlier

PEBL 2.x uses a different installation structure than older versions:

**Old (0.14):**
```
/usr/local/bin/pebl
/usr/local/share/pebl/  (resources)
```

**New (2.x):**
```
/usr/local/pebl2/bin/pebl2
/usr/local/pebl2/  (all resources here)
/usr/local/bin/pebl2  (symlink for convenience)
```

Both versions can coexist! The old PEBL 0.14 won't interfere with PEBL 2.x.

## Advanced Configuration

### Custom Installation Prefix

You can install to any location and PEBL will work correctly:

```bash
# Example: Install to a specific project directory
make install PREFIX=/opt/research/experiments DESTDIR=""
```

Then add to your PATH:
```bash
export PATH="/opt/research/experiments/pebl2/bin:$PATH"
```

### Multiple Versions

Run multiple PEBL versions side-by-side:

```bash
# PEBL 2.2 in /usr/local/pebl2/
# PEBL 2.1 in /usr/local/pebl2.1/
# Development version in ~/pebl-dev/
```

Each version is completely self-contained and independent.

## See Also

- [README.md](../README.md) - Project overview
- [doc/DEPENDENCIES.md](DEPENDENCIES.md) - Detailed dependency information
- [doc/COMPILING_ON_WINDOWS.md](COMPILING_ON_WINDOWS.md) - Windows build instructions
- [doc/APPIMAGE_README.md](APPIMAGE_README.md) - AppImage build instructions
