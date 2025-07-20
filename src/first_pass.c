/**
 * @file first_pass.c
 * @brief Implements the first pass of the assembler.
 */

#include <stdio.h>
#include <string.h>

#include "first_pass.h"
#include "symbol_table.h"
#include "utils.h"

BOOL run_first_pass(const char *s_file, int *icf, int dcf)
{
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    int ic = BASE_IC_ADDRESS;
    int dc = 0;
    int line_number = 0;
    BOOL error_occurred = FALSE;

    /* Open the source file. */
    fp = fopen(s_file, "r");
}
