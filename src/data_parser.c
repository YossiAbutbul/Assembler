/**
 * @file data_parser.c
 * @brief Implementation of the data parsing functions for assembler directives.
 */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../include/data_parser.h"
#include "../include/error.h"
#include "../include/constants.h"
#include "../include/first_pass.h"
#include "../include/data_image.h"
#include "../include/utils.h"

/**
 * @brief Check if the data line has missing commas between numbers
 *
 * @param line The line to check.
 * @return TRUE if missing commas detected, FALSE otherwise.
 * @note This is an internal function used only in this file.
 */
static BOOL has_missing_commas(const char *line)
{
    const char *p = line;
    int number_count = 0;
    int comma_count = 0;
    BOOL in_number = FALSE;

    /* Skip leading whitespace */
    while (isspace((unsigned char)*p))
        p++;

    while (*p && *p != ';') /* Stop at comment */
    {
        if (isdigit((unsigned char)*p) || (*p == '-' && isdigit((unsigned char)*(p + 1))))
        {
            /* Found start of a number */
            if (!in_number)
            {
                number_count++;
                in_number = TRUE;
            }
        }
        else if (*p == ',')
        {
            comma_count++;
            in_number = FALSE;
        }
        else if (isspace((unsigned char)*p))
        {
            in_number = FALSE;
        }
        else
        {
            /* Invalid character - let other validation handle it */
            in_number = FALSE;
        }
        p++;
    }

    /* If we have multiple numbers, we should have (number_count - 1) commas */
    /* Example: "1 2 3" has 3 numbers, should have 2 commas, but has 0 */
    if (number_count > 1 && comma_count != (number_count - 1))
    {
        return TRUE;
    }

    return FALSE;
}

/**
 * @brief Parses the operand list of a ".data" directive.
 *
 * This function extracts a comma-separated list of integers and adds
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
    BOOL found_comma = FALSE;
    const char *p;
    char *endptr;
    long temp_value;

    /* Check for null line pointer */
    if (!line)
        return;

    /* Skip leading whitespaces */
    while (isspace((unsigned char)*line))
        line++;

    /* Check if line is empty after directive */
    if (*line == '\0')
    {
        print_line_error(filename, line_num, ERROR_SYNTAX);
        err_found = TRUE;
        return;
    }

    /* Check for leading comma */
    if (*line == ',')
    {
        print_line_error(filename, line_num, ERROR_SYNTAX);
        err_found = TRUE;
        return;
    }

    /* Check for trailing comma */
    p = line + strlen(line) - 1;
    while (p > line && isspace((unsigned char)*p))
        p--;
    if (*p == ',')
    {
        print_line_error(filename, line_num, ERROR_SYNTAX);
        err_found = TRUE;
        return;
    }

    /* Check for double commas */
    for (p = line; *p; p++)
    {
        if (*p == ',')
        {
            if (found_comma)
            {
                print_line_error(filename, line_num, ERROR_SYNTAX);
                err_found = TRUE;
                return;
            }
            found_comma = TRUE;
        }
        else if (!isspace((unsigned char)*p))
        {
            found_comma = FALSE;
        }
    }

    /* Check for missing commas between numbers */
    if (has_missing_commas(line))
    {
        print_line_error(filename, line_num, ERROR_MISSING_COMMA);
        err_found = TRUE;
        return;
    }

    /* Creates a copy of the line to not change it directly. */
    copy = (char *)malloc(strlen(line) + 1);

    /* Checks for memory allocation error. */
    if (!copy)
    {
        print_line_error(filename, line_num, ERROR_MEMORY_ALLOCATION_FAILED);
        err_found = TRUE;
        return;
    }
    strcpy(copy, line);

    /* Split the line by ',' and process each token */
    token = strtok(copy, ",");
    while (token != NULL)
    {
        /* Trim leading whitespaces */
        while (isspace((unsigned char)*token))
            token++;

        /* Assign a ptr to the end of the token. */
        ptr = token + strlen(token) - 1;

        /* Trim trailing whitespaces */
        while (ptr > token && isspace((unsigned char)*ptr))
        {
            *ptr = '\0';
            ptr--;
        }

        /* Check if the token is empty after trimming */
        if (*token == '\0')
        {
            print_line_error(filename, line_num, ERROR_SYNTAX);
            err_found = TRUE;
            free(copy);
            return;
        }

        /* Validate that token is a valid number */
        temp_value = strtol(token, &endptr, 10);

        /* Check if conversion was successful */
        if (*endptr != '\0' || endptr == token)
        {
            print_line_error(filename, line_num, ERROR_INVALID_OPERAND);
            err_found = TRUE;
            free(copy);
            return;
        }

        /* Convert to integer */
        value = (int)temp_value;

        /* Validate range (-512 to +511) */
        if (value < -512 || value > 511)
        {
            print_line_error(filename, line_num, ERROR_DATA_OUT_OF_RANGE);
            err_found = TRUE;
            /* Continue to check other values and report all range errors */
        }
        else
        {
            /* Store the data value only if no errors */
            if (!store_data(value, filename, line_num))
            {
                free(copy);
                return;
            }

            /* Increment data counter */
            DC++;
        }

        /* Get the next token */
        token = strtok(NULL, ",");
    }

    free(copy);
}

