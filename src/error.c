/**
 * @file error.c
 * @brief Implements error handling functions for the assembler project.
 */

#include <stdio.h>
#include "../include/error.h"

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
        fprintf(stderr, "Error: Macro syntax error in file %s.\n", filename);
        break;
    case EXIT_FIRST_PASS_ERROR:
        fprintf(stderr, "Error: First pass error in file %s.\n", filename);
        break;
    case EXIT_SECOND_PASS_ERROR:
        fprintf(stderr, "Error: Second pass error in file %s.\n", filename);
        break;
    case EXIT_WRITE_ERROR:
        fprintf(stderr, "Error: Failed writing output files for %s.\n", filename);
        break;

    default:
        fprintf(stderr, "Error: General error occurred while processing %s.\n", filename);
    }
}

/**
 * @brief Prints a line-level error message with the line number and type.
 *
 * @param filename Pointer to the file name where the error occurred.
 * @param line_number The line number where the error occurred.
 * @param err_type The type of error that occurred.
 */
void print_line_error(const char *filename, int line_number, ErrorType err_type)
{
    const char *error_messages[] = {
        /*  0 */ "Invalid label",                                                       /* ERROR_INVALID_LABEL */
        /*  1 */ "Label must start with a letter",                                      /* ERROR_LABEL_SYNTAX */
        /*  2 */ "Duplicate label",                                                     /* ERROR_DUPLICATE_LABEL */
        /*  3 */ "Unknown instruction",                                                 /* ERROR_UNKNOWN_INSTRUCTION */
        /*  4 */ "Invalid directive",                                                   /* ERROR_INVALID_DIRECTIVE */
        /*  5 */ "Too many operands",                                                   /* ERROR_TOO_MANY_OPERANDS */
        /*  6 */ "Too few operands",                                                    /* ERROR_TOO_FEW_OPERANDS */
        /*  7 */ "Invalid operand",                                                     /* ERROR_INVALID_OPERAND */
        /*  8 */ "Invalid matrix syntax",                                               /* ERROR_INVALID_MATRIX */
        /*  9 */ "Data value out of range",                                             /* ERROR_DATA_OUT_OF_RANGE */
        /* 10 */ "Undefined symbol",                                                    /* ERROR_UNDEFINED_SYMBOL */
        /* 11 */ "Entry not defined",                                                   /* ERROR_ENTRY_NOT_DEFINED */
        /* 12 */ "Label cannot be used on .extern",                                     /* ERROR_LABEL_ON_EXTERN */
        /* 13 */ "External conflict",                                                   /* ERROR_EXTERNAL_CONFLICT */
        /* 14 */ "String too long",                                                     /* ERROR_STRING_TOO_LONG */
        /* 15 */ "Memory allocation failed",                                            /* ERROR_MEMORY_ALLOCATION_FAILED */
        /* 16 */ "Syntax error",                                                        /* ERROR_SYNTAX */
        /* 17 */ "Reserved word cannot be used as label",                               /* ERROR_RESERVED_WORD */
        /* 18 */ "Line exceeds maximum length of 80 characters",                        /* ERROR_LINE_TOO_LONG */
        /* 19 */ "General error",                                                       /* ERROR_GENERAL */
        /* 20 */ "Data image overflow",                                                 /* ERROR_DATA_IMAGE_OVERFLOW */
        /* 21 */ "Instruction image overflow",                                          /* ERROR_INSTRUCTION_IMAGE_OVERFLOW */
        /* 22 */ "Memory address out of bounds",                                        /* ERROR_ADDRESS_OUT_OF_BOUNDS */
        /* 23 */ "Invalid immediate value - missing number after #",                    /* ERROR_INVALID_IMMEDIATE_VALUE */
        /* 24 */ "Invalid register - only r0 through r7 are allowed",                   /* ERROR_INVALID_REGISTER */
        /* 25 */ "Invalid matrix access - must be label[register][register]",           /* ERROR_INVALID_MATRIX_ACCESS */
        /* 26 */ "Matrix access missing register index - need [reg][reg]",              /* ERROR_MATRIX_MISSING_REGISTER */
        /* 27 */ "Invalid register in matrix access - must be r0-r7",                   /* ERROR_MATRIX_INVALID_REGISTER */
        /* 28 */ "Matrix register field too long - excessive whitespace",               /* ERROR_MATRIX_REGISTER_TOO_LONG */
        /* 29 */ "Invalid addressing mode combination for instruction",                 /* ERROR_INVALID_ADDRESSING_MODE */
        /* 30 */ "Invalid source operand addressing mode",                              /* ERROR_INVALID_SOURCE_ADDRESSING */
        /* 31 */ "Invalid target operand addressing mode",                              /* ERROR_INVALID_TARGET_ADDRESSING */
        /* 32 */ "Invalid matrix dimensions - rows and columns must be greater than 0", /* ERROR_INVALID_MATRIX_DIMENSIONS */
        /* 33 */ "Too many values provided for matrix size",                            /* ERROR_MATRIX_TOO_MANY_VALUES */
        /* 34 */ "String must be enclosed in double quotes",                            /* ERROR_STRING_MISSING_QUOTES */
        /* 35 */ "String missing closing quote",                                        /* ERROR_STRING_UNCLOSED */
        /* 36 */ "String contains invalid non-ASCII character"};                        /* ERROR_STRING_INVALID_CHARACTER */

    size_t err_cnt = (sizeof(error_messages) / sizeof(error_messages[0]));

    if ((int)err_type < 0 || (size_t)err_type >= err_cnt)
    {
        fprintf(stderr, "Error: Unknown error type %d in file %s at line %d.\n", (int)err_type, filename, line_number);
    }
    else
    {
        fprintf(stderr, "Error in file %s at line %d: %s.\n", filename, line_number, error_messages[err_type]);
    }
}