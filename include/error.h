/**
 * @file error.h
 * @brief Contains error handling functions for the assembler project.
 */

#ifndef ERROR_H
#define ERROR_H

#include "constants.h"

/**
 * @brief Exit codes for the assembler stages
 */
typedef enum
{
    EXIT_SUCCESS_CODE = 0,
    EXIT_GENERAL_ERROR,
    EXIT_FILE_NOT_FOUND,
    EXIT_MACRO_SYNTAX_ERROR,
    EXIT_FIRST_PASS_ERROR,
    EXIT_SECOND_PASS_ERROR,
    EXIT_WRITE_ERROR,
    EXIT_FILE_EMPTY
} ExitCode;

/**
 * @brief Error types for the assembler
 */
typedef enum
{
    /* === label and Syntax Erros === */
    ERROR_INVALID_LABEL,
    ERROR_LABEL_SYNTAX,
    ERROR_DUPLICATE_LABEL,

    /* === Instruction and Directive Errors === */
    ERROR_UNKNOWN_INSTRUCTION,
    ERROR_INVALID_DIRECTIVE,

    /* === Operand Errors === */
    ERROR_TOO_MANY_OPERANDS,
    ERROR_TOO_FEW_OPERANDS,
    ERROR_INVALID_OPERAND,
    ERROR_INVALID_MATRIX,
    ERROR_DATA_OUT_OF_RANGE,

    /* === Symbol and Entry Errors === */
    ERROR_UNDEFINED_SYMBOL,
    ERROR_ENTRY_NOT_DEFINED,
    ERROR_LABEL_ON_EXTERN,
    ERROR_EXTERNAL_CONFLICT,

    /* === Special Cases === */
    ERROR_STRING_TOO_LONG,
    ERROR_MEMORY_ALLOCATION_FAILED,

    /* === General Errors === */
    ERROR_SYNTAX,
    ERROR_RESERVED_WORD,
    ERROR_LINE_TOO_LONG,
    ERROR_GENERAL,
    ERROR_DATA_IMAGE_OVERFLOW,

    /* === Instruction Generation Erros === */
    ERROR_INSTRUCTION_IMAGE_OVERFLOW,
    ERROR_ADDRESS_OUT_OF_BOUNDS,

    /* === Invalid Operands Errors === */
    ERROR_INVALID_IMMEDIATE_VALUE,
    ERROR_INVALID_REGISTER,
    ERROR_INVALID_MATRIX_ACCESS,
    ERROR_MATRIX_MISSING_REGISTER,
    ERROR_MATRIX_INVALID_REGISTER
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