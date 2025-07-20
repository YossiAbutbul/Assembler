/**
 * @file first_pass.h
 * @brief Header file for the first pass of the assembler.
 */

#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "constants.h"

#define BASE_IC_ADDRESS 100 /* Base address for instruction count */

/**
 * @brief Runs the first pass of the assembler on the given source file.
 * This function processes the source file to build the symbol table,
 * resolve labels, and prepare for the second pass.
 *
 * @param s_file The name of the source file to process.
 * @param icf Pointer to store the instruction count final (ICF).
 * @param dcf Pointer to store the data count final (DCF).
 * @return TRUE if the first pass was successful, FALSE otherwise.
 */
BOOL run_first_pass(const char *s_file, int *icf, int *dcf);

#endif