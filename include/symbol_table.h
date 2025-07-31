/**
 * @file symbol_table.h
 * @brief Contains the defintion of the symbol table and it mangment functions.
 */

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "constants.h"

#define MAX_SYMBOL_NAME_LENGTH 32 /* max 30 chars + /0 + /n */

/**
 * @brief The type of a symbol in the symbol table.
 */
typedef enum
{
    SYMBOL_CODE,     /* Symbol represents a code address (instruction) */
    SYMBOL_DATA,     /* Symbol represents a data address (variable/constant) */
    SYMBOL_EXTERNAL, /* Symbol is defined in another file */
    SYMBOL_ENTRY     /* Symbol is exported for use by other files */
} SymbolType;

/**
 * @brief Represents a symbol in the symbol table.
 */
typedef struct Symbol
{
    char name[MAX_SYMBOL_NAME_LENGTH]; /* The label name */
    int address;                       /* The symbol address */
    SymbolType type;                   /* Type: CODE / DATA / EXTERNAL / ENTRY */
    BOOL is_entry;                     /* Is this symbol an entry? */
    BOOL is_external;                  /* Is this symbol external? */
    struct Symbol *next;               /* Pointer to the next symbol in the linked list */
} Symbol;

/**
 * @brief Initializes the symbol table.
 *
 * This function sets up the symbol table for use by clearing any existing
 * symbols and preparing for new symbol additions.
 * @note This function will be called at the start of processing each source file.
 */
void init_symbol_table(void);

/**
 * @brief Check if a label is already defined in the symbol table.
 *
 * This function checks wether a label already exists in the symbol
 * table without returning the actual symbol.
 *
 * @param name The name of the label to check.
 * @return TRUE if the label exist, FALSE otherwise.
 */
BOOL is_label_defined(const char *name);

/**
 * @brief Adds a new symbol to the symbol table.
 *
 * THis function creates a new symbol entry and adds it to the beginning
 * of the symbol table linked list. It preforms validation to ensure
 * no duplicate symbol are added.
 *
 * @param name      Symbol name.
 * @param address   Memory address (IC / DC).
 * @param type      The type of the symbol (CODE, DATA, EXTERNAL, ENTRY).
 * @return TRUE if added successfully, FALSE if already exists or memory allocation failed.
 */
BOOL add_symbol(const char *name, int address, SymbolType type);

/**
 * @brief Marks an existing symbol as .entry.
 *
 * This function finds a symbol in the symbol table and marks it as an
 * entry point that can be refernced from other files.
 *
 * @param name The name of the symbol to mark as entry.
 * @return TRUE if the symbol was found and marked as entry,
 *         FALSE if the symbol does not exist.
 */
BOOL mark_symbol_as_entry(const char *name);

/**
 * @brief Pull a symbol from the symbol table by name.
 *
 * This function provides read-only access to symbols in the symbol table.
 * @note The returned pointer should not be used to modify the symbol.
 *
 * @param name The name of the symbol to retrieve.
 * @return const Symbol* Pointer to the symbol if found, NULL otherwise.
 */
const Symbol *get_symbol(const char *name);

/**
 * @brief Updates the addresses of all DATA symbols in the symbol table.
 *
 * This function adjusts the addresses of DATA symbols by adding the instruction count final (ICF)
 * from the first pass to their addresses, allowing for correct memory allocation.
 * This is necessary because data is placed after instructions in the final memory layout.
 *
 * @param icf The instruction count from the first pass.
 */
void update_data_symbols(int icf);

/**
 * @brief Free all memory allocated for the symbol table.
 *
 * This function releases all memory allocated for the symbol table.
 * @note Called when processing is complete to prevent memory leaks.
 */
void free_symbol_table(void);

/**
 * @brief Print the symbol table to the console.
 *
 * This function displays all symbols in the table with thier
 * properties.
 *
 * @note This function is primarily for debugging purposes.
 */
void print_symbol_table(void);

#endif