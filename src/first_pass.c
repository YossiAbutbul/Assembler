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

/* Flag to indicate if an error was found */
BOOL err_found = FALSE;

static InstructionData instruction_table[MAX_INSTRUCTION_IMAGE_SIZE];
static int instruction_count = 0;

/* === Internal Helper Prototypes functions === */
static void
process_line(const char *line, const char *filename, int line_num);
static void handle_extern_directive(const char *line, const char *filename, int line_num);
static void handle_entry_directive(const char *line, const char *filename, int line_num);
static void handle_data_directive(const char *label, const char *directive, const char *line, const char *filename, int line_num);
static void handle_instruction(const char *label, const char *line, const char *filename, int line_num);
static int build_first_instruction_word(const Instruction *instruction);
static void encode_immediate_operands(const Instruction *instruction, InstructionData *inst_data);

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
BOOL first_pass(FILE *am_file, const char *filename)
{
    char line[MAX_LINE_LENGTH];
    int line_num = 0;
    int ICF, DCF; /* For step 18: Final IC and DC values */

    /* Step 1: Initialize IC <- BASE_IC_ADDRESS, DC <- 0 */
    IC = BASE_IC_ADDRESS;  /* Reset instruction counter */
    DC = 0;                /* Reset data counter */
    err_found = FALSE;     /* Reset error flag */
    instruction_count = 0; /* Intialize instruction storage */

    /* Step 2: Process each line in the file */
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
            continue;

        /* Process the line */
        process_line(line, filename, line_num);
    }

    /* Step 17: if errors found in first pass, stop here */
    if (err_found)
        return FALSE;

    /* Step 18: Save final values of IC and DC */
    ICF = IC;
    DCF = DC;

    /* Update data symbols with the current IC */
    update_data_symbols(ICF);

    /* Step 20: Start second pass (todo: Handled by main assembler) */
    return TRUE;
}

/* === Internal Helper functions === */

/**
 * @brief Get stored instruction data for second pass.
 *
 * Step 15 implementaion - provides access to stored IC/L values.
 *
 * @param index The index of the instruction to retrieve.
 * @return Pointer to the InstructionData if valid index, NULL otherwise.
 * @note This is an internal function.
 */

const InstructionData *get_instruction_data(int index)
{
    if (index >= 0 && index < instruction_count)
        return &instruction_table[index];

    /* If invalid index */
    return NULL;
}

/**
 * @brief Get total nu,ber of stored instruction.
 *
 * @return The instruction count number.
 * @note This is an internal function to safely return the instruction count.
 */
