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

/* === Internal Helper Function Prototypes === */

static void process_line_second_pass(const char *line, const char *filename, int line_num, int *current_ic, AssemblyContext *context);
static void handle_instruction_second_pass(const char *line, const char *filename, int line_num, int *current_ic, AssemblyContext *context);
static void handle_entry_directive_second_pass(const char *line, const char *filename, int line_num, AssemblyContext *context);
static BOOL encode_instruction(const Instruction *instruction, int address, const char *filename, int line_num, AssemblyContext *context);
static BOOL encode_operand(const Operand *operand, int address, BOOL is_source, const char *filename, int line_num, AssemblyContext *context);
static BOOL encode_instruction_with_stored_data(const Instruction *instruction, const InstructionData *inst_data, int address, const char *filename, int line_num, AssemblyContext *context);

static int create_instruction_word(int opcode, AddressingMode source_mode, AddressingMode target_mode);
static BOOL init_instruction_image(InstructionImage *image);
static BOOL store_instruction_word(InstructionImage *image, int word, int address);
static void add_entry_symbol(AssemblyContext *context, const char *name, int address);
static void add_external_reference(AssemblyContext *context, const char *name, int address);
static void free_instruction_image(InstructionImage *image);
static void free_entry_list(EntryNode *list);
static void free_external_list(ExternalNode *list);
static BOOL validate_first_pass_data(void);

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
 * @brief Handles instruction lines during the second pass using ore-calculated
 * first pass data.
 *
 * @param line          The complete instruction line.
 * @param filename      Name of the source file (for error reporting).
 * @param line_num      The current line number (for error reporting).
 * @param current_ic    Pointer to the current instruction counter (IC)
 * @param context       Assembly context for storing results.
 */
static void handle_instruction_second_pass(const char *line, const char *filename, int line_num, int *current_ic, AssemblyContext *context)
{
    static int instruction_index = 0; /* Track which stored instruction we are processing */
    const InstructionData *inst_data;
    Instruction instruction;
    char *instruction_part;
    char line_copy[MAX_LINE_LENGTH + 1];
    char label[MAX_LABEL_LENGTH + 1];

    /* Get pre-calculated instruction data from first pass */
    inst_data = get_instruction_data(instruction_index);
    if (!inst_data)
    {
        print_line_error(filename, line_num, ERROR_GENERAL);
        err_found = TRUE;
        context->has_errors = TRUE;
        return;
    }

    /* Verify IC alignment with first pass */
    if (inst_data->ic_address != *current_ic)
    {
        print_line_error(filename, line_num, ERROR_GENERAL);
        err_found = TRUE;
        context->has_errors = TRUE;
        return;
    }

    /* Creates a copy of the line for processing */
    strncpy(line_copy, line, MAX_LINE_LENGTH);
    line_copy[MAX_LINE_LENGTH] = '\0'; /* Null terminate the copy */

    instruction_part = line_copy;

    /* Skip label if exists */
    if (extract_label(line, label))
        instruction_part = skip_label(instruction_part);

    /* Parse the instruction (for opernad details for e.g.) */
    if (!parse_instruction(instruction_part, filename, line_num, &instruction))
        return;

    /* Use pre-calculated data for encoding */
    if (!encode_instruction_with_stored_data(&instruction, inst_data, *current_ic, filename, line_num, context))
        return;

    /* Update instruction counter using pre-calculated word count */
    *current_ic += inst_data->word_count; /*todo : check if there is no mosmatchh with first pass and instruction parser word_count*/
    instruction_index++;
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
    mark_symbol_as_entry(label);
    add_entry_symbol(context, label, symbol->address);
}

/**
 * @brief Encodes a complete instruction into the machine code.
 *
 * @param instruction   The parsed instruction to encode.
 * @param address       The address where this instruction starts.
 * @param filename      Source filenmae (for error reporting).
 * @param line_num      Current line number (for error reporting).
 * @param context       Assembly context for storing results.
 * @return TRUE if encoding successful, FALSE otherwise.
 */
