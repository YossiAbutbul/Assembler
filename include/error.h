/* error.h */

#ifndef ERROR_H
#define ERROR_H

#include "constants.h"

/**
 * Prints an error message based on the exit code and the filename.
 *
 * @param exit_code The exit code indicating the type of error.
 * @param filename The filename associated with the error (without .as extension).
 */

void report_error(ExitCode exit_code, const char *filename);

#endif