#!/bin/bash
#
# PEBL AppImage Build Script
#

set -e

PEBL_VERSION="${1:-2.2}"
SKIP_BUILD="${2}"
APPIMAGE_NAME="PEBL-${PEBL_VERSION}-x86_64.AppImage"
APPDIR="AppDir"
LINUXDEPLOY_URL="https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
LINUXDEPLOY="linuxdeploy-x86_64.AppImage"

echo "========================================="
echo "PEBL AppImage Builder v${PEBL_VERSION}"
echo "========================================="

# Download linuxdeploy
if [ ! -f "${LINUXDEPLOY}" ]; then
    echo "Downloading linuxdeploy..."
    wget -q "${LINUXDEPLOY_URL}" -O "${LINUXDEPLOY}"
    chmod +x "${LINUXDEPLOY}"
fi

# Clean AppDir and old AppImage
echo "Cleaning AppImage artifacts..."
rm -rf "${APPDIR}" "${APPIMAGE_NAME}"

# Build (skip if --skip-build flag is set)
if [ "$SKIP_BUILD" != "--skip-build" ]; then
    make clean 2>/dev/null || true
    echo "Building PEBL components..."
    echo "  - pebl2 (main executable)"
    make main -j$(nproc)
    echo "  - pebl-launcher (GUI launcher)"
    make pebl-launcher -j$(nproc)
    echo "  - pebl-validator (script validator)"
    make validator -j$(nproc)
else
    echo "Skipping build (using existing binaries)..."
    if [ ! -f "bin/pebl2" ]; then
        echo "ERROR: bin/pebl2 not found!"
        exit 1
    fi
    if [ ! -f "bin/pebl-launcher" ]; then
        echo "WARNING: bin/pebl-launcher not found - launcher will not be included"
    fi
    if [ ! -f "bin/pebl-validator" ]; then
        echo "WARNING: bin/pebl-validator not found - validator will not be included"
    fi
fi

# Install
echo "Installing to AppDir..."
make install DESTDIR="${APPDIR}" PREFIX=/usr

# Create symlinks for AppImage compatibility
# linuxdeploy expects executables in /usr/bin/
mkdir -p "${APPDIR}/usr/bin"
ln -sf ../pebl2/bin/pebl2 "${APPDIR}/usr/bin/pebl2"

# Create symlinks for launcher and validator if they exist
if [ -f "${APPDIR}/usr/pebl2/bin/pebl-launcher" ]; then
    ln -sf ../pebl2/bin/pebl-launcher "${APPDIR}/usr/bin/pebl-launcher"
    echo "✓ pebl-launcher installed"
fi

if [ -f "${APPDIR}/usr/pebl2/bin/pebl-validator" ]; then
    ln -sf ../pebl2/bin/pebl-validator "${APPDIR}/usr/bin/pebl-validator"
    echo "✓ pebl-validator installed"
fi

# Verify essential files
if [ ! -f "${APPDIR}/usr/pebl2/bin/pebl2" ]; then
    echo "ERROR: pebl2 not found at ${APPDIR}/usr/pebl2/bin/pebl2"
    exit 1
fi
if [ ! -f "${APPDIR}/usr/pebl2/bin/launcher.pbl" ]; then
    echo "ERROR: launcher.pbl not found at ${APPDIR}/usr/pebl2/bin/launcher.pbl"
    exit 1
fi
if [ ! -f "${APPDIR}/usr/pebl2/bin/pebl-launcher" ]; then
    echo "WARNING: pebl-launcher not found - GUI launcher will not be available"
fi

# Create custom AppRun that detects how it was called
cat > "${APPDIR}/AppRun" << 'APPRUN_EOF'
#!/bin/bash
# PEBL AppImage AppRun
# Detects how we were called via symlink name and runs appropriate executable

# Get the directory where the AppImage is mounted
APPDIR="${APPDIR:-$(dirname "$(readlink -f "$0")")}"

# Get the name we were called as (could be symlink name or AppImage name)
CALLED_AS="$(basename "$ARGV0")"

case "$CALLED_AS" in
    pebl2|pebl2-cli)
        # Called as pebl2 - run CLI
        exec "${APPDIR}/usr/pebl2/bin/pebl2" "$@"
        ;;
    pebl-validator|*validator*)
        # Called as validator
        exec "${APPDIR}/usr/pebl2/bin/pebl-validator" "$@"
        ;;
    launcher|pebl-launcher-old)
        # Called as launcher - run old-school launcher.pbl
        exec "${APPDIR}/usr/pebl2/bin/pebl2" "${APPDIR}/usr/pebl2/bin/launcher.pbl" "$@"
        ;;
    pebl-launcher|PEBL*|*)
        # Default: run GUI launcher
        exec "${APPDIR}/usr/pebl2/bin/pebl-launcher" "$@"
        ;;
esac
APPRUN_EOF

chmod +x "${APPDIR}/AppRun"
echo "✓ Created AppRun entrypoint script"

# Desktop file
mkdir -p "${APPDIR}/usr/share/applications"

