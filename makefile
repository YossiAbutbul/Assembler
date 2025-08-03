CC = gcc
CFLAGS = -ansi -pedantic -Wall -Werror -m32
INCLUDES = -Iinclude
BUILD_DIR = build

# ==== Sources ====
TEST_PREPROCESSOR_SRC = tests/test_preprocessor.c
TEST_SYMBOL_TABLE_SRC = tests/test_symbol_table.c
PREPROCESSOR_SRC = src/preprocessor.c
SYMBOL_TABLE_SRC = src/symbol_table.c
ERROR_SRC = src/error.c
OUTPUT_SRC = src/output.c
UTILS_SRC = src/utils.c
DATA_IMAGE_SRC = src/data_image.c
DATA_PARSER_SRC = src/data_parser.c
INSTRUCTION_PARSER_SRC = src/instruction_parser.c
FIRST_PASS_SRC = src/first_pass.c
SECOND_PASS_SRC = src/second_pass.c
ASSEMBLER_SRC = src/assembler.c
MAIN_SRC = src/main.c

# ==== Test Executables ====
PREPROCESSOR_TEST_EXEC = $(BUILD_DIR)/preprocessor_test
SYMBOL_TABLE_TEST_EXEC = $(BUILD_DIR)/test_symbol_table
OUTPUT_TEST_EXEC = $(BUILD_DIR)/output_test

# ==== Main Assembler Executable ====
ASSEMBLER_EXEC = $(BUILD_DIR)/assembler

# ==== Object Files ====
TEST_PREPROCESSOR_OBJ = $(BUILD_DIR)/test_preprocessor.o
PREPROCESSOR_OBJ = $(BUILD_DIR)/preprocessor.o
TEST_SYMBOL_TABLE_OBJ = $(BUILD_DIR)/test_symbol_table.o
SYMBOL_TABLE_OBJ = $(BUILD_DIR)/symbol_table.o
ERROR_OBJ = $(BUILD_DIR)/error.o
OUTPUT_OBJ = $(BUILD_DIR)/output.o
UTILS_OBJ = $(BUILD_DIR)/utils.o
DATA_IMAGE_OBJ = $(BUILD_DIR)/data_image.o
DATA_PARSER_OBJ = $(BUILD_DIR)/data_parser.o
INSTRUCTION_PARSER_OBJ = $(BUILD_DIR)/instruction_parser.o
FIRST_PASS_OBJ = $(BUILD_DIR)/first_pass.o
SECOND_PASS_OBJ = $(BUILD_DIR)/second_pass.o
ASSEMBLER_OBJ = $(BUILD_DIR)/assembler.o
MAIN_OBJ = $(BUILD_DIR)/main.o

# ==== Core assembler objects (used by main assembler) ====
CORE_OBJS = $(PREPROCESSOR_OBJ) $(SYMBOL_TABLE_OBJ) $(ERROR_OBJ) $(OUTPUT_OBJ) \
           $(UTILS_OBJ) $(DATA_IMAGE_OBJ) $(DATA_PARSER_OBJ) \
           $(INSTRUCTION_PARSER_OBJ) $(FIRST_PASS_OBJ) $(SECOND_PASS_OBJ) \
           $(ASSEMBLER_OBJ)

# ==== Platform-aware executable suffix ====
ifeq ($(OS),Windows_NT)
	EXE_EXT = .exe
else
	EXE_EXT =
endif

.PHONY: all clean run_test run_test_symbol_table run_test_output test assembler

# ==== Build All ====
all: $(ASSEMBLER_EXEC) $(PREPROCESSOR_TEST_EXEC) $(SYMBOL_TABLE_TEST_EXEC) $(OUTPUT_TEST_EXEC)

# ==== Main Assembler Target ====
assembler: $(ASSEMBLER_EXEC)

# ==== Test Targets ====
test: $(PREPROCESSOR_TEST_EXEC) $(SYMBOL_TABLE_TEST_EXEC) $(OUTPUT_TEST_EXEC)

# ==== Object Compilation Rules ====
$(BUILD_DIR)/test_preprocessor.o: $(TEST_PREPROCESSOR_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/preprocessor.o: $(PREPROCESSOR_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/test_symbol_table.o: $(TEST_SYMBOL_TABLE_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/symbol_table.o: $(SYMBOL_TABLE_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/error.o: $(ERROR_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/output.o: $(OUTPUT_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/utils.o: $(UTILS_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/data_image.o: $(DATA_IMAGE_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/data_parser.o: $(DATA_PARSER_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/instruction_parser.o: $(INSTRUCTION_PARSER_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/first_pass.o: $(FIRST_PASS_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/second_pass.o: $(SECOND_PASS_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/assembler.o: $(ASSEMBLER_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/main.o: $(MAIN_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# ==== Executable Build Rules ====

# Main Assembler
$(ASSEMBLER_EXEC): $(CORE_OBJS) $(MAIN_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# Test Executables
$(PREPROCESSOR_TEST_EXEC): $(TEST_PREPROCESSOR_OBJ) $(PREPROCESSOR_OBJ) $(ERROR_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

$(SYMBOL_TABLE_TEST_EXEC): $(TEST_SYMBOL_TABLE_OBJ) $(SYMBOL_TABLE_OBJ) $(ERROR_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

$(OUTPUT_TEST_EXEC): tests/test_output.c $(OUTPUT_OBJ) $(ERROR_OBJ) $(DATA_IMAGE_OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

# ==== Run Targets ====
run_test: $(PREPROCESSOR_TEST_EXEC)
	./$(PREPROCESSOR_TEST_EXEC)

run_test_symbol_table: $(SYMBOL_TABLE_TEST_EXEC)
	./$(SYMBOL_TABLE_TEST_EXEC)

run_test_output: $(OUTPUT_TEST_EXEC)
	./$(OUTPUT_TEST_EXEC)

# ==== Run All Tests ====
run_all_tests: run_test run_test_symbol_table run_test_output

# ==== Clean ====
clean:
	rm -f $(BUILD_DIR)/*.o $(ASSEMBLER_EXEC)* $(PREPROCESSOR_TEST_EXEC)* \
	      $(SYMBOL_TABLE_TEST_EXEC)* $(OUTPUT_TEST_EXEC)* tests/*.am tests/*.ob \
	      tests/*.ent tests/*.ext

# ==== Create build directory if it doesn't exist ====
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# ==== Ensure build directory exists for all object files ====
$(BUILD_DIR)/%.o: | $(BUILD_DIR)