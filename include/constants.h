/**
 * @file constants.h
 * @brief Contains global constants and definitions used throughout the assembler project.
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

/* Global Variables: */
#define MIN_ARGC 2               /* Minimum number of arguments for the assembler */
#define MAX_LINE_LENGTH 81       /* Adds room for null-terminator */
#define MAX_FILE_NAME_LENGTH 290 /* Maximum length for file names */

/* Define a Boolean type*/
typedef enum
{
    FALSE = 0,
    TRUE = 1
} BOOL;

/* Exit codes for the assembler stages */
typedef enum
{
    EXIT_SUCCESS_CODE = 0,
    EXIT_GENERAL_ERROR = 1,
    EXIT_FILE_NOT_FOUND = 2,
    EXIT_MACRO_SYNTAX_ERROR = 3,
    EXIT_FIRST_PASS_ERROR = 4,
    EXIT_SECOND_PASS_ERROR = 5,
    EXIT_WRITE_ERROR = 6
} ExitCode;

#endif