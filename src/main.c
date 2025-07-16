/* main.c
 * ToDo: add documentation for the main.c file.
 */

#include <stdio.h>
#include <stdlib.h>
#include "assembler.h"

int main(int argc, char *argv[])
{
    int i;
    ExitCode result;

    if (argc < 2)
    {
        printf("Usage: %s file1 [file2 ...]\n", argv[0]);
        return EXIT_GENERAL_ERROR;
    }

    for (i = 1; i < argc; i++)
    {
        printf("Assembling file: %s.as\n", argv[i]);
        result = assemble(argv[i]);

        if (result != EXIT_SUCCESS_CODE)
        {
            switch (result)
            {
            case EXIT_FILE_NOT_FOUND:
                fprintf(stderr, "Error: File '%s' not found.\n", argv[i]);
                break;
            case EXIT_MACRO_SYNTAX_ERROR:
                fprintf(stderr, "Error: Macro syntax error in file '%s'.\n", argv[i]);
                break;
            case EXIT_FIRST_PASS_ERROR:
                fprintf(stderr, "Error: First pass error in file '%s'.\n", argv[i]);
                break;
            case EXIT_SECOND_PASS_ERROR:
                fprintf(stderr, "Error: Second pass error in file '%s'.\n", argv[i]);
                break;
            case EXIT_WRITE_ERROR:
                fprintf(stderr, "Error: Write error for file '%s'.\n", argv[i]);
                break;
            default:
                fprintf(stderr, "Error: General error while processing file '%s'.\n", argv[i]);
            }
            return result;
        }
    }

    return EXIT_SUCCESS_CODE;
}