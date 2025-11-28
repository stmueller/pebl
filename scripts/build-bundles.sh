#!/bin/bash
#
# Universal PEBL Bundle Builder
#
# Reads bundle-config.json and builds all configured bundles using Emscripten's file_packager.
#
# Usage:
#   ./scripts/build-bundles.sh [OPTIONS] [bundle_name]
#
# Options:
#   -h, --help       Show this help message
#   -l, --list       List all available bundles
#   -v, --verbose    Verbose output
#
# Arguments:
#   bundle_name      Optional. Build only specified bundle (e.g., "core", "pcst")
#                    Without argument: builds ALL bundles
#
# Examples:
#   ./scripts/build-bundles.sh              # Build everything
#   ./scripts/build-bundles.sh pcst         # Build only PCST
#   ./scripts/build-bundles.sh core         # Build only core bundle
#   ./scripts/build-bundles.sh --list       # List all configured bundles
#

set -e  # Exit on error

# Parse command line options
VERBOSE=false
LIST_ONLY=false
TARGET_BUNDLE=""

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            head -n 24 "$0" | tail -n +2 | sed 's/^# //' | sed 's/^#//'
            exit 0
            ;;
        -l|--list)
            LIST_ONLY=true
            shift
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        *)
            TARGET_BUNDLE="$1"
            shift
            ;;
    esac
done

# Determine directories
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PEBL_DIR="$( cd "$SCRIPT_DIR/.." && pwd )"
CONFIG_FILE="$PEBL_DIR/bundle-config.json"
FILE_PACKAGER="$PEBL_DIR/libs/emsdk/upstream/emscripten/tools/file_packager.py"

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

# Check dependencies
if ! command -v jq &> /dev/null; then
    echo -e "${RED}Error: jq is required but not installed.${NC}"
    echo "Install with: sudo apt-get install jq"
    exit 1
fi

if [ ! -f "$CONFIG_FILE" ]; then
    echo -e "${RED}Error: bundle-config.json not found at:${NC}"
    echo "  $CONFIG_FILE"
    exit 1
fi

if [ ! -f "$FILE_PACKAGER" ]; then
    echo -e "${RED}Error: file_packager.py not found at:${NC}"
    echo "  $FILE_PACKAGER"
    echo ""
    echo "Make sure Emscripten SDK is installed and initialized."
    exit 1
fi

# Function to list all bundles
list_bundles() {
    echo "Available bundles:"
    echo ""

    # Core bundle
    local core_name=$(jq -r '.bundles.core.name' "$CONFIG_FILE")
    local core_desc=$(jq -r '.bundles.core.description' "$CONFIG_FILE")
    echo -e "  ${CYAN}${core_name}${NC} (core runtime)"
    echo "    $core_desc"
    echo ""

    # Core battery bundle
    if jq -e '.bundles.core_battery' "$CONFIG_FILE" >/dev/null 2>&1; then
        local battery_name=$(jq -r '.bundles.core_battery.name' "$CONFIG_FILE")
        local battery_desc=$(jq -r '.bundles.core_battery.description' "$CONFIG_FILE")
        echo -e "  ${CYAN}${battery_name}${NC} (standard battery)"
        echo "    $battery_desc"
        echo ""
    fi

    # Test bundles
    local bundle_count=$(jq '.bundles.test_bundles | length' "$CONFIG_FILE")
    if [ "$bundle_count" -gt 0 ]; then
        echo "  Individual test bundles:"
        jq -r '.bundles.test_bundles[] | "    \(.name) - \(.description)"' "$CONFIG_FILE"
    fi
}

if [ "$LIST_ONLY" = true ]; then
    list_bundles
    exit 0
fi

