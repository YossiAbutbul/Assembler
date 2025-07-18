/**
 * @file preprocessor.h
 * @brief Header file for the preprocessor module of the assembler.
 */

#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include "constants.h"

#define MAX_MACROS 50
#define MAX_MACRO_NAME_LENGTH 32 /* Maximum allowed lenfth of macro name + Null terminator + Margin for unexpected charachters / debugging */
#define MAX_MACRO_LINES 100

/**
 * @brief the preprocessor on the given source file (with .as extension).
 *
 * Reads 'filename.as', expands macros, and writes the result to 'filename.am'.
 *
 * @param filename The base name of the source file to be preprocessed (without .as extension).
 * @return EXIT_SUCCESS_CODE if successful,
 *         or a relevent error code if an error occurs.
 */
ExitCode preprocess(const char *filename);

#endif