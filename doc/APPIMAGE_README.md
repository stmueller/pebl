# PEBL AppImage Build Guide

This directory contains scripts to build PEBL as a portable AppImage for Linux.

## Quick Build

```bash
make appimage
```

This will:
1. Download linuxdeploy (if needed)
2. Build PEBL
3. Install to AppDir
4. Bundle all dependencies
5. Create `PEBL-2.1-x86_64.AppImage`

## Manual Build

```bash
./build-appimage.sh 2.1
```

## Testing the AppImage

After building, test it:

```bash
# Run the launcher (GUI)
./PEBL-2.1-x86_64.AppImage

# Or use the convenience symlink
./pebl2

# Run a specific script
./pebl2 test.pbl
./pebl2 battery/stroop/stroop.pbl

# Show help
./pebl2 --help
```

## What Gets Bundled

The AppImage includes:
- PEBL binary (`pebl2`)
- All battery test scripts (~100+ experiments)
- Media files (fonts, images, sounds)
- PEBL libraries (Design.pbl, Utility.pbl, etc.)
- launcher.pbl (GUI launcher)
- Documentation
- All SDL2 dependencies

**Total size:** ~80-120 MB (self-contained)

## How It Works

1. **BinReloc Path Detection:** PEBL uses BinReloc to detect where it's running from
2. **AppImage Mount:** AppImage mounts to `/tmp/.mount_PEBL-XXXXXX/`
3. **Launcher Detection:** When run without arguments, finds `launcher.pbl` automatically
4. **User Directory:** Launcher copies files to `~/Documents/pebl-exp.2.1/`
5. **Normal Operation:** Everything works identically to native installation

## User Instructions

For end users downloading the AppImage:

```bash
# Download from GitHub releases
wget https://github.com/muellerlab/pebl/releases/download/v2.1/PEBL-2.1-x86_64.AppImage

# Make executable
chmod +x PEBL-2.1-x86_64.AppImage

# Run (double-click or command-line)
./PEBL-2.1-x86_64.AppImage
```

### Optional: Install to ~/bin

```bash
mkdir -p ~/bin
mv PEBL-2.1-x86_64.AppImage ~/bin/pebl2
chmod +x ~/bin/pebl2

# If ~/bin is in PATH, you can now run:
pebl2
pebl2 myexperiment.pbl
```

### Optional: Desktop Integration

The AppImage includes a desktop file. You can:

1. **Manual Integration:**
   ```bash
   mkdir -p ~/.local/share/applications
   cat > ~/.local/share/applications/pebl2.desktop <<EOF
   [Desktop Entry]
   Name=PEBL 2
   Exec=/path/to/PEBL-2.1-x86_64.AppImage
   Icon=pebl2
   Type=Application
   Categories=Science;Education;
   EOF
   ```

2. **Using appimaged (automatic):**
   ```bash
   # Install appimaged daemon
   sudo apt install appimaged

   # Move AppImage to ~/Applications
   mkdir -p ~/Applications
   mv PEBL-2.1-x86_64.AppImage ~/Applications/

   # appimaged auto-detects and integrates
   ```

## Distribution

### GitHub Releases

```bash
# Create a release
gh release create v2.1 \
  --title "PEBL 2.1" \
  --notes "Release notes here"

# Upload AppImage
gh release upload v2.1 PEBL-2.1-x86_64.AppImage
```

### Direct Download

Upload to your web server:
```bash
scp PEBL-2.1-x86_64.AppImage user@server:/var/www/pebl/downloads/
```

Then link from your website:
```html
<a href="downloads/PEBL-2.1-x86_64.AppImage">
  Download PEBL 2.1 for Linux (AppImage)
</a>
```

## Advantages Over System Installation

| Feature | AppImage | System Install |
|---------|----------|----------------|
| Installation | None (just download) | Requires `make install` |
| Dependencies | Bundled | Must install separately |
| Admin rights | Not needed | Requires sudo |
| Multiple versions | Easy (different files) | Conflicts |
| Portability | Run from USB | Fixed location |
| Updates | Download new version | Recompile |
| Uninstall | Delete file | `make uninstall` |
| Disk space | ~100MB per version | ~50MB (shared libs) |

## Troubleshooting

### AppImage Won't Run

**Problem:** `./PEBL-2.1-x86_64.AppImage: Permission denied`

**Solution:**
```bash
chmod +x PEBL-2.1-x86_64.AppImage
```

### Missing Libraries

**Problem:** `error while loading shared libraries: libSDL2.so.2`

**This shouldn't happen** - libraries are bundled. If it does:

```bash
# Extract and check
./PEBL-2.1-x86_64.AppImage --appimage-extract
ldd squashfs-root/usr/bin/pebl2
```

Contact the PEBL developers with the output.

### FUSE Not Available

**Problem:** `fusermount: failed to open /etc/fuse.conf`

