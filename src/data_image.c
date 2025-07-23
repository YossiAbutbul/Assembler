/**
 * @file data_image.c
 * @brief
 */
#include <stdio.h>
#include "../include/data_image.h"
#include "../include/constants.h"
#include "../include/error.h"
#include "../include/first_pass.h"

static int data_image[MAX_DATA_IMAGE_SIZE];

BOOL store_data(int value, const char *filename, int line_num)
{
    if (DC >= MAX_DATA_IMAGE_SIZE)
    {
        print_line_error(filename, line_num, ERROR_DATA_IMAGE_OVERFLOW);
        error
    }
}