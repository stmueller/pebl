#!/bin/bash
#
# Generate Font Bundles for PEBL Web Platform
#
# This script creates separate font bundles for different language/use cases:
#   - fonts_noto: International fonts (Thai, Devanagari, Bengali, Arabic, Hebrew, etc.)
#   - fonts_cjk: Chinese/Japanese/Korean fonts (very large)
#   - fonts_specialty: Specialty fonts for vision tests, etc.
#
# The default pebl2 bundle already includes DejaVu fonts (1.3MB), so we don't
# duplicate those here.
#
# Usage:
#   ./scripts/generate-font-bundles.sh [dev|prod]
#
# Arguments:
#   dev  - Generate bundles in bin/ for local development/testing
#   prod - Generate bundles in ../PEBLOnlinePlatform/runtime/ for deployment
#

set -e  # Exit on error

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Determine mode (dev or prod)
MODE="${1:-dev}"

if [[ "$MODE" != "dev" && "$MODE" != "prod" ]]; then
    echo -e "${RED}Error: Invalid mode '$MODE'. Use 'dev' or 'prod'${NC}"
    exit 1
fi

# Set paths
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PEBL_ROOT="$SCRIPT_DIR/.."
FONTS_SRC="$PEBL_ROOT/media/fonts"

if [[ "$MODE" == "prod" ]]; then
    OUTPUT_DIR="$PEBL_ROOT/../PEBLOnlinePlatform/runtime"
    echo -e "${GREEN}Production mode: Generating bundles in $OUTPUT_DIR${NC}"
else
    OUTPUT_DIR="$PEBL_ROOT/bin"
    echo -e "${YELLOW}Development mode: Generating bundles in $OUTPUT_DIR${NC}"
fi

# Find file_packager.py
FP="$PEBL_ROOT/libs/emsdk/upstream/emscripten/tools/file_packager.py"

if [[ ! -f "$FP" ]]; then
    echo -e "${RED}Error: file_packager.py not found at $FP${NC}"
    echo "Make sure emsdk is installed in libs/emsdk/"
    exit 1
fi

echo "Using file_packager: $FP"
echo "Source fonts: $FONTS_SRC"
echo ""

# Create temporary directory for organizing fonts
TEMP_DIR=$(mktemp -d)
trap "rm -rf $TEMP_DIR" EXIT

echo "=========================================="
echo "  Font Bundle Generation"
echo "=========================================="
echo ""
echo "Note: Specialty fonts (Optician-Sans, Stimulasia, Humanistic) are"
echo "      included in the core pebl2 bundle, not generated here."
echo ""

#
# Bundle 1: fonts_noto - International fonts (non-CJK)
#
echo -e "${GREEN}[1/2] Creating fonts_noto bundle...${NC}"

NOTO_DIR="$TEMP_DIR/fonts_noto/media/fonts"
mkdir -p "$NOTO_DIR"

# Copy Noto fonts (excluding CJK which is huge)
cp "$FONTS_SRC/NotoSansThai-Regular.ttf" "$NOTO_DIR/" 2>/dev/null || true
cp "$FONTS_SRC/NotoSansDevanagari-Regular.ttf" "$NOTO_DIR/" 2>/dev/null || true
cp "$FONTS_SRC/NotoSansBengali-Regular.ttf" "$NOTO_DIR/" 2>/dev/null || true
cp "$FONTS_SRC/NotoSansArabic-Regular.ttf" "$NOTO_DIR/" 2>/dev/null || true
cp "$FONTS_SRC/NotoSansHebrew-Regular.ttf" "$NOTO_DIR/" 2>/dev/null || true
cp "$FONTS_SRC/NotoSansGeorgian-Regular.ttf" "$NOTO_DIR/" 2>/dev/null || true
cp "$FONTS_SRC/NotoSans-Regular.ttf" "$NOTO_DIR/" 2>/dev/null || true
cp "$FONTS_SRC/NotoSans-Bold.ttf" "$NOTO_DIR/" 2>/dev/null || true
cp "$FONTS_SRC/NotoSansMono-Regular.ttf" "$NOTO_DIR/" 2>/dev/null || true
cp "$FONTS_SRC/NotoSansMono-Bold.ttf" "$NOTO_DIR/" 2>/dev/null || true
cp "$FONTS_SRC/NotoSerif-Regular.ttf" "$NOTO_DIR/" 2>/dev/null || true
cp "$FONTS_SRC/NotoSerif-Bold.ttf" "$NOTO_DIR/" 2>/dev/null || true

# Count files
NOTO_COUNT=$(find "$NOTO_DIR" -type f | wc -l)
NOTO_SIZE=$(du -sh "$NOTO_DIR" | cut -f1)

