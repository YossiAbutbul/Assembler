/**
 * @file first_pass.c
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

/* Initializiation */
int IC = BASE_IC_ADDRESS; /* Instruction counter */
int DC = 0;               /* Data counter */
int ICF = 0;              /* Final instruction counter */
int DCF = 0;              /* Final Data counter */
BOOL err_found = FALSE;   /* Flag to indicate if an error was found */

static InstructionData instruction_table[MAX_INSTRUCTION_IMAGE_SIZE];
static int instruction_count = 0; /*todo: rethink why i have both IC and instruction_count*/

/* === Internal Helper Prototypes functions === */
static void process_line(const char *line, const char *filename, int line_num);
static void handle_extern_directive(const char *line, const char *filename, int line_num);
static void handle_entry_directive(const char *line, const char *filename, int line_num);
static void handle_data_directive(const char *label, const char *directive, const char *line, const char *filename, int line_num);
static void handle_instruction(const char *label, const char *line, const char *filename, int line_num);
static int build_first_instruction_word(const Instruction *instruction);
static void encode_immediate_operands(const Instruction *instruction, InstructionData *inst_data);

/**
 * @brief Performs the first pass on the given .am source file.
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
    int line_num;
    int ICF;

    /* Initialize counters */
    IC = BASE_IC_ADDRESS;
    DC = 0;
    err_found = FALSE;
    instruction_count = 0;
    line_num = 0;

    /* Process each line in the file */
    while (fgets(line, sizeof(line), am_file))
    {
        line_num++;

        /* Check line length */
        if (strchr(line, '\n') == NULL && !feof(am_file))
        {
            print_line_error(filename, line_num, ERROR_LINE_TOO_LONG);
            err_found = TRUE;

            /* Skip the rest of the line */
            while (!feof(am_file) && fgetc(am_file) != '\n')
                continue;

            /* Skip processing this line and go to next iteration */
            continue;
        }

        /* Remove comments BEFORE trimming whitespace */
        remove_comments(line);

        /* Trim leading and trailing whitespace */
        trim_whitespace(line);

        /* Skip empty lines and comments */
        if (is_whitespace(line) || is_comment(line))
        {
            continue;
        }

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

    return TRUE;
}

/* === Internal Helper functions === */

/**
 * @brief Get stored instruction data for second pass.
 *
 * Step 15 implementation - provides access to stored IC/L values.
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
 * @brief Get total number of stored instructions.
 *
 * @return The instruction count number.
 * @note This is an internal function to safely return the instruction count.
 */
int get_instruction_count(void)
{
    return instruction_count;
}

/**
 * @brief Processes a single line during the first pass.
 *
 * Identifies the line type and delegates to appropriate handler functions.
 * Handles labels, instructions and directives according to the algorithm.
 *
 * @param line      The line content to process.
 * @param filename  Name of the source file (for error reporting).
 * @param line_num  Current line number (for error reporting).
 */
