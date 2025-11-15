#!/bin/bash

# Build all examples for twai-idf-can project
# Usage: ./build_all_examples.sh [clean|build|flash|monitor|menuconfig]

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
EXAMPLES_DIR="${SCRIPT_DIR}/examples"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default action
ACTION="${1:-build}"

# Check if IDF_PATH is set
if [ -z "$IDF_PATH" ]; then
    echo -e "${RED}Error: IDF_PATH is not set. Please source ESP-IDF environment first.${NC}"
    echo "Run: . \$HOME/esp/esp-idf/export.sh"
    exit 1
fi

# Find all example directories (those with CMakeLists.txt)
EXAMPLES=(
    "send"
    "receive_poll"
    "receive_interrupt"
)

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Building all TWAI-IDF-CAN examples${NC}"
echo -e "${BLUE}Action: ${ACTION}${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

SUCCESS_COUNT=0
FAIL_COUNT=0
FAILED_EXAMPLES=()

for example in "${EXAMPLES[@]}"; do
    EXAMPLE_PATH="${EXAMPLES_DIR}/${example}"
    
    if [ ! -d "$EXAMPLE_PATH" ]; then
        echo -e "${YELLOW}Warning: Example directory not found: ${example}${NC}"
        continue
    fi
    
    if [ ! -f "${EXAMPLE_PATH}/CMakeLists.txt" ]; then
        echo -e "${YELLOW}Warning: No CMakeLists.txt in: ${example}${NC}"
        continue
    fi
    
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}Processing: ${example}${NC}"
    echo -e "${BLUE}========================================${NC}"
    
    cd "$EXAMPLE_PATH"
    
    # Check if target is set (sdkconfig.defaults or sdkconfig exists)
    if [ ! -f "sdkconfig" ] && [ "$ACTION" = "build" ]; then
        echo -e "${YELLOW}Warning: Target not set for ${example}${NC}"
        echo -e "${YELLOW}Run: ./set_target_all.sh esp32s3${NC}"
        echo -e "${YELLOW}(or your chip: esp32, esp32s2, esp32c3, esp32c6, etc.)${NC}"
        echo ""
    fi
    
    # Run idf.py command (don't exit on error, continue with other examples)
    if idf.py $ACTION 2>&1; then
        echo -e "${GREEN}✓ Success: ${example}${NC}"
        echo ""
        ((SUCCESS_COUNT++))
    else
        echo -e "${RED}✗ Failed: ${example}${NC}"
        echo ""
        ((FAIL_COUNT++))
        FAILED_EXAMPLES+=("$example")
    fi
done

cd "$SCRIPT_DIR"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Build Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "${GREEN}Successful: ${SUCCESS_COUNT}${NC}"
echo -e "${RED}Failed: ${FAIL_COUNT}${NC}"

if [ ${FAIL_COUNT} -gt 0 ]; then
    echo -e "${RED}Failed examples:${NC}"
    for failed in "${FAILED_EXAMPLES[@]}"; do
        echo -e "${RED}  - ${failed}${NC}"
    done
    exit 1
else
    echo -e "${GREEN}All examples built successfully!${NC}"
    exit 0
fi

