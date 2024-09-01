#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>

#define MAX_LABEL_LENGTH 31
#define MAX_LABELS 100

typedef struct {
    char name[MAX_LABEL_LENGTH];
    int address;
    bool is_extern;
    bool is_entry;
    bool is_data;
} Symbol;

typedef struct {
    Symbol symbols[MAX_LABELS];
    int count;
} SymbolTable;

void init_symbol_table(SymbolTable *table);
void add_symbol(SymbolTable *table, const char *name, int address, bool is_extern, bool is_entry, bool is_data);
Symbol* find_symbol(SymbolTable *table, const char *name);
void update_symbol(SymbolTable *table, const char *name, int address, bool is_extern, bool is_entry, bool is_data);
void add_to_symbol_table(SymbolTable *table, const char *name, int address, bool is_extern, bool is_entry);
void mark_as_entry(SymbolTable *symbol_table, const char *label);

#endif /* SYMBOL_TABLE_H */

