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
void init_data_image()
{
    /* Free all existing data*/
    if (data_image.data != NULL)
        free(data_image.data);

    /* Allocates initial capacity */
    data_image.data = (int *)malloc(INITIAL_DATA_CAPACITY * sizeof(int));
    if (data_image.data == NULL)
    {
        /*toDo: update error handling*/
        fprint(stderr, "Error: Failed to initialize data image\n");
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
 *
 * @param filename Pointer to filename
 */

BOOL store_data(int value, const char *filename, int line_num)
{
    /* Check for data overflow */
    if (DC >= MAX_DATA_IMAGE_SIZE)
    {
        print_line_error(filename, line_num, ERROR_DATA_IMAGE_OVERFLOW);
        err_found = TRUE;
        return FALSE;
    }

    /* Insert value into the data image */
    data_image[DC++] = value;
    return TRUE;
}
