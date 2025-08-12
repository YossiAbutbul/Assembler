CFLAGS = -g -m32 -std=c90 -pedantic -Wall -ansi
BUILD_DIR = build

# Object files in build directory
OBJS = $(BUILD_DIR)/assembler.o $(BUILD_DIR)/main.o $(BUILD_DIR)/preprocessor.o $(BUILD_DIR)/first_pass.o $(BUILD_DIR)/second_pass.o $(BUILD_DIR)/symbol_table.o $(BUILD_DIR)/utils.o $(BUILD_DIR)/data_image.o $(BUILD_DIR)/data_parser.o $(BUILD_DIR)/instruction_parser.o $(BUILD_DIR)/output.o $(BUILD_DIR)/error.o

# Create build directory and build assembler
$(BUILD_DIR)/assembler: $(BUILD_DIR) $(OBJS)
	gcc $(CFLAGS) $(OBJS) -o $(BUILD_DIR)/assembler

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/assembler.o: src/assembler.c include/assembler.h include/preprocessor.h include/first_pass.h include/second_pass.h include/output.h include/symbol_table.h include/data_image.h include/error.h include/constants.h
	gcc $(CFLAGS) -c src/assembler.c -o $(BUILD_DIR)/assembler.o

$(BUILD_DIR)/main.o: src/main.c include/assembler.h include/error.h include/constants.h
	gcc $(CFLAGS) -c src/main.c -o $(BUILD_DIR)/main.o

$(BUILD_DIR)/preprocessor.o: src/preprocessor.c include/preprocessor.h include/error.h include/constants.h
	gcc $(CFLAGS) -c src/preprocessor.c -o $(BUILD_DIR)/preprocessor.o

$(BUILD_DIR)/first_pass.o: src/first_pass.c include/first_pass.h include/symbol_table.h include/utils.h include/error.h include/constants.h include/data_parser.h include/instruction_parser.h
	gcc $(CFLAGS) -c src/first_pass.c -o $(BUILD_DIR)/first_pass.o

$(BUILD_DIR)/second_pass.o: src/second_pass.c include/second_pass.h include/symbol_table.h include/utils.h include/error.h include/constants.h include/instruction_parser.h include/first_pass.h include/assembler_types.h
	gcc $(CFLAGS) -c src/second_pass.c -o $(BUILD_DIR)/second_pass.o

$(BUILD_DIR)/symbol_table.o: src/symbol_table.c include/symbol_table.h include/error.h include/constants.h
	gcc $(CFLAGS) -c src/symbol_table.c -o $(BUILD_DIR)/symbol_table.o

$(BUILD_DIR)/utils.o: src/utils.c include/utils.h include/constants.h include/symbol_table.h
	gcc $(CFLAGS) -c src/utils.c -o $(BUILD_DIR)/utils.o

$(BUILD_DIR)/data_image.o: src/data_image.c include/data_image.h include/constants.h include/error.h include/first_pass.h
	gcc $(CFLAGS) -c src/data_image.c -o $(BUILD_DIR)/data_image.o

$(BUILD_DIR)/data_parser.o: src/data_parser.c include/data_parser.h include/error.h include/constants.h include/first_pass.h include/data_image.h
	gcc $(CFLAGS) -c src/data_parser.c -o $(BUILD_DIR)/data_parser.o

$(BUILD_DIR)/instruction_parser.o: src/instruction_parser.c include/instruction_parser.h include/utils.h include/error.h include/first_pass.h include/constants.h
	gcc $(CFLAGS) -c src/instruction_parser.c -o $(BUILD_DIR)/instruction_parser.o

$(BUILD_DIR)/output.o: src/output.c include/output.h include/error.h include/constants.h include/data_image.h include/first_pass.h include/second_pass.h include/assembler_types.h
	gcc $(CFLAGS) -c src/output.c -o $(BUILD_DIR)/output.o

$(BUILD_DIR)/error.o: src/error.c include/error.h include/constants.h
	gcc $(CFLAGS) -c src/error.c -o $(BUILD_DIR)/error.o

# Default target
assembler: $(BUILD_DIR)/assembler

clean:
	rm -rf $(BUILD_DIR)