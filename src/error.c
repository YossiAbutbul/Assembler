/**
 * @file error.c
 * @brief Implements error handling functions for the assembler project.
 */

#include <stdio.h>
#include "../include/error.h"

/**
 * @brief Shared error messages array - organized by ErrorType enum order
 */
static const char *error_messages[] = {
    /* === Label and Syntax Errors === */
    /*  0 */ "Invalid label",                                                   /* ERROR_INVALID_LABEL */
    /*  1 */ "Label must start with a letter",                                  /* ERROR_LABEL_SYNTAX */
    /*  2 */ "Duplicate label",                                                 /* ERROR_DUPLICATE_LABEL */
    /*  3 */ "Reserved word cannot be used as label",                           /* ERROR_RESERVED_WORD */
    /*  4 */ "Syntax error",                                                    /* ERROR_SYNTAX */
    /*  5 */ "Line exceeds maximum length of 80 characters",                    /* ERROR_LINE_TOO_LONG */
    /*  6 */ "Missing whitespace between instruction / directive and operands", /* ERROR_MISSING_WHITESPACE */

    /* === Instruction and Directive Errors === */
    /*  7 */ "Unknown instruction", /* ERROR_UNKNOWN_INSTRUCTION */
    /*  8 */ "Invalid directive",   /* ERROR_INVALID_DIRECTIVE */
    /*  9 */ "Too many operands",   /* ERROR_TOO_MANY_OPERANDS */
    /* 10 */ "Too few operands",    /* ERROR_TOO_FEW_OPERANDS */
    /* 11 */ "Invalid operand",     /* ERROR_INVALID_OPERAND */

    /* === Immediate Value and Register Errors === */
    /* 12 */ "Invalid immediate value format (must be #number)",    /* ERROR_INVALID_IMMEDIATE_VALUE */
    /* 13 */ "Immediate value out of range (must be -512 to +511)", /* ERROR_DATA_OUT_OF_RANGE */
    /* 14 */ "Invalid register - only r0 through r7 are allowed",   /* ERROR_INVALID_REGISTER */
    /* 15 */ "Invalid addressing mode combination for instruction", /* ERROR_INVALID_ADDRESSING_MODE */
    /* 16 */ "Invalid source operand addressing mode",              /* ERROR_INVALID_SOURCE_ADDRESSING */
    /* 17 */ "Invalid target operand addressing mode",              /* ERROR_INVALID_TARGET_ADDRESSING */

    /* === Matrix Access Errors === */
    /* 18 */ "Invalid matrix syntax",                                                 /* ERROR_INVALID_MATRIX */
    /* 19 */ "Invalid matrix access - must be label[register][register]",             /* ERROR_INVALID_MATRIX_ACCESS */
    /* 20 */ "Matrix access missing register index - need label[register][register]", /* ERROR_MATRIX_MISSING_REGISTER */
    /* 21 */ "Invalid register in matrix access - must be r0-r7",                     /* ERROR_MATRIX_INVALID_REGISTER */
    /* 22 */ "Matrix register field too long - excessive whitespace",                 /* ERROR_MATRIX_REGISTER_TOO_LONG */
    /* 23 */ "Immediate values not allowed in matrix indices",                        /* ERROR_MATRIX_IMMEDIATE_NOT_ALLOWED */
    /* 24 */ "Invalid matrix dimensions - rows and columns must be greater than 0",   /* ERROR_INVALID_MATRIX_DIMENSIONS */
    /* 25 */ "Too many values provided for matrix size",                              /* ERROR_MATRIX_TOO_MANY_VALUES */

    /* === String Processing Errors === */
    /* 26 */ "String too long",                             /* ERROR_STRING_TOO_LONG */
    /* 27 */ "String must be enclosed in double quotes",    /* ERROR_STRING_MISSING_QUOTES */
    /* 28 */ "String missing closing quote",                /* ERROR_STRING_UNCLOSED */
    /* 29 */ "String contains invalid non-ASCII character", /* ERROR_STRING_INVALID_CHARACTER */

    /* === Symbol and Entry Errors === */
    /* 30 */ "Undefined symbol",                /* ERROR_UNDEFINED_SYMBOL */
    /* 31 */ "Entry not defined",               /* ERROR_ENTRY_NOT_DEFINED */
    /* 32 */ "Label cannot be used on .extern", /* ERROR_LABEL_ON_EXTERN */
    /* 33 */ "External conflict",               /* ERROR_EXTERNAL_CONFLICT */

    /* === Macro Expansion Errors === */
    /* 34 */ "Cannot use reserved word as macro name", /* ERROR_MACRO_RESERVED_WORD */
    /* 35 */ "Extra text after macro name",            /* ERROR_MACRO_EXTRA_TEXT */
    /* 36 */ "Missing 'mcroend' for macro",            /* ERROR_MACRO_MISSING_END */
    /* 37 */ "Missing macro name after 'mcro'",        /* ERROR_MACRO_MISSING_NAME */

    /* === Memory and System Errors === */
    /* 38 */ "Memory allocation failed",     /* ERROR_MEMORY_ALLOCATION_FAILED */
    /* 39 */ "Data image overflow",          /* ERROR_DATA_IMAGE_OVERFLOW */
    /* 40 */ "Instruction image overflow",   /* ERROR_INSTRUCTION_IMAGE_OVERFLOW */
    /* 41 */ "Memory address out of bounds", /* ERROR_ADDRESS_OUT_OF_BOUNDS */

    /* === General Errors === */
    /* 42 */ "General error",                 /* ERROR_GENERAL */
    /* 43 */ "Missing comma between operands" /* ERROR_MISSING_COMMA */
};

