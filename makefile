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

# ==== Executables ====
PREPROCESSOR_TEST_EXEC = $(BUILD_DIR)/preprocessor_test
SYMBOL_TABLE_TEST_EXEC = $(BUILD_DIR)/test_symbol_table

# ==== Object Files ====
TEST_PREPROCESSOR_OBJ = $(BUILD_DIR)/test_preprocessor.o
PREPROCESSOR_OBJ = $(BUILD_DIR)/preprocessor.o
TEST_SYMBOL_TABLE_OBJ = $(BUILD_DIR)/test_symbol_table.o
SYMBOL_TABLE_OBJ = $(BUILD_DIR)/symbol_table.o
ERROR_OBJ = $(BUILD_DIR)/error.o

# ==== Platform-aware executable suffix ====
ifeq ($(OS),Windows_NT)
	EXE_EXT = .exe
else
	EXE_EXT =
endif

.PHONY: all clean run_test run_test_symbol_table

# ==== Build All ====
all: $(PREPROCESSOR_TEST_EXEC) $(SYMBOL_TABLE_TEST_EXEC)

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

# ==== Executable Build Rules ====
$(PREPROCESSOR_TEST_EXEC): $(TEST_PREPROCESSOR_OBJ) $(PREPROCESSOR_OBJ) $(ERROR_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

$(SYMBOL_TABLE_TEST_EXEC): $(TEST_SYMBOL_TABLE_OBJ) $(SYMBOL_TABLE_OBJ) $(ERROR_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# ==== Run Targets ====
run_test: $(PREPROCESSOR_TEST_EXEC)
	./$(PREPROCESSOR_TEST_EXEC)

run_test_symbol_table: $(SYMBOL_TABLE_TEST_EXEC)
	./$(SYMBOL_TABLE_TEST_EXEC)

# ==== Clean ====
clean:
	rm -f $(BUILD_DIR)/*.o $(PREPROCESSOR_TEST_EXEC)* $(SYMBOL_TABLE_TEST_EXEC)* tests/*.am
