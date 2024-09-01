#include "first_pass.h"
#include "symbol_table.h"
#include "data_image.h"
#include "instructions.h"
#include "directives.h"
#include "second_pass.h" 
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 80

void remove_empty_lines(const char *filename) {
    FILE *file = fopen(filename, "r");
    FILE *temp = fopen("temp.asm", "w");
    char line[MAX_LINE_LENGTH];

    if (!file || !temp) {
        fprintf(stderr, "Error: Unable to open file %s or temp file\n", filename);
        if (file) fclose(file);
        if (temp) fclose(temp);
        return;
    }

    printf("Removing empty lines from %s\n", filename);

    while (fgets(line, MAX_LINE_LENGTH, file)) {
        char *ptr = line;
        while (isspace(*ptr)) ptr++;
        if (*ptr != '\0') {
            fprintf(temp, "%s", line);
        }
    }

    fclose(file);
    fclose(temp);

    remove(filename);
    rename("temp.asm", filename);

    printf("Empty lines removed from %s\n", filename);
}

void first_pass(const char *filename, SymbolTable *symbol_table, DataImage *data_image, DataImage *code_image) {
    int ic = 100; /* Instruction Counter */
    int dc = 0;   /* Data Counter */
    FILE *file;
    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    int i;
    int in_macro = 0;


    printf("Starting first pass with ic = %d, dc = %d\n", ic, dc);

    /* Remove empty lines from the file */
    remove_empty_lines(filename);

    /* Print the content of the file after removing empty lines */
    printf("Content of %s after removing empty lines:\n", filename);
    file = fopen(filename, "r");
    if (file) {
        while (fgets(line, MAX_LINE_LENGTH, file)) {
            printf("%s", line);
        }
        fclose(file);
    }

    file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Unable to open file %s\n", filename);
        return;
    }
    printf("File opened successfully\n");

    while (fgets(line, MAX_LINE_LENGTH, file)) {
        char *ptr;
        char label[MAX_LABEL_LENGTH] = {0};
        char *colon;
        char opcode[MAX_LABEL_LENGTH];
        int instruction_size;

        line_number++;
        printf("Processing line %d: %s", line_number, line);

        /* Remove trailing whitespace */
        for (i = strlen(line) - 1; i >= 0 && isspace(line[i]); i--) {
            line[i] = '\0';
        }

        /* Skip leading whitespace and empty lines */
        ptr = line;
        while (isspace(*ptr)) ptr++;
        if (*ptr == '\0' || *ptr == ';') continue;

        /* Check for label */
        colon = strchr(ptr, ':');
        if (colon) {
            strncpy(label, ptr, colon - ptr);
            label[colon - ptr] = '\0';
            ptr = colon + 1;
            while (isspace(*ptr)) ptr++;
        }

        /* Process directives */
        if (*ptr == '.') {
            printf("Processing directive: %s\n", ptr);
            if (label[0] != '\0' && find_symbol(symbol_table, label) == NULL) {
                printf("Adding label: %s with address %d\n", label, dc);
                add_to_symbol_table(symbol_table, label, dc, false, true);
            }
            process_directive(ptr, symbol_table, data_image, &dc, label, line_number);
            printf("After directive: dc = %d\n", dc);
        } else {
            /* Check for macro */
            if (strncmp(ptr, "mcr", 3) == 0) {
                in_macro = 1;
                printf("Start of macro\n");
                if (label[0] != '\0' && find_symbol(symbol_table, label) == NULL) {
                    printf("Adding label for macro: %s with address %d\n", label, ic);
                    add_to_symbol_table(symbol_table, label, ic, false, false);
                }
                continue;
            } else if (strncmp(ptr, "endmcr", 6) == 0) {
                in_macro = 0;
                printf("End of macro\n");
                continue;
            }

            /* Process instructions if not in a macro */
            if (!in_macro) {
                sscanf(ptr, "%s", opcode);
                printf("Processing instruction: %s\n", opcode);

             if (is_valid_opcode(opcode)) {
             if (label[0] != '\0' && find_symbol(symbol_table, label) == NULL) {
                  printf("Adding label: %s with address %d\n", label, ic);
                  add_to_symbol_table(symbol_table, label, ic, false, false);
            }
             instruction_size = parse_instruction(ptr, symbol_table, code_image, &ic);
             printf("Instruction: %s, Size: %d\n", opcode, instruction_size);
             ic += instruction_size;
             printf("After instruction: ic = %d\n", ic);
             } else {
               fprintf(stderr, "Error in line %d: Invalid opcode '%s'\n", line_number, opcode);
            }
   }
        }
    }



    printf("Before final update: ic = %d, dc = %d\n", ic, dc);

    /* Update data symbols addresses */
    for (i = 0; i < symbol_table->count; i++) {
        if (symbol_table->symbols[i].is_data) {
            printf("Updating address of symbol %s from %d to %d\n",
                   symbol_table->symbols[i].name,
                   symbol_table->symbols[i].address,
 symbol_table->symbols[i].address + ic);
            symbol_table->symbols[i].address += ic;
        }
    }



    printf("Final symbol table:\n");
    for (i = 0; i < symbol_table->count; i++) {
        printf("Symbol: %s, Address: %d, Is Data: %d\n", 
               symbol_table->symbols[i].name, 
               symbol_table->symbols[i].address, 
               symbol_table->symbols[i].is_data);
    }



    fclose(file);
    printf("First pass completed\n");
}