static BOOL encode_instruction(const Instruction *instruction, int address, const char *filename, int line_num, AssemblyContext *context)
{
    int instruction_word;
    int current_address = address;
    int register_word;

    /* Create the main instruction word */
    instruction_word = create_instruction_word(
        instruction->opcode,
        instruction->has_source ? instruction->source.mode : 0,
        instruction->has_target ? instruction->target.mode : 0);

    /* Store the instruction word */
    if (!store_instruction_word(context->instruction_image, instruction_word, current_address++))
    {
        print_line_error(filename, line_num, ERROR_INSTRUCTION_IMAGE_OVERFLOW);
        err_found = TRUE;
        context->has_errors = TRUE;
        return FALSE;
    }

    /* Handle register sharing optimization */
    if (instruction->has_source && instruction->has_target &&
        instruction->source.mode == ADDRESSING_REGISTER &&
        instruction->target.mode == ADDRESSING_REGISTER)
    {
        register_word = (instruction->source.value << 6) | (instruction->target.value << 2) | 0x00;

        /* Store the word */
        if (!store_instruction_word(context->instruction_image, register_word, current_address))
        {
            print_line_error(filename, line_num, ERROR_INSTRUCTION_IMAGE_OVERFLOW);
            err_found = TRUE;
            context->has_errors = TRUE;
            return FALSE;
        }

        return TRUE;
    }

    /* Encode source operand if exist */
    if (instruction->has_source)
    {
        if (!encode_operand(&instruction->source, current_address, TRUE, filename, line_num, context))
            return FALSE;

        /* Update address based on source operand encoding */
        switch (instruction->source.mode)
        {
        case ADDRESSING_IMMEDIATE:
        case ADDRESSING_DIRECT:
            current_address += 1;
            break;

        case ADDRESSING_MATRIX:
            current_address += 2;
            break;

        case ADDRESSING_REGISTER:
            current_address += 1;
            break;
        }
    }

    /* Encode target operand if exist */
    if (instruction->has_target)
    {
        if (!encode_operand(&instruction->target, current_address, FALSE, filename, line_num, context))
            return FALSE;
    }

    return TRUE;
}

/**
 * @brief Encodes a single operand into machine code.
 *
 * @param operand       The operand to encode.
 * @param address       The address where this operand should be stored.
 * @param is_source     TRUE if this source operand, FALSE for target.
 * @param filename      Source file name (for error reporting).
 * @param line_num      Current line number (for error reporting).
 * @param context       Assembly context for storing results.
 * @return TRUE if encoding successful, FALSE otherwise.
 */
