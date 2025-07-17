CC = gcc
CFLAGS = -ansi -pedantic -Wall -Werror -m32
INCLUDES = -Iinclude
BUILD_DIR = build
TEST_SRC = tests/test_preprocessor.c
PREPROCESSOR_SRC = src/preprocessor.c

TEST_OBJ = $(BUILD_DIR)/test_preprocessor.o
PREPROCESSOR_OBJ = $(BUILD_DIR)/preprocessor.o
EXEC = $(BUILD_DIR)/preprocessor_test

.PHONY: all clean run_test

all: $(EXEC)

$(EXEC): $(TEST_OBJ) $(PREPROCESSOR_OBJ)
	$(CC) $(CFLAGS) $(TEST_OBJ) $(PREPROCESSOR_OBJ) -o $@

$(BUILD_DIR)/test_preprocessor.o: $(TEST_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/preprocessor.o: $(PREPROCESSOR_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

run_test: $(EXEC)
	./$(EXEC)

clean:
	rm -f $(BUILD_DIR)/*.o $(EXEC) tests/*.am