# Function to build a single bundle from JSON config
build_bundle() {
    local bundle_json="$1"
    local bundle_type="$2"  # "core" or "test"

    local name=$(echo "$bundle_json" | jq -r '.name')
    local output_dir=$(echo "$bundle_json" | jq -r '.output_dir')
    local description=$(echo "$bundle_json" | jq -r '.description')
    local type=$(echo "$bundle_json" | jq -r '.type')

    # Skip if target specified and doesn't match
    if [ -n "$TARGET_BUNDLE" ] && [ "$name" != "$TARGET_BUNDLE" ]; then
        return
    fi

    echo "=========================================="
    echo -e "${BLUE}Building: ${CYAN}${name}${NC} (${type})"
    echo "=========================================="
    echo "$description"
    echo ""

    # Create output directory
    local full_output_dir="$PEBL_DIR/$output_dir"
    mkdir -p "$full_output_dir"

    # Start building file_packager command
    local data_file="$full_output_dir/$name.data"
    local js_file="$full_output_dir/$name.js"

    local cmd="python3"
    cmd="$cmd \"$FILE_PACKAGER\""
    cmd="$cmd \"$data_file\""

    # Check for manifest file
    local manifest_file=$(echo "$bundle_json" | jq -r '.manifest // empty')
    local manifest_tests=()

    if [ -n "$manifest_file" ] && [ -f "$PEBL_DIR/$manifest_file" ]; then
        echo -e "${CYAN}Using manifest: $manifest_file${NC}"
        # Read test list from manifest
        mapfile -t manifest_tests < <(jq -r '.tests[]' "$PEBL_DIR/$manifest_file")
        echo "  Tests in manifest: ${#manifest_tests[@]}"
        echo ""
    fi

    # Process includes
    echo "Including:"
    local includes=$(echo "$bundle_json" | jq -c '.includes[]')
    while IFS= read -r include; do
        local path=$(echo "$include" | jq -r '.path')
        local mount=$(echo "$include" | jq -r '.mount')
        local inc_desc=$(echo "$include" | jq -r '.description // empty')

        # Check if path exists
        local full_path="$PEBL_DIR/$path"
        if [ ! -e "$full_path" ]; then
            echo -e "  ${YELLOW}Warning: Path not found: $path${NC}"
            continue
        fi

        # If manifest is specified and path is upload-battery, include only manifest tests
        if [ ${#manifest_tests[@]} -gt 0 ] && [[ "$path" == *"upload-battery"* ]]; then
            echo -e "  ${GREEN}✓${NC} $path → $mount ${CYAN}(manifest-filtered)${NC}"
            if [ -n "$inc_desc" ]; then
                echo "    $inc_desc"
            fi

            # Include each test from manifest individually
            for test in "${manifest_tests[@]}"; do
                local test_path="$full_path/$test"
                if [ -e "$test_path" ]; then
                    cmd="$cmd --preload \"$test_path@$mount/$test\""
                else
                    echo -e "    ${YELLOW}Warning: Test not found: $test${NC}"
                fi
            done
        else
            # Normal include - add entire path
            local size=$(du -sh "$full_path" 2>/dev/null | awk '{print $1}')
            echo -e "  ${GREEN}✓${NC} $path → $mount ${CYAN}($size)${NC}"
            if [ -n "$inc_desc" ]; then
                echo "    $inc_desc"
            fi
            cmd="$cmd --preload \"$full_path@$mount\""
        fi
    done <<< "$includes"

    echo ""

    # Process excludes
    local excludes=$(echo "$bundle_json" | jq -r '.excludes[]? // empty')
    if [ -n "$excludes" ]; then
        echo "Excluding patterns:"
        while IFS= read -r exclude; do
            echo "  - $exclude"
            cmd="$cmd --exclude \"$exclude\""
        done <<< "$excludes"
        echo ""
    fi

    # Add standard options
    cmd="$cmd --js-output=\"$js_file\""

    # Process options from config
    if [ "$(echo "$bundle_json" | jq -r '.options.separate_metadata // true')" = "true" ]; then
        cmd="$cmd --separate-metadata"
    fi

    if [ "$(echo "$bundle_json" | jq -r '.options.use_preload_cache // true')" = "true" ]; then
        cmd="$cmd --use-preload-cache"
    fi

    if [ "$(echo "$bundle_json" | jq -r '.options.lz4 // false')" = "true" ]; then
        cmd="$cmd --lz4"
        echo "Using LZ4 compression"
    fi

    # Execute file_packager
    echo "Running file_packager..."
    if [ "$VERBOSE" = true ]; then
        echo "Command: $cmd"
        echo ""
    fi

    eval "$cmd"

    echo ""
    echo -e "${GREEN}✓ Bundle created successfully!${NC}"
    echo ""

    # Show output files with sizes
    echo "Output files:"
    if [ -f "$data_file" ]; then
        local data_size=$(ls -lh "$data_file" | awk '{print $5}')
        echo "  ${name}.data: $data_size"
    fi

    if [ -f "$js_file" ]; then
        local js_size=$(ls -lh "$js_file" | awk '{print $5}')
        echo "  ${name}.js: $js_size"
    fi

    local meta_file="${data_file}.metadata.json"
    if [ -f "$meta_file" ]; then
        local meta_size=$(ls -lh "$meta_file" | awk '{print $5}')
        echo "  ${name}.data.metadata.json: $meta_size"
    fi

    echo ""
    echo "Location: $output_dir/"
    echo ""
}

# Main build process
echo "=========================================="
echo "  PEBL Bundle Builder"
echo "=========================================="
echo ""

if [ -n "$TARGET_BUNDLE" ]; then
    echo "Target: $TARGET_BUNDLE"
else
    echo "Building: ALL bundles"
fi
echo ""

BUNDLES_BUILT=0

# Build core bundle
core_bundle=$(jq -c '.bundles.core' "$CONFIG_FILE")
if [ "$core_bundle" != "null" ]; then
    core_name=$(echo "$core_bundle" | jq -r '.name')
    if [ -z "$TARGET_BUNDLE" ] || [ "$TARGET_BUNDLE" = "$core_name" ] || [ "$TARGET_BUNDLE" = "core" ]; then
        build_bundle "$core_bundle" "core"
        BUNDLES_BUILT=$((BUNDLES_BUILT + 1))
    fi
fi

# Build core battery bundle
core_battery_bundle=$(jq -c '.bundles.core_battery' "$CONFIG_FILE")
if [ "$core_battery_bundle" != "null" ]; then
    core_battery_name=$(echo "$core_battery_bundle" | jq -r '.name')
    if [ -z "$TARGET_BUNDLE" ] || [ "$TARGET_BUNDLE" = "$core_battery_name" ] || [ "$TARGET_BUNDLE" = "core-battery" ] || [ "$TARGET_BUNDLE" = "core_battery" ]; then
        build_bundle "$core_battery_bundle" "battery"
        BUNDLES_BUILT=$((BUNDLES_BUILT + 1))
    fi
fi

# Build test bundles
test_bundle_count=$(jq '.bundles.test_bundles | length' "$CONFIG_FILE")
if [ "$test_bundle_count" -gt 0 ]; then
    for i in $(seq 0 $(($test_bundle_count - 1))); do
        bundle=$(jq -c ".bundles.test_bundles[$i]" "$CONFIG_FILE")
        name=$(echo "$bundle" | jq -r '.name')

        # Check if we should build this bundle
        if [ -z "$TARGET_BUNDLE" ] || [ "$TARGET_BUNDLE" = "$name" ]; then
            build_bundle "$bundle" "test"
            BUNDLES_BUILT=$((BUNDLES_BUILT + 1))
        fi
    done
fi

# Check if target was found
if [ -n "$TARGET_BUNDLE" ] && [ "$BUNDLES_BUILT" -eq 0 ]; then
    echo -e "${RED}Error: Bundle '${TARGET_BUNDLE}' not found in configuration.${NC}"
    echo ""
    echo "Run './scripts/build-bundles.sh --list' to see available bundles."
    exit 1
fi

echo "=========================================="
echo -e "${GREEN}✓ Bundle build complete!${NC}"
echo "=========================================="
echo ""
