#include "directives.h"
#include <stdio.h>

void test_parse_data_directive() {
    DataImage data_image = {{0}, 0};
    parse_data_directive(".data 5, -10, 15", &data_image, NULL);
    for (int i = 0; i < data_image.data_count; i++) {
        printf("%d ", data_image.data[i]);
    }
    printf("\n");
}

void test_parse_string_directive() {
    DataImage data_image = {{0}, 0};
    parse_string_directive(".string \"hello\"", &data_image, NULL);
    for (int i = 0; i < data_image.data_count; i++) {
        printf("%c", data_image.data[i]);
    }
    printf("\n");
}

void test_parse_entry_directive() {
    SymbolTable symbol_table = {{{0}}, 0};
    parse_entry_directive(".entry LABEL", &symbol_table);
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
    parse_extern_directive(".extern EXTERNAL_LABEL", &symbol_table);
    for (int i = 0; i < symbol_table.label_count; i++) {
        printf("Label: %s, Address: %d, is_extern: %d, is_entry: %d\n",
            symbol_table.labels[i].name,
            symbol_table.labels[i].address,
            symbol_table.labels[i].is_extern,
            symbol_table.labels[i].is_entry);
    }
}

int main() {
    printf("Testing .data directive:\n");
    test_parse_data_directive();
    
    printf("Testing .string directive:\n");
    test_parse_string_directive();
    
    printf("Testing .entry directive:\n");
    test_parse_entry_directive();
    
    printf("Testing .extern directive:\n");
    test_parse_extern_directive();
    
    return 0;
}

