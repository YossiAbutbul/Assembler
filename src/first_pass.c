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
#include "../include/data_parser.h"
#include "../include/instruction_parser.h"

/* Initalize IC and DC */
int IC = BASE_IC_ADDRESS; /* Instruction counter */
int DC = 0;               /* Data counter */

/* Initalize ERR flag */
BOOL err_found = FALSE; /* Flag to indicate if an error was found */

/* === Internal Helper Prototypes functions === */
static void process_line(const char *line, const char *filename, int line_num);
static void handle_extern_directive(const char *line, const char *filename, int line_num);
static void handle_entry_directive(const char *line, const char *filename, int line_num);
static void handle_data_directive(const char *label, const char *directive, const char *line, const char *filename, int lineno);
static void handle_instruction(const char *label, const char *filename, int line_number);

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

    IC = BASE_IC_ADDRESS; /* Reset instruction counter */
    DC = 0;               /* Reset data counter */
    err_found = FALSE;    /* Reset error flag */

    /* Process each line in the file */
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
        /* Trim leading and trailing whitespace */
        trim_whitespace(line);

        /* Skips empty lines and comments */
        if (is_whitespace(line) || is_comment(line))
            continue; /* Skip empty lines and comments */

        /* Process the line */
        process_line(line, filename, line_num);

        /* Update data symbols with the current IC */
        update_data_symbols(IC);
    }
}

/* === Internal Helper functions === */

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
    char label[MAX_LABEL_LENGTH + 1];      /* +1 for the '\0' */
    char first_token[MAX_LINE_LENGTH + 1]; /* +1 for the '\0' */
    char *rest = NULL;
    BOOL has_label = FALSE;

    /* Create copy of the line. */
    char buffer[MAX_LINE_LENGTH + 1];
    strncpy(buffer, line, MAX_LINE_LENGTH);
    buffer[MAX_LINE_LENGTH] = '\0'; /* Ensure null-termination */
    rest = buffer;

    /* Step 1 - Extract label if exsist. */
    if (extract_label(rest, label))
    {
        has_label = TRUE;

        /* Validate the extracted label */
        if (!is_valid_label(label))
        {
            print_line_error(filename, line_num, ERROR_INVALID_LABEL);
            err_found = TRUE;
            return; /* Skip processing this line */
        }

        /* Check for duplicate label definition */
        if (is_label_defined(label))
        {
            print_line_error(filename, line_num, ERROR_DUPLICATE_LABEL);
            err_found = TRUE;
            return; /* Skip processing this line */
        }

        /* Remove label (and ':') from the line. */
        rest = skip_label(rest);
    }

    /* Step 2 - Extract first token (opcode or directive). */
    if (!get_next_token(rest, first_token))
    {
        print_line_error(filename, line_num, ERROR_SYNTAX);
        err_found = TRUE;
        return; /* Skip processing this line */
    }

    /* Step 3 - Handle Directives */
    if (strcmp(first_token, ".data") == 0 || strcmp(first_token, ".string") == 0 || strcmp(first_token, ".mat") == 0)
    {
        handle_data_directive(has_label ? label : NULL, first_token, rest, filename, line_num);
    }
    else if (strcmp(first_token, ".entry") == 0)
    {
        handle_entry_directive(rest, filename, line_num);
    }
    else if (strcmp(first_token, ".extern") == 0)
    {
        /* Labels cannot be defined on .extern lines */
        if (has_label)
        {
            print_line_error(filename, line_num, ERROR_LABEL_ON_EXTERN);
            err_found = TRUE;
            return; /* Skip processing this line */
        }
        handle_extern_directive(rest, filename, line_num);
    }

    /* Step 4 - Handle Instructions */
    else if (is_instruction(first_token))
    {
        handle_instruction(has_label ? label : NULL, line, filename, line_num);
    }
    /* Step 5 - Handle Unknown Instructions/Directives */
    else
    {
        print_line_error(filename, line_num, ERROR_UNKNOWN_INSTRUCTION);
        err_found = TRUE;
        return; /* Skip processing this line */
    }
}

/***
 * @brief Handles instruction lines during the first pass.
 *
 * This function parses an instruction line, validates its syntax and operands,
 * adds any label to the symbol table, and updates the IC (instruction counter).
 *
 * @param label     Optional label for instruction (can be NULL if none).
 * @param line      The complete instruction line.
 * @param filename  Name of the source file (for error reporting).
 * @param line_num  The current line number in the source file (for error reporting).
 */
static void handle_instruction(const char *label, const char *filename, int line_number)
{
    Instruction instruction;
    char *instruction_part;
    char line_copy[MAX_LINE_LENGTH + 1];

    /* Create a copy of the line for processing */
    strncpy(line_copy, line, MAX_LINE_LENGTH);
    line_copy[MAX_LINE_LENGTH] = '\0';

    /* Skip the label part if it exists */
    instruction_part = line_copy;
    if (label != NULL)
        instruction_part = skip_label(instruction_part);

    /* Add label to symbol table if it exists */
    if (label != NULL)
    {
        if (!add_symbol(label, IC, SYMBOL_CODE))
        {
            print_line_error(filename, line_num, ERROR_DUPLICATE_LABEL);
            err_found = TRUE;
            return;
        }
    }

    /* Parse the instruction */
    if (!parse_instruction(instruction_part, filename, line_num, &instruction))
        return; /* Error already reported bt parse_instruction */

    /* Update instruction counter (IC) by the word count of this instruction */
    IC += instruction.word_count;
}

