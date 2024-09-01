#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "second_pass.h"
#include "first_pass.h"
#include "symbol_table.h"
#include "data_image.h"


void test_second_pass(void);
void print_output_files(const char* filename);

/* הצהרות לפונקציות חיצוניות */#ifndef FIRST_PASS_FUNCTIONS
#define FIRST_PASS_FUNCTIONS
void first_pass(const char *filename, SymbolTable *symbol_table, DataImage *data_image, DataImage *code_image);
void print_symbol_table(const SymbolTable *table);
void print_data_image(DataImage *data_image, const char *name);
#endif

int main(void) {
    printf("Starting second pass tests:\n\n");
    
    test_second_pass();
    
    printf("\nPrinting contents of output files:\n");
    print_output_files("input");
    
    printf("\nSecond pass tests completed.\n");
    return 0;
}

void test_second_pass(void) {
    SymbolTable symbol_table;
    DataImage code_image, data_image;
    IncompleteInstructionTable incomplete_instructions;
    const char* input_filename = "input.asm.am";
    int result;

    printf("Testing second_pass:\n");
    
    /* Initialize data structures */
    init_symbol_table(&symbol_table);
    init_data_image(&code_image);
    init_data_image(&data_image);
    init_incomplete_instruction_table(&incomplete_instructions);
    
    /* Perform first pass */
    printf("Performing first pass...\n");
    first_pass(input_filename, &symbol_table, &data_image, &code_image);
    
    /* Print first pass results (for debugging) */
    printf("First pass completed. Symbol table and code image generated.\n");
    print_symbol_table(&symbol_table);
    print_data_image(&code_image, "Code Image after First Pass");
    
    /* Perform second pass */
    printf("Performing second pass...\n");
    result = second_pass(input_filename, &symbol_table, &data_image, &code_image, &incomplete_instructions);
    
    printf("Second pass result: %s\n", result ? "Success" : "Failure");
    
    /* Print final results (for debugging) */
    print_symbol_table(&symbol_table);
    print_data_image(&code_image, "Code Image after Second Pass");
}

void print_output_files(const char* filename) {
    char full_filename[256];
    FILE *file;
    int c;

    /* Print .ob file contents */
    sprintf(full_filename, "%s.asm.ob", filename);
    printf("\nContents of %s:\n", full_filename);
    file = fopen(full_filename, "r");
    if (file) {
        printf("Successfully opened %s\n", full_filename); /* Debugging print */
        while ((c = getc(file)) != EOF) {
            putchar(c);
        }
        fclose(file);
    } else {
        perror("Error opening .ob file"); /* Debugging print */
        printf("Unable to open %s\n", full_filename);
    }

    /* Print .ent file contents */
    sprintf(full_filename, "%s.asm.ent", filename);
    printf("\nContents of %s:\n", full_filename);
    file = fopen(full_filename, "r");
    if (file) {
        printf("Successfully opened %s\n", full_filename); /* Debugging print */
        while ((c = getc(file)) != EOF) {
            putchar(c);
        }
        fclose(file);
    } else {
        perror("Error opening .ent file"); /* Debugging print */
        printf("Unable to open %s\n", full_filename);
    }

    /* Print .ext file contents */
    sprintf(full_filename, "%s.asm.ext", filename);
    printf("\nContents of %s:\n", full_filename);
    file = fopen(full_filename, "r");
    if (file) {
        printf("Successfully opened %s\n", full_filename); /* Debugging print */
        while ((c = getc(file)) != EOF) {
            putchar(c);
        }
        fclose(file);
    } else {
        perror("Error opening .ext file"); /* Debugging print */
        printf("Unable to open %s\n", full_filename);
    }
}


