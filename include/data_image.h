/**
 * @file data_image.h
 * @brief Header file for the data_image storage mechanism.
 */

#ifndef DATA_IMAGE_H
#define DATA_IMAGE_H

#include "../include/constants.h"

/**
 * @brief Intitalize the data image for a new assembly file.
 *
 * This function setsup the data image with initial capacity and
 * prepares it for storing data values during the first pass.
 */
void init_data_image(void);

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
 */
BOOL store_data(int value, const char *filename, int line_num);

/**
 * @brief Retrieves a stored data value at a given index.
 *
 * This function provides safe access to data values stored in the data image.
 * It preformes bounds checking to ensure the index is within valid range.
 *
 * @param index Index in the data image array.
 * @return The stored value, or 0 if index is out of bounds. //Todo: re-think the 0 return.
 */
int get_data_at(int index);

/**
 * @brief Returns the current number of items in the data image.
 *
 * This function provides access to the current data counter (DC) value
 * which represents the number of data words stored on the data image.
 *
 * @return Size of the data image (DC).
 */
int get_data_size(void);

/**
 * @brief Get a pointer to the entire data array for output generation.
 *
 * This function provides access to the internal data array for
 * genrating output files.
 *
 * @return Pointer to the data array, or NULL if not initialized.
 */
const int *get_data_array(void);

/**
 * @brief Free all memory allocated for the data image.
 *
 * This function releases all memory used by the data image.
 * Used when processing is complete to prevent memory leaks.
 */
void free_data_image(void);

#endif