static void process_line(const char *line, const char *filename, int line_num)
{
    char label[MAX_LABEL_LENGTH + 1];
    char first_token[MAX_LINE_LENGTH + 1];
    char *rest;
    BOOL has_label;
    char buffer[MAX_LINE_LENGTH + 1];
    char *colon_pos;
    char potential_label[MAX_LABEL_LENGTH + 10]; /* Extra space for overflow detection */
    int j;

    /* Initialization */
    has_label = FALSE;
    label[0] = '\0';

    /* Create copy of the line */
    strncpy(buffer, line, MAX_LINE_LENGTH);
    buffer[MAX_LINE_LENGTH] = '\0';
    rest = buffer;

    /* Skip leading whitespace */
    while (isspace((unsigned char)*rest))
        rest++;

    /* Check if this line starts with a directive */
    if (*rest == '.')
    {
        has_label = FALSE;
    }
    else
    {
        /* Check if there's a colon in the line (potential label) */
        colon_pos = strchr(rest, ':');
        if (colon_pos != NULL)
        {
            /* Extract the text before the colon to check if it's a valid label attempt */
            j = 0;
            while (rest + j < colon_pos && j < sizeof(potential_label) - 1 && !isspace((unsigned char)rest[j]))
            {
                potential_label[j] = rest[j];
                j++;
            }
            potential_label[j] = '\0';

            /* Check for label validation errors */

            /* Check if label is too long */
            if (strlen(potential_label) > MAX_LABEL_LENGTH)
            {
                print_line_error(filename, line_num, ERROR_LABEL_SYNTAX);
                err_found = TRUE;
                return;
            }

            /* Check if label starts with digit */
            if (potential_label[0] != '\0' && isdigit((unsigned char)potential_label[0]))
            {
                print_line_error(filename, line_num, ERROR_LABEL_SYNTAX);
                err_found = TRUE;
                return;
            }

            /* Check if label starts with non-letter (but not digit - already checked) */
            if (potential_label[0] != '\0' && !isalpha((unsigned char)potential_label[0]))
            {
                print_line_error(filename, line_num, ERROR_LABEL_SYNTAX);
                err_found = TRUE;
                return;
            }

            /* Check for invalid characters in label */
            for (j = 0; potential_label[j] != '\0'; j++)
            {
                if (!isalnum((unsigned char)potential_label[j]) && potential_label[j] != '_')
                {
                    print_line_error(filename, line_num, ERROR_LABEL_SYNTAX);
                    err_found = TRUE;
                    return;
                }
            }

            /* Check if the text before colon is a reserved word */
            if (is_reserved_word(potential_label))
            {
                print_line_error(filename, line_num, ERROR_RESERVED_WORD);
                err_found = TRUE;
                return;
            }

            /* Try to extract valid label */
            if (extract_label(buffer, label))
            {
                /* Valid label extracted */
                has_label = TRUE;

                /* Double-check label validity */
                if (!is_valid_label(label))
                {
                    print_line_error(filename, line_num, ERROR_LABEL_SYNTAX);
                    err_found = TRUE;
                    return;
                }

                /* Check for duplicate label definition */
                if (is_label_defined(label))
                {
                    print_line_error(filename, line_num, ERROR_DUPLICATE_LABEL);
                    err_found = TRUE;
                    return;
                }

                /* Skip past the label for further processing */
                rest = skip_label(buffer);
            }
            else
            {
                /* Invalid label (extract_label failed but colon exists) */
                print_line_error(filename, line_num, ERROR_LABEL_SYNTAX);
                err_found = TRUE;
                return;
            }
        }
    }

    /* Extract first token (opcode or directive) */
    if (!get_next_token(rest, first_token))
    {
        print_line_error(filename, line_num, ERROR_SYNTAX);
        err_found = TRUE;
        return;
    }

    /* Handle different types of statements */
    if (strcmp(first_token, ".data") == 0 || strcmp(first_token, ".string") == 0 || strcmp(first_token, ".mat") == 0)
    {
        handle_data_directive(has_label ? label : NULL, first_token, rest, filename, line_num);
    }
    else if (strcmp(first_token, ".entry") == 0)
    {
        if (has_label)
        {
            print_line_error(filename, line_num, ERROR_LABEL_ON_EXTERN);
            err_found = TRUE;
            return;
        }
        handle_entry_directive(rest, filename, line_num);
    }
    else if (strcmp(first_token, ".extern") == 0)
    {
        if (has_label)
        {
            print_line_error(filename, line_num, ERROR_LABEL_ON_EXTERN);
            err_found = TRUE;
            return;
        }
        handle_extern_directive(rest, filename, line_num);
    }
    else if (is_instruction(first_token))
    {
        handle_instruction(has_label ? label : NULL, line, filename, line_num);
    }
    else
    {
        print_line_error(filename, line_num, ERROR_UNKNOWN_INSTRUCTION);
        err_found = TRUE;
        return;
    }
}

/**
 * @brief Handles instruction lines during the first pass.
 *
 * This function parses an instruction line, validates its syntax and operands,
 * adds any label to the symbol table, and updates the IC (instruction counter).
 *
 * @param label     Optional label for instruction (can be NULL if none).
 * @param line      The complete instruction line.
 * @param filename  Name of the source file (for error reporting).
 * @param line_num  The current line number in the source file (for error reporting).
 * @note This handles step 11-16 in the first pass algorithm.
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
        return; /* Error already reported by parse_instruction */

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
 * @note This is a part of the step 14 in the first pass algorithm.
 */
