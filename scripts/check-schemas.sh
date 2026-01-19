#!/bin/bash
#
# check-schemas.sh
# Verify all battery tests have parameter schema files
#
# Usage: ./check-schemas.sh [directory]
#        ./check-schemas.sh upload-battery

set -e

BATTERY_DIR="${1:-upload-battery}"

if [ ! -d "$BATTERY_DIR" ]; then
    echo "Error: Directory not found: $BATTERY_DIR"
    exit 1
fi

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "========================================"
echo "  Schema File Checker"
echo "========================================"
echo "Checking: $BATTERY_DIR"
echo ""

MISSING_COUNT=0
PRESENT_COUNT=0
INVALID_COUNT=0

for dir in "$BATTERY_DIR"/*/; do
    if [ ! -d "$dir" ]; then
        continue
    fi

    testname=$(basename "$dir")

    # Find the .pbl file (might not match directory name)
    pbl_file=$(find "$dir" -maxdepth 1 -name "*.pbl" -type f | head -1)

    # Skip if no .pbl file (not a test directory)
    if [ -z "$pbl_file" ] || [ ! -f "$pbl_file" ]; then
        continue
    fi

    # Schema filename must match .pbl filename (not directory name)
    pbl_basename=$(basename "$pbl_file")
    schema="$dir/${pbl_basename}.schema.json"

    if [ ! -f "$schema" ]; then
        echo -e "${RED}✗ Missing schema: $testname (${pbl_basename})${NC}"
        echo -e "   Expected: ${schema}"
        MISSING_COUNT=$((MISSING_COUNT + 1))
    else
        # Check if schema is valid JSON
        if [ ! -s "$schema" ]; then
            echo -e "${YELLOW}⚠ Empty schema: $testname${NC}"
            INVALID_COUNT=$((INVALID_COUNT + 1))
        else
            # Check for JSON format (basic check for "parameters" key)
            if ! grep -q '"parameters"' "$schema"; then
                echo -e "${YELLOW}⚠ Invalid format (missing 'parameters' key): $testname${NC}"
                INVALID_COUNT=$((INVALID_COUNT + 1))
            else
                # Count parameters in JSON
                param_count=$(grep -o '"name":' "$schema" | wc -l)
                echo -e "${GREEN}✓ Has schema: $testname (${pbl_basename}, $param_count parameters)${NC}"
                PRESENT_COUNT=$((PRESENT_COUNT + 1))
            fi
        fi
    fi
done

echo ""
echo "========================================"
echo "Summary:"
echo "  ✓ Valid schemas:   $PRESENT_COUNT"
if [ $INVALID_COUNT -gt 0 ]; then
    echo -e "  ${YELLOW}⚠ Invalid schemas: $INVALID_COUNT${NC}"
fi
if [ $MISSING_COUNT -gt 0 ]; then
    echo -e "  ${RED}✗ Missing schemas: $MISSING_COUNT${NC}"
fi
echo "========================================"

if [ $MISSING_COUNT -gt 0 ] || [ $INVALID_COUNT -gt 0 ]; then
    echo ""
    echo "Action needed: Create or fix schema files for tests listed above."
    echo "Schema format: parameter_name|default_value|description"
    exit 1
else
    echo ""
    echo "All tests have valid schema files!"
    exit 0
fi
