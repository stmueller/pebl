#!/bin/bash
#
# Deploy PEBL compiled files to PEBLOnlinePlatform
#
# This script copies the compiled Emscripten files and battery tests
# from the PEBL build directory to the online platform directory.
#
# Usage:
#   ./deploy-to-online-platform.sh [production|test]
#
# Arguments:
#   production - Deploy production build (pebl2.*) [default]
#   test       - Deploy test build (pebl2-test.*)

set -e  # Exit on error

# Determine script directory (where PEBL is built)
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PEBL_DIR="$SCRIPT_DIR/.."
BIN_DIR="$PEBL_DIR/bin"
BATTERY_SRC="$PEBL_DIR/upload-battery"
EMSCRIPTEN_DIR="$PEBL_DIR/emscripten"

# Target directory (sibling to pebl_CL)
ONLINE_PLATFORM_DIR="$SCRIPT_DIR/../../PEBLOnlinePlatform"
RUNTIME_DIR="$ONLINE_PLATFORM_DIR/runtime"
BATTERY_DIR="$ONLINE_PLATFORM_DIR/battery"

# Build type (production or test)
BUILD_TYPE="${1:-production}"

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "========================================"
echo "  PEBL Online Platform Deployment"
echo "========================================"
echo ""

# Check if online platform directory exists
if [ ! -d "$ONLINE_PLATFORM_DIR" ]; then
    echo -e "${RED}Error: Online platform directory not found at:${NC}"
    echo "  $ONLINE_PLATFORM_DIR"
    echo ""
    echo "Please ensure PEBLOnlinePlatform exists as a sibling directory to pebl_CL"
    exit 1
fi

# Create runtime directory if it doesn't exist
if [ ! -d "$RUNTIME_DIR" ]; then
    echo -e "${YELLOW}Creating runtime directory...${NC}"
    mkdir -p "$RUNTIME_DIR"
fi

