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
 * @param am_file Pointer to the .am source file (after macro expansion).
 * @param filename Pointer to the source file (for error reporting).
 * @return TRUE if the first pass was successful, FALSE otherwise.
 */
void first_pass(FILE *am_file, const char *filename)
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
            while (!feof(am_file) && fgetc(am_file) != '\n')
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
static void process_line(const char *line, const char *filename, int line_num)
{
    char label[MAX_LABEL_LENGTH + 1]; /* +1 for the '\0' */
    char first_token[MAX_LINE_LENGTH + 1]; /* +1 for the '\0' */
    char *rest = NULL;
    BOOL has_label = FALSE;

    /* Create copy of the line. */
    char buffer[MAX_LINE_LENGTH + 1];
    strncpy(buffer, line, MAX_LINE_LENGTH);
    buffer[MAX_LINE_LENGTH] = '\0'; /* Ensure null-termination */
    rest = buffer;

    /* Step 1 - Extract label if exsist. */
    if (extract_label(rest, label)){
        has_label = TRUE;
    

        if (!is_valid_label(label)){
            print_line_error(filename, line_num, ERROR_INVALID_LABEL);
            err_found = TRUE;
            return; /* Skip processing this line */
        }

        if (is_label_defined(label)){
            print_line_error(filename, line_num, ERROR_DUPLICATE_LABEL);
            err_found = TRUE;
            return; /* Skip processing this line */
        }

        /* Remove label (and ':') from the line. */
        rest = skip_label(rest); 
    }

    /* Step 2 - Extract first token (opcode or directive). */
    if (!get_next_token(rest, first_token)){
        print_line_error(filename, line_num, ERROR_SYNTAX);
        err_found = TRUE;
        return; /* Skip processing this line */
    }

    /* Step 3 - Handle Directives */
    if (strcmp(first_token, ".data") == 0 || strcmp(first_token, ".string") == 0 || strcmp(first_token, ".mat") == 0){
        /* ToDo: implement handle_entry_directive function. */
    }
    else if (strcmp(first_token, ".entry") == 0){
        /* ToDo: implement handle_entry_directive function. */
    }
    else if (strcmp(first_token, ".extern") == 0){
        if (has_label){
            print_line_error(filename, line_num, ERROR_LABEL_ON_EXTERN);
            err_found = TRUE;
            return; /* Skip processing this line */
        }
        /* TODO: implement handle_extern_directive function. */
    }

    /* Step 4 - Handle Instructions */
    else if (is_instruction(first_token)){
        /* TODO: implement handle_instruction function. */
    } else {
        print_line_error(filename, line_num, ERROR_UNKNOWN_INSTRUCTION);
        err_found = TRUE;
        return; /* Skip processing this line */
    }      
}