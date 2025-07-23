/**
 * @file data_image.h
 * @brief Header file for the data_image storage mechanism.
 */

#ifndef DATA_IMAGE_H
#define DATA_IMAGE_H

#include "../include/constants.h"

/**
 * @brief Stores a value in the data array if there is a room.
 *
 * @param value     The integer value to store.
 * @param filename  Pointer to the file name (for error reporting).
 * @param line_num  Current line number (for error reporting).
 * @return TRUE if stored successfully, False if overflow occured.
 */

BOOL store_data(int value, const char *filename, int line_num);

/**
 * @brief Retrieves a stored data value at a given index.
 *
 * @param index Index in the data image array.
 * @return The stored value, or 0 if index is out of bounds. //Todo: rething the 0 return.
 */
int get_data_at(int index);

/**
 * @brief Returns the current nu,ber of items in the data image (DC).
 *
 * @return Size of the data image (DC).
 */
int get_data_size();

#endif