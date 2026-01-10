# Running PEBL from Command Line with AppImage

## Problem

AppImages have a single entry point defined in the desktop file. If the launcher is the default entry point, how do you run `pebl2` directly from the command line?

---

## Solution Options

### Option 1: AppImage Exec (RECOMMENDED)

Most AppImages built with linuxdeploy support the `--appimage-exec` flag:

```bash
# Run a PEBL script directly
./PEBL-2.3-x86_64.AppImage --appimage-exec pebl2 script.pbl -s 123

# Run pebl2 with any arguments
./PEBL-2.3-x86_64.AppImage --appimage-exec pebl2 --help

# Run the validator
./PEBL-2.3-x86_64.AppImage --appimage-exec pebl-validator script.pbl

# Run the launcher (default behavior)
./PEBL-2.3-x86_64.AppImage
# or explicitly:
./PEBL-2.3-x86_64.AppImage --appimage-exec pebl-launcher
```

**Pros:**
- Clean, doesn't require extraction
- Works from any directory
- Official AppImage feature

**Cons:**
- Verbose command
- Not all users know about --appimage-exec

---

### Option 2: Shell Wrapper Script (BEST USER EXPERIENCE)

Create a wrapper script that detects what to run:

**Implementation in build-appimage.sh:**

```bash
# Create wrapper script that replaces the symlink
cat > bin/pebl2 << 'EOF'
#!/bin/bash
# PEBL AppImage Wrapper
# Detects if running as CLI or GUI

APPIMAGE="$(dirname "$0")/PEBL-2.3-x86_64.AppImage"

# If no arguments or first arg is not a .pbl file, launch GUI
if [ $# -eq 0 ]; then
    exec "$APPIMAGE"
fi

# Check if first argument is a .pbl file or a pebl2 flag
first_arg="$1"
if [[ "$first_arg" == *.pbl ]] || [[ "$first_arg" == -* ]]; then
    # Run pebl2 directly
    exec "$APPIMAGE" --appimage-exec pebl2 "$@"
else
    # Launch GUI
    exec "$APPIMAGE"
fi
EOF

chmod +x bin/pebl2
```

**Usage:**
```bash
# Run GUI launcher (no args)
./bin/pebl2

# Run PEBL script (detects .pbl)
./bin/pebl2 script.pbl -s 123

# Run with flags (detects -)
./bin/pebl2 --version
./bin/pebl2 --help
```

**Pros:**
- Intuitive - users just run `pebl2`
- Backward compatible with old behavior
- Clean command line

**Cons:**
- Requires wrapper script in bin/
- Only works from installation directory

---

### Option 3: Extract and Add to PATH (PERSISTENT)

For development or frequent CLI use, extract the AppImage once:

```bash
# Extract AppImage
cd ~/opt  # or wherever you want
./PEBL-2.3-x86_64.AppImage --appimage-extract

# Add to PATH
echo 'export PATH="$HOME/opt/squashfs-root/usr/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc

# Now use directly
pebl2 script.pbl -s 123
pebl-launcher
pebl-validator script.pbl
```

**Pros:**
- Fast startup (no FUSE mounting)
- Clean commands
- All executables accessible

**Cons:**
- Takes up disk space (~100MB extracted)
- Not portable anymore
- Updates require re-extraction

---

### Option 4: Multiple Symlinks with Different Entry Points (ADVANCED)

AppImages can check their own filename to decide what to run.

**Implementation - Add to AppDir/usr/bin/pebl-entrypoint:**

```bash
#!/bin/bash
# Detect how we were called

APPIMAGE_NAME="$(basename "$ARGV0")"

case "$APPIMAGE_NAME" in
    pebl2|pebl2-cli|*pebl2*.AppImage)
        # Called as pebl2 - run CLI
        exec /usr/pebl2/bin/pebl2 "$@"
        ;;
    pebl-launcher|PEBL*)
        # Called as launcher - run GUI
        exec /usr/pebl2/bin/pebl-launcher "$@"
        ;;
    pebl-validator|*validator*)
        # Called as validator
        exec /usr/pebl2/bin/pebl-validator "$@"
        ;;
    *)
        # Default: run launcher
        exec /usr/pebl2/bin/pebl-launcher "$@"
        ;;
esac
```

**Update desktop file:**
```ini
Exec=pebl-entrypoint
```

**Usage:**
```bash
# Create symlinks with different names
ln -s PEBL-2.3-x86_64.AppImage pebl2
ln -s PEBL-2.3-x86_64.AppImage pebl-launcher
ln -s PEBL-2.3-x86_64.AppImage pebl-validator

# Run based on symlink name
./pebl2 script.pbl              # Runs CLI
./pebl-launcher                 # Runs GUI
./pebl-validator script.pbl     # Runs validator
```

**Pros:**
- One AppImage, multiple commands
- Elegant solution
- No extraction needed

**Cons:**
- Requires custom entry point script
- More complex build setup

---

### Option 5: Environment Variable Override

Add environment variable check to launcher:

