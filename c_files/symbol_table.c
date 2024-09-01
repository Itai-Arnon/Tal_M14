#include "symbol_table.h"
#include <stdio.h>
#include <string.h>

void init_symbol_table(SymbolTable *table) {
    table->count = 0;
}

void add_symbol(SymbolTable *table, const char *name, int address, bool is_extern, bool is_entry, bool is_data) {
    if (table->count < MAX_LABELS) {
        strncpy(table->symbols[table->count].name, name, MAX_LABEL_LENGTH);
        table->symbols[table->count].address = address;
        table->symbols[table->count].is_extern = is_extern;
        table->symbols[table->count].is_entry = is_entry;
        table->symbols[table->count].is_data = is_data;
        table->count++;
    }
}

Symbol* find_symbol(SymbolTable *table, const char *name) {
    int i;
    for (i = 0; i < table->count; i++) {
        if (strncmp(table->symbols[i].name, name, MAX_LABEL_LENGTH) == 0) {
            printf("Debug [find_symbol]: Found symbol %s with address %d\n", name, table->symbols[i].address);
            return &table->symbols[i];
        }
    }
    printf("Debug [find_symbol]: Symbol %s not found in table\n", name);
    return NULL;
}


void update_symbol(SymbolTable *table, const char *name, int address, bool is_extern, bool is_entry, bool is_data) {
    Symbol *symbol = find_symbol(table, name);
    if (symbol) {
        symbol->address = address;
        symbol->is_extern = is_extern;
        symbol->is_entry = is_entry;
        symbol->is_data = is_data;
    }
}

void add_to_symbol_table(SymbolTable *table, const char *name, int address, bool is_extern, bool is_data) {
    add_symbol(table, name, address, is_extern, false, is_data);
}

void mark_as_entry(SymbolTable *symbol_table, const char *label) {
    Symbol *symbol = find_symbol(symbol_table, label);
    if (symbol) {
        symbol->is_entry = true;
    }
}



void print_symbol_table(const SymbolTable *table) {
    int i;
    printf("Symbol Table:\n");
    for (i = 0; i < table->count; i++) {
        printf("Label: %s, Address: %d, Is External: %d, Is Entry: %d, Is Data: %d\n",
               table->symbols[i].name,
               table->symbols[i].address,
               table->symbols[i].is_extern,
               table->symbols[i].is_entry,
               table->symbols[i].is_data);
    }
    printf("\n");
}