/**
 * @brief Parses a ".string" directive and stores its characters into the data image.
 *
 * This function processes a string literal enclosed in double quotes. The string is
 * interpreted as sequence of printable ASCII characters, with each character stored
 * as one word in the data segment. A null-terminator is appended to indicate the
 * end of the string.
 *
 * The parser extracts all characters between the first and last double quote on the line.
 * Quotes within the string are allowed and treated as regular characters.
 * Any non-whitespace content following the closing quote is considered invalid.
 *
 * Errors are reported if:
 * - The string is not enclosed in double quotes.
 * - Characters fall outside the ASCII range [0-127].
 * - Trailing non-whitespace characters are present after the closing quote.
 *
 * @param line      Pointer to the line content after the  ".string" directive.
 * @param filename  Pointer to the source file (for error reporting).
 * @param line_num  The current line number (for error reporting).
 */
void parse_string_value(const char *line, const char *filename, int line_num)
{
    const char *start, *end;
    const char *p;
    unsigned char ch;
    char *line_copy;
    BOOL has_invalid_char = FALSE;

    /* Check for null line pointer*/
    if (!line)
        return;

    /* Creates a copy of the line and remove comments */
    line_copy = (char *)malloc(strlen(line) + 1);
    if (!line_copy)
    {
        print_line_error(filename, line_num, ERROR_MEMORY_ALLOCATION_FAILED);
        err_found = TRUE;
        return;
    }

    strcpy(line_copy, line);
    remove_comments(line_copy);

    /* Skip leading whitespaces */
    p = line_copy;
    while (isspace((unsigned char)*line))
        p++;

    /* Check for any non-whitespace characters before the first quote */
    if (*p != '"' && *p != '\0')
    {
        print_line_error(filename, line_num, ERROR_STRING_MISSING_QUOTES);
        err_found = TRUE;
        free(line_copy);
        return;
    }

    /* Check if line starts with a quote */
    if (*p != '"')
    {
        print_line_error(filename, line_num, ERROR_STRING_MISSING_QUOTES);
        err_found = TRUE;
        free(line_copy);
        return;
    }

    /* Skip opening quote */
    start = p + 1;

    /* Find last quote (search from the end) */
    end = strrchr(start, '"');
    if (!end)
    {
        print_line_error(filename, line_num, ERROR_STRING_UNCLOSED);
        err_found = TRUE;
        free(line_copy);
        return;
    }

    /* Scan characters between start and end */
    for (p = start; p < end; p++)
    {
        ch = (unsigned char)*p;

        /* Check if a character is valid ASCII (0-127) */
        if (ch < 32 || ch > 126) /* Printable ASCII characters */
        {
            if (!has_invalid_char)
            {
                print_line_error(filename, line_num, ERROR_STRING_INVALID_CHARACTER);
                err_found = TRUE;
                has_invalid_char = TRUE;
            }
        }
        else if (!has_invalid_char)
        {
            if (!store_data((int)ch, filename, line_num))
            {
                free(line_copy);
                return;
            }
            DC++;
        }
    }

    /* Append null terminator only if no errors occurred */
    if (!has_invalid_char && !err_found)
    {
        if (!store_data(0, filename, line_num))
        {
            free(line_copy);
            return;
        }
        DC++;
    }

    /* Check for extra characters after the closing quote */
    p = end + 1;
    while (*p != '\0')
    {
        if (!isspace((unsigned char)*p))
        {
            /* We hit semicolon - it is a comment */
            if (*p == ';')
                break;

            /* Otherwise, invalid ending */
            print_line_error(filename, line_num, ERROR_SYNTAX);
            err_found = TRUE;
            free(line_copy);
            return;
        }
        p++;
    }
    free(line_copy);
}

