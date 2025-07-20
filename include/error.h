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
    ERR_EXTERN_CONFLICT,
    ERR_MEMORY_ALLOCATION,
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
 * @brief Prints a line-level error message with the line number and type.
 * @param filename Pointer to the file name where the error occurred.
 * @param line_number The line number where the error occurred.
 * @param err_type The type of error that occurred.
 */
void print_line_error(const char *filename, int line_number, ErrorType err_type);

#endif