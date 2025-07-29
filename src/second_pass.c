/**
 * @file second_pass.c
 * @brief Implements the second pass of the assmenbler.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "../include/second_pass.h"
#include "../include/symbol_table.h"
#include "../include/utils.h"
#include "../include/error.h"
#include "../include/constants.h"
#include "../include/instruction_parser.h"
#include "../include/first_pass.h"
#include "../include/assembler_types.h"

/* External variables from first pass */
extern int IC;
extern int DC;
extern BOOL err_found;

/* Initial capaciry for instruction image */
#define INITIAL_INSTRUCTION_CAPACITY 1000

/* === Internal Helper Function Prototypes === */
static void process_line_second_pass(const char *line, const char *filename, int line_num, int *current_ic, AssemblyContext *context);
static void handle_instruction_second_pass(const char *line, const char *filename, int line_num, int *current_ic, AssemblyContext *context);
static void handle_entry_directive_second_pass(const char *line, const char *filename, int line_num, AssemblyContext *context);
static BOOL encode_instruction(const Instruction *instruction, int address, const char *filename, int line_num, AssemblyContext *context);
static BOOL encode_operand(const Operand *operand, int address, BOOL is_source, const char *filename, int line_num, AssemblyContext *context);
static int create_instruction_word(int opcode, AddressingMode source_mode, AddressingMode target_mode);
static BOOL init_instruction_image(InstructionImage *image);
static BOOL store_instruction_word(InstructionImage *image, int word, int address);
static void add_entry_symbol(AssemblyContext *context, const char *name, int address);
static void add_external_reference(AssemblyContext *context, const char *name, int address);
static void free_instruction_image(InstructionImage *image);
static void free_entry_list(EntryNode *list);
static void free_external_list(ExternalNode *list);

/**
 * @brief Initializes an assembly context structure.
 *
 * @param context Pointer to the assembly context to initalize.
 * @return TRUE if initalization successful, FALSE if memory allocation failed.
 */
BOOL init_assembly_context(AssemblyContext *context)
{
    if (!context)
        return FALSE;

    /* Initalizes all fields */
    context->instruction_image = (InstructionImage *)malloc(sizeof(InstructionImage));
    if (!context->instruction_image)
        return FALSE;

    /* This ensures that the instruction_image is only used if it has been properly initialized */
    if (!init_instruction_image(context->instruction_image))
    {
        free(context->instruction_image);
        context->instruction_image = NULL;
        return FALSE;
    }

    context->entry_list = NULL;
    context->external_list = NULL;
    context->IC = BASE_IC_ADDRESS;
    context->DC = 0;
    context->ICF = 0;
    context->DCF = 0;
    context->has_errors = FALSE;

    return TRUE;
}

/**
 * @brief Preforms the second pass on the given .am source file.
 *
 * Completes the assembly process by:
 * - Resolving symbol addresses using the symbol table.
 * - Generating complete machine code for all instructions.
 * - Processing .entry directives and building entry list.
 * - Recording external symbol refernces for linking.
 * - Validating all symbols refernces are defined.
 *
 * @param am_file   Pointer to the .am source file (after macro expansion).
 * @param filename  Pointer to the source file (for error reporting).
 * @param context   Assembly context to store results.
 * @return TRUE if second pass completed successfully, FALSE if errors ocurred.
 */
BOOL second_pass(FILE *am_file, const char *filename, AssemblyContext *context)
{
    char line[MAX_LINE_LENGTH];
    int line_num = 0;
    int current_ic = BASE_IC_ADDRESS;

    if (!am_file || !filename || !context)
        return FALSE;

    /* Reset the file pointer back to the start of the file (can be moved after first pass)*/
    rewind(am_file);

    /* Process each line in the file */
    while (fgets(line, sizeof(line), am_file))
    {
        line_num++;

        /* Trim leading and trailing whitespaces */
        trim_whitespace(line);

        /* Skip empty lines and comments */
        if (is_whitespace(line) || is_comment(line))
            continue;

        process_line_second_pass(line, filename, line_num, &current_ic, context);
    }

    /* Update final counters */
    context->ICF = current_ic;
    context->DCF = DC;
    context->has_errors = err_found;

    /* Second pass completed if no errors found */
    return !err_found;
}