# Deploy based on build type
if [ "$BUILD_TYPE" = "production" ]; then
    echo -e "${GREEN}Deploying PRODUCTION build...${NC}"
    echo ""

    # Check if build files exist
    if [ ! -f "$BIN_DIR/pebl2.html" ]; then
        echo -e "${RED}Error: Production build not found in $BIN_DIR${NC}"
        echo "Please run 'make em-opt' first to build the production version"
        exit 1
    fi

    # Copy compiled PEBL files
    echo "Copying compiled PEBL files..."
    cp -v "$BIN_DIR/pebl2.html" "$RUNTIME_DIR/"
    cp -v "$BIN_DIR/pebl2.js" "$RUNTIME_DIR/"
    cp -v "$BIN_DIR/pebl2.wasm" "$RUNTIME_DIR/"
    cp -v "$BIN_DIR/pebl2.data" "$RUNTIME_DIR/"

    # Copy core-battery bundle
    echo ""
    echo "Copying core-battery bundle..."
    if [ -f "$BIN_DIR/core-battery.data" ]; then
        cp -v "$BIN_DIR/core-battery.data" "$RUNTIME_DIR/"
        cp -v "$BIN_DIR/core-battery.js" "$RUNTIME_DIR/"
        [ -f "$BIN_DIR/core-battery.js.metadata" ] && cp -v "$BIN_DIR/core-battery.js.metadata" "$RUNTIME_DIR/"
        echo -e "${GREEN}  ✓ Core-battery bundle deployed${NC}"
    else
        echo -e "${YELLOW}  Warning: core-battery bundle not found - run './scripts/build-bundles.sh core-battery' first${NC}"
    fi

    # Copy test-bundles directory
    echo ""
    echo "Copying test-bundles..."
    if [ -d "$BIN_DIR/test-bundles" ]; then
        mkdir -p "$RUNTIME_DIR/test-bundles"
        cp -v "$BIN_DIR/test-bundles"/* "$RUNTIME_DIR/test-bundles/" 2>/dev/null || echo "  (No test bundles found)"
        echo -e "${GREEN}  ✓ Test bundles deployed to runtime/test-bundles/${NC}"
    else
        echo -e "${YELLOW}  Note: No test-bundles directory found - run './scripts/build-bundles.sh' to create bundles${NC}"
    fi

    # Package private_tasks into separate data file
    echo ""
    echo "Building private_uploads.data bundle..."
    PRIVATE_TASKS_DIR="$SCRIPT_DIR/private_tasks"
    PRIVATE_BATTERY_DIR="$ONLINE_PLATFORM_DIR/private_battery"
    FP="$PEBL_DIR/libs/emsdk/upstream/emscripten/tools/file_packager.py"

    if [ -f "$FP" ] && [ -d "$PRIVATE_TASKS_DIR" ]; then
        echo "  Found private_tasks directory with:"
        ls -1 "$PRIVATE_TASKS_DIR" | sed 's/^/    - /'

        # Use file_packager to create separate data bundle
        # Mount at /usr/local/share/pebl2/private_battery to match API expectations
        echo "  Running file_packager to create private_uploads.data..."
        python3 "$FP" \
            "$RUNTIME_DIR/private_uploads.data" \
            --preload "$PRIVATE_TASKS_DIR@/usr/local/share/pebl2/private_battery" \
            --js-output="$RUNTIME_DIR/private_uploads.js" \
            --separate-metadata \
            --use-preload-cache

        echo -e "${GREEN}  ✓ private_uploads.data created successfully${NC}"
        echo "  Output files:"
        echo "    - $RUNTIME_DIR/private_uploads.data"
        echo "    - $RUNTIME_DIR/private_uploads.js"
        ls -lh "$RUNTIME_DIR/private_uploads.data" | awk '{print "    Size: " $5}'

        # Copy private test params directories to web-accessible location
        # This allows requirements.json and other params files to be fetched via HTTP
        echo ""
        echo "Copying private test params to web-accessible location..."
        mkdir -p "$PRIVATE_BATTERY_DIR"

        for test_dir in "$PRIVATE_TASKS_DIR"/*; do
            if [ -d "$test_dir" ]; then
                test_name=$(basename "$test_dir")
                params_src="$test_dir/params"

                if [ -d "$params_src" ]; then
                    params_dest="$PRIVATE_BATTERY_DIR/$test_name/params"
                    mkdir -p "$params_dest"

                    # Copy params files (especially requirements.json)
                    echo "  Copying params for $test_name..."
                    cp -v "$params_src"/*.json "$params_dest/" 2>/dev/null || echo "    (No JSON params files found)"
                fi
            fi
        done

        echo -e "${GREEN}  ✓ Private test params copied to $PRIVATE_BATTERY_DIR${NC}"
    else
        if [ ! -f "$FP" ]; then
            echo -e "${YELLOW}  Warning: file_packager.py not found at $FP${NC}"
        fi
        if [ ! -d "$PRIVATE_TASKS_DIR" ]; then
            echo -e "${YELLOW}  Note: No private_tasks directory found - skipping${NC}"
            echo "  Create $PRIVATE_TASKS_DIR and add private tests to include them"
        fi
    fi

    # Note: pebl-launcher.html is maintained directly in PEBLOnlinePlatform/runtime/
    # and is NOT copied from emscripten/ directory

    # Copy battery tests (exclude data directories and backup files)
    echo "Copying battery tests (excluding data directories and backup files)..."
    if [ -d "$BATTERY_SRC" ]; then
        if command -v rsync &> /dev/null; then
            # Use rsync if available (better for excluding patterns)
            rsync -av \
                --exclude='*/data/' \
                --exclude='data/' \
                --exclude='*~' \
                --exclude='*#*' \
                --exclude='.#*' \
                --exclude='*.swp' \
                --exclude='*.bak' \
                --exclude='.*.swp' \
                "$BATTERY_SRC/" "$BATTERY_DIR/"
        else
            # Fallback to cp with find (exclude data directories and backup files)
            echo "  (rsync not found, using cp with manual exclusion)"
            find "$BATTERY_SRC" -type f \
                -not -path '*/data/*' \
                -not -name '*~' \
                -not -name '*#*' \
                -not -name '.#*' \
                -not -name '*.swp' \
                -not -name '*.bak' \
                -not -name '.*.swp' \
                -exec cp --parents -v {} "$BATTERY_DIR/" \; 2>/dev/null || echo "  (No battery files to copy)"
        fi
    else
        echo -e "${YELLOW}Warning: Battery source directory not found at $BATTERY_SRC${NC}"
    fi

    # Copy private battery tests (exclude data directories and backup files)
    echo ""
    echo "Copying private battery tests (excluding data directories and backup files)..."
    if [ -d "$PRIVATE_TASKS_DIR" ]; then
        mkdir -p "$PRIVATE_BATTERY_DIR"
        if command -v rsync &> /dev/null; then
            # Use rsync if available (better for excluding patterns)
            rsync -av \
                --exclude='*/data/' \
                --exclude='data/' \
                --exclude='*~' \
                --exclude='*#*' \
                --exclude='.#*' \
                --exclude='*.swp' \
                --exclude='*.bak' \
                --exclude='.*.swp' \
                "$PRIVATE_TASKS_DIR/" "$PRIVATE_BATTERY_DIR/"
        else
            # Fallback to cp with find (exclude data directories and backup files)
            echo "  (rsync not found, using cp with manual exclusion)"
            find "$PRIVATE_TASKS_DIR" -type f \
                -not -path '*/data/*' \
                -not -name '*~' \
                -not -name '*#*' \
                -not -name '.#*' \
                -not -name '*.swp' \
                -not -name '*.bak' \
                -not -name '.*.swp' \
                -exec cp --parents -v {} "$PRIVATE_BATTERY_DIR/" \; 2>/dev/null || echo "  (No private battery files to copy)"
        fi
        echo -e "${GREEN}  ✓ Private battery tests copied to $PRIVATE_BATTERY_DIR${NC}"
    else
        echo -e "${YELLOW}  Note: No private_tasks directory found - skipping${NC}"
    fi

    echo ""
    echo -e "${GREEN}✓ Production deployment complete!${NC}"