/**
 * @brief Handles the "".extern" directive in the first pass.
 *
 * This function parses the operand after ".extern", validates it is a legal label,
 * and adds it to the symbol table with the type SYMBOL_EXTERNAL type and address 0 (ToDo: Check the addres).
 *
 * Errors are reported if:
 * - No Operand is found.
 * - The label is unvalid or already defined.
 *
 * @param line       The line content after the ".extern" directive.
 * @param filename   Pointer to the file name of the source file (for error reporting).
 * @param line_num   The current line number in the source file.
 */
static void handle_extern_directive(const char *line, const char *filename, int line_num)
{
    char label[MAX_LABEL_LENGTH + 1];

    /* Skip leading whitespace. */
    while (isspace(*line))
        line++;

    /* Checks if the next token can be extracted. */
    if (!get_next_token(line, label))
    {
        print_line_error(filename, line_num, ERROR_SYNTAX);
        err_found = TRUE;
        return; /* Skip processing this line. */
    }

    /* Validate the label. */
    if (!is_valid_label(label))
    {
        print_line_error(filename, line_num, ERROR_INVALID_LABEL);
        err_found = TRUE;
        return; /* Skip processing this line. */
    }

    /* Check if the label already exists in the symbol table. */
    if (is_label_defined(label))
    {
        print_line_error(filename, line_num, ERROR_DUPLICATE_LABEL);
        err_found = TRUE;
        return; /* Skip processing this line. */
    }

    /* Add the label to the symbol table as an external symbol. */
    add_symbol(label, 0, SYMBOL_EXTERNAL);
}

/**
 * @brief Handles the ".entry" directive during the first pass.
 *
 * This function extracts the label after the ".entry" directive and validates it.
 * The label is not added to the symbol table in the first pass, instead it is
 * marked using the `mark_symbol_as_entry` function during the second pass.
 *
 * However, the label name is still stored and validates for syntax duplication
 * in order to catch ".entry" on a label that's also defined as ".extern".
 *
 * Errors are reported if:
 * - No operand is found after ".entry".
 * - The label is invalid or already defined.
 *
 * @param line       The line content after the ".entry" directive.
 * @param filename   Pointer to the file name of the source file (for error reporting).
 * @param line_num   The current line number in the source file.
 */
static void handle_entry_directive(const char *line, const char *filename, int line_num)
{
    char label[MAX_LABEL_LENGTH + 1];

    /* Skip leading whitespace. */
    while (isspace((unsigned char)*line))
        line++;

    /* Checks if the next token can be extracted. */
    if (!get_next_token(line, label))
    {
        print_line_error(filename, line_num, ERROR_SYNTAX);
        err_found = TRUE;
        return; /* Skip processing this line. */
    }

    /* Validate the label. */
    if (!is_valid_label(label))
    {
        print_line_error(filename, line_num, ERROR_INVALID_LABEL);
        err_found = TRUE;
        return; /* Skip processing this line. */
    }
    /* Note: the actual marking as entry will be on the second pass, for now just validate */
}

/**
 * @brief Handles "."data", ".string", and ".mat" directives during the first pass.
 *
 * This function parses and validates the operand(s) after a data-related directive.
 * For a valid labels, it adds the symbol to the symbol table as a "SYMBOL_DATA" type,
 * and updates the data counter (DC) accordingly.
 *
 * Errors are reported if:
 * - Invalid lable.
 * - Duplicate label.
 * - Syntax issues (bad numbers, strings or matrix).
 *
 * @param label     Optional label for the directive (can be NULL ot "" if none).
 * @param directive The directive type (e.g., ".data", ".string", ".mat
 * @param line      The rest of the line after the directive.
 * @param filename  Pointer to the file name of the source file (for error reporting).
 * @param line_num  Pointer to the current line number in the source file.
 */

static void handle_data_directive(const char *label, const char *directive, const char *line, const char *filename, int lineno)
{
    /* Handle label if exsist */
    if (label != NULL && label[0] != '\0')
    {

        /* Validates the label. */
        if (!is_valid_label(label))
        {
            print_line_error(filename, lineno, ERROR_INVALID_LABEL);
            err_found = TRUE;
            return; /* Skip processing this line */
        }

        /* Check if the label already exists in the symbol table. */
        if (is_label_defined(label))
        {
            print_line_error(filename, lineno, ERROR_DUPLICATE_LABEL);
            err_found = TRUE;
            return; /* Skip processing this line */
        }

        /* Adds the symbol to the symbol_table. */
        add_symbol(label, DC, SYMBOL_DATA);
    }

    /* Parse the directive based on its type */
    if (strcmp(directive, ".data") == 0)
    {
        parse_data_values(line, filename, line_num);
    }

    else if (strcmp(directive, ".string") == 0)
    {
        parse_string_value(line, filename, line_num);
    }

    else if (strcmp(directive, ".mat") == 0)
    {
        parse_matrix(line, filename, line_num);
    }

    else
    {
        print_line_error(filename, line_num, ERROR_INVALID_DIRECTIVE);
        err_found = TRUE;
    }
}