/**
 * @brief Get the size of the error messages array
 */
static const size_t error_messages_count = (sizeof(error_messages) / sizeof(error_messages[0]));

/**
 * @brief Prints an error message based on the exit code and the filename.
 *
 * @param exit_code The exit code indicating the type of error.
 * @param filename The filename associated with the error (without .as extension).
 */
void report_error(ExitCode exit_code, const char *filename)
{
    switch (exit_code)
    {
    case EXIT_FILE_NOT_FOUND:
        fprintf(stderr, "Error: File %s.as not found.\n", filename);
        break;
    case EXIT_MACRO_SYNTAX_ERROR:
        fprintf(stderr, "Error: Macro syntax error in file %s.as.\n", filename);
        break;
    case EXIT_MACRO_RESERVED_WORD:
        fprintf(stderr, "ERROR: Macro syntax error in file %s.as.\n", filename);
        break;
    case EXIT_MACRO_EXTRA_TEXT:
        fprintf(stderr, "ERROR: Macro syntax error in file %s.as.\n", filename);
        break;
    case EXIT_MACRO_MISSING_END:
        fprintf(stderr, "ERROR: Macro syntax error in file %s.as.\n", filename);
        break;
    case EXIT_FIRST_PASS_ERROR:
        fprintf(stderr, "Error: First pass error in file %s.am.\n", filename); /* .am */
        break;
    case EXIT_SECOND_PASS_ERROR:
        fprintf(stderr, "Error: Second pass error in file %s.am.\n", filename); /* .am */
        break;
    case EXIT_WRITE_ERROR:
        fprintf(stderr, "Error: Failed writing output files for %s.\n", filename);
        break;

    default:
        fprintf(stderr, "Error: General error occurred while processing %s.\n", filename);
    }
}

/**
 * @brief Prints a line-level error message with the line number and type for .am files.
 *
 * @param filename Pointer to the file name where the error occurred.
 * @param line_number The line number where the error occurred.
 * @param err_type The type of error that occurred.
 */
void print_line_error(const char *filename, int line_number, ErrorType err_type)
{
    if ((int)err_type < 0 || (size_t)err_type >= error_messages_count)
    {
        fprintf(stderr, "Error: Unknown error type %d in file %s at line %d.\n", (int)err_type, filename, line_number);
    }
    else
    {
        fprintf(stderr, "Error in file %s at line %d: %s.\n", filename, line_number, error_messages[err_type]);
    }
}
