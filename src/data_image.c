/**
 * @file data_image.c
 * @brief implemmet basic data image functions.
 * Todo: make better doc.
 */
#include <stdio.h>
#include "../include/data_image.h"
#include "../include/constants.h"
#include "../include/error.h"
#include "../include/first_pass.h"

static int data_image[MAX_DATA_IMAGE_SIZE];

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

int get_data_at(int index)
{
    if (index >= 0 && index < DC)
        return data_image[index];
    return 0;
}

int get_data_size() return DC;
