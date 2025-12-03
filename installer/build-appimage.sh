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
    echo "Building PEBL..."
    make -j$(nproc)
else
    echo "Skipping build (using existing binary)..."
    if [ ! -f "bin/pebl2" ]; then
        echo "ERROR: bin/pebl2 not found!"
        exit 1
    fi
fi

# Install
echo "Installing to AppDir..."
make install DESTDIR="${APPDIR}" PREFIX=/usr

# Create symlink for AppImage compatibility
# linuxdeploy expects the executable in /usr/bin/, so create a symlink
mkdir -p "${APPDIR}/usr/bin"
ln -sf ../pebl2/bin/pebl2 "${APPDIR}/usr/bin/pebl2"

# Verify - using new relocatable directory structure
if [ ! -f "${APPDIR}/usr/pebl2/bin/pebl2" ]; then
    echo "ERROR: pebl2 not found at ${APPDIR}/usr/pebl2/bin/pebl2"
    exit 1
fi
if [ ! -f "${APPDIR}/usr/pebl2/bin/launcher.pbl" ]; then
    echo "ERROR: launcher.pbl not found at ${APPDIR}/usr/pebl2/bin/launcher.pbl"
    exit 1
fi

# Desktop file
mkdir -p "${APPDIR}/usr/share/applications"
cat > "${APPDIR}/usr/share/applications/PEBL2.desktop" << EOF
[Desktop Entry]
Version=1.0
Name=PEBL 2
Comment=Psychology Experiment Building Language
Exec=pebl2
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
    --executable="${APPDIR}/usr/bin/pebl2" \
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

# Symlink in bin/
cd bin
ln -sf "$(basename ${APPIMAGE_NAME})" pebl2-appimage
cd ..

echo "========================================="
echo "✓ AppImage Complete: ${APPIMAGE_NAME}"
echo "  Size: $(du -sh ${APPIMAGE_NAME} | cut -f1)"
echo ""
echo "Test: ./bin/pebl2-appimage"
echo "========================================="
