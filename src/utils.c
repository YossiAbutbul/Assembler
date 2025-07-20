/**
 * @file utils.c
 * @brief Implementation of utility functions used in the assembler.
 */

#include <string.h>
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

    if (!str || *str == '\0')
    {
        return;
    }

    start = str;
    /* Trim leading whitespaces */
    while (*start && issapce((unsigned char)*start))
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

    if (!str)
        return TRUE;

    for (i = 0; str[i] != '\0'; i++){
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

    if (!line)
        return FALSE;

    /* Skip leading whitespaces */
    for (i = 0; line[i] != '\0'; i++){
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
BOOL extract_label(const char *line, char *label_out){
    int i = 0, j = 0;
    
    if (!line || !label_out)
        return FALSE;

    /* Skip leading whitespace. */
    while (isspace((unsigned char)line[i]))
        i++;
    
    /* Check if the label starts with a letter. */
    if(!isalpha((unsigned char)line[i]))
        return FALSE;
    
    /* Copy the label until we hit a ':' */
    while (line[i] && !issapce((unsigned char)line[i]) && line[i] != ':' && j < MAX_LABEL_LENGTH){
        label_out[j++] = line[i++];
    }

    if (line[i] == ':' && j > 0) {
        label_out[j] = '\0';    /* Null-terminate the label */
        return TRUE;            /* Valid label found */
    }
    return FALSE; /* No valid label found */
}


/**
 * @brief Check if a string starts with a given prefix.
 *
 * @param str Pointer to the string to check.
 * @param prefix Pointer to the prefix to match.
 * @return TRUE if the string starts with the prefix, FALSE otherwise.
 */
BOOL starts_with(const char *str, const char *prefix)
{
    size_t len_prefix, len_str; /* used size_t in order to be always positive */

    if (!str || !prefix)
        return FALSE;

    len_prefix = strlen(prefix);
    len_str = strlen(str);

    if (len_str < len_prefix)
        return FALSE; /* str is shorter than prefix */

    return (strncmp(str, prefix, len_prefix) == 0) ? TRUE : FALSE; /* Compare the beginning of str with prefix. if not equal means there is no match with the prefix. */
}