int get_instruction_count(void)
{
    return instruction_count;
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
    char label[MAX_LABEL_LENGTH + 1];
    char first_token[MAX_LINE_LENGTH + 1];
    char *rest = NULL;
    BOOL has_label = FALSE;

    /* Create copy of the line. */
    char buffer[MAX_LINE_LENGTH + 1];
    strncpy(buffer, line, MAX_LINE_LENGTH);
    buffer[MAX_LINE_LENGTH] = '\0'; /* Ensure null-termination */
    rest = buffer;

    /* Step 3 - Extract label if exsist. */
    if (extract_label(rest, label))
    {
        /* Step 4: Turn on has_label flag */
        has_label = TRUE;

        /* Step 4: Validate the extracted label */
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

    /* Extract first token (opcode or directive). */
    if (!get_next_token(rest, first_token))
    {
        print_line_error(filename, line_num, ERROR_SYNTAX);
        err_found = TRUE;
        return; /* Skip processing this line */
    }

    /* Step 5 - Handle Directives */
    if (strcmp(first_token, ".data") == 0 || strcmp(first_token, ".string") == 0 || strcmp(first_token, ".mat") == 0)
    {
        handle_data_directive(has_label ? label : NULL, first_token, rest, filename, line_num);
    }
    /* Step 8-9: Handle .entry directive */
    else if (strcmp(first_token, ".entry") == 0)
    {
        handle_entry_directive(rest, filename, line_num);
    }
    /* Step 10: Handle .extern directive */
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

    /* Step 11 - Handle Instructions */
    else if (is_instruction(first_token))
    {
        handle_instruction(has_label ? label : NULL, line, filename, line_num);
    }
    /* Handle Unknown Instructions/Directives */
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
 * @note This handles step 11-16 in the first pass algorytm.
 */
static void handle_instruction(const char *label, const char *line, const char *filename, int line_num)
{
    Instruction instruction;
    char *instruction_part;
    char line_copy[MAX_LINE_LENGTH + 1];
    InstructionData *inst_data;

    /* Create a copy of the line for processing */
    strncpy(line_copy, line, MAX_LINE_LENGTH);
    line_copy[MAX_LINE_LENGTH] = '\0';

    /* Skip the label part if it exists */
    instruction_part = line_copy;
    if (label != NULL)
        instruction_part = skip_label(instruction_part);

    /* Step 11: Add label to symbol table if it exists */
    if (label != NULL)
    {
        if (!add_symbol(label, IC, SYMBOL_CODE))
        {
            print_line_error(filename, line_num, ERROR_DUPLICATE_LABEL);
            err_found = TRUE;
            return;
        }
    }

    /* Step 12: Parse the instruction */
    if (!parse_instruction(instruction_part, filename, line_num, &instruction))
        return; /* Error already reported bt parse_instruction */

    /* Step 13: Calculate word count (L) already done in get_instruction_word_count */

    /* Step 14: Build binary code for the first word and immediate operands */
    if (instruction_count < MAX_INSTRUCTION_IMAGE_SIZE)
    {
        inst_data = &instruction_table[instruction_count];

        /* Step 15: Store IC and L values */
        inst_data->ic_address = IC;
        inst_data->word_count = instruction.word_count;

        /* Step 14: Build first instruction word */
        inst_data->first_word = build_first_instruction_word(&instruction);

        /* Step 14: Encode immediate operands */
        encode_immediate_operands(&instruction, inst_data);

        instruction_count++;
    }

    /* Step 16: Update IC by L (instruction word count) */
    IC += instruction.word_count;
}

/**
 * @brief Build the first word for a given instruction based on its
 * opcode and addressing modes.
 *
 * @param instruction Struct containing the operation code and operand addressing modes.
 * @return The binary encoding for the main instruction code.
 * @note This is a part of the step 14 in the first pass algorytm.
 */
static int build_first_instruction_word(const Instruction *instruction)
{
    int word = 0;

    /* Bits 6-9: opcode */
    word |= (instruction->opcode << 6);

    /* Bits 4-5: source addressing mode */
    if (instruction->has_source)
        word |= (instructionc->source.mode << 4);

    /* Bits 2-3: target addressing mode */
    if (instruction->has_target)
        word |= (instruction->target.mode << 2);

    /* Bits 0-1: A,R,E = 00 for instruction words */
    word |= 0x00;

    return word;
}

/**
 * @brief Encode immediate operands values from the given Instruction and stores them into
 * the InstructionData struct.
 *
 * This is used during the first pass of the assembler to prepare memory words for operands
 * with immediate addressing mode.
 *
 * @param instruction   Pointer to the instruction containing the operands.
 * @param inst_data     Pointer to the struct where the encoded immediate words and their count will be stored.
 * @note This a part of step 14 in the first pass algorytm.
 */
static void encode_immediate_operands(const Instruction *instruction, InstructionData *inst_data)
{
    inst_data->immediate_count = 0;

    /* Check source operand for immediate addressing */
    if (instruction->has_source && instruction->source.mode == ADDRESSING_IMMEDIATE)
    {
        /* Encode immediate value: shit left by 2 bits, A,R,E = 00 */
        inst_data->immediate_words[inst_data->immediate_count] = (instruction->source.value << 2) | 0x00;
        inst_data->immediate_count++;
    }

    /* Check target opernad for immediate addressing */
    if (instruction->has_target && instruction->target.mode == ADDRESSING_IMMEDIATE)
    {
        /* Encode immediate value: shit left by 2 bits, A,R,E = 00 */
        inst_data->immediate_words[inst_data->immediate_count] = (instruction->target.value << 2) | 0x00;
        inst_data->immediate_count++;
    }
}

/**
 * @brief Handles the "".extern" directive in the first pass.
 *
 * This function parses the operand after ".extern", validates it is a legal label,
 * and adds it to the symbol table with the type SYMBOL_EXTERNAL type and address 0.
 *
 * Errors are reported if:
 * - No Operand is found.
 * - The label is unvalid or already defined.
 *
 * @param line       The line content after the ".extern" directive.
 * @param filename   Pointer to the file name of the source file (for error reporting).
 * @param line_num   The current line number in the source file.
 * @note This is part of step 10 in the first pass algorytm.
 */
static void handle_extern_directive(const char *line, const char *filename, int line_num)
{
    char label[MAX_LABEL_LENGTH + 1];

    /* Skip leading whitespace. */
    while (isspace(*line))
        line++;

    /* Extract the label name */
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
 * @note This is part of step 9 in the first pass algorytm.
 * @note Actual marking as entry happens in second pass.
 */
static void handle_entry_directive(const char *line, const char *filename, int line_num)
{
    char label[MAX_LABEL_LENGTH + 1];

    /* Skip leading whitespace. */
    while (isspace((unsigned char)*line))
        line++;

    /* Extract the label name */
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
 * @note This is part of steps 5-7 in the first pass algorytm.
 */

static void handle_data_directive(const char *label, const char *directive, const char *line, const char *filename, int line_num)
{
    /* step 6: Handle label if exsist */
    if (label != NULL && label[0] != '\0')
    {

        /* Validates the label. */
        if (!is_valid_label(label))
        {
            print_line_error(filename, line_num, ERROR_INVALID_LABEL);
            err_found = TRUE;
            return; /* Skip processing this line */
        }

        /* Check if the label already exists in the symbol table. */
        if (is_label_defined(label))
        {
            print_line_error(filename, line_num, ERROR_DUPLICATE_LABEL);
            err_found = TRUE;
            return; /* Skip processing this line */
        }

        /* Add symbol to table with DC value */
        add_symbol(label, DC, SYMBOL_DATA);
    }

    /* step 7: Parse the directive based on its type and update DC */
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

/**
 * @brief Cleanup first pass instruction storage betwenn files.
 * @note This function will be called at the end of processing each file in the main.c
 */
void cleanup_first_pass_data(void)
{
    inst_count = 0;

    /* Clear the instruction table */
    memset(instruction_table, sizeof(instruction_table));
}