**Update LauncherUI.cpp main():**
```cpp
int main(int argc, char* argv[])
{
    // Check if we should run pebl2 instead of launcher
    const char* cliMode = getenv("PEBL_CLI");
    if (cliMode && strcmp(cliMode, "1") == 0) {
        // Find pebl2 and exec it
        ExperimentRunner runner;
        std::string pebl2 = runner.GetPEBLExecutablePath();

        std::vector<char*> args;
        args.push_back(const_cast<char*>(pebl2.c_str()));
        for (int i = 1; i < argc; i++) {
            args.push_back(argv[i]);
        }
        args.push_back(nullptr);

        execvp(pebl2.c_str(), args.data());
        return 1;  // exec failed
    }

    // Normal launcher startup...
}
```

**Usage:**
```bash
# Run launcher (default)
./PEBL-2.3-x86_64.AppImage

# Run CLI
PEBL_CLI=1 ./PEBL-2.3-x86_64.AppImage script.pbl -s 123

# Create alias
alias pebl2='PEBL_CLI=1 ./PEBL-2.3-x86_64.AppImage'
pebl2 script.pbl
```

**Pros:**
- No extraction needed
- No additional scripts
- Clean for aliases

**Cons:**
- Requires environment variable
- Less intuitive

---

## Recommended Implementation

**For PEBL 2.3 Release:**

**1. Primary: Use --appimage-exec (Document in README)**

```bash
# Add to README and help text
./PEBL-2.3-x86_64.AppImage --appimage-exec pebl2 script.pbl
```

**2. Convenience: Add wrapper script to bin/**

Create `bin/pebl2` wrapper that detects CLI vs GUI usage:

```bash
#!/bin/bash
APPIMAGE="$(cd "$(dirname "$0")" && pwd)/PEBL-2.3-x86_64.AppImage"

if [ $# -eq 0 ]; then
    exec "$APPIMAGE"
elif [[ "$1" == *.pbl ]] || [[ "$1" == -* ]]; then
    exec "$APPIMAGE" --appimage-exec pebl2 "$@"
else
    exec "$APPIMAGE"
fi
```

**3. Document extraction for power users:**

```bash
# Extract once for persistent CLI access
./PEBL-2.3-x86_64.AppImage --appimage-extract
export PATH="$PWD/squashfs-root/usr/bin:$PATH"
pebl2 script.pbl  # Now works directly
```

---

## Update build-appimage.sh

Add wrapper script creation after AppImage is built:

```bash
# After: mv "${APPIMAGE_NAME}" "bin/${APPIMAGE_NAME}"

echo "Creating command-line wrapper..."
cat > "bin/pebl2" << 'WRAPPER_EOF'
#!/bin/bash
# PEBL Command Line Wrapper
# Usage: pebl2 [script.pbl] [options]

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
APPIMAGE="$SCRIPT_DIR/PEBL-2.3-x86_64.AppImage"

if [ ! -f "$APPIMAGE" ]; then
    echo "Error: PEBL AppImage not found at $APPIMAGE"
    exit 1
fi

# No arguments: launch GUI
if [ $# -eq 0 ]; then
    exec "$APPIMAGE"
    exit $?
fi

# First argument is .pbl file or starts with -: run CLI
if [[ "$1" == *.pbl ]] || [[ "$1" == -* ]]; then
    exec "$APPIMAGE" --appimage-exec pebl2 "$@"
    exit $?
fi

# Otherwise: launch GUI
exec "$APPIMAGE"
WRAPPER_EOF

chmod +x "bin/pebl2"

echo "Creating validator wrapper..."
cat > "bin/pebl-validator" << 'VALIDATOR_EOF'
#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
APPIMAGE="$SCRIPT_DIR/PEBL-2.3-x86_64.AppImage"
exec "$APPIMAGE" --appimage-exec pebl-validator "$@"
VALIDATOR_EOF

chmod +x "bin/pebl-validator"
```

---

## Documentation for Users

**In README:**

### Command Line Usage

**GUI Launcher:**
```bash
./bin/PEBL-2.3-x86_64.AppImage
```

**Run PEBL Script:**
```bash
# Option 1: Use wrapper script (recommended)
./bin/pebl2 my_experiment.pbl -s 123

# Option 2: Use --appimage-exec
./bin/PEBL-2.3-x86_64.AppImage --appimage-exec pebl2 my_experiment.pbl -s 123

# Option 3: Extract for persistent CLI access
./bin/PEBL-2.3-x86_64.AppImage --appimage-extract
export PATH="$PWD/squashfs-root/usr/bin:$PATH"
pebl2 my_experiment.pbl -s 123
```

**Run Validator:**
```bash
# Use wrapper script
./bin/pebl-validator script.pbl

# Or use --appimage-exec
./bin/PEBL-2.3-x86_64.AppImage --appimage-exec pebl-validator script.pbl
```

---

## Summary

**Recommended approach:**
1. ✅ Default: Double-click AppImage runs GUI launcher
2. ✅ CLI: Create `bin/pebl2` wrapper that detects .pbl files
3. ✅ Validator: Create `bin/pebl-validator` wrapper
4. ✅ Document --appimage-exec for advanced users
5. ✅ Document extraction method for power users

**Implementation effort:** ~30 minutes
**User experience:** Excellent - "just works" for both GUI and CLI
