# Assembler Project

A comprehensive two-stage assembler written in ANSI C90. This assembler translates assembly language source files into costume base-4 code, supporting various data types, instruction formats, and addressing modes.

## Table of Contents
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Overview](#overview)
- [Features](#features)
- [Architecture Specifications](#architecture-specifications)
- [Instruction Set](#instruction-set)
- [Addressing Modes](#addressing-modes)
- [Data Directives](#data-directives)
- [Symbol Management](#symbol-management)
- [Output Files](#output-files)
- [Base-4 Encoding](#base-4-encoding)
- [Error Handling](#error-handling)
- [Data Structures](#data-structures)
- [Build Instructions](#build-instructions)
- [Examples](#examples)
- [File Structure](#file-structure)

## Getting Started

### Prerequisites
Before building the assembler, ensure you have:
- **ANSI C90 compatible compiler** (GCC recommended)
- **GNU Make** build system
- **POSIX-compliant system** (Linux, macOS, or WSL on Windows)
- **Git** for cloning the repository

### Clone the Repository
```bash
# Clone the project from GitHub
git clone https://github.com/YossiAbutbul/Assembler.git

# Navigate to the project directory
cd Assembler
```

### Quick Build
```bash
# Clean any previous builds
make clean

# Build the assembler
make

# The executable 'assembler' will be created in the project root
```

## Usage

### Basic Command Syntax
```bash
./assembler filename1 [filename2 filename3 ...]
```

### File Processing Flow
1. **Input**: Source files with `.as` extension
2. **Preprocessing**: Creates `.am` files (after macro expansion)
3. **Assembly**: Generates output files if successful

### Output Files Generated
- **Object file**: `filename.ob` (always created if assembly succeeds)
- **Entries file**: `filename.ent` (created only if `.entry` symbols exist)
- **Externals file**: `filename.ext` (created only if external references exist)

### Command Examples
```bash
# Process a single file
./assembler program

# Process multiple files
./assembler test1 test2 test3
```

### Return Codes
- **0**: Success - all files assembled correctly
- **1-10**: Various error codes (see Error Handling section)

## Overview

The MMN14 assembler implements a complete three-stage assembly process:

### Assembly Stages

#### 1. Preprocessing Stage
The preprocessing stage handles macro expansion and prepares the source code for assembly:
- **Macro Detection**: Identifies `mcro` and `mcroend` directives
- **Macro Storage**: Stores macro definitions with their content
- **Macro Expansion**: Replaces macro calls with their defined content
- **File Generation**: Creates `.am` files (assembly after macro expansion)
- **Error Detection**: Validates macro syntax

#### 2. First Pass
Symbol table construction, syntax validation, and memory allocation:
- **Symbol Table Building**: Records all labels and their addresses
- **Syntax Validation**: Checks instruction format and operand validity
- **Memory Allocation**: Calculates instruction and data memory requirements
- **Directive Processing**: Handles `.data`, `.string`, `.mat`, `.extern` directives
- **Address Calculation**: Assigns memory addresses to instructions and data

#### 3. Second Pass
Code generation, symbol resolution, and output file creation:
- **Symbol Resolution**: Resolves all symbol references using the symbol table
- **Machine Code Generation**: Converts instructions to binary machine code
- **Entry Processing**: Handles `.entry` directives and builds entry symbol list
- **External References**: Tracks external symbol usage for linking
- **Base-4 encoding**: Encode all machine code data image into costume base-4 coding (see below)
- **Output Generation**: Creates final output files (if assemble successed) 

The assembler processes assembly source files (`.as`) and generates object files (`.ob`), entry files (`.ent`), and external reference files (`.ext`) as needed.

## Features

### Core Functionality
- ✅ **Macro preprocessing** with `mcro`/`mcroend` support
- ✅ **Two-stage assembly process** (first pass, second pass)
- ✅ **10-bit word architecture** (-512 to +511 range)
- ✅ **Base-4 encoding** for output files (a,b,c,d format)
- ✅ **Comprehensive error detection** and reporting
- ✅ **Memory management** with overflow protection
- ✅ **ANSI C90 compliance**

### Supported Features
- **Labels**: Up to 30 characters, must start with a letter (can't contain underscore and non-ASCII chars)
- **Comments**: Line comments starting with `;`
- **In-LineComments**: after assembly line ends, you can add `;` to in-line comment
- **Registers**: r0 through r7
- **Matrix operations**: 2D matrix addressing with `[register][register]`
- **String literals**: ASCII string storage with null termination
- **Entry/External symbols**: Cross-file symbol linking
- **Immediate values**: 10-bit signed integers

## Architecture Specifications

| Component | Specification |
|-----------|---------------|
| **Word Size** | 10 bits |
| **Value Range** | -512 to +511 (two's complement) |
| **Memory Addresses** | 0-255 (assembler uses 100-255) |
| **Registers** | r0, r1, r2, r3, r4, r5, r6, r7 |
| **Base Address** | IC starts at address 100 |
| **Max Line Length** | 80 characters |
| **Max Label Length** | 30 characters |

## Instruction Set

The assembler supports 16 instructions organized by operand count:

### Two-Operand Instructions (Opcodes 0-4)
| Opcode | Instruction | Description | Source Modes | Target Modes |
|--------|-------------|-------------|--------------|--------------|
| 0 | `mov` | Move data | 0,1,2,3 | 1,2,3 |
| 1 | `cmp` | Compare | 0,1,2,3 | 0,1,2,3 |
| 2 | `add` | Add | 0,1,2,3 | 1,2,3 |
| 3 | `sub` | Subtract | 0,1,2,3 | 1,2,3 |
| 4 | `lea` | Load effective address | 1,2 | 1,2,3 |

### One-Operand Instructions (Opcodes 5-13)
| Opcode | Instruction | Description | Target Modes |
|--------|-------------|-------------|--------------|
| 5 | `clr` | Clear | 1,2,3 |
| 6 | `not` | Logical NOT | 1,2,3 |
| 7 | `inc` | Increment | 1,2,3 |
| 8 | `dec` | Decrement | 1,2,3 |
| 9 | `jmp` | Jump | 1,2,3 |
| 10 | `bne` | Branch if not equal | 1,2,3 |
| 11 | `jsr` | Jump to subroutine | 1,2,3 |
| 12 | `red` | Read input | 1,2,3 |
| 13 | `prn` | Print | 0,1,2,3 |

### No-Operand Instructions (Opcodes 14-15)
| Opcode | Instruction | Description |
|--------|-------------|-------------|
| 14 | `rts` | Return from subroutine |
| 15 | `stop` | Stop execution |

## Addressing Modes

| Mode | Name | Format | Description | Example |
|------|------|--------|-------------|---------|
| 0 | **Immediate** | `#value` | Constant value | `#42`, `#-10` |
| 1 | **Direct** | `label` | Memory address | `LOOP`, `DATA` |
| 2 | **Matrix** | `label[reg][reg]` | 2D array access | `MAT[r1][r2]` |
| 3 | **Register** | `register` | Register content | `r0`, `r7` |

## Data Directives

### `.data` - Integer Data
```assembly
NUMBERS: .data 10, -5, 0, 42
```
- Stores comma-separated integer values
- Each value occupies one word
- Range: -512 to +511

### `.string` - String Literals
```assembly
MESSAGE: .string "Hello World"
```
- Stores ASCII characters as individual words
- Automatically null-terminated
- Must be enclosed in double quotes

### `.mat` - Matrix Data
```assembly
MATRIX: .mat [3][2] 1,2,3,4,5,6
```
- Defines 2D matrices with dimensions [rows][cols]
- Values stored row-major order
- Missing values default to zero

### `.entry` - Export Symbols
```assembly
.entry MAIN
.entry RESULT
```
- Marks symbols for external visibility
- Creates `.ent` output file
- Symbol must be defined in current file

### `.extern` - Import Symbols
```assembly
.extern PRINTF
.extern SCANF
```
- Declares external symbols
- Creates `.ext` output file for references
- Symbols defined in other files

## Symbol Management

### Symbol Table Structure
```c
typedef struct Symbol {
    char name[MAX_SYMBOL_NAME_LENGTH];
    int address;
    SymbolType type;        // SYMBOL_CODE, SYMBOL_DATA, SYMBOL_EXTERN
    struct Symbol *next;
} Symbol;
```

### Symbol Types
- **CODE**: Instructions and code labels
- **DATA**: Data segment symbols (.data, .string, .mat)
- **EXTERN**: External symbol declarations

## Output Files

### Object File (`.ob`)
```
bbc dd
bcba abcba
bcbb cabbc
...
```
- First line: instruction_count data_count (base-4)
- Following lines: address machine_code (base-4)

### Entries File (`.ent`)
```
LENGTH caab
LOOP bccd 
...
```
- Symbol name followed by address (base-4)
- Generated only if entry symbols exist

### Externals File (`.ext`)
```
L3 bdca
W bcda
...
```
- External symbol references with addresses (base-4)
- Generated only if external references exist

## Base-4 Encoding

### Overview
The assembler uses a custom base-4 encoding system for all output files. This encoding converts 10-bit binary machine code into a unique character-based format for improved readability and storage efficiency.

### Encoding Format
The base-4 system uses four characters to represent binary digits:

| Binary Value | Base-4 Character |
|--------------|------------------|
| 00 | `a` |
| 01 | `b` |
| 10 | `c` |
| 11 | `d` |

### Word Encoding
Each 10-bit machine word is encoded as exactly 5 base-4 characters:
- **10 bits** = 5 pairs of 2 bits each
- Each pair converts to one base-4 character
- Result: 5-character string (e.g., `abcda`, `dddda`)

### Encoding Process
1. **Binary Word**: Take a 10-bit machine word (e.g., `1011001101`)
2. **Split into Pairs**: `10|11|00|11|01`
3. **Convert Each Pair**: `c|d|a|d|b`
4. **Final Result**: `cdadb`

### Examples
| Decimal Value | Binary (10-bit) | Base-4 Encoding |
|---------------|-----------------|-----------------|
| 100 | `0001100100` | `abcba` |
| 511 | `0111111111` | `bdddd` |
| -512 | `1000000000` | `caaaa` |
| 0 | `0000000000` | `aaaaa` |
| 255 | `0011111111` | `adddd` |

### Address Encoding
Memory addresses are also encoded in base-4 format:
- Instruction Counter (IC) addresses: 100-255
- Data addresses: Start after instructions (ICF + data_offset)
- All addresses in output files use base-4 encoding

### Usage in Output Files
- **Object File (.ob)**: All addresses and machine code in base-4
- **Entries File (.ent)**: Symbol addresses in base-4
- **Externals File (.ext)**: Reference addresses in base-4

This encoding system ensures consistent, readable output while maintaining the 10-bit precision required by the custom architecture.

## Error Handling

### Comprehensive Error Detection
The assembler provides detailed error reporting for all stages of assembly processing. Each error includes the filename, line number, and specific error description.

### Exit Codes
The assembler uses specific exit codes to indicate different types of failures:

| Exit Code | Description |
|-----------|-------------|
| `EXIT_SUCCESS_CODE` | Successful assembly |
| `EXIT_FILE_NOT_FOUND` | Input file not found |
| `EXIT_MACRO_SYNTAX_ERROR` | Macro processing error |
| `EXIT_FIRST_PASS_ERROR` | First pass assembly error |
| `EXIT_SECOND_PASS_ERROR` | Second pass assembly error |
| `EXIT_WRITE_ERROR` | Output file generation error |
| `EXIT_FILE_EMPTY` | Empty input file |

### Complete Error Types and Messages

#### Label and Syntax Errors
| Error Type | Message |
|------------|---------|
| `ERROR_INVALID_LABEL` | "Invalid label" |
| `ERROR_LABEL_SYNTAX` | "Label must start with a letter" |
| `ERROR_DUPLICATE_LABEL` | "Duplicate label" |
| `ERROR_RESERVED_WORD` | "Reserved word cannot be used as label" |
| `ERROR_SYNTAX` | "Syntax error" |
| `ERROR_LINE_TOO_LONG` | "Line exceeds maximum length of 80 characters" |
| `ERROR_MISSING_WHITESPACE` | "Missing whitespace between instruction / directive and operands" |

#### Instruction and Directive Errors
| Error Type | Message |
|------------|---------|
| `ERROR_UNKNOWN_INSTRUCTION` | "Unknown instruction" |
| `ERROR_INVALID_DIRECTIVE` | "Invalid directive" |
| `ERROR_TOO_MANY_OPERANDS` | "Too many operands" |
| `ERROR_TOO_FEW_OPERANDS` | "Too few operands" |
| `ERROR_INVALID_OPERAND` | "Invalid operand" |

#### Immediate Value and Register Errors
| Error Type | Message |
|------------|---------|
| `ERROR_INVALID_IMMEDIATE_VALUE` | "Invalid immediate value format (must be #number)" |
| `ERROR_DATA_OUT_OF_RANGE` | "Immediate value out of range (must be -512 to +511)" |
| `ERROR_INVALID_REGISTER` | "Invalid register - only r0 through r7 are allowed" |
| `ERROR_INVALID_ADDRESSING_MODE` | "Invalid addressing mode combination for instruction" |
| `ERROR_INVALID_SOURCE_ADDRESSING` | "Invalid source operand addressing mode" |
| `ERROR_INVALID_TARGET_ADDRESSING` | "Invalid target operand addressing mode" |

#### Matrix Access Errors
| Error Type | Message |
|------------|---------|
| `ERROR_INVALID_MATRIX` | "Invalid matrix syntax" |
| `ERROR_INVALID_MATRIX_ACCESS` | "Invalid matrix access - must be label[register][register]" |
| `ERROR_MATRIX_MISSING_REGISTER` | "Matrix access missing register index - need label[register][register]" |
| `ERROR_MATRIX_INVALID_REGISTER` | "Invalid register in matrix access - must be r0-r7" |
| `ERROR_MATRIX_REGISTER_TOO_LONG` | "Matrix register field too long - excessive whitespace" |
| `ERROR_MATRIX_IMMEDIATE_NOT_ALLOWED` | "Immediate values not allowed in matrix indices" |
| `ERROR_INVALID_MATRIX_DIMENSIONS` | "Invalid matrix dimensions - rows and columns must be greater than 0" |
| `ERROR_MATRIX_TOO_MANY_VALUES` | "Too many values provided for matrix size" |

#### String Processing Errors
| Error Type | Message |
|------------|---------|
| `ERROR_STRING_TOO_LONG` | "String exceeds maximum length" |
| `ERROR_STRING_MISSING_QUOTES` | "String must be enclosed in double quotes" |
| `ERROR_STRING_UNCLOSED` | "String missing closing quote" |
| `ERROR_STRING_INVALID_CHARACTER` | "String contains invalid non-ASCII character" |

#### Symbol and Entry Errors
| Error Type | Message |
|------------|---------|
| `ERROR_UNDEFINED_SYMBOL` | "Undefined symbol" |
| `ERROR_ENTRY_NOT_DEFINED` | "Entry symbol not defined in current file" |
| `ERROR_LABEL_ON_EXTERN` | "Label cannot be used with extern directive" |
| `ERROR_LABEL_ON_ENTRY` | "Label cannot be used with entry directive" |
| `ERROR_EXTERNAL_CONFLICT` | "Symbol conflicts with external declaration" |

#### Macro Expansion Errors
| Error Type | Message |
|------------|---------|
| `ERROR_MACRO_RESERVED_WORD` | "Cannot use reserved word as macro name" |
| `ERROR_MACRO_EXTRA_TEXT` | "Extra text after macro name" |
| `ERROR_MACRO_MISSING_END` | "Missing 'mcroend' for macro" |
| `ERROR_MACRO_MISSING_NAME` | "Missing macro name after 'mcro'" |
| `ERROR_MACRO_LABEL_CONFLICT` | "Macro name conflicts with existing label" |

#### Memory and System Errors
| Error Type | Message |
|------------|---------|
| `ERROR_MEMORY_ALLOCATION_FAILED` | "Memory allocation failed" |
| `ERROR_DATA_IMAGE_OVERFLOW` | "Data image size exceeded" |
| `ERROR_INSTRUCTION_IMAGE_OVERFLOW` | "Instruction image size exceeded" |
| `ERROR_ADDRESS_OUT_OF_BOUNDS` | "Memory address out of bounds" |

#### Data Directive Specific Errors
| Error Type | Message |
|------------|---------|
| `ERROR_DATA_NO_VALUES` | ".data directive requires at least one value" |
| `ERROR_DATA_TRAILING_COMMA` | ".data directive cannot end with comma" |
| `ERROR_DATA_LEADING_COMMA` | ".data directive cannot start with comma" |
| `ERROR_DATA_DOUBLE_COMMA` | ".data directive cannot have consecutive commas" |
| `ERROR_DATA_EMPTY_VALUE` | ".data directive has empty value between commas" |

#### Entry/Extern Directive Specific Errors
| Error Type | Message |
|------------|---------|
| `ERROR_ENTRY_MISSING_SYMBOL` | ".entry directive requires a symbol name" |
| `ERROR_EXTERN_MISSING_SYMBOL` | ".extern directive requires a symbol name" |
| `ERROR_ENTRY_EXTRA_TEXT` | ".entry directive cannot have extra text after symbol name" |
| `ERROR_EXTERN_EXTRA_TEXT` | ".extern directive cannot have extra text after symbol name" |

#### General Errors
| Error Type | Message |
|------------|---------|
| `ERROR_GENERAL` | "General error" |
| `ERROR_MISSING_COMMA` | "Missing comma between operands" |

### Error Reporting Format
All errors follow a consistent format for easy identification and debugging:
```
Error in file filename.as at line XX: Specific error message.
```

#### Example Error Messages
```bash
Error in file test.as at line 15: Invalid register - only r0 through r7 are allowed.
Error in file example.as at line 3: Matrix access missing register index - need label[register][register].
Error in file data.as at line 8: .data directive cannot end with comma.
Error in file macro.as at line 2: Cannot use reserved word as macro name.
```

## Data Structures

### Core Structures

#### AssemblyContext
```c
typedef struct {
    InstructionData instruction_image[MAX_INSTRUCTION_IMAGE_SIZE];
    int data_image[MAX_DATA_IMAGE_SIZE];
    Symbol *symbol_table;
    ExternalRef *external_list;
    EntrySymbol *entry_list;
    int instruction_count;
    int data_count;
    BOOL has_errors;
} AssemblyContext;
```

#### Instruction Representation
```c
typedef struct {
    int opcode;
    InstructionType type;
    Operand source;
    Operand target;
    int word_count;
    BOOL has_source;
    BOOL has_target;
} Instruction;
```

#### Operand Structure
```c
typedef struct {
    AddressingMode mode;
    int value;
    int reg1, reg2;
    char symbol_name[MAX_LABEL_LENGTH + 1];
    BOOL is_symbol;
} Operand;
```

## Build Instructions

### Requirements
- ANSI C90 compatible compiler
- GNU Make
- POSIX-compliant system

### Compilation
```bash
make clean      # Clean previous builds
make           # Build the assembler
```

### Compiler Flags
```bash
CFLAGS = -g -m32 -std=c90 -pedantic -Wall -ansi
```

## Examples

### Complete Assembly Program
```assembly
; Complex program example
.entry MAIN
.entry RESULT
.extern PRINTFUNC

; Data section
ARRAY:      .mat [3][3] 1,2,3,4,5,6,7,8,9
COUNTER:    .data 0
RESULT:     .data 0
MESSAGE:    .string "Processing complete"

; Main program
MAIN:       mov #0, r1           ; Initialize counter
            mov #3, r2           ; Array size

LOOP:       cmp r1, r2           ; Check if done
            bne CONTINUE         ; Continue if not done
            jmp DONE             ; Exit if done

CONTINUE:   mov ARRAY[r0][r0], r0 ; Get diagonal element
            add r3, RESULT       ; Add to result
            inc r1               ; Increment counter
            jmp LOOP             ; Continue loop

DONE:       jsr PRINTFUNC        ; Call external function
            lea MESSAGE, r4      ; Load message address
            prn #42              ; Print test value
            stop                 ; End program
```

### Macro Usage
```assembly
mcro SWAP
    mov r1, r3
    mov r2, r1
    mov r3, r2
mcroend

MAIN:   SWAP    ; Expands to the three mov instructions
        stop
```

## File Structure

```
project/
├── src/                     # Source files
│   ├── main.c              # Main entry point
│   ├── assembler.c         # Core assembler logic
│   ├── preprocessor.c      # Macro processing
│   ├── first_pass.c        # First pass implementation
│   ├── second_pass.c       # Second pass implementation
│   ├── instruction_parser.c # Instruction parsing
│   ├── data_parser.c       # Data directive parsing
│   ├── symbol_table.c      # Symbol management
│   ├── output.c            # Output file generation
│   ├── error.c             # Error handling
│   ├── utils.c             # Utility functions
│   └── data_image.c        # Data storage management
├── include/                 # Header files
│   ├── assembler.h
│   ├── constants.h
│   ├── assembler_types.h
│   └── [other headers]
├── tests/                   # Test files
│   ├── test1.as
│   ├── test2.as
│   └── [other tests]
├── build/                   # Build directory
└── makefile                # Build configuration
```

---

## License

This project is developed as part of the MMN-14 assignment for the Systems Programming Laboratory course.
