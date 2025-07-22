/**
 * @file data_parser.h
 * @brief Header file for all the data parser functions.
 */

#ifndef DATA_PARSER_H
#define DATA_PARSER_H

#include "../include/constants.h"

/**
 * @brief Parses the opernad list of a ".data" directive.
 *
 * This function extracts a comma-sparated list of integers and adds
 * each valid number to the data image.
 * Each value increases the data counter (DC).
 *
 * Errors are reported if:
 * - Invalid numeric format.
 * - Missing values between commas.
 * - Trailing / leading commas.
 *
 * @param line      Pointer to the line content after the ".data" directive.
 * @param filename  Pointer to the source file (for error reporting).
 * @param line_num  The current line number (for error reporting).
 */
void parse_data_values(const char *line, const char *filename, int line_num);

/**
 * @brief Parses a ".string" directive and stores its characters into the data image.
 *
 * This function processes a string kiteral enclosed in double quotes. The string is
 * interoreted as sequence of printable ASCII characters, with each character stored
 * as one word in the data segemet. A null-terminator is appended to indocates the
 * end of the string.
 *
 * The parser extracts all characters between the first and last double quote on the line.
 * Quotes withing the string are allowed and treated as regular characters.
 * Any non-whitespace content following the closing quote is considered invalid.
 *
 * Errors are reported if:
 * - The string is not enclosed in double quotes.
 * - Characters fall outside the ASCII range [0-127].
 * Triling non-whitespce characters are present after the closing quote.
 *
 * @param line      Pointer to the line content after the  ".string" directive.
 * @param filename  Pointer to the source file (for error reporting).
 * @param line_num  The current line number (for error reporting).
 *
 */
void parse_string_value(const char *line, const char *filename, int line_num);

void parse_matrix(const char *line, const char *filename, int line_num);

#endif