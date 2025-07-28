/***
 * @file instruction_parser.h
 * @brief Header for instruction parsing and encoding functionality.
 */

#ifndef INSTRUCTION_PARSER_H
#define INSTRUCTION_PARSER_H

#include "../include/constants.h"
#include "../include/symbol_table.h"

/**
 * @brief Addressing modes supported by this project.
 *
 * The supported modes:
 * - Mode 0: Immediate addressing (#value).
 * - Mode 1: Direct addressing (label).
 * - Mode 2: Matrix addressing (label[reg][reg]).
 * - Mode 3: Register addressing (register).
 */
typedef enum
{
    ADDRESSING_IMMEDIATE = 0, /*#value - Immediate constant */
    ADDRESSING_DIRECT = 1,    /* lable - direct memory refernce */
    ADDRESSING_MATRIX = 2,    /* label[reg][reg] - matrix element access */
    ADDRESSING_REGISTER = 3   /* register - register refernce */
} AddressingMode;

/**
 * @brief Instruction types based on operand count.
 */
typedef enum
{
    INST_TWO_OPERANDS, /* mov, cmp, add, sub, lea */
    INST_ONE_OPERAND,  /* clr, not, inc, dec, jmp, bne, jsr, red, prn */
    INST_NO_OPERNADS,  /* rts, stop */
    INST_INVALID = -1  /* Invalid opcode */
} InstructionType;

/**
 * @brief Represents a parsed operand with all necessary information.
 */
typedef struct
{
    AddressingMode mode;                    /* Addresing mode (0-3) */
    int value;                              /* For immediate values or register numbers */
    int reg1, reg2;                         /* For matrix addressing: [reg1][reg2]*/
    char symbol_name[MAX_LABEL_LENGTH + 1]; /* For symbol refernce */
    BOOL is_symbol;                         /* TRUE if operand refernces a symbol */
} Operand;

/**
 * @brief Represents a completly parsed instruction.
 */
typedef struct
{
    int opcode;           /* Instruction opcode (0-15) */
    InstructionType type; /* Number of opernads this instruction takes */
    Operand source;       /* Source opernad (for 2-opernad instructions) */
    Operand target;       /* Target opernad (for 1 and 2-opernad instructions)*/
    int word_count;       /* total memory words needed for this instruction */
    BOOL has_source;      /* TRUE if instruction has a source opernad */
    BOOL has_target;      /* TRUE if instruction has a target opernad */
} Instruction;

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
BOOL parse_instruction(const char *line, const char *filename, int line_num, Instruction *instruction);

/**
 * @brief Get instruction opcode from instruction name.
 *
 * This function maps instruction names (like "mov", "add") to their
 * corresponding opcodes (0-15) according to the opcodes table in the project instruction.
 *
 * @param instruction_name The instruction name.
 * @return Opcode value (0-15), or -1 if instruction is invalid.
 */
int get_instruction_opcode(const char *instruction_name);

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
int get_instruction_word_count(const Instruction *instruction);

/**
 * @brief Validate opernad addresing modes for specifc instruction.
 *
 * This function checks if the addressing modes used in an instruction
 * are valid according to the addressing mode compatibility table.
 *
 * @param opcode        The instruction opcoode (0-15).
 * @param source_mode   Source opernad addressing mode (ignored if there is no source).
 * @param target_mode   Target opernad addressing mode (ignored if there is no target).
 * @param has_source    TRUE if instruction has a source opernad
 * @param has_target    TRUE if instruction has a target opernad
 * @return TRUE if addresing mode combination is valid, FALSE otherwise.
 */
BOOL validate_addressing_modes(int opcode, AddressingMode source_mode, AddressingMode target_mode, BOOL has_source, BOOL has_target);