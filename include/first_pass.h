/**
 * @file first_pass.h
 * @brief Header file for the first pass of the assembler.
 */

#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "../include/constants.h"

/* === Global Variables: === */

extern int IC;         /* Instruction counter */
extern int DC;         /* Data counter */
extern BOOL err_found; /* Flag to indicate if an error was found */

/**
 * @brief Preforms the first pass on the given .am source file.
 *
 * Scans the file line by line to:
 * - Identify labels, instructions and directives.
 * - Add symbols to the symbol table.
 * - Count IC and DC.
 * - Validate syntax and operand legality.
 * - Print errors if any are found.
 *
 * @param am_file Open file pointer to the .am source file (after macro expansion).
 * @param filename Name of the source file (for error reporting).
 * @return TRUE if the first pass was successful, FALSE otherwise.
 */
BOOL first_pass(FILE *am_file, const char *filename);

#endif