/**
 * @file instruction_parser.c
 * @brief Implementation of instruction parsing for the assembler.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../include/instruction_parser.h"
#include "../include/utils.h"
#include "../include/error.h"
#include "../include/first_pass.h"

/* === Internal Helpr Function Prototypes === */
static BOOL parse_opernad(const char *opernad_str, Operand *opernad, const char *filename, int line_num);
static BOOL is_register(const char *str, int *reg_num);
static BOOL is_immediate(const char *str, const *value);
static BOOL is_matrix_reference(const char *str, Operand *operand, const char *filename, int line_num);
static InstructionType get_instruction_type(int opcode);

/**
 * @brief Instruction opcode mapping table.
 *
 * Maps instruction names to their opcodes.
 */
typedef struct
{
    const char *name;
    int opcode;
} InstructionMapping;

static const InstructionMapping instruction_table[] = {
    {"mov", 0}, {"cmp", 1}, {"add", 2}, {"sub", 3}, {"lea", 4}, {"clr", 5}, {"not", 6}, {"inc", 7}, {"dec", 8}, {"jmp", 9}, {"bne", 10}, {"jsr", 11}, {"red", 12}, {"prn", 13}, {"rts", 14}, {"stop", 15}};

/* Define for number of supported instructuions in the assmbler */
#define INSTRUCTION_COUNT (sizeof(instruction_table) / sizeof(instruction_table[0]))

/**
 * @brief Parse an instruction line during first pass.
 *
 * This function takes a line containing an instruction (without any label)
 * ans parses it into it components: opcode and opernads.
 * It validates the instruction syntax and opernad addressing modes.
 *
 * @param line          Pointer to the instruction line.
 * @param filename      Pointer to the source file name (for error reporting).
 * @param line_num      Current line number (for error reporting).
 * @param instruction   Pointer to the output struct for parsed instruction data.
 * @return TRUE if parsing successful, FALSE if syntax error occured.
 */
BOOL parse_instruction(const char *line, const char *filename, int line_num, Instruction *instruction)
{
    char instruction_name[MAX_LINE_LENGTH];
    char source_operand[MAX_LINE_LENGTH] = "";
    char target_operand[MAX_LINE_LENGTH] = "";
    char *line_copy;
    char *token;
    int token_count = 0;

    /* Validate input parameters */
    if (!line || !instruction || !filename)
        return FALSE;

    /* Initialize instruction struct */
    memset(instruction, 0, sizeof(Instruction));
    instruction->opcode = -1;
    instruction->word_count = 1; /* At least the instruction name itself */

    /* Creates a copy of the line */
    line_copy = strdup(line);
    if (!line_copy)
    {
        print_line_error(filename, line_num, ERROR_MEMORY_ALLOCATION_FAILED);
        err_found = TRUE;
        return FALSE;
    }

    /* Parse instruction name */
    token = strtok(line_copy, " \t,");
    if (!token)
    {
        print_line_error(filename, line_num, ERROR_SYNTAX);
        err_found = TRUE;
        free(line_copy);
        return FALSE;
    }

    /* Copy instruction name and get opcode */
    strncpy(instruction_name, token, sizeof(instruction_name) - 1);
    instruction_name[sizeof(instruction_name) - 1] = '\0';
    instruction->opcode = get_instruction_opcode(instruction_name);

    /* Checks wether there is no such opcode */
    if (instruction->opcode == -1)
    {
        print_line_error(filename, line_num, ERROR_UNKNOWN_INSTRUCTION);
        err_found = TRUE;
        free(line_copy);
        return FALSE;
    }

    /* Determine instruction type */
    instruction->type = get_instruction_type(instruction->opcode);

    /* Parse opernads based on instruction type */
    while ((token = strtok(NULL, ",")) != NULL)
    {
        /* Trim leading whitespace from token */
        while (isspace((unsigned char)*token))
            token++;

        /* Skip empty tokens */
        if (strlen(token) == 0)
            continue;

        token_count++;

        /* Checks number of opernads in instruction */
        if (token_count == 1)
        {
            strcpy(source_operand, token, sizeof(source_operand) - 1);
            source_operand[sizeof(source_operand) - 1] = '\0'; /* Adds null terminator */
        }
        else if (token_count == 2)
        {
            strcpy(target_operand, token, sizeof(target_operand) - 1);
            target_operand[sizeof(target_operand) - 1] = '\0'; /* Adds null terminator */
        }
        else
        {
            /* Too many opernads */
            print_line_error(filename, line_num, ERROR_TOO_MANY_OPERANDS);
            err_found = TRUE;
            free(line_copy);
            return FALSE;
        }
    }
    free(line_copy);

    /* Validate opernad count based on instruction type */
    switch (instruction->type)
    {
    case INST_NO_OPERNADS:
        /* Checks if there is too many opernands */
        if (token_count != 0)
        {
            print_line_error(filename, line_num, ERROR_TOO_MANY_OPERANDS);
            err_found = TRUE;
            return FALSE;
        }
        /* No Opernads instruction */
        instruction->has_source = FALSE;
        instruction->has_target = FALSE;
        break;
    case INST_ONE_OPERAND:
        if (token_count != 1)
        {
            if (token_count == 0)
                print_line_error(filename, line_num, ERROR_TOO_FEW_OPERANDS);
            else
                print_line_error(filename, line_num, ERROR_TOO_MANY_OPERANDS);
        }
        err_found = TRUE;
        return FALSE;

        /* For one-opernad instruction, the opernad goes in target */
        if (!parse_operand(source_operand, &instruction->target, filename, line_num))
            return FALSE;

        /* One opernad instruction */
        instruction->has_source = FALSE;
        instruction->has_target = TRUE;
        break;

    case INST_TWO_OPERANDS:
        if (token_count != 2)
        {
            if (token_count < 2)
                print_line_error(filename, line_num, ERROR_TOO_FEW_OPERANDS);
            else
                print_line_error(filename, line_num, ERROR_TOO_MANY_OPERANDS);
            err_found = TRUE;
            return FALSE;
        }

        /* Parse both opernads */
        if (!parse_opernad(source_operand, &instruction->source, filename, line_num) ||
            !parse_opernad(target_operand, &instruction->target, filename, line_num))
        {
            return FALSE;
        }

        /* Two opernads instruction */
        instruction->has_source = TRUE;
        instruction->has_target = TRUE;
        break;
    }

    /* Validate addressing modes */
    if (!validate_addresing_modes(instruction->opcode,
                                  instruction->has_source ? instruction->source.mode : 0,
                                  instruction->has_target ? instruction->target.mode : 0,
                                  instruction->has_source, instruction->has_target))
    {
        print_line_error(filename, line_num, ERROR_INVALID_OPERAND);
        err_found = TRUE;
        return FALSE;
    }

    /* Calculate total word count */
    instruction->word_count = get_instruction_word_count(instruction);

    return TRUE;
}

