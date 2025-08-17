/**
 * @file preprocessor.c
 * @brief Handles macro preprocessing for the assembler project.
 *
 * This module reads a `.as` source file, processes macro definitions,
 * expands macro calls, and generates a corresponding `.am` file with
 * all macros expanded in-place.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/error.h"
#include "../include/preprocessor.h"
#include "../include/constants.h"
#include "../include/utils.h"

/**
 * @struct Macro
 * @brief Represents a macro and its body lines.
 */
typedef struct
{
    char name[MAX_MACRO_NAME_LENGTH]; /**< Macro name */
    char *lines[MAX_MACRO_LINES];     /**< Lines of macro body */
    int line_count;                   /**< Number of lines stored */
} Macro;

static Macro macro_table[MAX_MACROS]; /**< Global macro table */
static int macro_count = 0;           /**< Count of stored macros */

/**
 * @brief Extracts the first word from a given line.
 *
 * Skips leading whitespace and copies characters until the first space
 * or tab is encountered. Used to detect keywords or macro calls.
 *
 * @param line The input line to parse.
 * @param dest Destination buffer to store the extracted word.
 */
static void get_first_word(const char *line, char *dest)
{
    int i = 0, j = 0;

    while (line[i] && isspace((unsigned char)line[i]))
        i++;

    while (line[i] && !isspace((unsigned char)line[i]) && j < MAX_MACRO_NAME_LENGTH - 1)
        dest[j++] = line[i++];

    dest[j] = '\0';
}

/**
 * @brief Searches for a macro in the macro table.
 *
 * @param name The name of the macro to find.
 * @return Index of the macro in the table if found, -1 otherwise.
 */
static int find_macro(const char *name)
{
    int i;
    for (i = 0; i < macro_count; i++)
    {
        if (strcmp(macro_table[i].name, name) == 0)
            return i;
    }
    return -1;
}

/**
 * @brief Registers a new macro name in the macro table.
 *
 * @param name The name of the macro to add.
 * @return TRUE if successfully added, FALSE if already exists or table is full.
 */
static BOOL add_macro(const char *name)
{
    if (macro_count >= MAX_MACROS || find_macro(name) != -1)
        return FALSE;

    strcpy(macro_table[macro_count].name, name);
    macro_table[macro_count].line_count = 0;
    macro_count++;
    return TRUE;
}

/**
 * @brief Adds a line to a macro body in the table.
 *
 * Copies the line into dynamically allocated memory,
 * appending a newline if not present.
 *
 * @param name Name of the macro to add the line to.
 * @param line The line content.
 * @return TRUE on success, FALSE on failure or overflow.
 */
static BOOL add_line_to_macro(const char *name, const char *line)
{
    int i = find_macro(name);
    char *stored_line;
    size_t len; /* used size_t in order to be always positive */

    if (i == -1 || macro_table[i].line_count >= MAX_MACRO_LINES)
        return FALSE;

    len = strlen(line);
    stored_line = (char *)malloc(len + 2);
    if (!stored_line)
        return FALSE;

    strcpy(stored_line, line);

    if (len == 0 || stored_line[len - 1] != '\n')
    {
        stored_line[len] = '\n';
        stored_line[len + 1] = '\0';
    }
    else
    {
        stored_line[len] = '\0';
    }

    macro_table[i].lines[macro_table[i].line_count++] = stored_line;
    return TRUE;
}

/**
 * @brief Writes a macro's body to the output `.am` file.
 *
 * Strips leading whitespace from each line and ensures lines end with `\n`.
 *
 * @param am_file The output file pointer.
 * @param name The macro name to write.
 */
static void write_macro_body(FILE *am_file, const char *name)
{
    int i = find_macro(name), j;
    char *line;
    size_t len; /* used size_t in order to be always positive */

    if (i == -1)
        return;

    for (j = 0; j < macro_table[i].line_count; j++)
    {
        line = macro_table[i].lines[j];

        while (*line && isspace((unsigned char)*line))
            line++;

        fputs(line, am_file);

        len = strlen(line);
        if (len == 0 || line[len - 1] != '\n')
            fputc('\n', am_file);
    }
}

/**
 * @brief Frees all allocated memory used by macros.
 */
static void free_macro_table(void)
{
    int i, j;
    for (i = 0; i < macro_count; i++)
    {
        for (j = 0; j < macro_table[i].line_count; j++)
            free(macro_table[i].lines[j]);
    }
    macro_count = 0;
}

/**
 * @brief Main entry point for the preprocessor.
 *
 * Reads a `.as` file, expands any macro usages, and outputs the
 * result to a `.am` file in the same directory.
 *
 * @param filename Base name of the file (no extension).
 * @return One of the predefined ExitCode values indicating success or error.
 */
