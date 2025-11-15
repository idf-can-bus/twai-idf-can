#!/bin/bash

# Set target chip for all examples
# Usage: ./set_target_all.sh esp32s3

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
EXAMPLES_DIR="${SCRIPT_DIR}/examples"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Target chip (first argument)
TARGET="${1:-}"

# Check if target is provided
if [ -z "$TARGET" ]; then
    echo -e "${RED}Error: Target chip not specified${NC}"
    echo ""
    echo "Usage: $0 <target>"
    echo ""
    echo "Available targets:"
    echo "  esp32      - Original ESP32"
    echo "  esp32s2    - ESP32-S2"
    echo "  esp32s3    - ESP32-S3"
    echo "  esp32c3    - ESP32-C3"
    echo "  esp32c6    - ESP32-C6"
    echo "  esp32h2    - ESP32-H2"
    echo ""
    echo "Example: $0 esp32s3"
    exit 1
fi

# Check if IDF_PATH is set
if [ -z "$IDF_PATH" ]; then
    echo -e "${RED}Error: IDF_PATH is not set. Please source ESP-IDF environment first.${NC}"
    echo "Run: . \$HOME/esp/esp-idf/export.sh"
    exit 1
fi

# Find all example directories
EXAMPLES=(
    "send"
    "receive_poll"
    "receive_interrupt"
)

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Setting target: ${TARGET}${NC}"
echo -e "${BLUE}For all TWAI-IDF-CAN examples${NC}"
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
    
    echo -e "${BLUE}Setting target for: ${example}${NC}"
    
    cd "$EXAMPLE_PATH"
    
    # Remove build directory if it exists (set-target needs clean state)
    if [ -d "build" ]; then
        echo -e "${YELLOW}  Removing existing build directory...${NC}"
        rm -rf build
    fi
    
    # Run set-target (don't exit on error, continue with other examples)
    if idf.py set-target "$TARGET" 2>&1; then
        echo -e "${GREEN}✓ Success: ${example} → ${TARGET}${NC}"
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
echo -e "${BLUE}Summary${NC}"
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
    echo -e "${GREEN}All examples configured for ${TARGET}!${NC}"
    echo ""
    echo -e "${BLUE}Next step: Build the examples${NC}"
    echo "  ./build_all_examples.sh"
    exit 0
fi

