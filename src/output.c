/**
 * @file output.c
 * @brief Implementation of output file generation for assembler.
 *
 * According to the instructions on mmn14:
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
#include "../include/first_pass.h"
#include "../include/second_pass.h"

/**
 * @brief Generate all output files for successful assembly.
 *
 * This function creates the required output files based on the assembly results:
 * - Always create .ob file with machine code.
 * - Creates .ent file only if entry symbols exist.
 * - Creates .ext file only if external references exist.
 *
 * @param filename      Base filename (without extension).
 * @param context       Assembly context containing all generated data.
 * @return TRUE if all files generated successfully, FALSE otherwise.
 */
BOOL generate_output_files(const char *filename, const AssemblyContext *context)
{
    if (!filename || !context)
        return FALSE;

    /* Don't generate output files if there were assembly errors */
    if (context->has_errors)
        return FALSE;

    printf("DEBUG OUTPUT: Starting output generation\n");
    printf("DEBUG OUTPUT: context->entry_list = %p\n", (void *)context->entry_list);
    printf("DEBUG OUTPUT: context->external_list = %p\n", (void *)context->external_list);

    /* Always generate .ob file */
    if (!generate_object_file(filename, context))
        return FALSE;

    /* Generate .ent file only if entries exist */
    if (context->entry_list)
    {
        printf("DEBUG OUTPUT: Generating .ent file\n");
        if (!generate_entries_file(filename, context))
            return FALSE;
    }
    else
        printf("DEBUG OUTPUT: No entries - skipping .ent file\n");

    /* Generate .ext file only if externals exist */
    if (context->external_list)
    {
        printf("DEBUG OUTPUT: Generating .ext file\n");
        if (!generate_externals_file(filename, context))
            return FALSE;
    }
    else
        printf("DEBUG OUTPUT: No externals - skipping .ext file\n");

    printf("DEBUG OUTPUT: Output generation complete\n");
    /* If all creations were successful, return TRUE */
    return TRUE;
}

/**
 * @brief Generate .ob (object) file containing machine code.
 *
 * Format:
 * - First line: instruction_count data_count.
 * - Following lines: address machine_code in base-4 format (a,b,c,d).
 *
 * @param filename      Base filename (without extension).
 * @param context       Assembly context containing instruction and data image.
 * @return TRUE if file generated successfully, FALSE if error occurred.
 */
