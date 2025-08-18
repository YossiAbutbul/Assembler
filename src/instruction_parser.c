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

/* === Internal Helper Function Prototypes === */
static BOOL parse_operand(const char *operand_str, Operand *operand, const char *filename, int line_num);
static BOOL is_register(const char *str, int *reg_num);
static BOOL is_immediate(const char *str, int *value);
static BOOL is_matrix_reference(const char *str, Operand *operand, const char *filename, int line_num);
static InstructionType get_instruction_type(int opcode);

/**
 * @brief Instruction opcode mapping table.
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
 * and parses it into it components: opcode and operands.
 * It validates the instruction syntax and operand addressing modes.
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
    line_copy = (char *)malloc(strlen(line) + 1);
    if (!line_copy)
    {
        print_line_error(filename, line_num, ERROR_MEMORY_ALLOCATION_FAILED);
        err_found = TRUE;
        return FALSE;
    }
    strcpy(line_copy, line);

    /* Remove in line comments before parsing */
    remove_comments(line_copy);

    /* Parse instruction name */
    token = strtok(line_copy, " \t");
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

    /* Check valid instruction type */
    if (instruction->type == INST_INVALID)
    {
        print_line_error(filename, line_num, ERROR_UNKNOWN_INSTRUCTION);
        err_found = TRUE;
        free(line_copy);
        return FALSE;
    }

    /* Parse operands based on instruction type */
    while ((token = strtok(NULL, ",")) != NULL)
    {
        /* Trim leading whitespace from token */
        while (isspace((unsigned char)*token))
            token++;

        /* Skip empty tokens */
        if (strlen(token) == 0)
            continue;

        token_count++;

        /* Checks number of operands in instruction */
        if (token_count == 1)
        {
            strncpy(source_operand, token, sizeof(source_operand) - 1);
            source_operand[sizeof(source_operand) - 1] = '\0'; /* Adds null terminator */
        }
        else if (token_count == 2)
        {
            strncpy(target_operand, token, sizeof(target_operand) - 1);
            target_operand[sizeof(target_operand) - 1] = '\0'; /* Adds null terminator */
        }
        else
        {
            /* Too many operands */
            print_line_error(filename, line_num, ERROR_TOO_MANY_OPERANDS);
            err_found = TRUE;
            free(line_copy);
            return FALSE;
        }
    }
    free(line_copy);

    /* Validate operand count based on instruction type */
    switch (instruction->type)
    {
    case INST_NO_OPERANDS:
        /* Checks if there is too many operands */
        if (token_count != 0)
        {
            print_line_error(filename, line_num, ERROR_TOO_MANY_OPERANDS);
            err_found = TRUE;
            return FALSE;
        }
        /* No operands instruction */
        instruction->has_source = FALSE;
        instruction->has_target = FALSE;
        break;
    case INST_ONE_OPERAND:
        if (token_count != 1)
        {
            print_line_error(filename, line_num,
                             token_count == 0 ? ERROR_TOO_FEW_OPERANDS : ERROR_TOO_MANY_OPERANDS);
            err_found = TRUE;
            return FALSE;
        }

        /* For one-operand instruction, the operand goes in target */
        if (!parse_operand(source_operand, &instruction->target, filename, line_num))
            return FALSE;

        /* One operand instruction */
        instruction->has_source = FALSE;
        instruction->has_target = TRUE;
        break;

    case INST_TWO_OPERANDS:
        if (token_count != 2)
        {
            print_line_error(filename, line_num,
                             token_count < 2 ? ERROR_TOO_FEW_OPERANDS : ERROR_TOO_MANY_OPERANDS);
            err_found = TRUE;
            return FALSE;
        }

        /* Parse both operands */
        if (!parse_operand(source_operand, &instruction->source, filename, line_num) ||
            !parse_operand(target_operand, &instruction->target, filename, line_num))
        {
            return FALSE;
        }

        /* Two operands instruction */
        instruction->has_source = TRUE;
        instruction->has_target = TRUE;
        break;
    case INST_INVALID:
        print_line_error(filename, line_num, ERROR_UNKNOWN_INSTRUCTION);
    }

    /* Validate addressing modes */
    if (!validate_addressing_modes(instruction->opcode,
                                   instruction->has_source ? instruction->source.mode : 0,
                                   instruction->has_target ? instruction->target.mode : 0,
                                   instruction->has_source, instruction->has_target))
    {
        if (instruction->opcode == 0) /* mov */
        {
            if (instruction->target.mode == ADDRESSING_IMMEDIATE)
                print_line_error(filename, line_num, ERROR_INVALID_TARGET_ADDRESSING);
            else
                print_line_error(filename, line_num, ERROR_INVALID_ADDRESSING_MODE);
        }
        else if (instruction->opcode == 4) /* lea */
        {
            if (instruction->source.mode == ADDRESSING_IMMEDIATE)
                print_line_error(filename, line_num, ERROR_INVALID_SOURCE_ADDRESSING);
            else if (instruction->target.mode == ADDRESSING_IMMEDIATE)
                print_line_error(filename, line_num, ERROR_INVALID_TARGET_ADDRESSING);
            else
                print_line_error(filename, line_num, ERROR_INVALID_ADDRESSING_MODE);
        }
        else if (instruction->opcode == 2 || instruction->opcode == 3) /* add/sub */
        {
            if (instruction->target.mode == ADDRESSING_IMMEDIATE)
                print_line_error(filename, line_num, ERROR_INVALID_TARGET_ADDRESSING);
            else
                print_line_error(filename, line_num, ERROR_INVALID_ADDRESSING_MODE);
        }
        else
        {
            print_line_error(filename, line_num, ERROR_INVALID_ADDRESSING_MODE);
        }

        err_found = TRUE;
        return FALSE;
    }

    /* Calculate total word count */
    instruction->word_count = get_instruction_word_count(instruction);

    return TRUE;
}