static int build_first_instruction_word(const Instruction *instruction)
{
    int word = 0;

    /* Bits 6-9: opcode */
    word |= (instruction->opcode << 6);

    /* Bits 4-5: source addressing mode */
    if (instruction->has_source)
    {
        word |= (instruction->source.mode << 4);
    }
    else
    {
        word |= (0 << 4); /* Explicitly set source mode to 0 when no source operand */
    }

    /* Bits 2-3: target addressing mode */
    if (instruction->has_target)
    {
        word |= (instruction->target.mode << 2);
    }
    else
    {
        word |= (0 << 2); /* Explicitly set target mode to 0 when no target operand */
    }

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
 * @note This a part of step 14 in the first pass algorithm.
 */
static void encode_immediate_operands(const Instruction *instruction, InstructionData *inst_data)
{
    int value;

    inst_data->immediate_count = 0;

    /* Check source operand for immediate addressing */
    if (instruction->has_source && instruction->source.mode == ADDRESSING_IMMEDIATE)
    {
        /* Store source opernad for immediate addressing */
        value = instruction->source.value;

        /* Convert negative values to 10-bit two's complement representation */
        if (value < 0)
        {
            value = 1024 + value; /* 1024 = 2^10 for 10 bit two's complement */
        }

        /* Ensure value is within 10-bit range (0 to 1023) */
        value = value & 1023; /* 1023 = 0x3FF = (2^10 - 1) */

        inst_data->immediate_word[inst_data->immediate_count] = value;
        inst_data->immediate_count++;
    }

    /* Check target operand for immediate addressing */
    if (instruction->has_target && instruction->target.mode == ADDRESSING_IMMEDIATE)
    {
        value = instruction->target.value;

        /* Convert negative values to 10-bit two's complement representation */
        if (value < 0)
        {
            value = 1024 + value; /* 1024 = 2^10 for 10 bit two's complement */
        }

        /* Ensure value is within 10-bit range (0 to 1023) */
        value = value & 1023; /* 1023 = 0x3FF = (2^10 - 1) */

        inst_data->immediate_word[inst_data->immediate_count] = value;
        inst_data->immediate_count++;
    }
}

/**
 * @brief Handles the ".extern" directive in the first pass.
 *
 * This function parses the operand after ".extern", validates it is a legal label,
 * and adds it to the symbol table with the type SYMBOL_EXTERNAL type and address 0.
 *
 * Errors are reported if:
 * - No Operand is found.
 * - The label is invalid or already defined.
 *
 * @param line       The line content after the ".extern" directive.
 * @param filename   Pointer to the file name of the source file (for error reporting).
 * @param line_num   The current line number in the source file.
 * @note This is part of step 10 in the first pass algorithm.
 */
static void handle_extern_directive(const char *line, const char *filename, int line_num)
{
    char label[MAX_LABEL_LENGTH + 1];
    const char *ptr = line;

    /* Skip whitespace */
    while (isspace((unsigned char)*ptr))
        ptr++;

    /* Find the start of .extern in the line and skip past it */
    if (strncmp(ptr, ".extern", 7) == 0)
    {
        ptr += 7;
    }

    /* Skip whitespace after .extern */
    while (isspace((unsigned char)*ptr))
        ptr++;

    /* Extract the label name */
    if (!get_next_token(ptr, label))
    {
        print_line_error(filename, line_num, ERROR_SYNTAX);
        err_found = TRUE;
        return;
    }

    /* Validate the label */
    if (!is_valid_label(label))
    {
        print_line_error(filename, line_num, ERROR_INVALID_LABEL);
        err_found = TRUE;
        return;
    }

    /* Check if already defined */
    if (is_label_defined(label))
    {
        print_line_error(filename, line_num, ERROR_DUPLICATE_LABEL);
        err_found = TRUE;
        return;
    }

    /* Add external symbol to table */
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
 * - The label is invalid.
 *
 * @param line       The line content after the ".entry" directive.
 * @param filename   Pointer to the file name of the source file (for error reporting).
 * @param line_num   The current line number in the source file.
 * @note This is part of step 9 in the first pass algorithm.
 * @note Actual marking as entry happens in second pass.
 */
static void handle_entry_directive(const char *line, const char *filename, int line_num)
{
    char label[MAX_LABEL_LENGTH + 1];
    const char *ptr = line;

    /* Skip whitespace */
    while (isspace((unsigned char)*ptr))
        ptr++;

    /* Find the start of .entry in the line and skip past it */
    if (strncmp(ptr, ".entry", 6) == 0)
    {
        ptr += 6;
    }

    /* Skip whitespace after .entry */
    while (isspace((unsigned char)*ptr))
        ptr++;

    /* Extract the label name */
    if (!get_next_token(ptr, label))
    {
        print_line_error(filename, line_num, ERROR_SYNTAX);
        err_found = TRUE;
        return;
    }

    /* Validate the label */
    if (!is_valid_label(label))
    {
        print_line_error(filename, line_num, ERROR_INVALID_LABEL);
        err_found = TRUE;
        return;
    }
}

/**
 * @brief Handles ".data", ".string", and ".mat" directives during the first pass.
 *
 * This function parses and validates the operand(s) after a data-related directive.
 * For a valid labels, it adds the symbol to the symbol table as a "SYMBOL_DATA" type,
 * and updates the data counter (DC) accordingly.
 *
 * Errors are reported if:
 * - Invalid label.
 * - Duplicate label.
 * - Syntax issues (bad numbers, strings or matrix).
 *
 * @param label     Optional label for the directive (can be NULL or "" if none).
 * @param directive The directive type (e.g., ".data", ".string", ".mat").
 * @param line      The rest of the line after the directive.
 * @param filename  Pointer to the file name of the source file (for error reporting).
 * @param line_num  Pointer to the current line number in the source file.
 * @note This is part of steps 5-7 in the first pass algorithm.
 */
static void handle_data_directive(const char *label, const char *directive, const char *line, const char *filename, int line_num)
{
    char *operands_start;
    char line_copy[MAX_LINE_LENGTH + 1];

    /* Handle label if exists */
    if (label != NULL && label[0] != '\0')
    {
        /* Validate the label */
        if (!is_valid_label(label))
        {
            print_line_error(filename, line_num, ERROR_INVALID_LABEL);
            err_found = TRUE;
            return;
        }

        /* Check for duplicate label */
        if (is_label_defined(label))
        {
            print_line_error(filename, line_num, ERROR_DUPLICATE_LABEL);
            err_found = TRUE;
            return;
        }

        /* Add symbol to table with current DC value */
        add_symbol(label, DC, SYMBOL_DATA);
    }

    /* Make a copy of the line to work with */
    strncpy(line_copy, line, MAX_LINE_LENGTH);
    line_copy[MAX_LINE_LENGTH] = '\0';

    /* Remove comments from the line */
    remove_comments(line_copy);

    /* Find where the directive starts in the line */
    operands_start = strstr(line_copy, directive);
    if (operands_start != NULL)
    {
        /* Move past the directive name */
        operands_start += strlen(directive);

        /* Skip whitespace after directive */
        while (isspace((unsigned char)*operands_start))
            operands_start++;
    }
    else
    {
        print_line_error(filename, line_num, ERROR_SYNTAX);
        err_found = TRUE;
        return;
    }

    /* Parse based on directive type */
    if (strcmp(directive, ".data") == 0)
    {
        parse_data_values(operands_start, filename, line_num);
    }
    else if (strcmp(directive, ".string") == 0)
    {
        parse_string_value(operands_start, filename, line_num);
    }
    else if (strcmp(directive, ".mat") == 0)
    {
        parse_matrix(operands_start, filename, line_num);
    }
    else
    {
        print_line_error(filename, line_num, ERROR_INVALID_DIRECTIVE);
        err_found = TRUE;
    }
}

/**
 * @brief Cleanup first pass instruction storage between files.
 * @note This function will be called at the end of processing each file in the main.c
 */
void cleanup_first_pass_data(void)
{
    instruction_count = 0;

    /* Clear the instruction table */
    memset(instruction_table, 0, sizeof(instruction_table));
}