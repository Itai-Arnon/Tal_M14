#include "directives.h"
#include "instructions.h"
#include <stdio.h>

void test_parse_data_directive() {
    DataImage data_image = {0};
    parse_data_directive(".data 7, -57, 17, 9", &data_image, "XYZ");

    printf("Test parse_data_directive:\n");
    printf("Label: XYZ\n");
    for (int i = 0; i < data_image.data_count; i++) {
        printf("Data[%d]: %d\n", i, data_image.data[i]);
    }
}

void test_parse_string_directive() {
    DataImage data_image = {0};
    parse_string_directive(".string \"abcdef\"", &data_image, "STR");

    printf("Test parse_string_directive:\n");
    printf("Label: STR\n");
    for (int i = 0; i < data_image.data_count; i++) {
        printf("Data[%d]: %d\n", i, data_image.data[i]);
    }
}

void test_parse_entry_directive() {
    SymbolTable symbol_table = {{{0}}, 0};
    parse_entry_directive(".entry HELLO", &symbol_table);

    printf("Test parse_entry_directive:\n");
    for (int i = 0; i < symbol_table.label_count; i++) {
        printf("Label: %s, Address: %d, is_extern: %d, is_entry: %d\n", 
               symbol_table.labels[i].name, 
               symbol_table.labels[i].address,
               symbol_table.labels[i].is_extern,
               symbol_table.labels[i].is_entry);
    }
}

void test_parse_extern_directive() {
    SymbolTable symbol_table = {{{0}}, 0};
    parse_extern_directive(".extern WORLD", &symbol_table);

    printf("Test parse_extern_directive:\n");
    for (int i = 0; i < symbol_table.label_count; i++) {
        printf("Label: %s, Address: %d, is_extern: %d, is_entry: %d\n", 
               symbol_table.labels[i].name, 
               symbol_table.labels[i].address,
               symbol_table.labels[i].is_extern,
               symbol_table.labels[i].is_entry);
    }
}

void test_parse_instruction() {
    SymbolTable symbol_table = {{{0}}, 0};
    DataImage code_image = {0};
    int ic = 0;
    parse_instruction("MAIN: add r3, LIST", &symbol_table, &code_image, &ic);

    printf("Test parse_instruction:\n");
    for (int i = 0; i < symbol_table.label_count; i++) {
        printf("Label: %s, Address: %d, is_extern: %d, is_entry: %d\n", 
               symbol_table.labels[i].name, 
               symbol_table.labels[i].address,
               symbol_table.labels[i].is_extern,
               symbol_table.labels[i].is_entry);
    }
}

int main() {
    test_parse_data_directive();
    test_parse_string_directive();
    test_parse_entry_directive();
    test_parse_extern_directive();
    test_parse_instruction();
    return 0;
}
