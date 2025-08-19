/**
 * @file data_image.c
 * @brief Implementation of basic data image functions.
 *
 * The data image stores all data values from .data, .string, and .mat directives.
 * It uses dynamic memory allocation to efficiently manage varying amounts of data.
 */

#include <stdio.h>
#include <stdlib.h>

#include "../include/data_image.h"
#include "../include/constants.h"
#include "../include/error.h"

/* External variable from first_pass.c */
extern BOOL err_found;

/**
 * @brief Dynamic data image structure to store data values efficiently.
 */
typedef struct
{
    int *data;    /* Dynamic array of data values */
    int size;     /* Current number of data elements */
    int capacity; /* Maximum capacity before reallocation needed */
} DataImage;

/* Global data image instance */
static DataImage data_image = {NULL, 0, 0};

/* Inital capacity for data image */
#define INITIAL_DATA_CAPACITY 100
#define DATA_GROWTH_FACTOR 2

/**
 * @brief Initalize the data image for new assembly file.
 *
 * This function sets up the data image with initial capacity and
 * prepares it for storing data values during the first pass.
 */
void init_data_image(void)
{
    /* Free all existing data*/
    if (data_image.data != NULL)
        free(data_image.data);

    /* Allocates initial capacity */
    data_image.data = (int *)malloc(MAX_DATA_IMAGE_SIZE * sizeof(int));
    if (data_image.data == NULL)
    {
        fprintf(stdout, "Error: Failed to initialize data image\n");
        exit(EXIT_GENERAL_ERROR);
    }

    /* Reset counters */
    data_image.size = 0;
    data_image.capacity = MAX_DATA_IMAGE_SIZE;
}

/**
 * @brief Stores a value in the data image.
 *
 * This function adds an integer value to the data image.
 * It integrates with the DC counter and provides comprehensive
 * error checking.
 *
 * @param value     The integer value to store.
 * @param filename  Pointer to the file name (for error reporting).
 * @param line_num  Current line number (for error reporting).
 * @return TRUE if stored successfully, False if error occured.
 * @note DC will be incremented by calling this function.
 */
BOOL store_data(int value, const char *filename, int line_num)
{
    /* Check if filename exist */
    if (!filename)
    {
        err_found = TRUE;
        return FALSE;
    }

    /* Initialize data image if not already done */
    if (data_image.data == NULL)
        init_data_image();

    /* Check if we are exceeding the memory limit */
    /* Note: this is not suppose to happen - the exact limit depends on th ICF */
    if (data_image.size >= MAX_DATA_IMAGE_SIZE)
    {
        print_line_error(filename, line_num, ERROR_DATA_IMAGE_OVERFLOW);
        err_found = TRUE;
        return FALSE;
    }

    /* Check if we're exceeding the initial capacity */
    if (data_image.size >= data_image.capacity)
    {
        print_line_error(filename, line_num, ERROR_DATA_IMAGE_OVERFLOW);
        err_found = TRUE;
        return FALSE;
    }

    /* Store the value at the cureent DC position */
    data_image.data[data_image.size] = value;
    data_image.size++;
    return TRUE;
}

/**
 * @brief Returns the current number of items in the data image.
 *
 * This function provides access to the current data counter (DC) value
 * which represents the number of data words stored on the data image.
 *
 * @return Size of the data image (number of stored data).
 */
int get_data_size(void)
{
    return data_image.size;
}

/**
 * @brief Get a pointer to the entire data array for output generation.
 *
 * This function provides access to the internal data array for
 * genrating output files.
 *
 * @return Pointer to the data array, or NULL if not initialized.
 */
const int *get_data_array(void)
{
    return data_image.data;
}

/**
 * @brief Free all memory allocated for the data image.
 *
 * This function releases all memory used by the data image.
 * Used when processing is complete to prevent memory leaks.
 */
void free_data_image(void)
{
    if (data_image.data != NULL)
    {
        free(data_image.data);
        data_image.data = NULL;
    }

    /* Reset Counters */
    data_image.size = 0;
    data_image.capacity = 0;
}