/**
 * @brief Processes a single line during the second pass.
 *
 * @param line          The line content to process.
 * @param filename      Name of the source file (for error reporting).
 * @param line_num      The current line number (for error reporting).
 * @param current_ic    Pointer to the current instruction counter (IC).
 * @param context       Assembly context for storing results.
 */
static void process_line_second_pass(const char *line, const char *filename, int line_num, int *current_ic, AssemblyContext *context)
{
    char first_token[MAX_LINE_LENGTH + 1];
    char *rest;
    char buffer[MAX_LINE_LENGTH + 1];

    /* Creates a copy of the line */
    strncpy(buffer, line, MAX_LINE_LENGTH);
    buffer[MAX_LINE_LENGTH] = '\0'; /* Null terminate the buffer */
    rest = buffer;

    /* Skip label if exists */
    if (extract_label(rest, first_token))
        rest = skip_label(rest);

    /* Get the first token (instruction or directive) */
    if (!get_next_token(rest, first_token))
        return; /* Safe handle this scenrio (not supposed to happened) */

    /* handle .entry directive */
    if (strcmp(first_token, ".entry") == 0)
        handle_entry_directive_second_pass(rest, filename, line_num, context);

    /* Skips .extern, .data, .string, .mat directives (already processed in first pass) */
    else if (strcmp(first_token, ".extern") == 0 ||
             strcmp(first_token, ".data") == 0 ||
             strcmp(first_token, ".string") == 0 ||
             strcmp(first_token, ".mat") == 0)
    {
        return;
    }

    /* Handle instructions */
    else if (is_instruction(first_token))
        handle_instruction_second_pass(line, filename, line_num, current_ic, context);
}

/**
 * @brief Handles instruction lines during the second pass.
 *
 * @param line          The complete instruction line.
 * @param filename      Name of the source file (for error reporting).
 * @param line_num      The current line number (for error reporting).
 * @param current_ic    Pointer to the current instruction counter (IC)
 * @param context       Assembly context for storing results.
 */
static void handle_instruction_second_pass(const char *line, const char *filename, int line_num, int *current_ic, AssemblyContext *context)
{
    Instruction instruction;
    char *instruction_part;
    char line_copy[MAX_LINE_LENGTH + 1];
    char label[MAX_LABEL_LENGTH + 1];

    /* Creates a copy of the line for processing */
    strncpy(line_copy, line, MAX_LINE_LENGTH);
    line_copy[MAX_LINE_LENGTH] = '\0'; /* Null terminate the line_copy */

    instruction_part = line_copy;

    /* Skip label if exist */
    if (extract_label(line, label))
        instruction_part = skip_label(instruction_part);

    /* Parse the instruction */
    if (!parse_instruction(instruction_part, filename, line_num, &instruction))
        return; /* Error already being reported in parse_instruction */

    if (!encode_instruction(&instruction, *current_ic, filename, line_num, context))
        return; /* Error already being reported in encode_instruction */

    /* Update instruction counter */
    *current_ic += instruction.word_count;
}

/**
 * @brief Handles .entry directive during the second pass.
 *
 * @param line          The line content after the ".entry" directive.
 * @param filename      Pointer to the file name (for error reporting).
 * @param line_num      The current line number (for error reporting).
 * @param context       Assembly context for storing results.
 */
static void handle_entry_directive_second_pass(const char *line, const char *filename, int line_num, AssemblyContext *context)
{
    char label[MAX_LABEL_LENGTH + 1];
    const Symbol *symbol;

    /* Skip leading whitespace */
    while (isspace((unsigned char)*line))
        line++;

    /* Extract the label name */
    if (!get_next_token(line, label))
    {
        print_line_error(filename, line_num, ERROR_SYNTAX);
        err_found = TRUE;
        context->has_errors = TRUE;
        return;
    }

    /* Find the symbol in the symbol table */
    symbol = get_symbol(label);
    if (!symbol)
    {
        print_line_error(filename, line_num, ERROR_UNDEFINED_SYMBOL);
        err_found = TRUE;
        context->has_errors = TRUE;
        return;
    }

    /* Check if it's an external symbol (can't be entry) */
    if (symbol->is_external)
    {
        print_line_error(filename, line_num, ERROR_EXTERNAL_CONFLICT);
        err_found = TRUE;
        context->has_errors = TRUE;
        return;
    }

    /* Mark symbol as entry and add to entry list */
}