BOOL generate_object_file(const char *filename, const AssemblyContext *context)
{
    FILE *ob_file;
    char ob_filename[MAX_FILE_NAME_LENGTH];
    char address_str[6], code_str[6];
    char inst_count_str[6], data_count_str[6];
    const InstructionImage *inst_image;
    const int *data_array;
    int data_size;
    int i;
    int data_start_address;
    int actual_instruction_count;

    printf("DEBUG OB: Starting object file generation\n");

    if (!filename || !context)
        return FALSE;

    /* Create .ob filename */
    sprintf(ob_filename, "%s.ob", filename);

    /* Open .ob file for writing */
    ob_file = fopen(ob_filename, "w");
    if (!ob_file)
    {
        report_error(EXIT_WRITE_ERROR, filename);
        return FALSE;
    }

    /* Get instruction and data information */
    inst_image = get_instruction_image(context);
    data_array = get_data_array();
    data_size = get_data_size();
    data_start_address = context->ICF; /* Data starts after instructions */

    printf("DEBUG OB: inst_image size = %d\n", inst_image ? inst_image->size : 0);
    printf("DEBUG OB: data_size = %d\n", data_size);
    printf("DEBUG OB: context->ICF = %d\n", context->ICF);
    printf("DEBUG OB: context->DCF = %d\n", context->DCF);

    /* Calculate actual instruction count (ICF - BASE_IC_ADDRESS) */
    actual_instruction_count = context->ICF - BASE_IC_ADDRESS;

    /* Write header line: instruction count and data count in base-4 */
    count_to_base4(actual_instruction_count, inst_count_str);
    count_to_base4(data_size, data_count_str);

    printf("DEBUG OB: Header - instructions: %d (%s), data: %d (%s)\n",
           actual_instruction_count, inst_count_str, data_size, data_count_str);

    fprintf(ob_file, "%s %s\n", inst_count_str, data_count_str);
    printf("DEBUG OB: Header written successfully\n");

    /* Write instruction image */
    if (inst_image)
    {
        printf("DEBUG OB: Writing %d instruction words\n", inst_image->size);
        for (i = 0; i < inst_image->size; i++)
        {
            address_to_base4(inst_image->addresses[i], address_str);
            decimal_to_base4(inst_image->code[i], code_str);
            fprintf(ob_file, "%s %s\n", address_str, code_str);
            printf("DEBUG OB: Instruction %d: addr=%d (%s), code=%d (%s)\n",
                   i, inst_image->addresses[i], address_str, inst_image->code[i], code_str);
        }
    }

    /* Write data image */
    if (data_array && data_size > 0)
    {
        printf("DEBUG OB: Writing %d data words starting at address %d\n", data_size, data_start_address);

        for (i = 0; i < data_size; i++)
        {
            address_to_base4(data_start_address + i, address_str);
            decimal_to_base4(data_array[i], code_str);
            fprintf(ob_file, "%s %s\n", address_str, code_str);

            printf("DEBUG OB: Data %d: addr=%d (%s), value=%d (%s)\n",
                   i, data_start_address + i, address_str, data_array[i], code_str);
        }
    }

    fclose(ob_file);
    printf("DEBUG OB: Object file generation complete\n");
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
 * @param context       Assembly context containing entry list..
 * @return TRUE if file generated successfully, FALSE if error occurred.
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
    sprintf(ent_filename, "%s.ent", filename);

    /* Open .ent filename for writing */
    ent_file = fopen(ent_filename, "w");
    if (!ent_file)
    {
        report_error(EXIT_WRITE_ERROR, filename);
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

/**
 * @brief Generate .ext (external) file containing external references.
 *
 * Format:
 * - symbol_name address (one per line).
 * Addresses in base-4 format (a,b,c,d)
 *
 * @param filename      Base filename (without extension).
 * @param context       Assembly context containing external reference list.
 * @return TRUE if file generated successfully, FALSE if error occurred.
 */
BOOL generate_externals_file(const char *filename, const AssemblyContext *context)
{
    FILE *ext_file;
    char ext_filename[MAX_FILE_NAME_LENGTH];
    char address_str[6];
    const ExternalNode *current;

    if (!filename || !context || !context->external_list)
        return FALSE;

    /* Create .ext filename */
    sprintf(ext_filename, "%s.ext", filename);

    /* Open file for writing */
    ext_file = fopen(ext_filename, "w");
    if (!ext_file)
    {
        report_error(EXIT_WRITE_ERROR, filename);
        return FALSE;
    }

    /* Write all external references */
    current = context->external_list;
    while (current)
    {
        decimal_to_base4(current->address, address_str);
        fprintf(ext_file, "%s %s\n", current->name, address_str);
        current = current->next;
    }

    fclose(ext_file);
    return TRUE;
}

/**
 * @brief Converts decimal value to base-4 format.
 *
 * Converts a decimal value to the unique base-4 format:
 * 0->a, 1->b, 2->c, 3->d
 * The converted word is exactly 5 digits with padding of 'a' if needed.
 *
 * Valid input range (10 bit two's complement method):
 * - Positive: 0 to +511 (aaaaa to bdddd).
 * - Negative: -1 to -512 (ddddd to caaaa)
 *
 * Implementation Details:
 * - Handles 10-bit two's complement values (-512 to +511).
 * - Negative values converted using two's complement representation.
 * - Output is exactly 5 base-4 chars using a,b,c,d.
 * - Leading 'a's pad values shorter than 5 digits.
 *
 * @param value     Decimal value to convert (valid range: -512 to +511).
 * @param output    Buffer for 5-character result + null-terminator.
 */
void decimal_to_base4(int value, char *output)
{
    const char digits[] = "abcd";
    char temp[6];
    int i = 0, j;

    if (!output)
        return;

    /* For 10-bit values, ensure we stay within 10-bit range */
    if (value < 0)
        value = (1 << 10) + value; /* Convert negative to positive representation */

    /* Ensure value fits in 10 bits (0-1023) */
    value &= 0x3FF; /* Keep only bits 0-9 */

    /* Convert to base 4 digits */
    if (value == 0)
    {
        strcpy(output, "aaaaa");
        return;
    }

    /* Extract base 4 digits (lsb first) */
    while (value > 0)
    {
        temp[i++] = digits[value % 4];
        value /= 4;
    }

    /* Pad with 'a' to make exactly 5 chars */
    while (i < 5)
        temp[i++] = 'a';

    temp[i] = '\0';

    /* Reverse string to get correct order (msb first) */
    for (j = 0; j < 5; j++)
        output[j] = temp[4 - j];

    output[5] = '\0';
}

/**
 * @brief Converts decimal value to base-4 without padding.
 *
 * Used for instruction and data counts in the header line.
 * Does not pad with leading 'a' characters.
 *
 * @param value     Decimal value to convert.
 * @param output    Buffer for result + null-terminator.
 */
void count_to_base4(int value, char *output)
{
    const char digits[] = "abcd";
    char temp[10]; /* Enough for any IC or DC count */
    int i = 0, j;

    if (!output)
        return;

    /* Handle zero case */
    if (value == 0)
    {
        strcpy(output, "a");
        return;
    }

    /* Convert to base 4 digits (lsb first) */
    while (value > 0)
    {
        temp[i++] = digits[value % 4];
        value /= 4;
    }

    temp[i] = '\0';

    /* Reverse string to get correct order (msb first) */
    for (j = 0; j < i; j++)
        output[j] = temp[i - 1 - j];

    output[i] = '\0'; /* Ensure null terminate */
}

/**
 * @brief Converts deciamk address to 4-character base-4 format.
 *
 * Used for memory addresses in the obkect file.
 *
 * @param value     Decimal value to convert.
 * @param output    Buffer for result + null-terminator.
 */
void address_to_base4(int value, char *output)
{
    const char digits[] = "abcd";
    char temp[5];
    int i = 0, j;

    if (!output)
        return;

    /* Convert to base 4 digits (lsb first) */
    if (value == 0)
    {
        strcpy(output, "aaaa");
        return;
    }

    /* Convert to base 4 digits (lsb first) */
    while (value > 0 && i < 4)
    {
        temp[i++] = digits[value % 4];
        value /= 4;
    }

    /* Pad with 'a' */
    while (i < 4)
        temp[i++] = 'a';

    temp[i] = '\0';

    /* Reverse string to get correct order (msb first) */
    for (j = 0; j < i; j++)
        output[j] = temp[i - 1 - j];

    output[4] = '\0'; /* Ensure null terminate */
}

/**
 * @brief Convert base-4 format back to decimal.
 *
 * @param base4_str Base-4 string to convert (must be exactly 5 chars: a,b,c,d only).
 * @return Decimal value (-512 to +511) if valid format,
 *         -9999 if invalid format (outside valid range for error detection).
 */
int base4_to_decimal(const char *base4_str)
{
    int result, digit_value;
    int power = 1;
    int i;

    /* Check for invalid format - outside valid 10-bit range */
    if (!base4_str || strlen(base4_str) != 5)
        return -9999;

    /* Convert from right to left (least significant digit first) */
    for (i = 4; i >= 0; i--)
    {
        switch (base4_str[i])
        {
        case 'a':
            digit_value = 0;
            break;

        case 'b':
            digit_value = 1;
            break;

        case 'c':
            digit_value = 2;
            break;

        case 'd':
            digit_value = 3;
            break;

        /* Invalid char - outside valid range */
        default:
            return -9999;
        }

        result += digit_value * power;
        power *= 4;
    }

    /* Handle two's complement for negative values (10-bits) */
    if (result >= 512)
        result -= 1024; /* Convert from unsigned to signed representation */

    /* Return the answer */
    return result;
}

/**
 * @brief Validate that a decimal value is within 10 bit range.
 *
 * @param value Decimal value to check.
 * @return TRUE if value is in range [-512, +511], FALSE otherwise.
 */
BOOL is_valid_dec_value(int value)
{
    return (value >= -512 && value <= 511);
}

/**
 * @brief Validate base-4 string format.
 *
 * @param base4_str String to validate.
 * @return TRUE if exactly 5 chars using only a,b,c,d, FALSE otherwise.
 */
BOOL is_valid_base4_string(const char *base4_str)
{
    int i;

    if (!base4_str || strlen(base4_str) != 5)
        return FALSE;

    for (i = 0; i < 5; i++)
    {
        if (base4_str[i] != 'a' && base4_str[i] != 'b' &&
            base4_str[i] != 'c' && base4_str[i] != 'd')
        {
            return FALSE;
        }
    }

    return TRUE;
}