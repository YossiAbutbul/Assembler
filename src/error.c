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
        /*  0 */ "Invalid label",
        /*  1 */ "Unknown instruction",
        /*  2 */ "Invalid directive",
        /*  3 */ "Duplicate label",
        /*  4 */ "Syntax error",
        /*  5 */ "Too many operands",
        /*  6 */ "Too few operands",
        /*  7 */ "Invalid operand",
        /*  8 */ "Undefined symbol",
        /*  9 */ "Entry not defined",
        /* 10 */ "External conflict",
        /* 11 */ "Memory allocation failed",
        /* 12 */ "Label cannot be used on .extern",
        /* 13 */ "Invalid matrix syntax",
        /* 14 */ "Data value out of range",
        /* 15 */ "String too long",
        /* 16 */ "Label must start with a letter",
        /* 17 */ "General error",
        /* 18 */ "Data image overflow",
        /* 19 */ "Instruction image overflow",
        /* 20 */ "Memory address out of bounds"};

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