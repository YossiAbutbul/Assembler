# Compiler and flags
CC = gcc
CFLAGS = -ansi -pedantic -Wall -Werror -m32

# Directories
SRC_DIR = src
BUILD_DIR = build
TEST_DIR = tests

# Executables
EXEC = assembler
TEST_EXEC = preprocessor_test

# Source and object files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES))

# Default build target
all: $(EXEC)

# Main executable build
$(EXEC): $(OBJ_FILES) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Object file compilation into build/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Create build directory if not exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# --------------------------
# Unit Test for Preprocessor
# --------------------------
# Test source files: test_preprocessor + preprocessor module
TEST_SRC = $(TEST_DIR)/test_preprocessor.c $(SRC_DIR)/preprocessor.c
TEST_OBJ = $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(TEST_SRC)))

# Compile test object files
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Build the preprocessor unit test
test_preprocessor: $(BUILD_DIR)/test_preprocessor.o $(BUILD_DIR)/preprocessor.o
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(TEST_EXEC) $^

# Run the unit test
run_test: test_preprocessor
	./$(BUILD_DIR)/$(TEST_EXEC)

# Cleanup
clean:
	rm -rf $(BUILD_DIR) *.am *.ob *.ent *.ext $(EXEC)

.PHONY: all clean test_preprocessor run_test
