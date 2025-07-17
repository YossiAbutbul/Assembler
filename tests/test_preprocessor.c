#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/preprocessor.h"
#include "../include/constants.h"

#define TEST_INPUT "tests/test1"
#define EXPECTED_OUTPUT_FILE "tests/expected_res/expected1.am"
#define GENERATED_OUTPUT_FILE "tests/test1.am"

int compare_files(const char *file1, const char *file2)
{
    FILE *f1 = fopen(file1, "r");
    FILE *f2 = fopen(file2, "r");
    char line1[100], line2[100];

    int result;

    if (!f1 || !f2)
    {
        printf("Error: Could not open one of the files.\n");
        return 0;
    }

    while (fgets(line1, sizeof(line1), f1) && fgets(line2, sizeof(line2), f2))
    {
        if (strcmp(line1, line2) != 0)
        {
            fclose(f1);
            fclose(f2);
            return 0;
        }
    }

    result = (feof(f1) && feof(f2));
    fclose(f1);
    fclose(f2);
    return result;
}

int main(void)
{
    printf("Running preprocessor unit test...\n");

    if (preprocess(TEST_INPUT) != EXIT_SUCCESS_CODE)
    {
        printf("Test failed: Preprocessor returned an error.\n");
        return 1;
    }

    if (!compare_files(EXPECTED_OUTPUT_FILE, GENERATED_OUTPUT_FILE))
    {
        printf("Test failed: Output .am file did not match expected output.\n");
        return 1;
    }

    printf("Test passed: Preprocessor output is correct.\n");
    return 0;
}