static BOOL encode_operand(const Operand *operand, int address, BOOL is_source, const char *filename, int line_num, AssemblyContext *context)
{
    const Symbol *symbol;
    int operand_word;

    switch (operand->mode)
    {
    case ADDRESSING_IMMEDIATE:
        /* Encode immediate value with A,R,E = 00 (the encoding is absolute) */
        /* Shift left by 2 bits to make room for A,R,E bits and sets the A,R,E to 0 */
        operand_word = (operand->value << 2) | 0x00;

        /* store the word */
        if (!store_instruction_word(context->instruction_image, operand_word, address))
        {
            print_line_error(filename, line_num, ERROR_INSTRUCTION_IMAGE_OVERFLOW);
            err_found = TRUE;
            context->has_errors = TRUE;
            return FALSE;
        }
        return TRUE;

    case ADDRESSING_DIRECT:
        /* Look up symbol address in the symbol table */
        symbol = get_symbol(operand->symbol_name);

        /* If symbol not exist */
        if (!symbol)
        {
            print_line_error(filename, line_num, ERROR_UNDEFINED_SYMBOL);
            err_found = TRUE;
            context->has_errors = TRUE;
            return FALSE;
        }

        if (symbol->is_external)
        {
            /* External symbol: A,R,E = 01 */
            operand_word = 0x01;
            add_external_reference(context, operand->symbol_name, address);
        }
        else
        {
            /* Internal symbol: use symbol address, A,R,E = 10 */
            operand_word = (symbol->address << 2) | 0x02;
        }

        /* Store the word */
        if (!store_instruction_word(context->instruction_image, operand_word, address))
        {
            print_line_error(filename, line_num, ERROR_INSTRUCTION_IMAGE_OVERFLOW);
            err_found = TRUE;
            context->has_errors = TRUE;
            return FALSE;
        }
        return TRUE;

    case ADDRESSING_MATRIX:
        /* First word: Look up symbol address in the symbol table */
        symbol = get_symbol(operand->symbol_name);

        /* If symbol not exist */
        if (!symbol)
        {
            print_line_error(filename, line_num, ERROR_UNDEFINED_SYMBOL);
            err_found = TRUE;
            context->has_errors = TRUE;
            return FALSE;
        }

        if (symbol->is_external)
        {
            operand_word = 0x01;
            add_external_reference(context, operand->symbol_name, address);
        }
        else
        {
            operand_word = (symbol->address << 2) | 0x02;
        }

        /* Store the first word */
        if (!store_instruction_word(context->instruction_image, operand_word, address))
        {
            print_line_error(filename, line_num, ERROR_INSTRUCTION_IMAGE_OVERFLOW);
            err_found = TRUE;
            context->has_errors = TRUE;
            return FALSE;
        }

        /* Second word: regitser indicates */
        operand_word = (operand->reg1 << 6) | (operand->reg2 << 2) | 0x00;

        if (!store_instruction_word(context->instruction_image, operand_word, address + 1))
        {
            print_line_error(filename, line_num, ERROR_INSTRUCTION_IMAGE_OVERFLOW);
            err_found = TRUE;
            context->has_errors = TRUE;
            return FALSE;
        }
        return TRUE;

    case ADDRESSING_REGISTER:
        /* Register encoding depends on wether we have both source and target registers */
        if (is_source)
        {
            /* Source register goes in bits 6-9, A,R,E = 00 */
            operand_word = (operand->value << 6) | 0x00;
        }
        else
        {
            /* Target register goes in bits 2-5, A,R,E = 00 */
            operand_word = (operand->value << 2) | 0x00;
        }

        if (!store_instruction_word(context->instruction_image, operand_word, address))
        {
            print_line_error(filename, line_num, ERROR_INSTRUCTION_IMAGE_OVERFLOW);
            err_found = TRUE;
            context->has_errors = TRUE;
            return FALSE;
        }
        return TRUE;
    }

    /* encode_operand did not succeed */

    print_line_error(filename, line_num, ERROR_INVALID_OPERAND);
    err_found = TRUE;
    context->has_errors = TRUE;
    return FALSE;
}

/**
 * @brief Encodes instruction using pre-calculated first pass data.
 *
 * @param instruction   The parsed instruction.
 * @param inst_data     Pre-calculated instruction data from first pass.
 * @param address       Current IC address.
 * @param filename      Source filename (for error reporting).
 * @param line_num      Current line number (for error reporting).
 * @param context       Assembly context for storing results.
 * @return TRUE if encoding successful, FALSE otherwise.
 */
