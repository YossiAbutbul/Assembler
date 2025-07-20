/**
* @file first_pass.h
* @brief Implements the first pass of the assembler.
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "../include/first_pass.h"
#include "../include/symbol_table.h"
#include "../include/utils.h"
#include "../include/error.h"
#include "../include/constants.h"

int IC = BASE_IC_ADDRESS; /* Instruction counter */
int DC = 0; /* Data counter */
BOOL err_found = FALSE; /* Flag to indicate if an error was found */

/* === Internal Helper Prototype function === */
static void process_line(const char *line, const char *filename, int line_num);

/**
 * @brief Preforms the first pass on the given .am source file.
 * 
 * Scans the file line by line to:
 * - Identify labels, instructions and directives.
 * - Add symbols to the symbol table.
 * - Count IC and DC.
 * - Validate syntax and operand legality.
 * - Print errors if any are found.
 *
 * @param am_file Open file pointer to the .am source file (after macro expansion).
 * @param filename Name of the source file (for error reporting).
 * @return TRUE if the first pass was successful, FALSE otherwise.
 */
void firs_pass(FILE am_file, const char *filename)
{
    char line[MAX_LINE_LENGTH];
    int line_num = 0;

    IC = BASE_IC_ADDRESS;   /* Reset instruction counter */
    DC = 0;                 /* Reset data counter */
    err_found = FALSE;      /* Reset error flag */

    while (fgets(line, sizeof(line), am_file))
    {
        line_num++;
        
        /* Checks if the line exceed MAX_LINE_LENGTH */
        if (strchr(line, '\n') == NULL && !feof(am_file))
        {
            print_line_error(filename, line_num, ERROR_SYNTAX);
            err_found = TRUE;
            /* Skips the rest of the line */
            while (!feof(am_files) && fgetc(am_file) != '\n')
                continue;
            
        }
        trim_whitespace(line); /* Trim leading and trailing whitespace */

        if (is_whitespace(line) || is_comment(line))
            continue; /* Skip empty lines and comments */
        
        /* Process the line */
        process_line(line, filename, line_num); 

        /* Update data symbols with the current IC */
        update_data_symbols(IC); 
    }
}

static void process_line(const char *line, const char *filename, int line_num)
{
    char label[MAX_LABEL_LENGTH + 1]; /* +1 for the '\0' */
    char first_token[MAX_LINE_LENGTH + 1]; /* +1 for the '\0' */
    char *rest_of_line = NULL;
    BOOL has_label = FALSE;
}