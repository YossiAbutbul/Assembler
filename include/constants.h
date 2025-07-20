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

#endif