static BOOL encode_instruction_with_stored_data(const Instruction *instruction, const InstructionData *inst_data, int address, const char *filename, int line_num, AssemblyContext *context)
{
    int current_address = address;
    int immediate_index = 0;
    int register_word;

    /* Store the pre-built first instruction word */
    if (!store_instruction_word(context->instruction_image, inst_data->first_word, current_address++))
    {
        print_line_error(filename, line_num, ERROR_INSTRUCTION_IMAGE_OVERFLOW);
        err_found = TRUE;
        context->has_errors = TRUE;
        return FALSE;
    }

    /* Handle register sharing optimization */
    if (instruction->has_source && instruction->has_target &&
        instruction->source.mode == ADDRESSING_REGISTER &&
        instruction->target.mode == ADDRESSING_REGISTER)
    {
        register_word = (instruction->source.value << 6) | (instruction->source.value << 2) | 0x00;

        if (!store_instruction_word(context->instruction_image, register_word, current_address))
        {
            print_line_error(filename, line_num, ERROR_INSTRUCTION_IMAGE_OVERFLOW);
            err_found = TRUE;
            context->has_errors = TRUE;
            return FALSE;
        }
        return TRUE;
    }

    /* Encode source operand if exist */
    if (instruction->has_source)
    {
        if (instruction->soucre.mode == ADDRESSING_IMMEDIATE)
        {
            /* Use pre-encoded immediate word */
            if (immediate_index < inst_data->immediate_count)
            {
                if (!store_instruction_word(context->instruction_image,
                                            inst_data->immediate_words[immediate_count++],
                                            current_address))
                {
                    print_line_error(filename, line_num, ERROR_INSTRUCTION_IMAGE_OVERFLOW);
                    err_found = TRUE;
                    context->has_errors = TRUE;
                    return FALSE;
                }
            }
        }
        else
        {
            /* Use existing encode_opernad for non-immediate operands */
            if (!encode_operand(&instruction->source, current_address, TRUE, filename, line_num, context))
                return FALSE;
        }

        /* Update address based on source operand encoding */
        switch (instruction->source.mode)
        {
        case ADDRESSING_IMMEDIATE:
        case ADDRESSING_DIRECT:
            current_address += 1;
            break;

        case ADDRESSING_MATRIX:
            current_address += 2;
            break;

        case ADDRESSING_REGISTER:
            current_address += 1;
            break;
        }
    }

    /* Encode target operand if exist */
    if (instruction->has_target)
    {
        if (instruction->target.mode == ADDRESSING_IMMEDIATE)
        {
            /* Use pre-encoded immediate word */
            if (immediate_index < inst_data->immediate_count)
            {
                if (!store_instruction_word(context->instruction_image,
                                            inst_data->immediate_words[immediate_count++],
                                            current_address))
                {
                    print_line_error(filename, line_num, ERROR_INSTRUCTION_IMAGE_OVERFLOW);
                    err_found = TRUE;
                    context->has_errors = TRUE;
                    return FALSE;
                }
            }
        }
        else
        {
            /* Use existing encode_opernad for non-immediate operands */
            if (!encode_operand(&instruction->target, current_address, FALSE, filename, line_num, context))
                return FALSE;
        }
    }
}

/**
 * @brief Creates the main instruction word with opcode and addressing modes.
 *
 * @param opcode        The instruction opcode (0-15).
 * @param source_mode   Source operand addressing mode
 * @param target_mode   Target addressing mode.
 * @return The encoded instruction word.
 */
static int create_instruction_word(int opcode, AddressingMode source_mode, AddressingMode target_mode)
{
    int word = 0;

    /* Bits 6-9: opcode */
    word |= (opcode << 6);

    /* Bits 4-5: source addressing mode */
    word |= (source_mode << 4);

    /* Bits 2-3: target addressing mode */
    word |= (target_mode << 2);

    /* Bits 0-1: A,R,E = 00 for instruction words */
    word |= 0x00;

    return word;
}

/**
 * @brief Initializes the instruction image storage.
 *
 * @param image Pointer to instruction image to initialize.
 * @param TRUE if successful, FALSE if memory allocation failed.
 */
static BOOL init_instruction_image(InstructionImage *image)
{
    /* Checks if the pointer is valid */
    if (!image)
        return FALSE;

    /* Sets initail capacity */
    image->capacity = MAX_INSTRUCTION_IMAGE_SIZE;

    /* Dynamicly stores the machine code values */
    image->code = (int *)malloc(image->capacity * sizeof(int));

    /* Stores the memory address for each machine code word */
    image->addresses = (int *)malloc(image->capacity * sizeof(int));

    /* Tracks how many words are currently stored */
    image->size = 0;

    /* Returns TRUE if both allocations succeeded*/
    return (image->code != NULL && image->addresses != NULL);
}

/**
 * @brief Stores an instruction word at the specified address.
 *
 * @param image     Pointer to instruction image.
 * @param word      The machine code word to store.
 * @param address   The address where to store the word.
 * @return TRUE if successful, FALSE if memory allocation failed.
 */
