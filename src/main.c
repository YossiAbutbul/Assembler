/**
* @file main.c
* @author Yossi Abutbul
* ToDo: add propper documentation.
*/

#include <stdio.h>
#include <stdlib.h>
#include "../include/assembler.h"
#include "../include/error.h"
#include "../include/constants.h"

int main(int argc, char *argv[])
{
    int i;
    ExitCode result;

    if (argc < MIN_ARGC)
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
            report_error(result, argv[i]);
            return result;
        }
    }

    return EXIT_SUCCESS_CODE;
}