elif [ "$BUILD_TYPE" = "test" ]; then
    echo -e "${GREEN}Deploying TEST build...${NC}"
    echo ""

    # Check if build files exist
    if [ ! -f "$BIN_DIR/pebl2-test.html" ]; then
        echo -e "${RED}Error: Test build not found in $BIN_DIR${NC}"
        echo "Please run 'make em-test' first to build the test version"
        exit 1
    fi

    # Copy compiled PEBL test files
    echo "Copying compiled PEBL test files..."
    cp -v "$BIN_DIR/pebl2-test.html" "$RUNTIME_DIR/"
    cp -v "$BIN_DIR/pebl2-test.js" "$RUNTIME_DIR/"
    cp -v "$BIN_DIR/pebl2-test.wasm" "$RUNTIME_DIR/"
    cp -v "$BIN_DIR/pebl2-test.data" "$RUNTIME_DIR/"

    echo ""
    echo -e "${GREEN}✓ Test deployment complete!${NC}"

else
    echo -e "${RED}Error: Unknown build type '$BUILD_TYPE'${NC}"
    echo "Usage: $0 [production|test]"
    exit 1
fi

echo ""
echo "Files deployed to:"
echo "  Runtime: $RUNTIME_DIR"
if [ "$BUILD_TYPE" = "production" ]; then
    echo "  Battery: $BATTERY_DIR"
    if [ -d "$PRIVATE_BATTERY_DIR" ]; then
        echo "  Private Battery (params only): $PRIVATE_BATTERY_DIR"
    fi
fi
echo ""
echo "========================================"
