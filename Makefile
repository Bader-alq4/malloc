# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -g -O0

# Project Structure
SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build

# Source files
SRCS = $(SRC_DIR)/malloc.c \
       $(SRC_DIR)/free.c \

# Object files
OBJS = $(BUILD_DIR)/malloc.o \
       $(BUILD_DIR)/free.o \

# Output library
LIB = $(BUILD_DIR)/liballocator.a

# Default target: build everything
all: $(LIB)

# Build static library
$(LIB): $(OBJS)
	ar rcs $(LIB) $(OBJS)
	@echo "Built static library: $(LIB)"

# Compile object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# TEST TARGETS
# Build and run each test

test-coalesce: $(LIB)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/test_coalesce \
		$(TEST_DIR)/test_coalesce.c $(LIB)
	@echo "Running coalesce test..."
	$(BUILD_DIR)/test_coalesce

test-stress: $(LIB)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/test_stress \
		$(TEST_DIR)/test_stress.c $(LIB)
	@echo "Running stress test..."
	$(BUILD_DIR)/test_stress

# Run ALL tests
test: test-coalesce test-stress

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	@echo "Cleaned build artifacts."

# PHONY targets (not actual files)
.PHONY: all clean test test-coalesce test-stress