#!/bin/bash
#
# check-offline-language.sh
# Scan battery test for offline-specific language that needs updating
#
# Usage: ./check-offline-language.sh testname
#        ./check-offline-language.sh upload-battery/stroop

set -e

TEST_DIR=$1

if [ -z "$TEST_DIR" ]; then
    echo "Usage: $0 <test-directory>"
    echo "Example: $0 upload-battery/stroop"
    exit 1
fi

if [ ! -d "$TEST_DIR" ]; then
    echo "Error: Directory not found: $TEST_DIR"
    exit 1
fi

# Color codes
RED='\033[0;31m'
YELLOW='\033[1;33m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

echo "========================================"
echo "  Offline Language Scanner"
echo "========================================"
echo "Scanning: $TEST_DIR"
echo ""

ISSUES_FOUND=0

# Define patterns to search for
PATTERNS=(
    "experimenter"
    "raise your hand"
    "ask questions"
    "alert.*when"
    "wait for the"
    "tell.*experimenter"
    "laboratory"
    "lab setting"
    "press.*when.*says"
    "contact.*researcher.*person"
)

echo -e "${YELLOW}=== Main test file (.pbl) ===${NC}"
for pattern in "${PATTERNS[@]}"; do
    results=$(grep -Hni "$pattern" "$TEST_DIR"/*.pbl 2>/dev/null || true)
    if [ -n "$results" ]; then
        echo -e "${RED}Found '$pattern':${NC}"
        echo "$results"
        echo ""
        ISSUES_FOUND=$((ISSUES_FOUND + 1))
    fi
done

echo -e "${YELLOW}=== Translation files (.json) ===${NC}"
if [ -d "$TEST_DIR/translations" ]; then
    for pattern in "${PATTERNS[@]}"; do
        results=$(grep -Hni "$pattern" "$TEST_DIR"/translations/*.json 2>/dev/null || true)
        if [ -n "$results" ]; then
            echo -e "${RED}Found '$pattern':${NC}"
            echo "$results"
            echo ""
            ISSUES_FOUND=$((ISSUES_FOUND + 1))
        fi
    done
else
    echo "  (No translations directory found)"
fi

echo -e "${YELLOW}=== About/description file ===${NC}"
results=$(grep -Hni "experimenter\|laboratory\|lab setting" "$TEST_DIR"/*.about.txt 2>/dev/null || true)
if [ -n "$results" ]; then
    echo -e "${RED}Found offline context:${NC}"
    echo "$results"
    echo ""
    ISSUES_FOUND=$((ISSUES_FOUND + 1))
fi

echo ""
echo "========================================"
if [ $ISSUES_FOUND -eq 0 ]; then
    echo -e "${GREEN}✓ No offline-specific language found${NC}"
else
    echo -e "${RED}✗ Found $ISSUES_FOUND potential issues${NC}"
    echo ""
    echo "Review the above findings and update as needed for online context."
fi
echo "========================================"