/**
 * @brief Parse a single operand string into Operand struct.
 *
 * This function determines the addressing mode and extracts the necessary
 * information from operand string.
 *
 * @param operand_str   The operand string to parse.
 * @param operand       Output operand struct.
 * @param filename      Source filename (for error reporting).
 * @param line_num      Current line number (for error reporting).
 * @return TRUE if parsing successful, FALSE if error occured.
 */
/**
 * @brief Parse a single operand string into Operand struct.
 *
 * This function determines the addressing mode and extracts the necessary
 * information from operand string.
 *
 * @param operand_str   The operand string to parse.
 * @param operand       Output operand struct.
 * @param filename      Source filename (for error reporting).
 * @param line_num      Current line number (for error reporting).
 * @return TRUE if parsing successful, FALSE if error occurred.
 */
static BOOL parse_operand(const char *operand_str, Operand *operand, const char *filename, int line_num)
{
    char trimmed[MAX_LINE_LENGTH];
    int i, j;
    char *endptr;
    long parsed_value;

    /* Initialize operand struct */
    memset(operand, 0, sizeof(Operand));

    /* Trim whitespace */
    for (i = 0, j = 0; operand_str[i] && j < MAX_LINE_LENGTH - 1; i++)
    {
        if (!isspace((unsigned char)operand_str[i]))
            trimmed[j++] = operand_str[i];
    }
    trimmed[j] = '\0';

    /* Check for immediate addressing (#value) */
    if (trimmed[0] == '#')
    {
        /* Check for space after # in original string */
        i = 0;
        while (operand_str[i] && operand_str[i] != '#')
            i++;
        if (operand_str[i] == '#' && (operand_str[i + 1] == ' ' || operand_str[i + 1] == '\t'))
        {
            print_line_error(filename, line_num, ERROR_INVALID_IMMEDIATE_VALUE);
            err_found = TRUE;
            return FALSE;
        }

        /* Check for double # */
        if (trimmed[1] == '#')
        {
            print_line_error(filename, line_num, ERROR_INVALID_IMMEDIATE_VALUE);
            err_found = TRUE;
            return FALSE;
        }

        /* Use is_immediate function for main validation */
        if (is_immediate(trimmed, &operand->value))
        {
            operand->mode = ADDRESSING_IMMEDIATE;
            operand->is_symbol = FALSE;
            return TRUE;
        }
        else
        {
            /* Check specifically for range errors vs format errors */
            if (trimmed[1] != '\0')
            {
                /* Try to parse to see if it's a range error */
                parsed_value = strtol(&trimmed[1], &endptr, 10);
                if (*endptr == '\0' && (parsed_value < -512 || parsed_value > 511))
                {
                    print_line_error(filename, line_num, ERROR_DATA_OUT_OF_RANGE);
                }
                else
                {
                    print_line_error(filename, line_num, ERROR_INVALID_IMMEDIATE_VALUE);
                }
            }
            else
            {
                print_line_error(filename, line_num, ERROR_INVALID_IMMEDIATE_VALUE);
            }
            err_found = TRUE;
            return FALSE;
        }
    }

    /* Check for register addressing - including invalid register formats */
    if (trimmed[0] == 'r')
    {

        /* Check for valid register first */
        if (is_register(trimmed, &operand->value))
        {
            operand->mode = ADDRESSING_REGISTER;
            operand->is_symbol = FALSE;
            return TRUE;
        }
        else
        {
            /* It starts with 'r' or 'R' but is not a valid register */
            print_line_error(filename, line_num, ERROR_INVALID_REGISTER);
            err_found = TRUE;
            return FALSE;
        }
    }

    /* Check for matrix addressing (label[reg][reg]) */
    if (strchr(trimmed, '['))
    {

        if (is_matrix_reference(trimmed, operand, filename, line_num))
        {
            operand->mode = ADDRESSING_MATRIX;
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    /* Check for direct addressing */
    if (is_valid_label(trimmed))
    {
        operand->mode = ADDRESSING_DIRECT;
        strncpy(operand->symbol_name, trimmed, MAX_LABEL_LENGTH);
        operand->symbol_name[MAX_LABEL_LENGTH] = '\0';
        operand->is_symbol = TRUE;
        return TRUE;
    }

    /* It's an invalid operand */
    print_line_error(filename, line_num, ERROR_INVALID_OPERAND);
    err_found = TRUE;
    return FALSE;
}

/**
 * @brief Check if a string represents a register (r0-r7).
 *
 * @param str       The string to check.
 * @param reg_num   Output parameter for register number (0-7).
 * @return TRUE if string is a valid register, FALSE otherwise.
 */
static BOOL is_register(const char *str, int *reg_num)
{
    /* Valide basic register params */
    if (!str || strlen(str) != 2) /* 2 is the len og register (e.g. r2) */
        return FALSE;

    if (str[0] == 'r' && str[1] >= '0' && str[1] <= '7')
    {
        /* Converts it from its ASCII value to its integer value */
        *reg_num = str[1] - '0';
        return TRUE;
    }

    /* Invalid register name */
    return FALSE;
}

/**
 * @brief Check if a string represents immediate addressing (#value).
 *
 * @param str   The string to check.
 * @param value Output parameter for immediate addressing (#value).
 * @return TRUE if string is valid immediate value, FALSE otherwise.
 */
static BOOL is_immediate(const char *str, int *value)
{
    char *endptr;
    const char *num_start;
    long parsed_value; /* strtol uses long */

    /* Check empty str or str not starting with '#' */
    if (!str || str[0] != '#')
        return FALSE;

    /* Set num_start to point after the '#' */
    num_start = str + 1;

    /* Check if there is anything after the '#' */
    if (*num_start == '\0')
        return FALSE;

    /* Check for space immediately after # */
    if (isspace((unsigned char)*num_start))
        return FALSE;

    /* Parse the number after the '#' */
    parsed_value = strtol(num_start, &endptr, 10);

    /* Check if the entire string after '#' is a valid integer */
    if (*endptr != '\0')
        return FALSE;

    /* Check if value is within valid 10-bit range (-512 to +511) */
    if (parsed_value < -512 || parsed_value > 511)
        return FALSE;

    /* Store the value */
    *value = (int)parsed_value;
    return TRUE;
}

/**
 * @brief Check if string represents matrix addressing (label[reg][reg]).
 *
 * @param str       The string to check.
 * @param operand   Output operand struct.
 * @param filename  Source filename (for error reporting).
 * @param line_num  Current line number (for error reporting).
 * @return TRUE if string is valid matrix refernce, FALSE otherwise.
 */
static BOOL is_matrix_reference(const char *str, Operand *operand, const char *filename, int line_num)
{
    char *br1, *br2, *br3, *br4;
    char label_part[MAX_LABEL_LENGTH + 1];
    char reg1_str[MAX_REG_STR_LEN], reg2_str[MAX_REG_STR_LEN];
    char trimmed_reg1[MAX_REG_STR_LEN], trimmed_reg2[MAX_REG_STR_LEN];
    int label_len;
    int i, j;

    /* Find first '[' */
    br1 = strchr(str, '[');
    if (!br1)
        return FALSE;

    /* find first ']'*/
    br2 = strchr(br1 + 1, ']');
    if (!br2)
    {
        print_line_error(filename, line_num, ERROR_MATRIX_MISSING_REGISTER);
        err_found = TRUE;
        return FALSE;
    }

    /* Find second '[' */
    br3 = strchr(br2, '[');
    if (!br3)
    {
        print_line_error(filename, line_num, ERROR_MATRIX_MISSING_REGISTER);
        err_found = TRUE;
        return FALSE;
    }

    /* Find seccond ']' */
    br4 = strchr(br3, ']');
    if (!br4)
    {
        print_line_error(filename, line_num, ERROR_MATRIX_MISSING_REGISTER);
        err_found = TRUE;
        return FALSE;
    }

    /* Extract label part */
    label_len = br1 - str;
    if (label_len <= 0 || label_len > MAX_LABEL_LENGTH)
    {
        print_line_error(filename, line_num, ERROR_INVALID_MATRIX_ACCESS);
        err_found = TRUE;
        return FALSE;
    }

    /* Create a copy */
    strncpy(label_part, str, label_len);
    label_part[label_len] = '\0'; /*Null terminate the label part */

    /* Validate no space between label and first bracket */
    for (i = label_len - 1; i >= 0; i--)
    {
        if (isspace((unsigned char)label_part[i]))
        {
            print_line_error(filename, line_num, ERROR_INVALID_MATRIX_ACCESS);
            err_found = TRUE;
            return FALSE; /* Found space before first '[' */
        }
        break;
    }

    /* Extract register strings */
    if (br2 - br1 - 1 >= sizeof(reg1_str) ||
        br4 - br3 - 1 >= sizeof(reg2_str))
    {
        print_line_error(filename, line_num, ERROR_MATRIX_REGISTER_TOO_LONG);
        err_found = TRUE;
        return FALSE;
    }

    /* Copying the substring between the first '[' and the first ']' into reg1_str */
    strncpy(reg1_str, br1 + 1, br2 - br1 - 1);
    reg1_str[br2 - br1 - 1] = '\0'; /* Null terminate the reg1_str */

    /* Copying the substring between the second '[' and the second ']' into reg2_str */
    strncpy(reg2_str, br3 + 1, br4 - br3 - 1);
    reg2_str[br4 - br3 - 1] = '\0'; /* Null terminate the reg2_str */

    /* Trim spaces from registers (allowed inside brackets) */
    for (i = 0, j = 0; reg1_str[i] && j < MAX_REG_STR_LEN - 1; i++)
    {
        if (!isspace((unsigned char)reg1_str[i]))
            trimmed_reg1[j++] = reg1_str[i];
    }
    /* Null terminate the string */
    trimmed_reg1[j] = '\0';

    /* Now for register 2 */
    for (i = 0, j = 0; reg2_str[i] && j < sizeof(trimmed_reg2) - 1; i++)
    {
        if (!isspace((unsigned char)reg2_str[i]))
            trimmed_reg2[j++] = reg2_str[i];
    }
    /* Null terminate the string */
    trimmed_reg2[j] = '\0';

    /* Validate label */
    if (!is_valid_label(label_part))
    {
        print_line_error(filename, line_num, ERROR_INVALID_LABEL);
        err_found = TRUE;
        return FALSE;
    }

    /* Check for immediate values in indices (not allowed) */
    if (trimmed_reg1[0] == '#')
    {
        print_line_error(filename, line_num, ERROR_MATRIX_IMMEDIATE_NOT_ALLOWED);
        err_found = TRUE;
        return FALSE;
    }

    if (trimmed_reg2[0] == '#')
    {
        print_line_error(filename, line_num, ERROR_MATRIX_IMMEDIATE_NOT_ALLOWED);
        err_found = TRUE;
        return FALSE;
    }

    /* Validate first register */
    if (!is_register(trimmed_reg1, &operand->reg1))
    {
        print_line_error(filename, line_num, ERROR_MATRIX_INVALID_REGISTER);
        err_found = TRUE;
        return FALSE;
    }

    /* Validate second register */
    if (!is_register(trimmed_reg2, &operand->reg2))
    {
        print_line_error(filename, line_num, ERROR_MATRIX_INVALID_REGISTER);
        err_found = TRUE;
        return FALSE;
    }

    /* Set operand fields */
    strncpy(operand->symbol_name, label_part, MAX_LABEL_LENGTH);
    operand->symbol_name[MAX_LABEL_LENGTH] = '\0'; /* Null terminate the symbol name */
    operand->is_symbol = TRUE;
    return TRUE;
}

/**
 * @brief Get instruction opcode from instruction name.
 *
 * This function maps instruction names (like "mov", "add") to their
 * corresponding opcodes (0-15) according to the opcodes table in the project instruction.
 *
 * @param instruction_name The instruction name.
 * @return Opcode value (0-15), or -1 if instruction is invalid.
 */
int get_instruction_opcode(const char *instruction_name)
{
    int i;

    /* Search to the instruction_table to find the instruction_name */
    for (i = 0; i < INSTRUCTION_COUNT; i++)
    {
        if (strcmp(instruction_name, instruction_table[i].name) == 0)
            return instruction_table[i].opcode;
    }

    /* If not found, return -1 */
    return -1;
}

/**
 * @brief Determine instruction type based on opcode.
 *
 * This function maps opcodes to their corresponding instruction types
 * according to the number of operands each instruction requires.
 *
 * @param opcode    This instruction code (0-15).
 * @return The instruction type indicating operand count requriements.
 */
static InstructionType get_instruction_type(int opcode)
{
    if (opcode >= 0 && opcode <= 4)
        return INST_TWO_OPERANDS; /* mov, cmp, add, sub, lea */

    if (opcode >= 5 && opcode <= 13)
        return INST_ONE_OPERAND; /* clr, not, inc, dec, jmp, bne, jsr, red, prn */

    if (opcode >= 14 && opcode <= 15)
        return INST_NO_OPERANDS; /* rts, stop */

    /* Invalid opcode */
    return INST_INVALID;
}

/**
 * @brief Calculate instruction memory word count.
 *
 * This function determines the total number of memory words required
 * to encode a complete instruction, including the base instruction word
 * and any additional operand words based on their addressing modes.
 *
 * @param instruction Pointer to the instruction to analyze.
 * @return The number of words needed for this instruction (1-5 words).
 */
int get_instruction_word_count(const Instruction *instruction)
{
    int count = 1; /* Base instruction word */

    if (!instruction)
        return 1;

    /* Special case: both operands are registers - they share one word */
    if (instruction->has_source && instruction->has_target &&
        instruction->source.mode == ADDRESSING_REGISTER &&
        instruction->target.mode == ADDRESSING_REGISTER)
    {
        return 2; /* Base instruction word + shared register word */
    }

    /* Add source operand words */
    if (instruction->has_source)
    {
        switch (instruction->source.mode)
        {
        case ADDRESSING_IMMEDIATE:
        case ADDRESSING_DIRECT:
        case ADDRESSING_REGISTER: /* FIX: Registers need their own word */
            count += 1;
            break;

        case ADDRESSING_MATRIX:
            count += 2;
            break;
        }
    }

    /* Add target operand words */
    if (instruction->has_target)
    {
        switch (instruction->target.mode)
        {
        case ADDRESSING_IMMEDIATE:
        case ADDRESSING_DIRECT:
        case ADDRESSING_REGISTER:
            count += 1;
            break;

        case ADDRESSING_MATRIX:
            count += 2;
            break;
        }
    }

    return count;
}

/**
 * @brief Validate operand addresing modes for specifc instruction.
 *
 * This function checks if the addressing modes used in an instruction
 * are valid according to the addressing mode compatibility table.
 *
 * @param opcode        The instruction opcode (0-15).
 * @param source_mode   Source operand addressing mode (ignored if there is no source).
 * @param target_mode   Target operand addressing mode (ignored if there is no target).
 * @param has_source    TRUE if instruction has a source operand
 * @param has_target    TRUE if instruction has a target operand
 * @return TRUE if addresing mode combination is valid, FALSE otherwise.
 */
BOOL validate_addressing_modes(int opcode, AddressingMode source_mode, AddressingMode target_mode, BOOL has_source, BOOL has_target)
{
    /* Invalid opcode */
    if (opcode < 0 || opcode > 15)
        return FALSE;

    switch (opcode)
    {
    case 0: /* mov source(0,1,2,3) -> target (1,2,3) */
        return has_source && has_target &&
               (source_mode >= 0 && source_mode <= 3 &&
                target_mode >= 1 && target_mode <= 3);

    case 1: /* cmp source(0,1,2,3) -> target (0,1,2,3) */
        return has_source && has_target &&
               (source_mode >= 0 && source_mode <= 3 &&
                target_mode >= 0 && target_mode <= 3);

    case 2: /* add source(0,1,2,3) -> target (1,2,3) */
    case 3: /* sub source(0,1,2,3) -> target (1,2,3) */
        return has_source && has_target &&
               (source_mode >= 0 && source_mode <= 3 &&
                target_mode >= 1 && target_mode <= 3);

    case 4: /* lea source(1,2) -> target (1,2,3) */
        return has_source && has_target &&
               (source_mode >= 1 && source_mode <= 2 &&
                target_mode >= 1 && target_mode <= 3);

    case 5:  /* clr */
    case 6:  /* not */
    case 7:  /* inc */
    case 8:  /* dec */
    case 9:  /* jmp */
    case 10: /* bne */
    case 11: /* jsr */
    case 12: /* red */
        /* One operand: target(1,2,3)*/
        return !has_source && has_target &&
               (target_mode >= 1 && target_mode <= 3);

    case 13: /* prn: target(0,1,2,3) */
        return !has_source && has_target &&
               (target_mode >= 0 && target_mode <= 3);

    case 14: /* rts */
    case 15: /*stop */
        /* No operands */
        return !has_source && !has_target;

    default:
        return FALSE;
    }
}