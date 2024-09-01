#include "pre_assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static void init_macro_table(MacroTable *table)
{
    table->macro_count = 0;
}

static Macro* find_macro(MacroTable *table, const char *name)
{
    int i;
    for (i = 0; i < table->macro_count; i++) {
        if (strcmp(table->macros[i].name, name) == 0) {
            return &table->macros[i];
        }
    }
    return NULL;
}

static void add_macro(MacroTable *table, const char *name)
{
    if (table->macro_count < MAX_MACROS) {
        strncpy(table->macros[table->macro_count].name, name, MAX_MACRO_NAME - 1);
        table->macros[table->macro_count].name[MAX_MACRO_NAME - 1] = '\0';
        table->macros[table->macro_count].line_count = 0;
        table->macro_count++;
    }
}

static void add_macro_line(Macro *macro, const char *line)
{
    if (macro->line_count < MAX_MACRO_LINES) {
        strncpy(macro->content[macro->line_count], line, MAX_LINE_LENGTH - 1);
        macro->content[macro->line_count][MAX_LINE_LENGTH - 1] = '\0';
        macro->line_count++;
    }
}

char* pre_assemble(const char *input_filename)
{
    FILE *input_file, *output_file;
    char line[MAX_LINE_LENGTH];
    char *output_filename;
    MacroTable macro_table;
    Macro *current_macro = NULL;
    char *macro_name;
    char first_word[MAX_MACRO_NAME];
    int in_macro_definition = 0;
    char *trimmed_line;
    Macro *macro;
    int i;

    init_macro_table(&macro_table);

    input_file = fopen(input_filename, "r");
    if (!input_file) {
        fprintf(stderr, "Error: Unable to open input file %s\n", input_filename);
        return NULL;
    }

    output_filename = malloc(strlen(input_filename) + 4);
    if (!output_filename) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(input_file);
        return NULL;
    }
    strcpy(output_filename, input_filename);
    strcat(output_filename, ".am");

    output_file = fopen(output_filename, "w");
    if (!output_file) {
        fprintf(stderr, "Error: Unable to create output file %s\n", output_filename);
        fclose(input_file);
        free(output_filename);
        return NULL;
    }

    while (fgets(line, sizeof(line), input_file)) {
        trimmed_line = line;

        /* Remove leading whitespace */
        while (isspace((unsigned char)*trimmed_line)) trimmed_line++;

        /* Skip empty lines completely */
        if (*trimmed_line == '\0' || *trimmed_line == '\n') {
            continue;
        }

        if (sscanf(trimmed_line, "%s", first_word) == 1) {
            if (strcmp(first_word, "macr") == 0) {
                macro_name = strtok(trimmed_line + 4, " \t\n");
                if (macro_name) {
                    add_macro(&macro_table, macro_name);
                    current_macro = &macro_table.macros[macro_table.macro_count - 1];
                    in_macro_definition = 1;
                }
            } else if (strcmp(first_word, "endmacr") == 0) {
                current_macro = NULL;
                in_macro_definition = 0;
            } else if (in_macro_definition) {
                add_macro_line(current_macro, trimmed_line);
            } else {
                macro = find_macro(&macro_table, first_word);
                if (macro) {
for (i = 0; i < macro->line_count; i++) {
    fputs(macro->content[i], output_file);
    fputc('\n', output_file);  /* זה יכול להוסיף שורה ריקה מיותרת*/
}
                } else {
                   if (strlen(trimmed_line) > 0) {
    fputs(line, output_file);
}
                    if (strchr(trimmed_line, '\n') == NULL) {
                        fputc('\n', output_file);
                    }
                }
            }
        } else if (!in_macro_definition) {
            fputs(trimmed_line, output_file);
            if (strchr(trimmed_line, '\n') == NULL) {
                fputc('\n', output_file);
            }
        }
    }

    fclose(input_file);
    fclose(output_file);

    return output_filename;
}