# Desktop file uses pebl-launcher (AppRun will handle routing)
EXEC_COMMAND="pebl-launcher"
echo "Desktop entry: Using pebl-launcher (AppRun handles symlink routing)"

cat > "${APPDIR}/usr/share/applications/PEBL2.desktop" << EOF
[Desktop Entry]
Version=1.0
Name=PEBL 2
Comment=Psychology Experiment Building Language
Exec=${EXEC_COMMAND}
Terminal=false
Type=Application
Categories=Science;
Icon=pebl2
StartupNotify=true
EOF

# AppStream metadata
mkdir -p "${APPDIR}/usr/share/metainfo"
cp installer/net.sourceforge.pebl.appdata.xml "${APPDIR}/usr/share/metainfo/net.sourceforge.pebl.appdata.xml"

# Icon - resize to exactly 256x256
mkdir -p "${APPDIR}/usr/share/icons/hicolor/256x256/apps"
if [ -f "media/images/pebl2.png" ]; then
    convert "media/images/pebl2.png" -resize 256x256 "${APPDIR}/usr/share/icons/hicolor/256x256/apps/pebl2.png"
elif [ -f "${APPDIR}/usr/pebl2/media/images/pebl2.png" ]; then
    convert "${APPDIR}/usr/pebl2/media/images/pebl2.png" -resize 256x256 "${APPDIR}/usr/share/icons/hicolor/256x256/apps/pebl2.png"
fi

# Summary
echo "Bundling: $(find ${APPDIR}/usr/pebl2/battery -name '*.pbl' | wc -l) battery scripts"
echo "AppDir size: $(du -sh ${APPDIR} | cut -f1)"

# Create AppImage
echo "Creating AppImage..."
./"${LINUXDEPLOY}" \
    --appdir="${APPDIR}" \
    --desktop-file="${APPDIR}/usr/share/applications/PEBL2.desktop" \
    --icon-file="${APPDIR}/usr/share/icons/hicolor/256x256/apps/pebl2.png" \
    --executable="${APPDIR}/usr/pebl2/bin/pebl-launcher" \
    --output appimage

# Find and rename, then move to bin/
CREATED=$(ls -t *.AppImage 2>/dev/null | grep -v "linuxdeploy" | head -1)
if [ -n "${CREATED}" ] && [ "${CREATED}" != "${APPIMAGE_NAME}" ]; then
    mv "${CREATED}" "${APPIMAGE_NAME}"
fi

if [ ! -f "${APPIMAGE_NAME}" ]; then
    echo "ERROR: AppImage not created"
    exit 1
fi

chmod +x "${APPIMAGE_NAME}"

# Move to bin/ directory
mv "${APPIMAGE_NAME}" "bin/${APPIMAGE_NAME}"
APPIMAGE_NAME="bin/${APPIMAGE_NAME}"

# Test
echo "Testing..."
./"${APPIMAGE_NAME}" --appimage-extract >/dev/null 2>&1
if [ -f "squashfs-root/usr/pebl2/bin/pebl2" ] && [ -f "squashfs-root/usr/pebl2/bin/launcher.pbl" ]; then
    echo "✓ AppImage structure verified"
else
    echo "✗ AppImage structure invalid"
    exit 1
fi
rm -rf squashfs-root

# Create symlink-based CLI wrappers
echo "Creating symlink-based command-line wrappers..."

# Create symlinks to the AppImage with specific names
# When called via these symlinks, the entrypoint script will route to the correct executable
cd bin

# Remove old wrapper scripts if they exist
rm -f pebl2 pebl-validator launcher

# Create symlinks (use basename to avoid bin/bin/ issue)
ln -sf "$(basename "${APPIMAGE_NAME}")" pebl2
ln -sf "$(basename "${APPIMAGE_NAME}")" pebl-validator
ln -sf "$(basename "${APPIMAGE_NAME}")" launcher

cd ..

echo "✓ Created bin/pebl2 (symlink to AppImage for CLI access)"
echo "✓ Created bin/pebl-validator (symlink to AppImage for validator access)"
echo "✓ Created bin/launcher (symlink to AppImage for old-school launcher.pbl)"

# Symlink in bin/
cd bin
ln -sf "$(basename ${APPIMAGE_NAME})" pebl2-appimage
cd ..

echo "========================================="
echo "✓ AppImage Complete: ${APPIMAGE_NAME}"
echo "  Size: $(du -sh ${APPIMAGE_NAME} | cut -f1)"
echo ""
echo "Usage:"
echo "  GUI (new):     ./bin/pebl2-appimage  (or double-click AppImage)"
echo "  GUI (old):     ./bin/launcher  (symlink - runs launcher.pbl)"
echo "  CLI:           ./bin/pebl2 script.pbl -s 123  (symlink)"
echo "  Validator:     ./bin/pebl-validator script.pbl  (symlink)"
echo ""
echo "Note: CLI/validator/launcher access uses symlinks with \$ARGV0 detection."
echo "      The AppImage detects which executable to run based on the"
echo "      symlink name used to invoke it."
echo "========================================="