/**
 * @brief Parses a ".mat" directive, extracts matrix dimensions and values,
 * and stores them into the data image.
 *
 * The matrix format must follow on of the patterns:
 * - [rows][cols] val1, val2, ..., valN
 * - [rows][cols]
 *
 * - The opening '[' must appear immediately after the directive name
 *   (no space between .mat and '[').
 * - Whitespace is permitted only inside the brackets.
 * - The number of values must not exceed rows * cols.
 * - If fewer values are provided, the remaining elements are implicitly initialized to zero.
 * - Matrix values are stored row by row (left to right, top to bottom).
 *
 * Each valid value increments the data counter 'DC'.
 *
 * Errors are reported if:
 * - Missing or malformed bracket structure.
 * - Non-numeric or invalid dimensions.
 * - Zero or negative dimenstions.
 * - Invalid operands.
 * - Too many values are provided.
 *
 * @param line      Pointer to the string following the ".mat" directive (starting with '[').
 * @param filename  Pointer to the source file (for error reporting).
 * @param line_num  The current line number (for error reporting).
 */
void parse_matrix(const char *line, const char *filename, int line_num)
{
    int rows, cols, exp_vals, val, i;
    char *copy, *values_part, *ptr;
    char *p_start, *p_mid, *p_end;
    char *token;
    int actual_values;
    char *endptr;

    /* Initialization */
    copy = NULL;
    values_part = NULL;
    actual_values = 0;

    /* Check for null line pointer */
    if (!line)
        return;

    /* Duplicate the line */
    copy = (char *)malloc(strlen(line) + 1);

    /* Checks for memory allocation error */
    if (!copy)
    {
        print_line_error(filename, line_num, ERROR_MEMORY_ALLOCATION_FAILED);
        err_found = TRUE;
        return;
    }
    strcpy(copy, line);

    /* Use ptr for parsing, keep copy for free() */
    ptr = copy;

    /* Skip leading whitespaces */
    while (isspace((unsigned char)*ptr))
        ptr++;

    /* Checks for '[' after ".mat" */
    if (*ptr != '[')
    {
        print_line_error(filename, line_num, ERROR_INVALID_MATRIX);
        err_found = TRUE;
        free(copy);
        return;
    }

    /* Find first ']' for rows */
    p_start = ptr + 1; /* Skips the '[' */
    p_mid = strchr(p_start, ']');

    /* Check for closing rows bracket */
    if (!p_mid)
    {
        print_line_error(filename, line_num, ERROR_INVALID_MATRIX);
        err_found = TRUE;
        free(copy);
        return;
    }

    /* Parse rows directly from the string */
    *p_mid = '\0'; /* Temporarily null-terminate */

    /* Skip whitespace at the beginning of rows string */
    while (isspace((unsigned char)*p_start))
        p_start++;

    /* Check if empty after trimming whitespace */
    if (*p_start == '\0')
    {
        print_line_error(filename, line_num, ERROR_INVALID_MATRIX);
        err_found = TRUE;
        free(copy);
        return;
    }

    /* Check for comma in rows (invalid syntax like [3,3]) */
    if (strchr(p_start, ','))
    {
        print_line_error(filename, line_num, ERROR_INVALID_MATRIX);
        err_found = TRUE;
        free(copy);
        return;
    }

    rows = (int)strtol(p_start, &endptr, 10);

    /* Trim trailing whitespace by checking endptr */
    while (isspace((unsigned char)*endptr))
        endptr++;

    if (*endptr != '\0')
    {
        print_line_error(filename, line_num, ERROR_INVALID_MATRIX);
        err_found = TRUE;
        free(copy);
        return;
    }

    if (rows <= 0)
    {
        print_line_error(filename, line_num, ERROR_INVALID_MATRIX_DIMENSIONS);
        err_found = TRUE;
        free(copy);
        return;
    }

    /* Restore the ']' and check for second '[' */
    *p_mid = ']';
    p_start = p_mid + 1;
    if (*p_start != '[')
    {
        print_line_error(filename, line_num, ERROR_INVALID_MATRIX);
        err_found = TRUE;
        free(copy);
        return;
    }

    /* Find second ']' for cols */
    p_start++; /* Skip the '[' */
    p_end = strchr(p_start, ']');
    if (!p_end)
    {
        print_line_error(filename, line_num, ERROR_INVALID_MATRIX);
        err_found = TRUE;
        free(copy);
        return;
    }

    /* Parse cols directly from the string */
    *p_end = '\0'; /* Temporarily null-terminate */

    /* Skip whitespace at the beginning of cols string */
    while (isspace((unsigned char)*p_start))
        p_start++;

    /* Check if empty after trimming whitespace */
    if (*p_start == '\0')
    {
        print_line_error(filename, line_num, ERROR_INVALID_MATRIX);
        err_found = TRUE;
        free(copy);
        return;
    }

    /* Check for comma in cols (invalid syntax like [3,3]) */
    if (strchr(p_start, ','))
    {
        print_line_error(filename, line_num, ERROR_INVALID_MATRIX);
        err_found = TRUE;
        free(copy);
        return;
    }

    cols = (int)strtol(p_start, &endptr, 10);

    /* Trim trailing whitespace by checking endptr */
    while (isspace((unsigned char)*endptr))
        endptr++;

    if (*endptr != '\0')
    {
        print_line_error(filename, line_num, ERROR_INVALID_MATRIX);
        err_found = TRUE;
        free(copy);
        return;
    }

    /* Validate cols dimension */
    if (cols <= 0)
    {
        print_line_error(filename, line_num, ERROR_INVALID_MATRIX_DIMENSIONS);
        err_found = TRUE;
        free(copy);
        return;
    }

    /* Restore the ']' */
    *p_end = ']';

    /* Calculate the expected numbers in the matrix */
    exp_vals = rows * cols;

    /* Skip to start of value list */
    values_part = p_end + 1;

    /* Skip whitespaces after closing bracket */
    while (isspace((unsigned char)*values_part))
        values_part++;

    /* Parse matrix values (if any)*/
    if (*values_part != '\0')
    {
        token = strtok(values_part, ",");
        while (token != NULL)
        {
            /* Skip spaces between ',' */
            while (isspace((unsigned char)*token))
                token++;

            /* Remove trailing whitespace */
            endptr = token + strlen(token) - 1;
            while (endptr > token && isspace((unsigned char)*endptr))
            {
                *endptr = '\0';
                endptr--;
            }

            /* Check if empty */
            if (*token == '\0')
            {
                print_line_error(filename, line_num, ERROR_SYNTAX);
                err_found = TRUE;
            }
            else
            {
                /* Check if valid integer */
                val = (int)strtol(token, &endptr, 10);

                if (*endptr != '\0')
                {
                    print_line_error(filename, line_num, ERROR_INVALID_OPERAND);
                    err_found = TRUE;
                }
                else
                {
                    actual_values++;

                    /* Check if we have too many values */
                    if (actual_values > exp_vals)
                    {
                        print_line_error(filename, line_num, ERROR_MATRIX_TOO_MANY_VALUES);
                        err_found = TRUE;
                        break;
                    }

                    if (!store_data(val, filename, line_num))
                    {
                        free(copy);
                        return;
                    }
                    DC++;
                }
            }
            token = strtok(NULL, ",");
        }
    }

    /* Fill missing values with 0 */
    if (actual_values < exp_vals)
    {
        for (i = 0; i < (exp_vals - actual_values); i++)
        {
            /* Store 0 in the data image */
            if (!store_data(0, filename, line_num))
            {
                free(copy);
                return;
            }
            DC++;
        }
    }

    /* Free memory of copy */
    free(copy);
}
