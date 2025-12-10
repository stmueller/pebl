#!/bin/bash
#
# migration-status.sh
# Check migration status of battery tests
#
# Usage: ./migration-status.sh

set -e

NATIVE_BATTERY="battery"
ONLINE_BATTERY="upload-battery"

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo "========================================"
echo "  PEBL Battery Migration Status"
echo "========================================"
echo ""

# Count tests
NATIVE_COUNT=0
ONLINE_COUNT=0
MISSING_SCHEMA=0
HAS_MIGRATION_NOTES=0

# Check native battery exists
if [ ! -d "$NATIVE_BATTERY" ]; then
    echo -e "${RED}Error: Native battery directory not found: $NATIVE_BATTERY${NC}"
    exit 1
fi

# Count native tests
for dir in "$NATIVE_BATTERY"/*/; do
    if [ -d "$dir" ] && [ -f "$dir/$(basename "$dir").pbl" ]; then
        NATIVE_COUNT=$((NATIVE_COUNT + 1))
    fi
done

echo -e "${BLUE}Native battery tests: $NATIVE_COUNT${NC}"

# Create online battery directory if it doesn't exist
if [ ! -d "$ONLINE_BATTERY" ]; then
    echo -e "${YELLOW}Online battery directory not found, creating...${NC}"
    mkdir -p "$ONLINE_BATTERY"
fi

# Check migration status
echo ""
echo "Migration Status:"
echo "----------------"

for native_dir in "$NATIVE_BATTERY"/*/; do
    if [ ! -d "$native_dir" ]; then
        continue
    fi

    testname=$(basename "$native_dir")
    native_pbl="$native_dir/${testname}.pbl"

    # Skip if not a test directory
    if [ ! -f "$native_pbl" ]; then
        continue
    fi

    online_dir="$ONLINE_BATTERY/$testname"
    online_pbl="$online_dir/${testname}.pbl"
    schema="$online_dir/${testname}.pbl.schema"
    migration_notes="$online_dir/MIGRATION_NOTES.txt"

    if [ -d "$online_dir" ] && [ -f "$online_pbl" ]; then
        ONLINE_COUNT=$((ONLINE_COUNT + 1))

        # Check for schema
        if [ ! -f "$schema" ]; then
            echo -e "${YELLOW}⚠ $testname - Migrated but MISSING SCHEMA${NC}"
            MISSING_SCHEMA=$((MISSING_SCHEMA + 1))
        elif [ -f "$migration_notes" ]; then
            echo -e "${GREEN}✓ $testname - Fully migrated with notes${NC}"
            HAS_MIGRATION_NOTES=$((HAS_MIGRATION_NOTES + 1))
        else
            echo -e "${GREEN}✓ $testname - Migrated (no migration notes)${NC}"
        fi
    else
        echo -e "${RED}✗ $testname - Not yet migrated${NC}"
    fi
done

# Summary
echo ""
echo "========================================"
echo "Summary:"
echo "========================================"
echo -e "Native tests:          ${BLUE}$NATIVE_COUNT${NC}"
echo -e "Migrated tests:        ${GREEN}$ONLINE_COUNT${NC}"
echo -e "Not yet migrated:      ${RED}$((NATIVE_COUNT - ONLINE_COUNT))${NC}"
echo ""
echo -e "Migration completion:  ${GREEN}$(( (ONLINE_COUNT * 100) / NATIVE_COUNT ))%${NC}"
echo ""

if [ $MISSING_SCHEMA -gt 0 ]; then
    echo -e "${YELLOW}⚠ Tests missing schemas: $MISSING_SCHEMA${NC}"
fi

echo -e "Tests with migration notes: $HAS_MIGRATION_NOTES / $ONLINE_COUNT"
echo ""

# List not yet migrated (first 10)
NOT_MIGRATED=0
echo "Next tests to migrate:"
echo "---------------------"
for native_dir in "$NATIVE_BATTERY"/*/; do
    if [ ! -d "$native_dir" ]; then
        continue
    fi

    testname=$(basename "$native_dir")
    native_pbl="$native_dir/${testname}.pbl"

    if [ ! -f "$native_pbl" ]; then
        continue
    fi

    online_dir="$ONLINE_BATTERY/$testname"
    online_pbl="$online_dir/${testname}.pbl"

    if [ ! -f "$online_pbl" ]; then
        echo "  • $testname"
        NOT_MIGRATED=$((NOT_MIGRATED + 1))

        if [ $NOT_MIGRATED -ge 10 ]; then
            echo "  ... and $((NATIVE_COUNT - ONLINE_COUNT - 10)) more"
            break
        fi
    fi
done

echo ""
echo "========================================"
