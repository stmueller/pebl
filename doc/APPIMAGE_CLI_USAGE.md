# Running PEBL from Command Line with AppImage

## Overview

The PEBL AppImage supports **multiple executables** in a single package using symlink-based routing. The AppImage detects which executable to run based on how it was invoked (via `$ARGV0`).

## Quick Start

The AppImage build creates these symlinks automatically in `bin/`:

```bash
# GUI Launcher (new C++ version)
./bin/pebl2-appimage
# or double-click PEBL-2.3-x86_64.AppImage

# Old launcher.pbl (PEBL script-based launcher)
./bin/launcher

# CLI (run PEBL scripts)
./bin/pebl2 script.pbl -s 123

# Validator (validate PEBL scripts)
./bin/pebl-validator script.pbl
```

All symlinks point to the same AppImage file. The AppImage's `AppRun` script checks `$ARGV0` to determine which executable to run.

---

## How It Works

### AppRun Script

The AppImage contains a custom `AppRun` script that detects the symlink name:

```bash
#!/bin/bash
# Get the name we were called as
CALLED_AS="$(basename "$ARGV0")"

case "$CALLED_AS" in
    pebl2|pebl2-cli)
        # Run CLI
        exec "${APPDIR}/usr/pebl2/bin/pebl2" "$@"
        ;;
    pebl-validator|*validator*)
        # Run validator
        exec "${APPDIR}/usr/pebl2/bin/pebl-validator" "$@"
        ;;
    launcher|pebl-launcher-old)
        # Run old launcher.pbl
        exec "${APPDIR}/usr/pebl2/bin/pebl2" "${APPDIR}/usr/pebl2/bin/launcher.pbl" "$@"
        ;;
    *)
        # Default: run GUI launcher
        exec "${APPDIR}/usr/pebl2/bin/pebl-launcher" "$@"
        ;;
esac
```

### Environment Variables

- **`$ARGV0`** - Contains the path used to invoke the AppImage (including symlink name)
- **`$APPDIR`** - Contains the path to the mounted AppImage filesystem

When you run `./bin/pebl2`, `$ARGV0` contains `./bin/pebl2`, and `basename "$ARGV0"` returns `pebl2`, which matches the case statement.

---

## Usage Examples

### GUI Launcher (New C++ Version)

```bash
# Double-click the AppImage in file manager
# OR from command line:
./bin/pebl2-appimage
# OR directly:
./bin/PEBL-2.3-x86_64.AppImage
```

This launches the new ImGui-based launcher with:
- Study management
- Chain editor
- Parameter editor
- Quick Launch for battery tests

### Old Launcher (launcher.pbl)

```bash
./bin/launcher
```

This runs the PEBL script-based launcher (`pebl2 launcher.pbl`), which provides the traditional file browser interface.

### CLI - Running PEBL Scripts

```bash
# Run a script with subject number
./bin/pebl2 myexperiment.pbl -s 001

# Run a script with parameters
./bin/pebl2 battery/stroop/stroop.pbl -s 001 --pfile params.json

# Run from any directory (use absolute path)
/path/to/pebl/bin/pebl2 /path/to/script.pbl
```

### Validator - Check Script Syntax

```bash
# Validate a single script
./bin/pebl-validator myexperiment.pbl

# Validate all scripts in a directory
find battery/stroop -name "*.pbl" -exec ./bin/pebl-validator {} \;
```

---

## Creating Custom Symlinks

You can create your own symlinks anywhere:

```bash
# Create symlinks in your home directory
cd ~
ln -s /path/to/PEBL-2.3-x86_64.AppImage pebl2
ln -s /path/to/PEBL-2.3-x86_64.AppImage pebl-launcher

# Now use from anywhere
~/pebl2 script.pbl -s 123
~/pebl-launcher
```

Or add to PATH:

