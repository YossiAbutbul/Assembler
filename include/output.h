/**
 * @file output.h
 * @brief Header file for output file generation module.
 *
 * This module handles the generation of all assembler output files:
 * - .ob files (object code in base-4 format (a,b,c,d)).
 * - .ent files (entry symbol and addresses).
 * - .ext files (external symbol refernces).
 *
 * According to the instructions on mmn14:
 * - All machine words are 10 bits (range: -512 to +511 in two's complement method).
 * - Memory addresses: 0-255 (8-bits), but the assembler uses 100-255 (IC start at 100).
 * - Base-4 encoding uses exactly 5 characters (at the encoded format):
 *   a,b,c,d where a=0, b=1, c=2, d=3.
 */

#ifndef OUTPUT_H
#define OUTPUT_H

#include "constants.h"
#include "assembler_types.h"
#include "data_image.h"

/**
 * @brief Genrate all output files for successful assembly.
 *
 * This function creates the required output files based on the assembly results:
 * - Always create .ob file with machine code.
 * - Creates .ent file only if entry symbols are exist.
 * - Creates .ext file only if external refernces are exist.
 *
 * @param filename      Base filename (without extension).
 * @param context       Assembly context containing all generated data.
 * @return TRUE if all files generated successfully, FALSE otherwise.
 */
BOOL generate_output_files(const char *filename, const AssemblyContext *context);

/**
 * @brief Generate .ob (object) file containing machine code.
 *
 * Format:
 * - First line: instruction_count data_count.
 * - Following lines: address machine code in base-4 format (a,b,c,d).
 *
 * @param filename      Base filename (without extension).
 * @param context       Assembly context containing instruction and data image.
 * @return TRUE if file generated successfully, FALSE if error occured.
 */
BOOL generate_object_file(const char *filename, const AssemblyContext *context);

/**
 * @brief Generate .ent (entries) file containing entry symbols.
 *
 * Format:
 * - symbol_name address (one per line).
 * Addresses in base-4 format (a,b,c,d)
 *
 * @param filename      Base filename (without extension).
 * @param context       Assembly context containing instruction and data image.
 * @return TRUE if file generated successfully, FALSE if error occured.
 */
BOOL generate_entries_file(const char *filename, const AssemblyContext *context);

/**
 * @brief Generate .ext (external) file containing external references.
 *
 * Format:
 * - symbol_name address (one per line).
 * Addresses in base-4 format (a,b,c,d)
 *
 * @param filename      Base filename (without extension).
 * @param context       Assembly context containing instruction and data image.
 * @return TRUE if file generated successfully, FALSE if error occured.
 */
BOOL generate_externals_file(const char *filename, const AssemblyContext *context);

/**
 * @brief Converts decimal value to base-4 format.
 *
 * Converts a secimal value to the unique base-4 format:
 * 0->1, 1->b, 2->c, 3->d
 * The converted word is exactly 5 digits with padding of 'a' if needed.
 *
 * Valid input range (10 bit two's complement method):
 * - Positive: 0 to +511 (aaaaa to bdddd).
 * - Negative: -1 to -512 (ddddd to caaaa)
 *
 * @param value     Decimal value to convert (valid range: -512 to +511).
 * @param output    Buffer for 5-character result + null-terminator.
 */
void decimal_to_base4(int value, char *output);

/**
 * @brief Convert base-4 format back to back to decimal.
 *
 * @param base4_str Base-4 string to convert (must be exactly 5 chars: a,b,c,d only).
 * @return Decimal value (-512 to +511) if valid format,
 *         -9999 if invalid format (outside valid range for error detection).
 */
int base4_to_decimal(const char *base4_str);

/**
 * @brief Validate that a decimal value is within 10 bit range.
 *
 * @param value Deciaml value to check.
 * @return TRUE if value is in range [-512, +511], FALSE otherwise.
 */
BOOL is_valid_dec_value(int value);

/**
 * @brief Validate base-4 string format.
 *
 * @param base4_str String to validate.
 * @return TRUE if exactly 5 chars using only a,b,c,d, FALSE otherwise.
 */
BOOL is_valid_base4_string(const char *base4_str);

#endif