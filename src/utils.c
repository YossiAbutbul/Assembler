/**
 * @file utils.c
 * @brief Implementation of utility functions used in the assembler.
 */

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "../include/utils.h"
#include "../include/constants.h"
#include "../include/symbol_table.h"

/* === Whitespace and Comment Utilities === */

/**
 * @brief Trim leading and trailing whitespace from a string in-place.
 *
 * @param str Pointer to the string to trim.
 */
void trim_whitespace(char *str)
{
    char *start, *end;

    /* Check for null or empty strings */
    if (!str || *str == '\0')
        return;

    start = str;
    /* Trim leading whitespaces */
    while (*start && isspace((unsigned char)*start))
    {
        start++;
    }

    /* Check if the string is all spaces. */
    if (*start == '\0')
    {
        *str = '\0'; /* Set to empty string if only spaces */
        return;
    }

    /* Shift the trimmed content to the beginning */
    if (start != str)
    {
        memmove(str, start, strlen(start) + 1);
    }

    /* Trim trailing spaces */
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
    {
        *end = '\0';
        end--;
    }
}

/**
 * @brief check if a string is entirely whitespace.
 *
 * @param str Pointer to a string to check.
 * @return TRUE if the string is empty or contains only whitespace, FALSE otherwise.
 */
BOOL is_whitespace(const char *str)
{
    int i;

    /* Checks for null pointer */
    if (!str)
        return TRUE;

    /* Iterate through each character */
    for (i = 0; str[i] != '\0'; i++)
    {
        if (!isspace((unsigned char)str[i]))
            return FALSE; /* Found a non-whitespace character. */
    }
    return TRUE; /* All characters are whitespace or the string is empty. */
}

/**
 * @brief Check if a line is a comment (starts with ';' after optional spaces).
 *
 * @param line Pointer to a string representing a line of a source code to check.
 * @return TRUE if the line is a comment, FALSE otherwise.
 */
BOOL is_comment(const char *line)
{
    int i;

    /* Checks for null pointer */
    if (!line)
        return FALSE;

    /* Skip leading whitespaces */
    for (i = 0; line[i] != '\0'; i++)
    {
        if (!isspace((unsigned char)line[i]))
            return (line[i] == ';') ? TRUE : FALSE;
    }

    return FALSE; /* Line is empty or contains only whitespace */
}

/* === Label Extraction and Validation === */

/**
 * @brief Extracts a label from the beginning of a line.
 *
 * @param line Pointer to the line to extract the label from.
 * @param label_out Pointer to a buffer where the extracted label will be stored.
 * @return TRUE if a label found and valid syntax (ends with ':'),
 *         FALSE otherwise.
 */
BOOL extract_label(const char *line, char *label_out)
{
    int i = 0, j = 0;

    /* Checks for null pointers */
    if (!line || !label_out)
        return FALSE;

    /* Skip leading whitespace */
    while (isspace((unsigned char)line[i]))
        i++;

    /* Check if the line starts with a directive (begins with '.') */
    if (line[i] == '.')
    {
        return FALSE;
    }

    /* Check if the label starts with a letter */
    if (!isalpha((unsigned char)line[i]))
    {
        return FALSE;
    }

    /* Copy the label until we hit a ':' or whitespace */
    /* Also validate that it contains only alphanumeric characters */
    while (line[i] && !isspace((unsigned char)line[i]) && line[i] != ':' && j < MAX_LABEL_LENGTH)
    {
        if (!isalnum((unsigned char)line[i]) && line[i] != '_') /* Allow underscores */
        {
            return FALSE; /* Invalid character in label */
        }
        label_out[j++] = line[i++];
    }

    /* Skip any whitespace between label and ':' */
    while (isspace((unsigned char)line[i]))
        i++;

    /* Check if we found a valid label ending with ':' */
    if (line[i] == ':' && j > 0)
    {
        label_out[j] = '\0'; /* Null-terminate the label */
        return TRUE;
    }

    return FALSE;
}

/**
 * @brief Skips a label (and ':') in a line and returns a pointer to the rest of the line.
 *
 * @param label Pointer to the label string to check.
 * @return Pointer to the rest of the line after the label.
 */
char *skip_label(const char *line)
{
    const char *p = line;

    /* Skip leading whitespace */
    while (isspace((unsigned char)*p))
        p++;

    /* Skip label characters until ':' */
    while (*p && *p != ':')
        p++;

    /* Skip the ':' character if found */
    if (*p == ':')
        p++;

    /* Skip any whitespace after the label */
    while (isspace((unsigned char)*p))
        p++;

    return (char *)p; /* Return pointer to the rest of the line */
}

/**
 * @brief Check if a label is valid according to assembler rules:
 * - Must start with a letter.
 * - Contain only alphanumeric characters.
 * - Not exceed MAX_LABEL_LENGTH (30 characters).
 * - Not be a reserved word.
 *
 * @param label Pointer to the label string to check.
 * @return TRUE if the label is valid, FALSE otherwise.
 */