static BOOL store_instruction_word(InstructionImage *image, int word, int address)
{
    if (!image)
        return FALSE;

    /* Check memory address bounds */
    if (address < 0 || address > 255)
    {
        return FALSE;
    }

    /* Check capacity */
    if (image->size >= image->capacity)
    {
        return FALSE;
    }

    /* Store the word */
    image->code[image->size] = word;
    image->addresses[image->size] = address;
    image->size++;
    return TRUE;
}

/**
 * @brief Adds a symbol to the entry list.
 *
 * @param context   Assembly context.
 * @param name      The symbol name.
 * @param address   The symbol address.
 */
static void add_entry_symbol(AssemblyContext *context, const char *name, int address)
{
    EntryNode *new_node = (EntryNode *)malloc(sizeof(EntryNode));

    if (!new_node)
        return;

    strncpy(new_node->name, name, MAX_SYMBOL_NAME_LENGTH - 1);
    new_node->name[MAX_SYMBOL_NAME_LENGTH - 1] = '\0'; /* Null terminate the name */
    new_node->address = address;
    new_node->next = context->entry_list;
    context->entry_list = new_node;
}

/**
 * @brief Adds an external symbol reference to the external list.
 *
 * @param context   Assembly context.
 * @param name      The external symbol name.
 * @param address   The address where the symbol is referenced.
 */
static void add_external_reference(AssemblyContext *context, const char *name, int address)
{
    ExternalNode *new_node = (ExternalNode *)malloc(sizeof(ExternalNode));

    if (!new_node)
        return;

    strncpy(new_node->name, name, MAX_SYMBOL_NAME_LENGTH - 1);
    new_node->name[MAX_SYMBOL_NAME_LENGTH - 1] = '\0'; /* Null terminate the name */
    new_node->address = address;
    new_node->next = context->external_list;
    context->external_list = new_node;
}

/**
 * @brief Gets the instruction image from the assembly context.
 */
const InstructionImage *get_instruction_image(const AssemblyContext *context)
{
    return context ? context->instruction_image : NULL;
}

/**
 * @brief Gets the entry list from the assembly context.
 */
const EntryNode *get_entry_list(const AssemblyContext *context)
{
    return context ? context->entry_list : NULL;
}

/**
 * @brief Gets the external list from the assembly context.
 */
const ExternalNode *get_external_list(const AssemblyContext *context)
{
    return context ? context->external_list : NULL;
}

/**
 * @brief Frees all memory allocated for the instruction image.
 */
static void free_instruction_image(InstructionImage *image)
{
    if (image)
    {
        free(image->code);
        free(image->addresses);
        image->code = NULL;
        image->addresses = NULL;
        image->size = 0;
        image->capacity = 0;
    }
}

/**
 * @brief Frees all memory allocated for the entry list.
 */
static void free_entry_list(EntryNode *list)
{
    EntryNode *current = list;
    EntryNode *next;

    while (current)
    {
        next = current->next;
        free(current);
        current = next;
    }
}

/**
 * @brief Frees all memory allocated for the external list.
 */
static void free_external_list(ExternalNode *list)
{
    ExternalNode *current = list;
    ExternalNode *next;

    while (current)
    {
        next = current->next;
        free(current);
        current = next;
    }
}

/**
 * @brief Cleanup function to free all assembly context resources.
 */
void cleanup_assembly_context(AssemblyContext *context)
{
    if (context)
    {
        if (context->instruction_image)
        {
            free_instruction_image(context->instruction_image);
            free(context->instruction_image);
            context->instruction_image = NULL;
        }

        free_entry_list(context->entry_list);
        context->entry_list = NULL;

        free_external_list(context->external_list);
        context->external_list = NULL;
    }
}

/**
 * @brief Validate first pass data before using it in second pass.
 */
static BOOL validate_first_pass_data(void)
{
    int expected_insts = get_instruction_count();

    if (expected_insts <= 0)
        return FALSE; /* No instructions found in first pass */

    /* todo: myabe insert another validation*/
    return TRUE;
}