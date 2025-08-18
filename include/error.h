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
    EXIT_FILE_EMPTY,
    EXIT_MACRO_RESERVED_WORD,
    EXIT_MACRO_EXTRA_TEXT,
    EXIT_MACRO_MISSING_END
} ExitCode;

/**
 * @brief Error types for the assembler
 */
typedef enum
{
    /* === Label and Syntax Errors === */
    ERROR_INVALID_LABEL,
    ERROR_LABEL_SYNTAX,
    ERROR_DUPLICATE_LABEL,
    ERROR_RESERVED_WORD,
    ERROR_SYNTAX,
    ERROR_LINE_TOO_LONG,
    ERROR_MISSING_WHITESPACE,

    /* === Instruction and Directive Errors === */
    ERROR_UNKNOWN_INSTRUCTION,
    ERROR_INVALID_DIRECTIVE,
    ERROR_TOO_MANY_OPERANDS,
    ERROR_TOO_FEW_OPERANDS,
    ERROR_INVALID_OPERAND,

    /* === Immediate Value and Register Errors === */
    ERROR_INVALID_IMMEDIATE_VALUE,
    ERROR_DATA_OUT_OF_RANGE,
    ERROR_INVALID_REGISTER,
    ERROR_INVALID_ADDRESSING_MODE,
    ERROR_INVALID_SOURCE_ADDRESSING,
    ERROR_INVALID_TARGET_ADDRESSING,

    /* === Matrix Access Errors === */
    ERROR_INVALID_MATRIX,
    ERROR_INVALID_MATRIX_ACCESS,
    ERROR_MATRIX_MISSING_REGISTER,
    ERROR_MATRIX_INVALID_REGISTER,
    ERROR_MATRIX_REGISTER_TOO_LONG,
    ERROR_MATRIX_IMMEDIATE_NOT_ALLOWED,
    ERROR_INVALID_MATRIX_DIMENSIONS,
    ERROR_MATRIX_TOO_MANY_VALUES,

    /* === String Processing Errors === */
    ERROR_STRING_TOO_LONG,
    ERROR_STRING_MISSING_QUOTES,
    ERROR_STRING_UNCLOSED,
    ERROR_STRING_INVALID_CHARACTER,

    /* === Symbol and Entry Errors === */
    ERROR_UNDEFINED_SYMBOL,
    ERROR_ENTRY_NOT_DEFINED,
    ERROR_LABEL_ON_EXTERN,
    ERROR_LABEL_ON_ENTRY,
    ERROR_EXTERNAL_CONFLICT,

    /* === Macro Expansion Errors === */
    ERROR_MACRO_RESERVED_WORD,
    ERROR_MACRO_EXTRA_TEXT,
    ERROR_MACRO_MISSING_END,
    ERROR_MACRO_MISSING_NAME,
    ERROR_MACRO_LABEL_CONFLICT,

    /* === Memory and System Errors === */
    ERROR_MEMORY_ALLOCATION_FAILED,
    ERROR_DATA_IMAGE_OVERFLOW,
    ERROR_INSTRUCTION_IMAGE_OVERFLOW,
    ERROR_ADDRESS_OUT_OF_BOUNDS,

    /* === Data Directive Specific Errors === */
    ERROR_DATA_NO_VALUES,
    ERROR_DATA_TRAILING_COMMA,
    ERROR_DATA_LEADING_COMMA,
    ERROR_DATA_DOUBLE_COMMA,
    ERROR_DATA_EMPTY_VALUE,

    /* === Entry/Extern Directive Specific Errors === */
    ERROR_ENTRY_MISSING_SYMBOL,
    ERROR_EXTERN_MISSING_SYMBOL,
    ERROR_ENTRY_EXTRA_TEXT,
    ERROR_EXTERN_EXTRA_TEXT,

    /* === General Errors === */
    ERROR_GENERAL,
    ERROR_MISSING_COMMA

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