```bash
# Create symlinks in a directory on your PATH
mkdir -p ~/.local/bin
cd ~/.local/bin
ln -s /path/to/PEBL-2.3-x86_64.AppImage pebl2
ln -s /path/to/PEBL-2.3-x86_64.AppImage pebl-validator
ln -s /path/to/PEBL-2.3-x86_64.AppImage launcher

# Ensure ~/.local/bin is in PATH (add to ~/.bashrc if not)
export PATH="$HOME/.local/bin:$PATH"

# Now use from anywhere without ./
pebl2 script.pbl -s 123
pebl-validator script.pbl
launcher
```

---

## Alternative: Extract AppImage

For users who need faster startup or persistent access without symlinks:

```bash
# Extract the AppImage
./PEBL-2.3-x86_64.AppImage --appimage-extract

# This creates squashfs-root/ directory
# Add to PATH
export PATH="$PWD/squashfs-root/usr/bin:$PATH"

# Now use executables directly (no FUSE mounting)
pebl2 script.pbl -s 123
pebl-launcher
pebl-validator script.pbl
```

**Pros:**
- Faster startup (no FUSE mounting overhead)
- Direct access to all executables
- No symlinks needed

**Cons:**
- Takes ~100MB disk space
- Not portable anymore
- Updates require re-extraction

---

## Technical Background

### Why Not --appimage-exec?

The `--appimage-exec` flag is not implemented in the current AppImage type2-runtime. We discovered this during development:

```bash
$ ./PEBL-2.3-x86_64.AppImage --appimage-exec pebl2 --version
--appimage-exec is not yet implemented in version https://github.com/AppImage/type2-runtime/...
```

### Symlink Approach (ARGV0)

The symlink approach is documented in official AppImage resources:

- [AppImage Environment Variables](https://docs.appimage.org/packaging-guide/environment-variables.html) - Documents `$ARGV0`
- [Bundling Command Line Tools](https://github.com/AppImage/AppImageKit/wiki/Bundling-command-line-tools) - Example implementations
- [GitHub Issue #419](https://github.com/AppImage/AppImageKit/issues/419) - Discussion of argv[0] detection

This pattern is similar to how tools like `xz` change behavior when invoked as `xzcat`, or how `busybox` provides multiple commands through symlinks.

---

## For Developers

### Building with Symlink Support

The `build-appimage.sh` script automatically:

1. Creates custom `AppRun` with symlink detection
2. Bundles all three executables (pebl2, pebl-launcher, pebl-validator)
3. Creates symlinks in `bin/` directory

```bash
# Build AppImage
make appimage

# Or rebuild with existing binaries
make appimage-fast
```

### AppRun Implementation

See `build-appimage.sh` lines 87-117 for the complete AppRun script that implements symlink routing.

### Adding New Entry Points

To add a new symlink-based entry point:

1. Add a new case in AppRun (in build-appimage.sh)
2. Create the symlink after AppImage build
3. Update documentation

Example:

```bash
# In AppRun case statement:
    my-tool|pebl-tool)
        exec "${APPDIR}/usr/pebl2/bin/my-tool" "$@"
        ;;

# After AppImage build:
ln -sf PEBL-2.3-x86_64.AppImage my-tool
```

---

## Summary

**PEBL AppImage provides 4 access modes:**

| Mode | Command | Description |
|------|---------|-------------|
| GUI (new) | `./bin/pebl2-appimage` | ImGui-based launcher with study management |
| GUI (old) | `./bin/launcher` | Traditional launcher.pbl file browser |
| CLI | `./bin/pebl2 script.pbl` | Run PEBL scripts from command line |
| Validator | `./bin/pebl-validator script.pbl` | Validate script syntax |

**All modes use the same AppImage file** - the difference is just the symlink name used to invoke it.

**Recommended workflow:**
1. GUI users: Double-click the AppImage or use `./bin/pebl2-appimage`
2. CLI users: Use `./bin/pebl2` symlink for running scripts
3. Developers: Use `./bin/pebl-validator` for syntax checking
4. Power users: Extract AppImage and add to PATH for direct access
