/**
 * @file symbol_table.c
 * @brief Implements the symbol table management functions for the assembler.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"

static Symbol *symbol_table_head = NULL; /* Pointer to the head of the symbol table linked list */

/**
 * @brief Initializes the symbol table.
 */
void init_symbol_table()
{
    symbol_table_head = NULL; /* Initialize the symbol table to an empty state */
}

/**
 * @brief Check if a symbol with the given name exists in the symbol table.
 *
 * @param name The name of the symbol to check.
 * @return Symbol* Pointer to the symbol if found, NULL otherwise.
 */
static Symbol *find_symbol(const char *name)
{
    Symbol *current = symbol_table_head;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
            return current; /* Symbol found */

        current = current->next; /* Move to the next symbol */
    }
    return NULL; /* Symbol not found */
}

/**
 * @brief Adds a new symbol to the symbol table.
 *
 * @param name Symbol name.
 * @param address Memory address (IC / DC).
 * @param type The type of the symbol (CODE, DATA, EXTERNAL, ENTRY).
 * @return TRUE if added successfully,
 *         FALSE if already exists or table is full.
 */
BOOL add_symbol(const char *name, int address, SymbolType type)
{
    Symbol *existing_symbol = find_symbol(name);
    Symbol *new_symbol;

    if (existing_symbol != NULL)
        return FALSE; /* Symbol already exists */

    new_symbol = (Symbol *)malloc(sizeof(Symbol));
    if (new_symbol == NULL)
    {
        fprintf(stderr, "Memory allocation failed in adding new symbol.\n");
        exit(EXIT_GENERAL_ERROR);
        /*ToDo: make an error function to handke errorr printing correctly and exiting code */
    }

    strncpy(new_symbol->name, name, MAX_SYMBOL_NAME_LENGTH - 1);
    new_symbol->name[MAX_SYMBOL_NAME_LENGTH - 1] = '\0'; /* Ensure null-termination */
    new_symbol->address = address;
    new_symbol->type = type;
    new_symbol->is_entry = FALSE;
    new_symbol->is_external = (type == SYMBOL_EXTERNAL) ? TRUE : FALSE; /* Set external flag if type is EXTERNAL */
    new_symbol->next = symbol_table_head;                               /* Insert at the beginning of the linked list */
    symbol_table_head = new_symbol;                                     /* Update the head of the linked list */

    return TRUE; /* Symbol added successfully */
}

/**
 * @brief Marks an existing symbol as .entry.
 *
 * @param name The name of the symbol to mark as entry.
 * @return TRUE if the symbol was found and marked as entry,
 *         FALSE if the symbol does not exist.
 */
BOOL mark_symbol_as_entry(const char *name)
{
    Symbol *symbol = find_symbol(name);
    if (symbol == NULL)
        return FALSE;

    symbol->is_entry = TRUE; /* Mark the symbol as entry */
    return TRUE;             /* Successfully marked as entry */
}

/**
 * @brief Pull a symbol from the symbol table by name.
 *
 * @param name The name of the symbol to retrieve.
 * @return const Symbol* Pointer to the symbol if found, NULL otherwise.
 */
const Symbol *get_symbol(const char *name)
{
    return find_symbol(name); /* Return the const symbol pointer if found, NULL otherwise */
}

/**
 * @brief Updates the addresses of all DATA symbols in the symbol table.
 *
 * This function adjusts the addresses of DATA symbols by adding the instruction count final (ICF)
 * from the first pass to their addresses, allowing for correct memory allocation.
 *
 * @param icf The instruction count from the first pass.
 */
void update_data_symbols(int icf)
{
    Symbol *cur = symbol_table_head;
    while (cur != NULL)
    {
        if (cur->type == SYMBOL_DATA)
        {
            cur->address += icf; /* Update the address of DATA symbols */
        }
        cur = cur->next; /* Move to the next symbol */
    }
}

/**
 * @brief Free all memory allocated for the symbol table.
 */
void free_symbol_table()
{
    Symbol *cur = symbol_table_head;
    Symbol *sym_to_free;

    while (cur != NULL)
    {
        sym_to_free = cur->next; /* Store the next symbol */
        cur = cur->next;         /* Move to the next symbol */
        free(sym_to_free);       /* Free the current symbol */
    }
    symbol_table_head = NULL; /* Reset the head of the symbol table */
}
