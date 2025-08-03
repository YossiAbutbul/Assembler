/**
 * @file first_pass.h
 * @brief Header file for the first pass of the assembler.
 */

#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include <stdio.h>

#include "../include/constants.h"

/* === Global Variables: === */

extern int IC;         /* Instruction counter */
extern int DC;         /* Data counter */
extern BOOL err_found; /* Flag to indicate if an error was found */

/* Storage for IC/L values and partial machine code  from first pass */
typedef struct
{
    int ic_address;        /* IC value when instruction was processed */
    int word_count;        /* L - number of words this instruction takes */
    int first_word;        /* First word of instruction - partial machine code */
    int immediate_word[4]; /* Stroage for immediate operand words (the 4 is the base of the new encoding) */
    int immediate_count;   /* Number of immediate words stored */
} InstructionData;

/**
 * @brief Get stored instruction data for second pass.
 *
 * @param index The index of the instruction to retrieve.
 * @return Pointer to the InstructionData if valid index, NULL otherwise.
 */
const InstructionData *get_instruction_data(int index);

/**
 * @brief Get total number of stored instructions.
 *
 * @return The instruction count.
 */
int get_instruction_count(void);

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

/**
 * @brief Cleanup first pass instruction storage betwenn files.
 * @note This function will be called at the end of processing each file in the main.c
 */
void cleanup_first_pass_data(void);

#endif