CFLAGS = -g -m32 -std=c90 -pedantic -Wall -ansi
BUILD_DIR = build

# Create build directory and build assembler
$(BUILD_DIR)/assembler: $(BUILD_DIR) assembler.o main.o preprocessor.o first_pass.o second_pass.o symbol_table.o utils.o data_image.o data_parser.o instruction_parser.o output.o error.o
	gcc $(CFLAGS) assembler.o main.o preprocessor.o first_pass.o second_pass.o symbol_table.o utils.o data_image.o data_parser.o instruction_parser.o output.o error.o -o $(BUILD_DIR)/assembler

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

assembler.o: src/assembler.c include/assembler.h include/preprocessor.h include/first_pass.h include/second_pass.h include/output.h include/symbol_table.h include/data_image.h include/error.h include/constants.h
	gcc $(CFLAGS) -c src/assembler.c

main.o: src/main.c include/assembler.h include/error.h include/constants.h
	gcc $(CFLAGS) -c src/main.c

preprocessor.o: src/preprocessor.c include/preprocessor.h include/error.h include/constants.h
	gcc $(CFLAGS) -c src/preprocessor.c

first_pass.o: src/first_pass.c include/first_pass.h include/symbol_table.h include/utils.h include/error.h include/constants.h include/data_parser.h include/instruction_parser.h
	gcc $(CFLAGS) -c src/first_pass.c

second_pass.o: src/second_pass.c include/second_pass.h include/symbol_table.h include/utils.h include/error.h include/constants.h include/instruction_parser.h include/first_pass.h include/assembler_types.h
	gcc $(CFLAGS) -c src/second_pass.c

symbol_table.o: src/symbol_table.c include/symbol_table.h include/error.h include/constants.h
	gcc $(CFLAGS) -c src/symbol_table.c

utils.o: src/utils.c include/utils.h include/constants.h include/symbol_table.h
	gcc $(CFLAGS) -c src/utils.c

data_image.o: src/data_image.c include/data_image.h include/constants.h include/error.h include/first_pass.h
	gcc $(CFLAGS) -c src/data_image.c

data_parser.o: src/data_parser.c include/data_parser.h include/error.h include/constants.h include/first_pass.h include/data_image.h
	gcc $(CFLAGS) -c src/data_parser.c

instruction_parser.o: src/instruction_parser.c include/instruction_parser.h include/utils.h include/error.h include/first_pass.h include/constants.h
	gcc $(CFLAGS) -c src/instruction_parser.c

output.o: src/output.c include/output.h include/error.h include/constants.h include/data_image.h include/first_pass.h include/second_pass.h include/assembler_types.h
	gcc $(CFLAGS) -c src/output.c

error.o: src/error.c include/error.h include/constants.h
	gcc $(CFLAGS) -c src/error.c

# Default target
assembler: $(BUILD_DIR)/assembler

clean:
	rm -f *.o
	rm -rf $(BUILD_DIR)