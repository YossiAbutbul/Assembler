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
typedef enum {
    SYMBOL_CODE,
    SYMBOL_DATA,
    SYMBOL_EXTERNAL,
    SYMBOL_ENTRY
}, SymbolType;

/**
 * @brief A single symbol entry in the symbol table.
 */
 typedef struct Symbol {
    char name[MAX_SYMBOL_NAME_LENGTH]; /**< The label name */
    int address;                       /**< The symbol address */
    SymbolType type;                   /**< Type: CODE / DATA / EXTERNAL / ENTRY */
    BOOL is_entry;                     /**< Is this symbol an entry? */
    BOOL is_external;                  /**< Is this symbol external? */
    struct Symbol *next;               /**< Pointer to the next symbol in the linked list */
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
 *          FALSE if already exists or table is full.
 */
 BOOL add_symbol(const char *name, int address, SymbolType type);