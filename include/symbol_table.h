/**
 * @file symbol_table.h
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
    SYMBOL_CODE,
    SYMBOL_DATA,
    SYMBOL_EXTERNAL,
    SYMBOL_ENTRY
} SymbolType;

/**
 * @file symbol_table.h
 * @brief Contains the definition of the symbol table and its management functions.
 */

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
 */
void init_symbol_table();

/**
 * @brief Adds a new symbol to the symbol table.
 *
 * @param name Symbol name.
 * @param address Memory address (IC / DC).
 * @param type The type of the symbol (CODE, DATA, EXTERNAL, ENTRY).
 * @return TRUE if added successfully,
 *         FALSE if already exists or table is full.
 */
BOOL add_symbol(const char *name, int address, SymbolType type);

/**
 * @brief Marks an existing symbol as .entry.
 *
 * @param name The name of the symbol to mark as entry.
 * @return TRUE if the symbol was found and marked as entry,
 *         FALSE if the symbol does not exist.
 */
BOOL mark_symbol_as_entry(const char *name);

/**
 * @brief Pull a symbol from the symbol table by name.
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
 *
 * @param icf The instruction count from the first pass.
 */
void update_data_symbols(int icf);

/**
 * @brief Free all memory allocated for the symbol table.
 */
void free_symbol_table();

/**
 * @brief Print the symbol table to the console.
 *
 * @note This function is primarily for debugging purposes.
 */
void print_symbol_table();

#endif