#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pre_assembler.h"
#include "first_pass.h"
#include "symbol_table.h"
#include "data_image.h"

void print_symbol_table(const SymbolTable *table);  /* הצהרה בלבד*/

/*void print_data_image(DataImage *data_image, const char *name) {
    int i;
    printf("%s:\n", name);
    for (i = 0; i < data_image->size; i++) {
        printf("%04d: %015d\n", i, data_image->data[i]);
    }
    printf("\n");
}*/

int main() {
    SymbolTable symbol_table;
    DataImage data_image;
    DataImage code_image;
    const char *input_filename = "input.asm";
    char *pre_assembled_filename;
    FILE *pre_assembled_file;
    char line[256];

    printf("Starting test_first_pass\n\n");

    /* Initialize data structures */
    init_symbol_table(&symbol_table);
    init_data_image(&data_image);
    init_data_image(&code_image);

    /* Pre-assemble the file */
    printf("Pre-assembling the file\n");
    pre_assembled_filename = pre_assemble(input_filename);
    if (!pre_assembled_filename) {
        fprintf(stderr, "Pre-assembly failed\n");
        return 1;
    }

    printf("Pre-assembled file created: %s\n", pre_assembled_filename);

    /* Print the contents of the pre-assembled file */
    pre_assembled_file = fopen(pre_assembled_filename, "r");
    if (!pre_assembled_file) {
        fprintf(stderr, "Error: Unable to open file %s\n", pre_assembled_filename);
        free(pre_assembled_filename);
        return 1;
    }

    printf("Contents of the pre-assembled file:\n");
    while (fgets(line, sizeof(line), pre_assembled_file)) {
        printf("%s", line);
    }
    fclose(pre_assembled_file);

    /* Run the first pass */
    printf("Running first pass\n");
    first_pass(pre_assembled_filename, &symbol_table, &data_image, &code_image);

    /* Print results */
    print_symbol_table(&symbol_table);
    print_data_image(&data_image, "Data Image");
    print_data_image(&code_image, "Code Image");

    /* Clean up */
    free(pre_assembled_filename);

    printf("Test finished\n");
    return 0;
}

