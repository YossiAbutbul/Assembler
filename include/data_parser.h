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

void parse_string_value(const char *line, const char *filename, int line_num);

void parse_matrix(const char *line, const char *filename, int line_num);

#endif