echo "  Fonts included: $NOTO_COUNT files ($NOTO_SIZE)"
echo "  Languages: Thai, Devanagari (Hindi/Marathi/Nepali), Bengali, Arabic, Hebrew, Georgian"

# Generate bundle (without LZ4 - pebl2 doesn't have LZ4 support)
python "$FP" "$OUTPUT_DIR/fonts_noto.data" \
    --preload "$TEMP_DIR/fonts_noto/media@/usr/local/share/pebl2/media" \
    --js-output="$OUTPUT_DIR/fonts_noto.js" \
    --no-node

BUNDLE_SIZE=$(ls -lh "$OUTPUT_DIR/fonts_noto.data" | awk '{print $5}')
echo -e "  ${GREEN}✓ Generated: fonts_noto.js + fonts_noto.data ($BUNDLE_SIZE)${NC}"
echo ""

#
# Bundle 2: fonts_cjk - Chinese/Japanese/Korean fonts
#
echo -e "${GREEN}[2/2] Creating fonts_cjk bundle...${NC}"

CJK_DIR="$TEMP_DIR/fonts_cjk/media/fonts"
mkdir -p "$CJK_DIR"

# Check if CJK font exists (it's a symlink, so use -L to follow it)
if [[ -e "$FONTS_SRC/NotoSansCJK-Regular.ttc" ]]; then
    cp -L "$FONTS_SRC/NotoSansCJK-Regular.ttc" "$CJK_DIR/" 2>/dev/null || {
        echo -e "  ${YELLOW}Warning: Could not copy NotoSansCJK-Regular.ttc (may be broken symlink)${NC}"
        echo -e "  ${YELLOW}Skipping fonts_cjk bundle${NC}"
        echo ""
        CJK_SKIP=1
    }
else
    echo -e "  ${YELLOW}Warning: NotoSansCJK-Regular.ttc not found${NC}"
    echo -e "  ${YELLOW}Skipping fonts_cjk bundle${NC}"
    echo ""
    CJK_SKIP=1
fi

if [[ -z "$CJK_SKIP" ]]; then
    # Also include Noto Mono and Serif for CJK languages
    cp "$FONTS_SRC/NotoSansMono-Regular.ttf" "$CJK_DIR/" 2>/dev/null || true
    cp "$FONTS_SRC/NotoSerif-Regular.ttf" "$CJK_DIR/" 2>/dev/null || true

    # Count files
    CJK_COUNT=$(find "$CJK_DIR" -type f | wc -l)
    CJK_SIZE=$(du -sh "$CJK_DIR" | cut -f1)

    echo "  Fonts included: $CJK_COUNT files ($CJK_SIZE)"
    echo "  Languages: Chinese (Simplified/Traditional), Japanese, Korean"
    echo "  Note: This bundle is large (~20MB) due to CJK character set size"

    # Generate bundle (without LZ4 - pebl2 doesn't have LZ4 support)
    python "$FP" "$OUTPUT_DIR/fonts_cjk.data" \
        --preload "$TEMP_DIR/fonts_cjk/media@/usr/local/share/pebl2/media" \
        --js-output="$OUTPUT_DIR/fonts_cjk.js" \
        --no-node

    BUNDLE_SIZE=$(ls -lh "$OUTPUT_DIR/fonts_cjk.data" | awk '{print $5}')
    echo -e "  ${GREEN}✓ Generated: fonts_cjk.js + fonts_cjk.data ($BUNDLE_SIZE)${NC}"
    echo ""
fi

echo "=========================================="
echo -e "${GREEN}✓ Font bundle generation complete!${NC}"
echo "=========================================="
echo ""
echo "Generated files in $OUTPUT_DIR:"
ls -lh "$OUTPUT_DIR"/fonts_*.js "$OUTPUT_DIR"/fonts_*.data | awk '{print "  " $5 "\t" $9}'
echo ""

if [[ "$MODE" == "prod" ]]; then
    echo -e "${GREEN}Production bundles ready for deployment!${NC}"
    echo ""
    echo "Next steps:"
    echo "  1. Commit the generated bundles to PEBLOnlinePlatform repository"
    echo "  2. Rebuild core pebl2 bundle with 'make fp' to include specialty fonts"
    echo "  3. Test with various language parameters"
else
    echo -e "${YELLOW}Development bundles created for local testing${NC}"
    echo ""
    echo "To deploy to production:"
    echo "  1. Run 'make fp' to rebuild pebl2.data with specialty fonts"
    echo "  2. Run './scripts/generate-font-bundles.sh prod'"
fi
echo ""
