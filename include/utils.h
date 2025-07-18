/**
 * @file first_pass.h
 * @brief Header file for utility functions used in the assembler.
 */

#ifndef UTILS_H
#define UTILS_H

#include "constants.h"

/**
 * @brief Trim leading and trailing whitespace from a string in-place.
 *
 * @param str Pointer to the string to trim.
 */
void trim_whitespace(char *str);

/**
 * @brief check if a string is entirely whitespace.
 *
 * @param str Pointer to a string to check.
 * @return TRUE if the string is empty or contains only whitespace, FALSE otherwise.
 */
BOOL is_whitespace(const char *str);

/**
 * @brief Check if a line is a comment (starts with ';' after optional spaces).
 *
 * @param line Pointer to a string representing a line of a source code to check.
 * @return TRUE if the line is a comment, FALSE otherwise.
 */
BOOL is_comment(const char *line);

/**
 * @brief Check if a string starts with a given prefix.
 *
 * @param str Pointer to the string to check.
 * @param prefix Pointer to the prefix to match.
 * @return TRUE if the string starts with the prefix, FALSE otherwise.
 */
BOOL starts_with(const char *str, const char *prefix);