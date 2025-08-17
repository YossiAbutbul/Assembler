/**
 * @file main.c
 * @author Yossi Abutbul
 * @brief Main entry point for the assembler program.
 *
 * This program processes assembly language source file (.as) and generates
 * output files. It handles multiple input files and processes
 * each one independently.
 *
 * Usage: "./assembler ./filename1 ./ filename2 [...]"
 *
 * For each input "filename", the program:
 * 1. Reads filename.as (source file).
 * 2. Generates filename.am (macro expanded file).
 * 3. Generates filename.ob (object file in base-4 encoding).
 * 4. Generates filename.ent (entry symbols, if any).
 * 5. Generates filename.ext (external references, if any).
 */

#include <stdio.h>
#include <stdlib.h>
#include "../include/assembler.h"
#include "../include/error.h"
#include "../include/constants.h"

/**
 * @brief Main entry point for assembler program.
 *
 * Processes command line arguments and assebles each specified file.
 * Each file is processed independently - if one fails, processing
 * continues with the next file. Errors are printed as the occur.
 *
 * @param argc Number of command line arguments.
 * @param argv Array of command line argument strings.
 * @return EXIT_SUCCESS_CODE if all files processed successfully,
 *         EXIT_GENERAL_ERROR if any file failed.
 */
int main(int argc, char *argv[])
{
    int i;
    ExitCode result;
    BOOL has_errors = FALSE;

    /* Check minimum number of arguments */
    if (argc < MIN_ARGC)
    {
        printf("Usage: %s file1 [file2 ...]\n", argv[0]);
        return EXIT_GENERAL_ERROR;
    }

    /* Process each input file */
    for (i = 1; i < argc; i++)
    {
        printf("Assembling file: %s.as\n", argv[i]);
        result = assemble(argv[i]);
        if (result == EXIT_FILE_EMPTY)
        {
            /* Empty file - don't print "Successfully assembled" */
            /* Continue to the next file */
        }

        else if (result != EXIT_SUCCESS_CODE)
        {
            has_errors = TRUE;

            /* Only print if there are more files to process */
            if (i < argc - 1)
                printf("Continuing with next file...\n");
        }
        else
            printf("Successfully assembled %s.as\n", argv[i]);

        /* Add blank line between files for readability */
        if (i < argc - 1)
            printf("\n");
    }

    /* Return success if all files succeeded, error if any failed */
    return has_errors ? EXIT_GENERAL_ERROR : EXIT_SUCCESS_CODE;
}