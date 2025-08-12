/**
 * @file assembler.h
 * @brief An assembler interface for converting assembly language source files (.as files) into machine code output.
 */

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "constants.h"
#include "error.h"

/***
 * Assembles the given source file (with .as extension) into machine code.
 *
 * This function coordinates the macro expansion, first pass, second pass and the file output stages
 * of the assembly process.
 *
 * @param filename The base name of the source file to be assembled (without .as extension).
 * @return EXIT_SUCCESS_CODE if the assembly was successful, error code otherwise.
 */

ExitCode assemble(const char *filename);

#endif