/**
 * @file first_pass.h
 * @brief Header file for utility functions used in the assembler.
 */

#ifndef UTILS_H
#define UTILS_H

#include "constants.h"

/* === Whitespace and Comment Utilities === */

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

/* === Label Parsing Utilities === */

/**
 * @brief Extracts a label from the beginning of a line.
 *
 * @param line Pointer to the line to extract the label from.
 * @param label_out Pointer to a buffer where the extracted label will be stored.
 * @return TRUE if a label found and valid syntax (ends with ':'), 
 *         FALSE otherwise.
 */
BOOL extract_label(const char *line, char *label_out);

/**
 * @brief Skips a label (and ':') in a line and returns a pointer to the rest of the line.
 *
 * @param label Pointer to the label string to check.
 * @return Pointer to the rest of the line after the label.
 */
char *skip_label(const char *line);

/**
 * @brief Check if a label is valid according to assembler rules:
 * - Must start with a letter.
 * - Contain only alphanumeric characters
 * - Not be a reserved word.
 *
 * @param label Pointer to the label string to check.
 * @return TRUE if the label is valid, FALSE otherwise.
 */
BOOL is_valid_label(const char *label);

/* === Label Parsing Utilities === */

/**
 * @brief Get the next token from a line.
 *
 * @param src Pointer to the source input line.
 * @param token_out Pointer to a buffer where the extracted token will be stored.
 * @return TRUE if a token was successfully extracted, FALSE if no more tokens are available.
 */
BOOL get_next_token(const char *src, char *token_out);

/**
 * @brief Check if a string starts with a given prefix.
 *
 * @param str Pointer to the string to check.
 * @param prefix Pointer to the prefix to match.
 * @return TRUE if the string starts with the prefix, FALSE otherwise.
 */
BOOL starts_with(const char *str, const char *prefix);