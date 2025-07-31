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
#include "../include/first_pass.h"

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

/***
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
    data_image.data = (int *)malloc(INITIAL_DATA_CAPACITY * sizeof(int));
    if (data_image.data == NULL)
    {
        /*toDo: update error handling*/
        fprintf(stderr, "Error: Failed to initialize data image\n");
        exit(EXIT_GENERAL_ERROR);
    }

    /* Reset counters */
    data_image.size = 0;
    data_image.capacity = INITIAL_DATA_CAPACITY;
}

/**
 * @brief Expand the data image capacity when needed.
 *
 * This function doubles the capacity of the data image
 * when the current capacity is exceeded.
 * todo: rethink this function
 *
 * @param filename Pointer to filename The current line number (for error reporting).
 * @param line_num The current line number (for error reporting).
 * @return TRUE if expansion successful, FALSE otherwise.
 * @note This is an internal function.
 */
static BOOL expand_data_image(const char *filename, int line_num)
{
    int new_capacity;
    int *new_data;
    int i;

    /* Calculate new capacity */
    new_capacity = data_image.capacity * DATA_GROWTH_FACTOR;

    /* Check if new capacity exceeds maximum allowed */
    if (new_capacity > MAX_DATA_IMAGE_SIZE)
    {
        new_capacity = MAX_DATA_IMAGE_SIZE;
        if (data_image.size >= new_capacity)
        {
            print_line_error(filename, line_num, ERROR_DATA_IMAGE_OVERFLOW);
            err_found = TRUE;
            return FALSE;
        }
    }

    /* Allocate new array */
    new_data = (int *)malloc(new_capacity * sizeof(int));
    if (new_data == NULL)
    {
        print_line_error(filename, line_num, ERROR_MEMORY_ALLOCATION_FAILED);
        err_found = TRUE;
        return FALSE;
    }

    /* Copy existing data */
    for (i = 0; i < data_image.size; i++)
        new_data[i] = data_image.data[i];

    /* Replace old array */
    free(data_image.data);
    data_image.data = new_data;
    data_image.capacity = new_capacity;

    return TRUE;
}

/**
 * @brief Stores a value in the data image.
 *
 * This function adds an integer value to the data image, expanding
 * the capacity if necessary. It integrates properly with the DC counter
 * and provides comprehensive error checking.
 *
 * @param value     The integer value to store.
 * @param filename  Pointer to the file name (for error reporting).
 * @param line_num  Current line number (for error reporting).
 * @return TRUE if stored successfully, False if error occured.
 * @note DC will be incremented by calling this function.
 */
BOOL store_data(int value, const char *filename, int line_num)
{
    /* Initialize data image if not already done */
    if (data_image.data == NULL)
        init_data_image(void);

    /* Check if expanding the capacity is needed */
    if (data_image.size >= data_image.capacity)
    {
        if (!expand_data_image(filename, line_num))
            return FALSE;
    }

    /* Store the value at the cureent DC position */
    data_image.data[data_image.size] = value;
    data_image.size++;
    return TRUE;
}

/**
 * @brief Retrieves a stored data value at a given index.
 *
 * This function provides safe access to data values stored in the data image.
 * It preformes bounds checking to ensure the index is within valid range.
 *
 * @param index Index in the data image array.
 * @return The stored value, or 0 if index is out of bounds. //Todo: re-think the 0 return.
 */
int get_data_at(int index)
{
    /* Check bounds before accessing the data image array */
    if (index >= 0 && index < data_image.size && data_image.data != NULL)
        return data_image.data[index];

    /* Return 0 (for now) for out of bounds access or uninitialized data */
    /* ToDo: re-think the rtuening of '0' as value */
    return 0;
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