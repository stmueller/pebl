#!/bin/bash
#
# Deploy PEBL Bundles to Online Platform
#
# Copies built bundles from pebl-dev to PEBLOnlinePlatform runtime directory
#
# Usage:
#   ./scripts/deploy-bundles.sh [OPTIONS] <platform_dir> [bundle_name]
#
# Options:
#   -h, --help       Show this help message
#   -n, --dry-run    Show what would be deployed without actually copying
#   -v, --verbose    Verbose output
#
# Arguments:
#   platform_dir     Path to PEBLOnlinePlatform or PEBLOnlinePlatform-dev directory
#   bundle_name      Optional. Deploy only specified bundle (e.g., "core", "pcst")
#                    Without argument: deploys ALL bundles
#
# Examples:
#   ./scripts/deploy-bundles.sh ~/PEBLOnlinePlatform-dev
#   ./scripts/deploy-bundles.sh ~/PEBLOnlinePlatform-dev pcst
#   ./scripts/deploy-bundles.sh --dry-run ~/PEBLOnlinePlatform-dev
#

set -e  # Exit on error

# Parse command line options
VERBOSE=false
DRY_RUN=false
PLATFORM_DIR=""
TARGET_BUNDLE=""

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            head -n 23 "$0" | tail -n +2 | sed 's/^# //' | sed 's/^#//'
            exit 0
            ;;
        -n|--dry-run)
            DRY_RUN=true
            shift
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        *)
            if [ -z "$PLATFORM_DIR" ]; then
                PLATFORM_DIR="$1"
            else
                TARGET_BUNDLE="$1"
            fi
            shift
            ;;
    esac
done

# Check if platform directory provided
if [ -z "$PLATFORM_DIR" ]; then
    echo "Error: Platform directory required"
    echo ""
    echo "Usage: $0 <platform_dir> [bundle_name]"
    echo "Run '$0 --help' for more information"
    exit 1
fi

# Determine directories
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PEBL_DIR="$( cd "$SCRIPT_DIR/.." && pwd )"
CONFIG_FILE="$PEBL_DIR/bundle-config.json"

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

# Validate platform directory
if [ ! -d "$PLATFORM_DIR" ]; then
    echo -e "${RED}Error: Platform directory not found:${NC}"
    echo "  $PLATFORM_DIR"
    exit 1
fi

RUNTIME_DIR="$PLATFORM_DIR/runtime"
if [ ! -d "$RUNTIME_DIR" ]; then
    echo -e "${RED}Error: Runtime directory not found:${NC}"
    echo "  $RUNTIME_DIR"
    echo ""
    echo "Make sure you're pointing to a valid PEBLOnlinePlatform directory."
    exit 1
fi

# Check config file
if [ ! -f "$CONFIG_FILE" ]; then
    echo -e "${RED}Error: bundle-config.json not found at:${NC}"
    echo "  $CONFIG_FILE"
    exit 1
fi

# Check dependencies
if ! command -v jq &> /dev/null; then
    echo -e "${RED}Error: jq is required but not installed.${NC}"
    echo "Install with: sudo apt-get install jq"
    exit 1
fi

# Function to copy bundle files
deploy_bundle() {
    local name="$1"
    local source_dir="$2"
    local dest_dir="$3"
    local bundle_type="$4"

    # Skip if target specified and doesn't match
    if [ -n "$TARGET_BUNDLE" ] && [ "$name" != "$TARGET_BUNDLE" ]; then
        return
    fi

    echo "----------------------------------------"
    echo -e "${BLUE}Deploying: ${CYAN}${name}${NC} (${bundle_type})"
    echo "----------------------------------------"

    local files_copied=0
    local files_missing=0

    # Files to deploy
    local extensions=("data" "js" "js.metadata")

    for ext in "${extensions[@]}"; do
        local source_file="$PEBL_DIR/$source_dir/${name}.${ext}"
        local dest_file="$dest_dir/${name}.${ext}"

        if [ -f "$source_file" ]; then
            local size=$(ls -lh "$source_file" | awk '{print $5}')

            if [ "$DRY_RUN" = true ]; then
                echo -e "  ${CYAN}[DRY RUN]${NC} Would copy: ${name}.${ext} (${size})"
            else
                if [ "$VERBOSE" = true ]; then
                    echo "  Copying: ${name}.${ext} (${size})"
                fi
                cp "$source_file" "$dest_file"
                echo -e "  ${GREEN}✓${NC} Deployed: ${name}.${ext} (${size})"
            fi
            files_copied=$((files_copied + 1))
        else
            echo -e "  ${YELLOW}⚠${NC}  Missing: ${name}.${ext}"
            files_missing=$((files_missing + 1))
        fi
    done

    echo ""

    if [ "$files_missing" -gt 0 ]; then
        echo -e "${YELLOW}Warning: ${files_missing} file(s) missing for bundle '${name}'${NC}"
        echo "Run './scripts/build-bundles.sh ${name}' to build this bundle first."
        echo ""
        return 1
    fi

    return 0
}

