/**
 * @file symbol_table.c
 * @brief Implements the symbol table management functions for the assembler.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/symbol_table.h"
#include "../include/error.h"

/* Global pointer to the head of the symbol table linked list */
static Symbol *symbol_table_head = NULL;

/* === Static Function Prototypes === */
static Symbol *find_symbol(const char *name);

/**
 * @brief Initializes the symbol table.
 *
 * This function sets up the symbol table for use by clearing any existing
 * symbols and preparing for new symbol additions.
 * @note This function will be called at the start of processing each source file.
 */
void init_symbol_table(void)
{
    /* Initialize the symbol table to an empty state */
    symbol_table_head = NULL;
}

/**
 * @brief Check if a label is already defined in the symbol table.
 *
 * This function checks wether a label already exists in the symbol
 * table without returning the actual symbol.
 *
 * @param name The name of the label to check.
 * @return TRUE if the label exist, FALSE otherwise.
 */
BOOL is_label_defined(const char *name)
{
    return (find_symbol(name) != NULL);
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

    /* Pass on the linked list looking for the symbol */
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
 * THis function creates a new symbol entry and adds it to the beginning
 * of the symbol table linked list. It preforms validation to ensure
 * no duplicate symbol are added.
 *
 * @param name      Symbol name.
 * @param address   Memory address (IC / DC).
 * @param type      The type of the symbol (CODE, DATA, EXTERNAL, ENTRY).
 * @return TRUE if added successfully, FALSE if already exists or memory allocation failed.
 */
BOOL add_symbol(const char *name, int address, SymbolType type)
{
    Symbol *existing_symbol = find_symbol(name);
    Symbol *new_symbol;

    /* Check if symbol already exists */
    if (existing_symbol != NULL)
        return FALSE;

    /* Allocate memory for new symbol */
    new_symbol = (Symbol *)malloc(sizeof(Symbol));
    if (new_symbol == NULL)
    {
        print_line_error("symbol_table", -1, ERROR_MEMORY_ALLOCATION_FAILED);
        return FALSE;
    }

    /* Initaalize the new symbol */
    strncpy(new_symbol->name, name, MAX_SYMBOL_NAME_LENGTH - 1);
    new_symbol->name[MAX_SYMBOL_NAME_LENGTH - 1] = '\0'; /* Ensure null-termination */
    new_symbol->address = address;
    new_symbol->type = type;
    new_symbol->is_entry = FALSE;
    new_symbol->is_external = (type == SYMBOL_EXTERNAL) ? TRUE : FALSE; /* Set external flag if type is EXTERNAL */
    new_symbol->next = symbol_table_head;                               /* Insert at the beginning of the linked list */
    symbol_table_head = new_symbol;                                     /* Update the head of the linked list */

    /* Symbol added successfully */
    return TRUE;
}

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
BOOL mark_symbol_as_entry(const char *name)
{
    Symbol *symbol = find_symbol(name);

    /* Check if the symbol exists */
    if (symbol == NULL)
        return FALSE;

    /* Mark the symbol as entry */
    symbol->is_entry = TRUE;
    return TRUE;
}

/**
 * @brief Pull a symbol from the symbol table by name.
 *
 * This function provides read-only access to symbols in the symbol table.
 * @note The returned pointer should not be used to modify the symbol.
 *
 * @param name The name of the symbol to retrieve.
 * @return const Symbol* Pointer to the symbol if found, NULL otherwise.
 */
const Symbol *get_symbol(const char *name)
{
    /* Return the const symbol pointer if found, NULL otherwise. */
    return find_symbol(name);
}

/**
 * @brief Updates the addresses of all DATA symbols in the symbol table.
 *
 * This function adjusts the addresses of DATA symbols by adding the instruction count final (ICF)
 * from the first pass to their addresses, allowing for correct memory allocation.
 * This is necessary because data is placed after instructions in the final memory layout.
 *
 * @param icf The instruction count from the first pass.
 */
void update_data_symbols(int icf)
{
    Symbol *cur = symbol_table_head;

    /* Pass all the symbols in the table */
    while (cur != NULL)
    {
        /* Update addresses of DATA symbols only */
        if (cur->type == SYMBOL_DATA)
        {
            /* Update the address of DATA symbols */
            cur->address += icf;
        }
        /* Move to the next symbol */
        cur = cur->next;
    }
}

/**
 * @brief Free all memory allocated for the symbol table.
 *
 * This function releases all memory allocated for the symbol table.
 * @note Called when processing is complete to prevent memory leaks.
 */
void free_symbol_table(void)
{
    Symbol *cur = symbol_table_head;
    Symbol *sym_to_free;

    /* Pass all the symbols in the table */
    while (cur != NULL)
    {
        sym_to_free = cur; /* Stores current symbol to free */
        cur = cur->next;   /* Move to the next symbol */
        free(sym_to_free); /* Free the current symbol */
    }

    /* Reset the head of the symbol table */
    symbol_table_head = NULL;
}

/**
 * @brief Print the symbol table to the console.
 *
 * This function displays all symbols in the table with thier
 * properties.
 *
 * @note This function is primarily for debugging purposes.
 */
void print_symbol_table(void)
{
    Symbol *cur = symbol_table_head;

    /* Print table header */
    printf("Symbol Table:\n");
    printf(" ----------------------------- \n");

    /* Print each symbol's information */
    while (cur != NULL)
    {
        printf("Name: %-31s | Addr: %4d | Type: %d | ENTRY: %d | EXTERN : %d\n",
               cur->name, cur->address, cur->type, cur->is_entry, cur->is_external);
        cur = cur->next; /* Move to the next symbol */
    }
}
