# Makefile for building all TWAI-IDF-CAN examples
# Usage: make [all|clean|flash|monitor|menuconfig]

# Find examples directory
EXAMPLES_DIR := examples
EXAMPLES := send receive_poll receive_interrupt

# Colors
RED := \033[0;31m
GREEN := \033[0;32m
YELLOW := \033[1;33m
BLUE := \033[0;34m
NC := \033[0m # No Color

.PHONY: all clean flash monitor menuconfig help $(EXAMPLES)

# Default target
all: build

# Build all examples
build:
	@echo "$(BLUE)========================================"
	@echo "Building all TWAI-IDF-CAN examples"
	@echo "========================================$(NC)"
	@for example in $(EXAMPLES); do \
		echo "$(BLUE)Building: $$example$(NC)"; \
		cd $(EXAMPLES_DIR)/$$example && idf.py build || exit 1; \
		cd ../..; \
		echo "$(GREEN)✓ Success: $$example$(NC)"; \
		echo ""; \
	done
	@echo "$(GREEN)All examples built successfully!$(NC)"

# Clean all examples
clean:
	@echo "$(BLUE)========================================"
	@echo "Cleaning all TWAI-IDF-CAN examples"
	@echo "========================================$(NC)"
	@for example in $(EXAMPLES); do \
		echo "$(BLUE)Cleaning: $$example$(NC)"; \
		cd $(EXAMPLES_DIR)/$$example && idf.py fullclean || true; \
		cd ../..; \
		echo "$(GREEN)✓ Cleaned: $$example$(NC)"; \
		echo ""; \
	done
	@echo "$(GREEN)All examples cleaned!$(NC)"

# Individual example targets
send:
	@echo "$(BLUE)Building: send$(NC)"
	@cd $(EXAMPLES_DIR)/send && idf.py build

receive_poll:
	@echo "$(BLUE)Building: receive_poll$(NC)"
	@cd $(EXAMPLES_DIR)/receive_poll && idf.py build

receive_interrupt:
	@echo "$(BLUE)Building: receive_interrupt$(NC)"
	@cd $(EXAMPLES_DIR)/receive_interrupt && idf.py build

# Help target
help:
	@echo "$(BLUE)TWAI-IDF-CAN Examples Build System$(NC)"
	@echo ""
	@echo "Available targets:"
	@echo "  $(GREEN)make$(NC) or $(GREEN)make build$(NC)      - Build all examples"
	@echo "  $(GREEN)make clean$(NC)              - Clean all examples"
	@echo "  $(GREEN)make send$(NC)               - Build only send example"
	@echo "  $(GREEN)make receive_poll$(NC)       - Build only receive_poll example"
	@echo "  $(GREEN)make receive_interrupt$(NC)  - Build only receive_interrupt example"
	@echo "  $(GREEN)make help$(NC)               - Show this help message"
	@echo ""
	@echo "For individual example operations (flash, monitor, menuconfig):"
	@echo "  cd examples/<example_name>"
	@echo "  idf.py flash monitor"

