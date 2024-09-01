#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "pre_assembler.h"
#include "first_pass.h"
#include "second_pass.h"
#include "symbol_table.h"
#include "data_image.h"
#include "instructions.h"

#define MAX_FILENAME 256
#define INPUT_DIR "input_and_output"

void process_file(const char* filename);

int main(int argc, char *argv[]) {
    int i;
    char input_filename[MAX_FILENAME];
    const char *suffix = ".as";
    size_t len;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1> [file2] ...\n", argv[0]);
        return 1;
    }

    for (i = 1; i < argc; i++) {
        len = strlen(argv[i]);

        if (len > 3 && strcmp(argv[i] + len - 3, suffix) == 0) {
            /* If the filename already ends with .as */
            sprintf(input_filename, "%s/%s", INPUT_DIR, argv[i]);
        } else {
            /* If the filename does not end with .as, add the suffix */
            sprintf(input_filename, "%s/%s.as", INPUT_DIR, argv[i]);
        }
        process_file(input_filename);
    }

    return 0;
}

void process_file(const char* filename) {
    char *pre_assembled_filename;
    SymbolTable symbol_table;
    DataImage data_image, code_image;
    IncompleteInstructionTable incomplete_instructions;
    char output_dir[MAX_FILENAME];
    char base_filename[MAX_FILENAME];
    char *dot_position;
    char ob_filename[MAX_FILENAME], ent_filename[MAX_FILENAME], ext_filename[MAX_FILENAME];
    char output_ob_filename[MAX_FILENAME], output_ent_filename[MAX_FILENAME], output_ext_filename[MAX_FILENAME];

    printf("Processing file: %s\n", filename);

    /* Pre-assembly stage */
    pre_assembled_filename = pre_assemble(filename);
    if (!pre_assembled_filename) {
        fprintf(stderr, "Pre-assembly failed for file: %s\n", filename);
        return;
    }

    /* Initialize data structures */
    init_symbol_table(&symbol_table);
    init_data_image(&data_image);
    init_data_image(&code_image);
    init_incomplete_instruction_table(&incomplete_instructions);

    /* First pass */
    first_pass(pre_assembled_filename, &symbol_table, &data_image, &code_image);
    
    /* Second pass */
    second_pass(pre_assembled_filename, &symbol_table, &data_image, &code_image, &incomplete_instructions);

    /* Remove .am extension from pre-assembled filename to create output directory */
    strncpy(base_filename, pre_assembled_filename, sizeof(base_filename) - 1);
    base_filename[sizeof(base_filename) - 1] = '\0';
    dot_position = strrchr(base_filename, '.');
    if (dot_position && strcmp(dot_position, ".am") == 0) {
        *dot_position = '\0';
    }

    /* Create output directory */
    sprintf(output_dir, "%s/%s", INPUT_DIR, base_filename);
    mkdir(output_dir, 0777);

    /* Move output files to the output directory */
    sprintf(ob_filename, "%s.ob", base_filename);
    sprintf(ent_filename, "%s.ent", base_filename);
    sprintf(ext_filename, "%s.ext", base_filename);

    sprintf(output_ob_filename, "%s/%s.ob", output_dir, base_filename);
    sprintf(output_ent_filename, "%s/%s.ent", output_dir, base_filename);
    sprintf(output_ext_filename, "%s/%s.ext", output_dir, base_filename);

    rename(ob_filename, output_ob_filename);
    rename(ent_filename, output_ent_filename);
    rename(ext_filename, output_ext_filename);

    /* Clean up */
    free(pre_assembled_filename);

    printf("Processing completed for file: %s\n", filename);
}