/**
 * @brief Parse a single opernad string into Operand struct.
 *
 * This function determines the addressing mode and extracts the necessary
 * information from opernad string.
 *
 * @param operand_str   The oprenad string to parse.
 * @param operand       Output operand struct.
 * @param filename      Source filename (for error reporting).
 * @param line_num      Current line number (for error reporting).
 * @return TRUE if parsing successful, FALSE if error occured.
 */
static BOOL parse_operand(const char *operand_str, Operand *operand, const char *filename, int line_num)
{
    char trimmed[MAX_LINE_LENGTH];
    int i, j;

    /* Initalize operand struct */
    memset(operand, 0, sizeof(Operand));

    /* Trim whitespace by copying characters from the operand_str to the trimmed array */
    for (i = 0, j = 0; operand_str[i] && j < MAX_LINE_LENGTH - 1; i++)
    {
        if (!isspace((unsigned char)operand_str[i]))
        {
            trimmed[j++] = operand_str[i];
        }
    }
    /* Null terminate the trimmed str */

    /* Check for register addressing (ro-r7) */
    if (is_register(trimmed, &operand->value))
    {
        operand->mode = ADDRESSING_REGISTER;
        operand->is_symbol = FALSE;
        return TRUE;
    }

    /* Check for matrix addressing (label[reg][reg]) */
    if (is_matrix_reference(trimmed, operand, filename, line_num))
    {
        operand->mode = ADDRESSING_MATRIX;
        return TRUE; /* is_symbol flag set by is_matrix_reference */
    }

    /* If we got to here, must be direct addressing (label) */
    if (is_valid_label(trimmed))
    {
        operand->mode = ADDRESSING_DIRECT;
        strncpy(operand->symbol_name, trimmed, MAX_LABEL_LENGTH);
        operand->symbol_name[MAX_LABEL_LENGTH] = '\0'; /* Null terminate */
        operand->is_symbol = TRUE;
        return TRUE;
    }

    /* Invalid operand */
    print_line_error(filename, line_num, ERROR_INVALID_OPERAND);
    err_found = TRUE;
    return FALSE;
}

/**
 * @brief Check if a strin represents a register (r0-r7)ץ
 *
 * @param str       The string to check.
 * @param reg_num   Output parameter for register number (0-7).
 * @return TRUE if string is a valid register, FALSE otherwise.
 */
static BOOL is_register(const char *str, int *reg_num)
{
    /* Valide basic register params */
    if (!str || strlen(str) != 2)
        return FALSE;

    if (str[0] == 'r' && str[1] >= '0' && str[1] <= 7)
    {
        /* Converts it from its ASCII value to its integer value */
        *reg_num = str[1] - '0';
        return TRUE;
    }

    /* Invalid register name */
    return FALSE;
}

/**
 * @brief Check if a string represents immesiate addressing (#value).
 *
 * @param str The string to check.
 * @param value Output parameter for immediate addressing (#value).
 * @return TRUE if string is valid immediate value, FALSE otherwise.
 */
static BOOL is_immediate(const)