# Main deployment
echo "=========================================="
echo "  PEBL Bundle Deployment"
echo "=========================================="
echo ""
echo "Platform: $PLATFORM_DIR"
echo "Runtime:  $RUNTIME_DIR"

if [ "$DRY_RUN" = true ]; then
    echo -e "${CYAN}Mode: DRY RUN (no files will be copied)${NC}"
fi

if [ -n "$TARGET_BUNDLE" ]; then
    echo "Target: $TARGET_BUNDLE"
else
    echo "Deploying: ALL bundles"
fi
echo ""

BUNDLES_DEPLOYED=0
BUNDLES_FAILED=0

# Deploy core bundle
core_bundle=$(jq -c '.bundles.core' "$CONFIG_FILE")
if [ "$core_bundle" != "null" ]; then
    core_name=$(jq -r '.bundles.core.name' "$CONFIG_FILE")
    core_output=$(jq -r '.bundles.core.output_dir' "$CONFIG_FILE")

    if [ -z "$TARGET_BUNDLE" ] || [ "$TARGET_BUNDLE" = "$core_name" ] || [ "$TARGET_BUNDLE" = "core" ]; then
        if deploy_bundle "$core_name" "$core_output" "$RUNTIME_DIR" "core"; then
            BUNDLES_DEPLOYED=$((BUNDLES_DEPLOYED + 1))
        else
            BUNDLES_FAILED=$((BUNDLES_FAILED + 1))
        fi
    fi
fi

# Deploy core battery bundle
core_battery_bundle=$(jq -c '.bundles.core_battery' "$CONFIG_FILE")
if [ "$core_battery_bundle" != "null" ]; then
    core_battery_name=$(jq -r '.bundles.core_battery.name' "$CONFIG_FILE")
    core_battery_output=$(jq -r '.bundles.core_battery.output_dir' "$CONFIG_FILE")

    if [ -z "$TARGET_BUNDLE" ] || [ "$TARGET_BUNDLE" = "$core_battery_name" ] || [ "$TARGET_BUNDLE" = "core-battery" ] || [ "$TARGET_BUNDLE" = "core_battery" ]; then
        if deploy_bundle "$core_battery_name" "$core_battery_output" "$RUNTIME_DIR" "battery"; then
            BUNDLES_DEPLOYED=$((BUNDLES_DEPLOYED + 1))
        else
            BUNDLES_FAILED=$((BUNDLES_FAILED + 1))
        fi
    fi
fi

# Deploy test bundles
test_bundle_count=$(jq '.bundles.test_bundles | length' "$CONFIG_FILE")
if [ "$test_bundle_count" -gt 0 ]; then
    # Create test-bundles subdirectory if it doesn't exist
    TEST_BUNDLES_DIR="$RUNTIME_DIR/test-bundles"
    if [ ! -d "$TEST_BUNDLES_DIR" ] && [ "$DRY_RUN" = false ]; then
        mkdir -p "$TEST_BUNDLES_DIR"
        echo -e "${GREEN}✓${NC} Created directory: runtime/test-bundles/"
        echo ""
    fi

    for i in $(seq 0 $(($test_bundle_count - 1))); do
        bundle=$(jq -c ".bundles.test_bundles[$i]" "$CONFIG_FILE")
        name=$(echo "$bundle" | jq -r '.name')
        output_dir=$(echo "$bundle" | jq -r '.output_dir')

        # Check if we should deploy this bundle
        if [ -z "$TARGET_BUNDLE" ] || [ "$TARGET_BUNDLE" = "$name" ]; then
            if deploy_bundle "$name" "$output_dir" "$TEST_BUNDLES_DIR" "test"; then
                BUNDLES_DEPLOYED=$((BUNDLES_DEPLOYED + 1))
            else
                BUNDLES_FAILED=$((BUNDLES_FAILED + 1))
            fi
        fi
    done
fi

# Check if target was found
if [ -n "$TARGET_BUNDLE" ] && [ "$BUNDLES_DEPLOYED" -eq 0 ] && [ "$BUNDLES_FAILED" -eq 0 ]; then
    echo -e "${RED}Error: Bundle '${TARGET_BUNDLE}' not found in configuration.${NC}"
    echo ""
    echo "Run './scripts/build-bundles.sh --list' to see available bundles."
    exit 1
fi

echo "=========================================="
if [ "$DRY_RUN" = true ]; then
    echo -e "${CYAN}Dry run complete - no files were copied${NC}"
elif [ "$BUNDLES_FAILED" -eq 0 ]; then
    echo -e "${GREEN}✓ Deployment complete!${NC}"
else
    echo -e "${YELLOW}⚠ Deployment completed with warnings${NC}"
    echo "  Deployed: $BUNDLES_DEPLOYED"
    echo "  Failed:   $BUNDLES_FAILED"
fi
echo "=========================================="
echo ""

exit 0
