/* error.c */

#include <stdio.h>
#include "error.h"

void report_error(ExitCode exit_code, const char *filename)
{
    switch (exit_code)
    {
    case EXIT_FILE_NOT_FOUND:
        fprintf(stderr, "Error: File %s.as not found.\n", filename);
        break;
    case EXIT_MACRO_SYNTAX_ERROR:
        fprintf(stderr, "Error: Macro syntax error in file %s.as.\n", filename);
        break;
    case EXIT_FIRST_PASS_ERROR:
        fprintf(stderr, "Error: First pass error in file %s.as.\n", filename);
        break;
    case EXIT_SECOND_PASS_ERROR:
        fprintf(stderr, "Error: Second pass error in file %s.as.\n", filename);
        break;
    case EXIT_WRITE_ERROR:
        fprintf(stderr, "Error: Write error while processing %s.as.\n", filename);
        break;
    default:
        fprintf(stderr, "Error: General error occurred while processing %s.as\n", filename);
    }
}