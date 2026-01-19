#!/bin/bash
#
# PEBL Validator AppImage Build Script
#

set -e

PEBL_VERSION="${1:-2.2}"
SKIP_BUILD="${2}"
APPIMAGE_NAME="pebl-validator-${PEBL_VERSION}-x86_64.AppImage"
APPDIR="ValidatorAppDir"
LINUXDEPLOY_URL="https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
LINUXDEPLOY="linuxdeploy-x86_64.AppImage"

echo "========================================="
echo "PEBL Validator AppImage Builder v${PEBL_VERSION}"
echo "========================================="

# Download linuxdeploy
if [ ! -f "${LINUXDEPLOY}" ]; then
    echo "Downloading linuxdeploy..."
    wget -q "${LINUXDEPLOY_URL}" -O "${LINUXDEPLOY}"
    chmod +x "${LINUXDEPLOY}"
fi

# Clean AppDir and old AppImage
echo "Cleaning validator AppImage artifacts..."
rm -rf "${APPDIR}" "${APPIMAGE_NAME}"

# Build (skip if --skip-build flag is set)
if [ "$SKIP_BUILD" != "--skip-build" ]; then
    # Only clean validator objects
    rm -rf obj-validator
    echo "Building PEBL validator..."
    make validator -j$(nproc)
else
    echo "Skipping build (using existing binary)..."
    if [ ! -f "bin/pebl-validator" ]; then
        echo "ERROR: bin/pebl-validator not found!"
        exit 1
    fi
fi

# Create minimal AppDir structure
# Validator looks for pebl-lib at ../pebl-lib relative to binary location
# Binary is at /usr/bin/pebl-validator, so pebl-lib should be at /usr/pebl-lib
echo "Creating AppDir structure..."
mkdir -p "${APPDIR}/usr/bin"
mkdir -p "${APPDIR}/usr/pebl-lib"

# Install validator binary
echo "Installing validator binary..."
cp bin/pebl-validator "${APPDIR}/usr/bin/pebl-validator"
chmod +x "${APPDIR}/usr/bin/pebl-validator"

# Install minimal pebl-lib files (validator needs these for validation)
echo "Installing pebl-lib files..."
cp pebl-lib/*.pbl "${APPDIR}/usr/pebl-lib/"

# Verify installation
if [ ! -f "${APPDIR}/usr/bin/pebl-validator" ]; then
    echo "ERROR: pebl-validator not found at ${APPDIR}/usr/bin/pebl-validator"
    exit 1
fi

# Desktop file (for AppImage metadata)
mkdir -p "${APPDIR}/usr/share/applications"
cat > "${APPDIR}/usr/share/applications/pebl-validator.desktop" << EOF
[Desktop Entry]
Version=1.0
Name=PEBL Validator
Comment=PEBL Script Validation Tool
Exec=pebl-validator
Terminal=true
Type=Application
Categories=Development;
Icon=pebl-validator
NoDisplay=true
EOF

# Create a simple icon (text-based since validator is CLI-only)
mkdir -p "${APPDIR}/usr/share/icons/hicolor/256x256/apps"
if command -v convert >/dev/null 2>&1 && [ -f "media/images/pebl2.png" ]; then
    # Use PEBL icon if available
    convert "media/images/pebl2.png" -resize 256x256 "${APPDIR}/usr/share/icons/hicolor/256x256/apps/pebl-validator.png"
else
    # Create a simple placeholder icon
    echo "Warning: ImageMagick not found or pebl2.png missing, creating placeholder icon"
    convert -size 256x256 xc:white -pointsize 72 -fill black -gravity center \
        -annotate +0+0 "PV" "${APPDIR}/usr/share/icons/hicolor/256x256/apps/pebl-validator.png" 2>/dev/null || true
fi

# Summary
echo "AppDir size: $(du -sh ${APPDIR} | cut -f1)"

# Create AppImage
echo "Creating AppImage..."
./"${LINUXDEPLOY}" \
    --appdir="${APPDIR}" \
    --desktop-file="${APPDIR}/usr/share/applications/pebl-validator.desktop" \
    --icon-file="${APPDIR}/usr/share/icons/hicolor/256x256/apps/pebl-validator.png" \
    --executable="${APPDIR}/usr/bin/pebl-validator" \
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
if [ -f "squashfs-root/usr/bin/pebl-validator" ]; then
    echo "✓ AppImage structure verified"

    # Test validator execution
    echo "Testing validator execution..."
    if ./squashfs-root/usr/bin/pebl-validator --version 2>&1 | grep -q "PEBL Validator" || true; then
        echo "✓ Validator executes successfully"
    fi
else
    echo "✗ AppImage structure invalid"
    exit 1
fi
rm -rf squashfs-root

echo "========================================="
echo "✓ Validator AppImage Complete: ${APPIMAGE_NAME}"
echo "  Size: $(du -sh ${APPIMAGE_NAME} | cut -f1)"
echo ""
echo "Usage: ./bin/pebl-validator-${PEBL_VERSION}-x86_64.AppImage [script.pbl]"
echo "========================================="
