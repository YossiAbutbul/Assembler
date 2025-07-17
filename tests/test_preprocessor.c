#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/preprocessor.h"
#include "../include/constants.h"

#define TEST_INPUT "tests/test1"
#define EXPECTED_OUTPUT_FILE "tests/expected_res/expected1.am"
#define GENERATED_OUTPUT_FILE "tests/test1.am"

#define LINE_BUF_SIZE 256

/* Trim leading and trailing whitespace */
int trim_newline(char *line)
{
    int len = strlen(line);
    while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
    {
        line[len - 1] = '\0';
        len--;
    }
    return len;
}

int compare_files(const char *file1, const char *file2)
{
    FILE *f1 = fopen(file1, "r");
    FILE *f2 = fopen(file2, "r");
    char line1[100], line2[100];
    int result = 1;

    if (!f1 || !f2)
    {
        printf("Error: Could not open one of the files.\n");
        if (f1) fclose(f1);
        if (f2) fclose(f2);
        return 0;
    }

    while (1)
    {
        char *l1 = fgets(line1, sizeof(line1), f1);
        char *l2 = fgets(line2, sizeof(line2), f2);

        if (l1 == NULL || l2 == NULL)
        {
            /* Break if both reached EOF */
            if (l1 == NULL && l2 == NULL)
                break;

            /* Otherwise, one has more lines */
            printf("Files have different number of lines.\n");
            result = 0;
            break;
        }

        trim_newline(line1);
        trim_newline(line2);

        if (strcmp(line1, line2) != 0)
        {
            printf("Mismatch:\nExpected: \"%s\"\nGot:      \"%s\"\n", line2, line1);
            result = 0;
            break;
        }
    }

    fclose(f1);
    fclose(f2);
    return result;
}


int main(void)
{
    printf("Running preprocessor unit test...\n");

    if (preprocess(TEST_INPUT) != EXIT_SUCCESS_CODE) {
        printf("Test failed: Preprocessor returned an error.\n");
        return 1;
    }

    if (!compare_files(EXPECTED_OUTPUT_FILE, GENERATED_OUTPUT_FILE)) {
        printf("Test failed: Output .am file did not match expected output.\n");
        return 1;
    }

    printf("Test passed: Preprocessor output is correct.\n");
    return 0;
}
