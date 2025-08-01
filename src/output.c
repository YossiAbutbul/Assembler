/**
 * @file output.c
 * @brief Implementation of output file generation for assembler.
 *
 *  * According to the instructions on mmn14:
 * - All machine words are 10 bits (range: -512 to +511 in two's complement method).
 * - Memory addresses: 0-255 (8-bits), but the assembler uses 100-255 (IC start at 100).
 * - Base-4 encoding uses exactly 5 characters (at the encoded format):
 *   a,b,c,d where a=0, b=1, c=2, d=3.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/output.h"
#include "../include/error.h"
#include "../include/constants.h"
#include "../include/data_image.h"

/**
 * @brief Genrate all output files for successful assembly.
 *
 * This function creates the required output files based on the assembly results:
 * - Always create .ob file with machine code.
 * - Creates .ent file only if entry symbols are exist.
 * - Creates .ext file only if external refernces are exist.
 *
 * @param filename      Base filename (without extension).
 * @param context       Assembly context containing all generated data.
 * @return TRUE if all files generated successfully, FALSE otherwise.
 */
BOOL generate_output_files(const char *filename, const AssemblyContext *context)
{
    if (!filename || !context)
        return FALSE;

    /* Always generate .ob file */
    if (!generate_object_file(filename, context))
        return FALSE;

    /* Generate .ent file only if entries exist */
    if (context->entry_list)
    {
        if (!generate_entries_file(filename, context))
            return FALSE;
    }

    /* Generate .ext file only if externals exist */
    if (context->external_list)
    {
        if (!generate_externals_file(filename, context))
            return FALSE;
    }

    /* If all creations were successful, return TRUE */
    return TRUE;
}

/**
 * @brief Generate .ob (object) file containing machine code.
 *
 * Format:
 * - First line: instruction_count data_count.
 * - Following lines: address machine code in base-4 format (a,b,c,d).
 *
 * @param filename      Base filename (without extension).
 * @param context       Assembly context containing instruction and data image.
 * @return TRUE if file generated successfully, FALSE if error occured.
 */
BOOL generate_object_file(const char *filename, const AssemblyContext *context)
{
    FILE *ob_file;
    cahr ob_filename[MAX_FILE_NAME_LENGTH];
    char address_str[6], code_str[6]; /* 6 because it is 5 chars + null-terminator */
    char inst_count_str[6], data_count_str[6];
    const InstructionImage *inst_image;
    const int *data_array;
    int data_size;
    int i;
    int data_start_address = context->ICF; /* Data starts after instructions */

    if (!filename || !context)
        return FALSE;

    /* Creates .ob filename (we use snprintf()  because its provides automatic buffer overflow protection) */
    snprintf(ob_filename, sizeof(ob_filename), "%s.ob", filename);

    /* Open .ob file for writing */
    ob_file = fopen(ob_filename, "w");
    if (!ob_file)
    {
        fprint(stderr, "Error: Cannot create file %s\n", ob_filename); /* todo: handle with error mechanizim*/
        return FALSE;
    }

    /* Get instruction and data information */
    inst_image = get_instruction_image(context);
    data_array = get_data_array();
    data_size = get_data_size();

    /* Write header line: total length of instruction section (in memory words)
       and total length of the data section (in memory words)*/
    decimal_to_base4(inst_image ? inst_image->size : 0, inst_count_str);
    decimal_to_base4(data_size, data_count_str);
    fprintf(ob_file, "%s %s\n", inst_count_str, data_count_str); /* todo: think if needed a speciakl function to write to file */

    /* Write instruction image */
    if (inst_image)
    {
        for (i = 0; i < inst_image->size; i++)
        {
            decimal_to_base4(inst_image->addresses[i], address_str);
            decimal_to_base4(inst_image->code[i], code_str);
            fprintf(ob_file, "%s %s\n", address_str, code_str);
        }
    }

    /* Write data image */
    if (data_array && data_size > 0)
    {
        for (i = 0; i < data_size; i++)
        {
            decimal_to_base4(data_start_address + i, address_str);
            decimal_to_base4(data_array[i], code_str);
            fprintf(ob_file, "%s %s\n", address_str, code_str);
        }
    }

    fclose(ob_file);
    return TRUE;
}

/**
 * @brief Generate .ent (entries) file containing entry symbols.
 *
 * Format:
 * - symbol_name address (one per line).
 * Addresses in base-4 format (a,b,c,d)
 *
 * @param filename      Base filename (without extension).
 * @param context       Assembly context containing instruction and data image.
 * @return TRUE if file generated successfully, FALSE if error occured.
 */
BOOL generate_entries_file(const char *filename, const AssemblyContext *context)
{
    FILE *ent_file;
    char ent_filename[MAX_FILE_NAME_LENGTH];
    char address_str[6];
    const EntryNode *current;

    if (!filename || !context || !(context->entry_list))
        return FALSE;

    /* Create .ent filename */
    snprintf(ent_filename, sizeof(ent_filename), "%s.ent", filename);

    /* Open .ent filename for writing */
    ent_file = fopen(ent_filename, "w");
    if (!ent_file)
    {
        fprint(stderr, "Error: Cannot create file %s\n", ent_filename);
        return FALSE;
    }

    /* Write all entry symbols */
    current = context->entry_list;
    while (current)
    {
        decimal_to_base4(current->address, address_str);
        fprintf(ent_file, "%s %s\n", current->name, address_str);
        current = current->next;
    }

    fclose(ent_file);
    return TRUE;
}