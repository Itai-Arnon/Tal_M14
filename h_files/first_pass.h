#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "symbol_table.h"
#include "data_image.h"
#include "directives.h"
#include "instructions.h"  /* לוודא שכל הפונקציות כלולות */

#define MAX_LINE_LENGTH 80

void first_pass(const char *filename, SymbolTable *symbol_table, DataImage *data_image, DataImage *code_image);
void mark_as_entry(SymbolTable *symbol_table, const char *label);
int parse_instruction(const char *line, SymbolTable *symbol_table, DataImage *code_image, int *ic);
void remove_empty_lines(const char *filename);  /* הוספת ההצהרה לפונקציה*/

#endif /* FIRST_PASS_H */

