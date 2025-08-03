/**
 * @file assembler.c
 * @brief Main assembler coordination implementation.
 *
 * This file implements the main assemble() function that coordinates
 * all phases of the assembly process:
 * - Preprocessing
 * - First pass
 * - Second pass
 * - Output files generation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/assembler.h"
#include "../include/preprocessor.h"
#include "../include/first_pass.h"
#include "../include/second_pass.h"
#include "../include/output.h"
#include "../include/symbol_table.h"
#include "../include/data_image.h"
#include "../include/error.h"
#include "../include/constants.h"

/**
 * @brief Assemble the given source file (with .as extenstion) into machine code.
 *
 * This function coordinates the macro expansion, first pass, second pass and the
 * file output stages of the assemblt process.
 *
 * @param filename The base name of the source file to assemble (without the .as extenstion).
 * @return EXIT_SUCCESS_CODE if the assemblt was successfull, error code otherise.
 */
ExitCode assemble(const char *filename)
{
    char am_filename[MAX_FILE_NAME_LENGTH];
    FILE *am_file;
    AssemblyContext context;
    ExitCode result;
    BOOL context_initialized;

    /* Initialize am file and result */
    am_file = NULL;
    result = EXIT_SUCCESS_CODE;
    context_initialized = FALSE;

    /* Validate input */
    if (!filename)
    {
        report_error(EXIT_GENERAL_ERROR, "invalid_input");
        return EXIT_GENERAL_ERROR;
    }

    /* === Phase 1 - Preprocessing (Macro Expansion) === */
    result = preprocess(filename);
    if (result != EXIT_SUCCESS_CODE)
    {
        report_error(result, filename);
        return result;
    }

    /* === Phase 2 - Preprocessing (Macro Expansion) === */

    /* Initialize symbol table for this file */
    init_symbol_table();

    /* Initialize data image for this file */
    init_data_image();

    /* Open the .am file created by the preprocessor */
    sprintf(am_filename, "%s.am", filename);
    am_file = fopen(am_filename, "r");
    if (am_file != NULL)
    {
        /* Preforms first pass */
        if (first_pass(am_file, filename))
        {
            /* === Phase 3 - Second Pass === */

            /* Initialize assembly context for second pass */
            if (init_assembly_context(&context))
            {
                context_initialized = TRUE;

                /* Preforms second pass */
                if (second_pass(am_file, filename, &context))
                {
                    /* === Phase 4 - Output Generation === */

                    /* Generate all output files (.ob, .ent, .ext) */
                    if (!generate_output_files(filename, &context))
                    {
                        report_error(EXIT_WRITE_ERROR, filename);
                        result = EXIT_WRITE_ERROR;
                    }
                }
                else
                {
                    report_error(EXIT_SECOND_PASS_ERROR, filename);
                    result = EXIT_SECOND_PASS_ERROR;
                }
            }
            else
            {
                report_error(EXIT_GENERAL_ERROR, filename);
                result = EXIT_GENERAL_ERROR;
            }
        }
        else
        {
            report_error(EXIT_FIRST_PASS_ERROR, filename);
            result = EXIT_FIRST_PASS_ERROR;
        }
    }
    else
    {
        report_error(EXIT_FILE_NOT_FOUND, filename);
        result = EXIT_FILE_NOT_FOUND;
    }

    /* Cleanup section */
    if (context_initialized)
        cleanup_assembly_context(&context);

    if (am_file)
        fclose(am_file);

    /* Clean up global data structures */
    free_symbol_table();
    free_data_image();
    cleanup_first_pass_data();

    return result;
}