BOOL is_valid_label(const char *label)
{
    int i, len;

    /* Check for null or empty label */
    if (!label || label[0] == '\0')
        return FALSE;

    /* Check if label starts with a letter */
    if (!isalpha((unsigned char)label[0]))
        return FALSE;

    len = strlen(label);

    /* Check the label length - must not exceed MAX_LABEL_LENGTH */
    if (len > MAX_LABEL_LENGTH)
        return FALSE;

    /* Check if the label contains only alphanumeric characters and underscores */
    for (i = 0; i < len; i++)
    {
        if (!isalnum((unsigned char)label[i]) && label[i] != '_')
            return FALSE;
    }

    /* Check if the label is a reserved word */
    if (is_reserved_word(label))
        return FALSE;

    return TRUE;
}

/* === Tokenization === */

/**
 * @brief Get the next token from a line.
 *
 * @param src Pointer to the source input line.
 * @param token_out Pointer to a buffer where the extracted token will be stored.
 * @return TRUE if a token was successfully extracted, FALSE if no more tokens are available.
 */
BOOL get_next_token(const char *src, char *token_out)
{
    int i = 0, j = 0;

    /* Checks for null pointers */
    if (!src || !token_out)
        return FALSE;

    /* Skip leading whitespace. */
    while (isspace((unsigned char)src[i]))
    {
        i++;
    }

    /* Check if we reached the end of the string. */
    if (src[i] == '\0')
    {
        return FALSE; /* No more tokens available */
    }

    /* Copy token until space or null. */
    while (src[i] && !isspace((unsigned char)src[i]) && j < MAX_LINE_LENGTH)
    {
        token_out[j++] = src[i++];
    }

    token_out[j] = '\0'; /* Null-terminate the token */

    return TRUE; /* Token successfully extracted */
}

/**
 * @brief Check if a word is a reserved word in the assembler.
 *
 * @param word Pointer to the word to check.
 * @return TRUE if the word is a reserved word, FALSE otherwise.
 */
BOOL is_reserved_word(const char *word)
{
    int i;

    /* === Instruction Reserved Names === */
    const char *instructions[] = {
        "mov", "cmp", "add", "sub", "lea",
        "clr", "not", "inc", "dec", "jmp",
        "bne", "jsr", "red", "prn", "rts", "stop"};

    /* === Register Reserved Names === */
    const char *
        registers[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

    /* === Directives Reserved Names === */
    const char *directives[] = {
        ".data", ".string", ".mat", ".entry", ".extern"};

    /*  Check against instruction names */
    for (i = 0; i < sizeof(instructions) / sizeof(instructions[0]); i++)
    {
        if (strcmp(word, instructions[i]) == 0)
            return TRUE; /* Found a reserved instruction. */
    }

    /* Check against register names */
    for (i = 0; i < sizeof(registers) / sizeof(registers[0]); i++)
    {
        if (strcmp(word, registers[i]) == 0)
            return TRUE; /* Found a reserved register. */
    }

    /* Check against directive names */
    for (i = 0; i < sizeof(directives) / sizeof(directives[0]); i++)
    {
        if (strcmp(word, directives[i]) == 0)
            return TRUE; /* Found a reserved directive. */
    }
    return FALSE; /* Not a reserved word */
}

/**
 * @brief Check if a word is a valid instruction name.
 *
 * This function checks if the given word matches any of the valid instruction names.
 *
 * @note I created such a function because of the frequency with which I use it.
 * @param word Pointer to the word to check.
 * @return TRUE if the word is a valid instruction, FALSE otherwise.
 */
BOOL is_instruction(const char *word)
{
    int i;

    /* Array of all valid instruction names */
    const char *instructions[] = {
        "mov", "cmp", "add", "sub", "lea",
        "clr", "not", "inc", "dec", "jmp",
        "bne", "jsr", "red", "prn", "rts", "stop"};

    /* Check if a word matches any instruction*/
    for (i = 0; i < sizeof(instructions) / sizeof(instructions[0]); i++)
    {
        if (strcmp(word, instructions[i]) == 0)
            return TRUE; /* Found a reserved instruction. */
    }

    return FALSE; /* Not a valid instruction name */
}

/**
 * @brief Remove comments from a line (everything after a ';').
 *
 * This function remove any in line comments
 * (comments on the same line as the assembler lines).
 *
 * @param line Pointer to the line to process.
 */
void remove_comments(char *line)
{
    char *comment_pos;

    /* Check for empty line */
    if (!line)
        return;

    comment_pos = strchr(line, ';');
    if (comment_pos != NULL)
        *comment_pos = '\0'; /* Truncate the string at the comment */
}