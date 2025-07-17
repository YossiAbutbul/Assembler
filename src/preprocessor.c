/**
 * @file preprocessor.c
 *
 * Implements the preprocessor logic:
 * 1. Reads the source file with .as extension.
 * 2. Expands macros defined in the source file.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "preprocessor.h"
#include "constants.h"

/* Defines Macro struct */
typedef struct
{
    char name[MAX_MACRO_NAME_LENGTH];
    char *lines[MAX_MACRO_LINES];
    char line_count;
} Macro;

static Macro macro_table[MAX_MACROS];
static int macro_count = 0;

/* Utils functions */

/**
 * @brief Utility: Trim leading and trailing whitespace from a string.
 * @param line The line to be trimmed.
 *
 * This function modifies the input string in place.
 * It removes leading and trailing whitespace characters (spaces, tabs, etc.)
 */
static void trim_whitespace(char *line)
{
    char *end;
    while (isspace((unsigned char)*line))
        line++;
    if (*line == 0) /* line with all spaces */
        return;

    end = line + strlen(line) - 1;
    while (end > line && isspace((unsigned char)*end))
        end--;
    *(end + 1) = '\0'; /* null terminate the string */
}

/**
 * @brief Utility: Get the first word from a line.
 * Extracts the first word from a line and stores it in the destination string.
 * This function assumes that dest has enough space to hold the first word.
 * It will null-terminate the string after extarcting the first word of a line.
 * @param line The line from which to extract the first word.
 * @param dest The destination string where the first word will be stored.
 *
 */
static void get_first_word(const char *line, char *dest)
{
    int i = 0;
    while (line[i] && !isspace((unsigned char)line[i]) && i < MAX_MACRO_NAME_LENGTH - 1)
    {
        dest[i] = line[i];
        i++;
    }
    dest[i] = '\0'; /* Null-terminate the string */
}

/**
 * @brief Utility: Find macro name in macro_table.
 * Searches for a macro by its name in the macro table.
 * @param name The name of the macro to search for.
 * @return The index of the macro in the macro table if found, otherwise -1 if not found.
 */
static int find_macro(const char *name)
{
    int i;
    for (i = 0; i < macro_count; i++)
    {
        if (strcmp(macro_table[i].name, name) == 0)
            return i; /* Return the index of the macro if found */
    }
    return -1; /* Return -1 if not found */
}

/**
 * @brief Utility: Add a new macro to the macro table.
 * Adds a new macro with the given name to the macro table.
 * If the macro already exists or the table is full, it returns FALSE.
 * @param name The name of the macro to be added.
 * @return TRUE if the macro was added successfully,
 *         FALSE otherwise.
 */
static BOOL add_macro(const char *name)
{
    if (macro_count >= MAX_MACROS || find_macro(name) != -1)
        return FALSE; /* Macro table is full or macro already exists */

    strcpy(macro_table[macro_count].name, name);
    macro_table[macro_count].line_count = 0; /* Initialize line count to 0 */

    ++macro_count; /* Increment macro count */
    return TRUE;   /* Successfully added the macro */
}

/**
 * @brief Utility: Add a line to macro body.
 * Adds a line to the body of an existing macro.
 * @param name The name of the macro to which the line will be added.
 * @param line The line to be added to the macro.
 * @return TRUE if the line was added successfully,
 *         FALSE if the macro does not exist or the line limit is reached.
 */
static BOOL add_line_to_macro(const char *name, const char *line)
{
    int i = find_macro(name);
    char *stored_line;

    if (i == -1 || macro_table[i].line_count >= MAX_MACRO_LINES)
        return FALSE; /* Macro not found or line limit reached */

    stored_line = (char *)malloc(strlen(line) + 1);
    if (!stored_line)
        return FALSE; /* Memory allocation failed */

    strcpy(stored_line, line);
    macro_table[i].lines[macro_table[i].line_count++] = stored_line;
    return TRUE; /* Successfully added the line to the macro */
}

static void write_macro_body(FILE *am_file, const char *name)
{
    int i = find_macro(name), j;
    if (i == -1)
        return; /* Macro not found */

    for (int j = 0; j < macro_table[i].line_count; ++j)
    {
        fputs(macro_table[i].lines[j], am_file);
    }
}

/**
 * Runs the preprocessor on the given source file (with .as extension).
 *
 * Reads 'filename.as', expands macros, and writes the result to 'filename.am'.
 *
 * @param filename The base name of the source file to be preprocessed (without .as extension).
 * @return EXIT_SUCCESS_CODE if successful,
 *         or a relevent error code if an error occurs.
 */
ExitCode preprocessor(const char *filename)
{
    /* array to store files names string */
    char as_filename[MAX_FILE_NAME_LENGTH];
    char am_filename[MAX_FILE_NAME_LENGTH];

    FILE *as_file, *am_file;

    char line[MAX_LINE_LENGTH];
    char first_word[MAX_MACRO_NAME_LENGTH];

    BOOL in_macro = FALSE; /* Flag to indicate if we are inside a macro definition */

    char current_macro_name[MAX_MACRO_NAME_LENGTH];

    /* Construct the file names */
    sprintf(as_filename, "%s.as", filename);
    sprintf(am_filename, "%s.am", filename);

    /* Open the source file for reading the .as file */
    as_file = fopen(as_filename, "r");
    if (!as_file)
        return EXIT_FILE_NOT_FOUND;

    /* Open the output file for writing the .am file */
    am_file = fopen(am_filename, "w");
    if (!am_file)
    {
        fclose(as_file);
        return EXIT_WRITE_ERROR;
    }

    while (fgets(line, MAX_LINE_LENGTH, as_file))
    {
        strcpy(first_word, ""); /* Reset first word*/

        get_first_word(line, first_word);
        trim_whitespace(line); /* Trim whitespace from the line */

        if (!in_macro)
        {
            if (strcmp(first_word, "mcro") == 0)
            {
                /* Begin macro */
                sscanf(line, "mcro %31s", current_macro_name);
                if (!add_macro(current_macro_name))
                {
                    fclose(as_file);
                    fclose(am_file);
                    return EXIT_MACRO_SYNTAX_ERROR; /* Macro syntax error */
                }
                in_macro = TRUE; /* Set the flag to indicate we are inside a macro */
                continue;        /* Skip to the next line */
            }

            if (find_macro(first_word) != -1)
            {
                /* If the first word is a macro, expand it */
                write_macro_body(am_file, first_word);
                continue; /* Skip to the next line */
            }

            fputs(line, am_file); /* Write the line to the output file */
        }
        else
        {
            if (strcmp(first_word, "mcroend") == 0)
            {
                /* End macro */
                in_macro = FALSE; /* Reset the flag */
                continue;         /* Skip to the next line */
            }

            if (!add_line_to_macro(current_macro_name, line))
            {
                fclose(as_file);
                fclose(am_file);
                return EXIT_MACRO_SYNTAX_ERROR; /* Error adding line to macro */
            }
        }
    }

    /* Close open files */
    fclose(as_file);
    fclose(am_file);

    return EXIT_SUCCESS_CODE;
}
