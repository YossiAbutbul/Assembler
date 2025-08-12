/**
 * @file constants.h
 * @brief Contains global constants and definitions used throughout the assembler project.
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

/* === Bolean Type Definition === */
typedef enum
{
    FALSE = 0,
    TRUE = 1
} BOOL;

/* === Global Variables: === */
#define MIN_ARGC 2                      /* Minimum number of arguments for the assembler */
#define MAX_LINE_LENGTH 81              /* Adds room for null-terminator */
#define MAX_LABEL_LENGTH 31             /* Maximum length for labels */
#define MAX_FILE_NAME_LENGTH 290        /* Maximum length for file names */
#define BASE_IC_ADDRESS 100             /* Initial value for the instruction counter */
#define MAX_DATA_IMAGE_SIZE 1000        /* Generous limit for image size */
#define MAX_INSTRUCTION_IMAGE_SIZE 1000 /* Generous limit for instruction image */
#define MAX_SYMBOL_NAME_LENGTH 32       /* max 30 chars + /0 + /n */

#endif