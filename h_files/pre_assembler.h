#ifndef PRE_ASSEMBLER_H
#define PRE_ASSEMBLER_H

#include "symbol_table.h"

#define MAX_MACRO_NAME 31
#define MAX_MACRO_LINES 100
#define MAX_LINE_LENGTH 80
#define MAX_MACROS 100


typedef struct {
    char name[MAX_MACRO_NAME];
    char content[MAX_MACRO_LINES][MAX_LINE_LENGTH];
    int line_count;
} Macro;

typedef struct {
    Macro macros[MAX_MACROS];
    int macro_count;
} MacroTable;

char* pre_assemble(const char *input_filename);

#endif

