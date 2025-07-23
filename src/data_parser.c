/**
 * @file data_parser.c
 * @brief Implementation od the data parsing functions for assembler directives.
 */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../include/data_parser.h"
#include "../include/error.h"
#include "../include/constants.h"
#include "../include/first_pass.h"
#include "../include/data_image.h"

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

    /* Check for null line pointer */
    if (!line)
        return;

    /* Creates a copy of the line to not change it directly. */
    copy = strdup(line);

    /* Checks for memory allocation error. */
    if (!copy)
    {
        print_line_error(filename, line_num, ERROR_MEMORY_ALLOCATION_FAILED);
        err_found = TRUE;
        return;
    }

    /* Split the line by ',' and process each token */
    token = strtok(copy, ",");
    while (token != NULL)
    {
        /* Trim leading whitespaces */
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

        /* Check if empty string after trimming */
        if (strlen(token) == 0)
        {
            print_line_error(filename, line_num, ERROR_SYNTAX);
            err_found = TRUE;
            /* Moves to the next token in the comma seprated list. */
            token = strtok(NULL, ",");
            continue;
        }

        /* Convert to int */
        value = (int)strtol(token, &ptr, 10);

        /* Check if conversion was successful */
        if (*ptr != '\0')
        {
            print_line_error(filename, line_num, ERROR_INVALID_OPERAND);
            err_found = TRUE;
        }
        else
        {
            /* Stores the value into the data image */
            if (!store_data(value, filename, line_num))
            {
                /* Error reported by store_data */
                free(copy);
                return;
            }
            DC++;
        }

        /* Get next token */
        token = strtok(NULL, ",");
    }

    /* Free all allocated memory */
    free(copy);
}

/**
 * @brief Parses a ".string" directive and stores its characters into the data image.
 *
 * This function processes a string literal enclosed in double quotes. The string is
 * interpreted as sequence of printable ASCII characters, with each character stored
 * as one word in the data segemet. A null-terminator is appended to indicate the
 * end of the string.
 *
 * The parser extracts all characters between the first and last double quote on the line.
 * Quotes withing the string are allowed and treated as regular characters.
 * Any non-whitespace content following the closing quote is considered invalid.
 *
 * Errors are reported if:
 * - The string is not enclosed in double quotes.
 * - Characters fall outside the ASCII range [0-127].
 * - Trailing non-whitespce characters are present after the closing quote.
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

    /* Check for null line pointer*/
    if (!line)
        return;

    /* Skip leading whitespaces. */
    while (isspace((unsigned char)*line))
        line++;

    /* Checks if a line starts with a quote */
    if (*line != '"')
    {
        print_line_error(filename, line_num, ERROR_SYNTAX);
        err_found = TRUE;
        return;
    }

    /* Skip opening quote */
    start = line + 1;

    /* Find last quote (search from the end of the start string) */
    end = strrchr(start, '"');
    if (!end)
    {
        print_line_error(filename, line_num, ERROR_SYNTAX);
        err_found = TRUE;
        return;
    }

    /* Scan characters between start and end */
    for (p = start; p < end; p++)
    {
        ch = (unsigned char)*p;

        /* Check if a character is valid ASCII (0-127) */
        if (ch > 127)
        {
            print_line_error(filename, line_num, ERROR_DATA_OUT_OF_RANGE);
            err_found = TRUE;
        }
        else
        {
            if (!store_data((int)ch, filename, line_num))
                return;

            DC++;
        }
    }

    /* Append null terminator */
    if (!store_data(0, filename, line_num))
        return;

    /*Incremet the DC */
    DC++;

    /* Check for extra characters after the closing quote */
    p = end + 1;
    while (*p != '\0')
    {
        if (!isspace((unsigned char)*p))
        {
            print_line_error(filename, line_num, ERROR_SYNTAX);
            err_found = TRUE;
            return;
        }
        p++;
    }
}

