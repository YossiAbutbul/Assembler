#include <stdio.h>
#include <stdlib.h> /* for EXIT_SUCCESS */
#include <string.h>

#include "symbol_table.h"
#include "error.h"
#include "constants.h"

/**
 * @brief Simple assertion utility for test results.
 */
void assert_true(BOOL condition, const char *message)
{
    if (!condition)
    {
        printf("❌ FAIL: %s\n", message);
    }
    else
    {
        printf("✅ PASS: %s\n", message);
    }
}

int main(void)
{
    const Symbol *sym;

    printf("Running symbol_table unit tests...\n\n");

    init_symbol_table();

    /* Add a CODE symbol */
    assert_true(add_symbol("MAIN", 100, SYMBOL_CODE), "Add symbol MAIN as CODE");
    assert_true(!add_symbol("MAIN", 200, SYMBOL_CODE), "Reject duplicate symbol MAIN");

    /* Add a DATA symbol */
    assert_true(add_symbol("ARRAY", 0, SYMBOL_DATA), "Add symbol ARRAY as DATA");

    /* Add an EXTERNAL symbol */
    assert_true(add_symbol("EXT_SYM", 0, SYMBOL_EXTERNAL), "Add EXTERNAL symbol EXT_SYM");

    /* Mark symbol as .entry */
    assert_true(mark_symbol_as_entry("MAIN"), "Mark MAIN as .entry");
    assert_true(!mark_symbol_as_entry("UNKNOWN"), "Reject mark of unknown symbol");

    /* Retrieve and check flags */
    sym = get_symbol("MAIN");
    assert_true(sym != NULL, "Retrieve MAIN from table");
    assert_true(sym->type == SYMBOL_CODE, "Check MAIN is CODE");
    assert_true(sym->is_entry == TRUE, "Check MAIN is marked as ENTRY");

    sym = get_symbol("EXT_SYM");
    assert_true(sym->is_external == TRUE, "Check EXT_SYM is marked as EXTERNAL");

    /* Update DATA symbol addresses */
    update_data_symbols(150);
    sym = get_symbol("ARRAY");
    assert_true(sym->address == 150, "ARRAY address updated by ICF=150");

    print_symbol_table();

    free_symbol_table();

    return EXIT_SUCCESS_CODE;
}
