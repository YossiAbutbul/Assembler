#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../include/data_parser.h"
#include "../include/error.h"
#include "../include/constants.h"

extern int DC;
extern BOOL error_found;

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
void parse_data_values(const char *line, const char *filename, int line_num)
{
    char *copy, *token, *ptr;
    int value;
    BOOL expect_value = TRUE;

    if (!line)
        return;

    /* Creates a copy of the line to not change it directly. */
    copy = strdup(line);

    /* Checks for memory allocation error. */
    if (!copy)
    {
        print_line_error(filename, line_num, ERROR_MEMORY_ALLOCATION_FAILED);
        error_found = TRUE;
        return;
    }

    /* Split the line by ',' */
    token = strtok(copy, ",");
    while (token != NULL)
    {
        /* Trim whitespaces */
        while (isspace((unsigned char)*token))
            token++;

        /* Assign a ptr to the end of the token. */
        ptr = token + strlen(token) - 1;
        /* Remove trailing whitespaces from the end. */
        while (ptr > token && isspace((unsigned char)*ptr))
        {
            *ptr = '\0';
            ptr--;
        }

        /* Check if empty string */
        if (strlen(token) == 0)
        {
            print_line_error(filename, line_num, ERROR_SYNTAX);
            error_found = TRUE;
            /* Moves to the next token in the comma seprated list. */
            token = strtok(NULL, ",");
            continue;
        }

        /* Convert to int. */
        value = (int)strtol(token, &ptr, 10);
        if (*ptr != '\0')
        {
            print_line_error(filename, line_num, ERROR_INVALID_OPERAND);
            error_found = TRUE;
        }
        else
        {
            /*ToDo: store the 'value' into the data image */
            DC++;
        }

        token = strtok(NULL, ",");
    }

    free(copy);
}

void parse_string_value(const char *line, const char *filename, int line_num)
{
    const char *start, *end;
    int i, len;

    if (!line)
        return;

    /* Skip leading whitespaces */
    while (isspace((unsigned char)*line))
        line++;

    /* Checks if starts with '"' (after the ".string"). */
    if (*line != '"')
    {
        print_line_error(filename, line_num, ERROR_SYNTAX);
        error_found = TRUE;
        return;
    }

    /* Skips the opening quote */
    start = ++line;

    /* Find the closing quote */
    end = strchr(start, '"');

    /* If no closing quote */
    if (!end)
    {
        print_line_error(filename, line_num, ERROR_SYNTAX);
        error_found = TRUE;
        return;
    }

    len = end - start;

    for (i = 0; i < len; i++)
    {
        /*ToDo: Store start[i] into the data image */
        DC++;
    }

    DC++;
    /*todo: update to supports 0-127 ascii value in strings */
}