/**
 * @brief Parses a ".mat" directive, extracts matrix dimentions and values,
 * and stores them into the data image.
 *
 * The matrix format must follow on of the patterns:
 * - [rows][cols] val1, val2, ..., valN
 * - [rows][cols]
 *
 * - The opening '[' must appear immediately aftee the directive name
 *   (no space between .mat and '[').
 * - Whitespace is permited only inside the brackets.
 * - The number of values must not exceed rows * cols.
 * - If fewer values are provided, the remaining elements are implicitly initialized to zero.
 * - Matrix values are stored row by row (left to right, top to bottom).
 *
 * Each valid value and dimenion increamets the data counter 'DC'.
 * The dimensions themselves occupy two words in the data image.
 *
 * Errors are reported if:
 * - Missing or malformed bracket structure.
 * - Non-numeric or invalid dimensions.
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
    char *copy = NULL, *values_part = NULL;
    char *p_start, *p_mid, *p_end;
    char *token;
    int actual_values = 0;
    char *endptr;

    /* Check for null line pointer */
    if (!line)
        return;

    /* Duplicate the line */
    copy = strdup(line);

    /* Checks for memory aloocation error */
    if (!copy)
    {
        print_line_error(filename, line_num, ERROR_MEMORY_ALLOCATION_FAILED);
        err_found = TRUE;
        return;
    }

    /* Skips leading whitespaces */
    while (isspace((unsigned char)*copy))
        copy++;

    /* Checks for '[' after ".mat" */
    if (*copy != '[')
    {
        print_line_error(filename, line_num, ERROR_INVALID_MATRIX);
        err_found = TRUE;
        free(copy);
        return;
    }

    /* Parse [rows] */
    p_start = copy + 1;
    p_mid = strchr(p_start, ']');

    /* Check for closing rows bracket */
    if (!p_mid)
    {
        print_line_error(filename, line_num, ERROR_INVALID_MATRIX);
        err_found = TRUE;
        free(copy);
        return;
    }
    *p_mid = '\0';

    /* Skip whitespaces inside the rows brackets */
    while (isspace((unsigned char)*p_start))
        p_start++;

    /* Converts the string pointed to by p_start into an integer,
        using base 10 (this is the rows value of the matrix) */
    rows = (int)strtol(p_start, &endptr, 10);

    /* Check if there are leftover characters after the number or rows < 0 */
    if (*endptr != '\0' || rows < 0)
    {
        print_line_error(filename, line_num, ERROR_INVALID_MATRIX);
        err_found = TRUE;
        free(copy);
        return;
    }

    /* Parse [cols] */

    /* Checks if immediately after the ']' there is a '[' */
    if (*(p_mid + 1) != '[')
    {
        print_line_error(filename, line_num, ERROR_INVALID_MATRIX);
        err_found = TRUE;
        free(copy);
        return;
    }

    /* Move a pointer 2 step forward (skips the ']' and the '[') */
    p_end = strchr(p_mid + 2, ']');
    if (!p_end)
    {
        print_line_error(filename, line_num, ERROR_INVALID_MATRIX);
        err_found = TRUE;
        free(copy);
        return;
    }
    *p_end = '\0';

    p_start = p_mid + 2;

    /* Skip whitespaces inside the rows brackets */
    while (isspace((unsigned char)*p_start))
        p_start++;

    /* Converts the string pointed to by p_start into an integer,
    using base 10 (this is the cols value of the matrix) */
    cols = (int)strtol(p_start, &endptr, 10);

    /* Checks if there are leftover characters after the number or cols < 0 */
    if (*endptr != '\0')
    {
        print_line_error(filename, line_num, ERROR_INVALID_MATRIX);
        err_found = TRUE;
        free(copy);
        return;
    }

    /* Calculate the expected numbers in the matrix */
    exp_vals = rows * cols;

    /* Skip to start of value list */
    values_part = p_end + 1;

    /* Skip whitespaces inside the rows brackets */
    while (isspace((unsigned char)*p_start))
        p_start++;

    /* Parse matrix values (if any)*/
    if (*values_part != '\0')
    {
        token = strtok(values_part, ",");
        while (token != NULL)
        {
            /* Skip spaces bweteen ',' */
            while (isspace((unsigned char)*token))
                token++;

            /* Checks if empty */
            if (*token == '\0')
            {
                print_line_error(filename, line_num, ERROR_SYNTAX);
                err_found = TRUE;
            }
            else
            {
                /* Checks if valid integer */
                val = (int)strtol(token, &endptr, 10);
                if (*endptr != '\0')
                {
                    print_line_error(filename, line_num, ERROR_INVALID_OPERAND);
                    err_found = TRUE;
                }
                else
                {
                    actual_values++;
                    /*ToDo: store values in data image */
                    if (!store_data(val, filename, line_num))
                    {
                        free(copy);
                        return
                    }
                    DC++;
                }
            }
            /* Gets the next tokem from the comma seprated ;ist of matrix values */
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
                free(copt);
                return;
            }
            DC++;
        }
    }

    /* Report error if overfilled */
    if (actual_values > exp_vals)
    {
        print_line_error(filename, line_num, ERROR_TOO_MANY_OPERANDS);
        err_found = TRUE;
    }

    /* Add space for dimensions */

    /* Store rows dimension */
    if (!store_data(rows, filename, line_num))
    {
        free(copy);
        return;
    }
    DC++;

    /* Store cols dimension */
    if (!store_data(cols, filename, line_num))
    {
        free(copy);
        return;
    }
    DC++;

    /* Free memory of copy */
    free(copy);
}