**Solution:** Use extract-and-run mode:
```bash
./PEBL-2.1-x86_64.AppImage --appimage-extract-and-run
```

Or install FUSE:
```bash
sudo apt install fuse libfuse2
```

### Launcher Not Appearing

**Problem:** AppImage runs but launcher doesn't appear

**Check:**
1. Run with a script to verify it works: `./pebl2 --help`
2. Check if launcher.pbl is bundled:
   ```bash
   ./PEBL-2.1-x86_64.AppImage --appimage-extract
   ls squashfs-root/usr/share/pebl2/pebl-lib/launcher.pbl
   ```

### Cannot Write to ~/Documents

**Problem:** Launcher fails to create `~/Documents/pebl-exp.2.1/`

**Solution:**
```bash
mkdir -p ~/Documents
chmod 755 ~/Documents
```

## Technical Details

### Build Process

1. **Compile PEBL:** `make main-real`
2. **Install to AppDir:** `make install DESTDIR=AppDir PREFIX=/usr`
3. **Bundle with linuxdeploy:** Copies SDL2 and dependencies
4. **Create AppImage:** SquashFS filesystem + AppImage runtime

### Directory Structure Inside AppImage

```
usr/
├── bin/
│   └── pebl2
├── lib/
│   ├── libSDL2-2.0.so.0
│   ├── libSDL2_image-2.0.so.0
│   ├── libSDL2_ttf-2.0.so.0
│   ├── libSDL2_mixer-2.0.so.0
│   ├── libSDL2_net-2.0.so.0
│   ├── libSDL2_gfx-1.0.so.0
│   ├── libcurl.so.4
│   └── ... (other dependencies)
└── share/
    ├── applications/
    │   └── PEBL2.desktop
    ├── icons/
    │   └── hicolor/
    │       └── 256x256/
    │           └── apps/
    │               └── pebl2.png
    └── pebl2/
        ├── battery/
        ├── media/
        ├── pebl-lib/
        │   └── launcher.pbl
        └── doc/
```

### Runtime Path Resolution

When the AppImage runs:

1. AppImage runtime mounts to: `/tmp/.mount_PEBL-ak7Xqw/`
2. Binary location: `/tmp/.mount_PEBL-ak7Xqw/usr/bin/pebl2`
3. BinReloc detects: `/tmp/.mount_PEBL-ak7Xqw/usr`
4. Finds resources at: `/tmp/.mount_PEBL-ak7Xqw/usr/share/pebl2/`
5. Launcher copies to: `~/Documents/pebl-exp.2.1/`

All paths are relative and relocatable - no hardcoded `/usr/local/` paths.

## Build Requirements

### On Build Machine

- GCC/G++ or Clang
- make
- SDL2 development libraries
- wget (to download linuxdeploy)
- Standard build tools

### On User Machine

- **Nothing!** (besides basic Linux)
- FUSE (usually pre-installed)
- X11 or Wayland display server

## Cleaning Up

```bash
# Clean AppImage artifacts
make appimage-clean

# Or manually
rm -rf AppDir
rm -f *.AppImage
rm -f linuxdeploy-*.AppImage
```

## Advanced: Custom Builds

### Build for Different Version

```bash
./build-appimage.sh 2.2
```

### Build with Custom Name

Edit `build-appimage.sh` and change:
```bash
APPIMAGE_NAME="PEBL-Custom-x86_64.AppImage"
```

### Bundle Additional Files

Edit the `install` target in Makefile to include more files, then rebuild:
```bash
make appimage
```

## FAQ

**Q: Can I run multiple PEBL versions?**
A: Yes! Each AppImage is independent:
```bash
./PEBL-2.0-x86_64.AppImage  # Uses ~/Documents/pebl-exp.2.0/
./PEBL-2.1-x86_64.AppImage  # Uses ~/Documents/pebl-exp.2.1/
```

**Q: Does it work on all Linux distributions?**
A: Yes! Tested on Ubuntu, Debian, Fedora, Arch, openSUSE. Works anywhere with FUSE support.

**Q: Can I modify experiments inside the AppImage?**
A: The AppImage filesystem is read-only, but the launcher automatically copies experiments to `~/Documents/pebl-exp.2.1/` where you can modify them.

**Q: How do I update?**
A: Download the new AppImage version. Old versions remain usable.

**Q: Can I create a .deb from the AppImage?**
A: No, but you can create a .deb with `make deb`. AppImage is an alternative distribution method.

**Q: Does it auto-update?**
A: No, AppImages don't auto-update. Download new versions manually. (Use Flatpak if you want auto-updates)

## See Also

- [AppImage Documentation](https://docs.appimage.org/)
- [linuxdeploy](https://github.com/linuxdeploy/linuxdeploy)
- [PEBL Manual](doc/pman/PEBLManual2.1.pdf)
- [PACKAGING_OPTIONS.md](PACKAGING_OPTIONS.md) - Overview of all packaging formats
