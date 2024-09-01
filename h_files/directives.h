#ifndef DIRECTIVES_H
#define DIRECTIVES_H

#include "symbol_table.h"
#include "data_image.h"

bool is_valid_label(const char *label, SymbolTable *symbol_table, const char *reserved_words[], int reserved_words_count);
bool is_reserved_word(const char *word, const char *reserved_words[], int reserved_words_count);
void process_directive(char *line, SymbolTable *symbol_table, DataImage *data_image, int *dc, const char *label, int line_number);
void parse_data_directive(const char *line, DataImage *data_image, int *dc);
void parse_string_directive(const char *line, DataImage *data_image, int *dc);

#endif

