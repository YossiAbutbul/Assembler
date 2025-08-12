/**
 * @file second_pass.h
 * @brief Header file for the second pass of the assembler.
 *
 * The second pass completes the assembly process by resolving all symbol
 * references, generating complete machine code, and preparing data for
 * output file generation.
 */

#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include <stdio.h>
#include "constants.h"
#include "assembler_types.h"

/**
 * @brief Performs the second pass on the given .am source file.
 *
 * Completes the assembly process by:
 * - Resolving symbol addresses using the symbol table.
 * - Generating complete machine code for all instructions.
 * - Processing .entry directives and building entry list.
 * - Recording external symbol references for linking.
 * - Validating all symbols references are defined.
 *
 * @param am_file   Pointer to the .am source file (after macro expansion).
 * @param filename  Pointer to the source file (for error reporting).
 * @param context   Assembly context to store results.
 * @return TRUE if second pass completed successfully, FALSE if errors occurred.
 */
BOOL second_pass(FILE *am_file, const char *filename, AssemblyContext *context);

/**
 * @brief Initializes an assembly context structure.
 *
 * Allocates and initializes all necessary data structures for the assembly
 * process.
 *
 * @param context Pointer to the assembly context to initialize.
 * @return TRUE if initialization successful, FALSE if memory allocation failed.
 */
BOOL init_assembly_context(AssemblyContext *context);

/**
 * @brief Gets the instruction image from the assembly context.
 *
 * Provides read-only access to the generated instruction machine code
 * for output file generation.
 *
 * @param context Pointer to the assembly context.
 * @return Pointer to the instruction image, or NULL if not initialized.
 */
const InstructionImage *get_instruction_image(const AssemblyContext *context);

/**
 * @brief Gets the entry list from the assembly context.
 *
 * Provides read-only access to the list of .entry symbols
 * for output file generation.
 *
 * @param context Pointer to the assembly context.
 * @return Pointer to the entry list, or NULL if no entries exist.
 */
const EntryNode *get_entry_list(const AssemblyContext *context);

/**
 * @brief Gets the external list from the assembly context.
 *
 * Provides read-only access to the list of external symbol refernces
 * for output file generation.
 *
 * @param context Pointer to the assembly context.
 * @return Pointer to the external list, or NULL if no externals exist.
 */
const ExternalNode *get_external_list(const AssemblyContext *context);

/**
 * @brief Cleanup function to free all assembly context resources.
 *
 * Releases all memory allocated for the assembly context including:
 * - Instruction image.
 * - Entry list.
 * - External list.
 *
 * @param context Pointer to the assembly context to clean up.
 */
void cleanup_assembly_context(AssemblyContext *context);

#endif