ExitCode preprocess(const char *filename)
{
    char as_filename[MAX_FILE_NAME_LENGTH];
    char am_filename[MAX_FILE_NAME_LENGTH];
    FILE *as_file, *am_file;
    char line[MAX_LINE_LENGTH];
    char first_word[MAX_MACRO_NAME_LENGTH];
    BOOL in_macro = FALSE;
    BOOL file_has_content = FALSE;
    char current_macro_name[MAX_MACRO_NAME_LENGTH];
    int line_num = 0;
    char temp_line[MAX_LINE_LENGTH];
    char macro_name_check[MAX_MACRO_NAME_LENGTH];
    char extra_text[MAX_LINE_LENGTH];

    sprintf(as_filename, "%s.as", filename);
    sprintf(am_filename, "%s.am", filename);

    as_file = fopen(as_filename, "r");
    if (!as_file)
    {
        report_error(EXIT_FILE_NOT_FOUND, filename);
        return EXIT_FILE_NOT_FOUND;
    }

    /* Check if the source file is empty */
    while (fgets(line, MAX_LINE_LENGTH, as_file))
    {
        if (strspn(line, " \t\r\n") == strlen(line))
            continue;

        strcpy(first_word, "");
        get_first_word(line, first_word);

        if (strlen(first_word) > 0 && first_word[0] != ';')
        {
            file_has_content = TRUE;
            break;
        }
    }

    if (!file_has_content)
    {
        printf("Warning: File %s is empty, no output files were created.\n", as_filename);
        fclose(as_file);
        return EXIT_FILE_EMPTY;
    }

    /* FIRST PASS: Validate macros only (don't create .am file yet) */
    rewind(as_file);
    line_num = 0;
    in_macro = FALSE;

    while (fgets(line, MAX_LINE_LENGTH, as_file))
    {
        line_num++;

        if (strspn(line, " \t\r\n") == strlen(line))
            continue;

        strcpy(first_word, "");
        get_first_word(line, first_word);

        /* Skip .entry and .extern directives */
        if (strcmp(first_word, ".entry") == 0 || strcmp(first_word, ".extern") == 0)
            continue;

        if (!in_macro && strcmp(first_word, "mcro") == 0)
        {
            if (sscanf(line, "mcro %31s", current_macro_name) != 1)
            {
                fclose(as_file);
                return EXIT_MACRO_SYNTAX_ERROR;
            }

            /* Check if macro name is a reserved word */
            if (is_reserved_word(current_macro_name))
            {
                print_line_error(as_filename, line_num, ERROR_MACRO_RESERVED_WORD);
                fclose(as_file);
                return EXIT_MACRO_RESERVED_WORD;
            }

            /* Check for extra text after macro name */
            strcpy(temp_line, line);
            remove_comments(temp_line);

            if (sscanf(temp_line, "mcro %31s %s", macro_name_check, extra_text) == 2)
            {
                print_line_error(as_filename, line_num, ERROR_MACRO_EXTRA_TEXT);
                fclose(as_file);
                return EXIT_MACRO_EXTRA_TEXT;
            }

            if (!add_macro(current_macro_name))
            {
                fclose(as_file);
                free_macro_table();
                return EXIT_MACRO_SYNTAX_ERROR;
            }

            in_macro = TRUE;
            continue;
        }

        if (in_macro)
        {
            if (strcmp(first_word, "mcroend") == 0)
            {
                in_macro = FALSE;
                continue;
            }

            if (!add_line_to_macro(current_macro_name, line))
            {
                fclose(as_file);
                free_macro_table();
                return EXIT_MACRO_SYNTAX_ERROR;
            }
            continue;
        }
    }

    /* Check if we're still in a macro at end of file */
    if (in_macro)
    {
        print_line_error(as_filename, line_num, ERROR_MACRO_MISSING_END);
        fclose(as_file);
        free_macro_table();
        return EXIT_MACRO_MISSING_END;
    }

    /* SECOND PASS: All macros are valid, now create .am file */
    rewind(as_file);

    am_file = fopen(am_filename, "w");
    if (!am_file)
    {
        fclose(as_file);
        report_error(EXIT_WRITE_ERROR, am_filename);
        return EXIT_WRITE_ERROR;
    }

    /* Reset for actual processing */
    in_macro = FALSE;

    while (fgets(line, MAX_LINE_LENGTH, as_file))
    {
        if (strspn(line, " \t\r\n") == strlen(line))
            continue;

        strcpy(first_word, "");
        get_first_word(line, first_word);

        /* Handle .entry and .extern directives */
        if (strcmp(first_word, ".entry") == 0 || strcmp(first_word, ".extern") == 0)
        {
            fputs(line, am_file);
            continue;
        }

        if (!in_macro && strcmp(first_word, "mcro") == 0)
        {
            sscanf(line, "mcro %31s", current_macro_name);
            in_macro = TRUE;
            continue;
        }

        if (in_macro)
        {
            if (strcmp(first_word, "mcroend") == 0)
            {
                in_macro = FALSE;
                continue;
            }
            continue;
        }

        if (find_macro(first_word) != -1)
        {
            write_macro_body(am_file, first_word);
        }
        else
        {
            char *ptr = line;
            while (*ptr && isspace((unsigned char)*ptr))
                ptr++;
            fputs(ptr, am_file);
        }
    }

    fclose(as_file);
    fclose(am_file);
    free_macro_table();
    return EXIT_SUCCESS_CODE;
}
