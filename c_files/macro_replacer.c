#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pre_assembler.h"



void print_macros(const char *filename) {
    FILE *file;
    char line[MAX_LINE_LENGTH];
    int in_macro = 0;
    char macro_name[MAX_LINE_LENGTH];

    file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Unable to open file %s\n", filename);
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "macr", 4) == 0) {
            sscanf(line + 4, "%s", macro_name);
            printf("Macro name: %s\n", macro_name);
            in_macro = 1;
        } else if (strncmp(line, "endmacr", 7) == 0) {
            in_macro = 0;
            printf("\n");  /* Print an extra newline after each macro */
        } else if (in_macro) {
            printf("%s", line);
        }
    }

    fclose(file);
}

int main() {
    const char *input_filename = "input.asm";
    char *pre_assembled_filename;

    printf("Starting test_pre_assembler\n");
    printf("Pre-assembling the file\n");

    pre_assembled_filename = pre_assemble(input_filename);
    if (!pre_assembled_filename) {
        fprintf(stderr, "Pre-assembly failed\n");
        return 1;
    }

    printf("Pre-assembled file created: %s\n", pre_assembled_filename);
    printf("Macros found:\n");
    print_macros(input_filename);

    free(pre_assembled_filename);
    printf("Test finished\n");
    return 0;
}
