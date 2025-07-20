/**
 * @file error.h
 * @brief Contains error handling functions for the assembler project.
 */

#ifndef ERROR_H
#define ERROR_H

#include "constants.h"

/** @brief Exit codes for the assembler stages */
typedef enum
{
    EXIT_SUCCESS_CODE = 0,
    EXIT_GENERAL_ERROR,
    EXIT_FILE_NOT_FOUND,
    EXIT_MACRO_SYNTAX_ERROR,
    EXIT_FIRST_PASS_ERROR,
    EXIT_SECOND_PASS_ERROR,
    EXIT_WRITE_ERROR
} ExitCode;

/** @brief Error types for the assembler */
typedef enum
{
    ERR_INVALID_LABEL,
    ERR_UNKNOWN_INSTRUCTION,
    ERR_INVALID_DIRECTIVE,
    ERR_DUPLICATE_LABEL,
    ERR_SYNTAX_ERROR,
    ERR_TOO_MANY_OPERANDS,
    ERR_TOO_FEW_OPERANDS,
    ERR_INVALID_OPERAND,
    ERR_UNDEFINED_SYMBOL,
    ERR_ENTRY_NOT_DEFINED,
    ERR_EXTERNAL_CONFLICT,
    ERR_GENRAL
} ErrorType;

/**
 * @brief Prints an error message based on the exit code and the filename.
 *
 * @param exit_code The exit code indicating the type of error.
 * @param filename The filename associated with the error (without .as extension).
 */

void report_error(ExitCode exit_code, const char *filename);

/**
 *
 */
void print_line_error(const char *filename, int line_number, ErrorType err_type)

#endif