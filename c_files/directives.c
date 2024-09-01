#include "directives.h"
#include "symbol_table.h"
#include "data_image.h"
#include "first_pass.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const char *reserved_words[] = {
    "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", 
    "jmp", "bne", "red", "prn", "jsr", "rts", "stop", 
    ".data", ".string", ".entry", ".extern", 
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"
};

int reserved_words_count = sizeof(reserved_words) / sizeof(reserved_words[0]);

bool is_valid_label(const char *label, SymbolTable *symbol_table, const char *reserved_words[], int reserved_words_count) {
    size_t i;
    int j;

    if (!isalpha(label[0])) {
        return false;
    }
    if (strlen(label) > MAX_LABEL_LENGTH) {
        return false;
    }
    for (i = 1; i < strlen(label); i++) {
        if (!isalnum(label[i]) && label[i] != '_') {
            return false;
        }
    }
    if (is_reserved_word(label, reserved_words, reserved_words_count)) {
        return false;
    }
    for (j = 0; j < symbol_table->count; j++) {
        if (strcmp(label, symbol_table->symbols[j].name) == 0) {
            return false;
        }
    }
    return true;
}

bool is_reserved_word(const char *word, const char *reserved_words[], int reserved_words_count) {
    int i;
    for (i = 0; i < reserved_words_count; i++) {
        if (strcmp(word, reserved_words[i]) == 0) {
            return true;
        }
    }
    return false;
}

void process_directive(char *line, SymbolTable *symbol_table, DataImage *data_image, int *dc, const char *label, int line_number) {
    if (strncmp(line, ".data", 5) == 0) {
        if (label[0] != '\0' && find_symbol(symbol_table, label) == NULL) {
            add_to_symbol_table(symbol_table, label, *dc, false, true);
        }
        parse_data_directive(line + 5, data_image, dc);
    } else if (strncmp(line, ".string", 7) == 0) {
        if (label[0] != '\0' && find_symbol(symbol_table, label) == NULL) {
            add_to_symbol_table(symbol_table, label, *dc, false, true);
        }
        parse_string_directive(line + 7, data_image, dc);
    } else if (strncmp(line, ".entry", 6) == 0) {
        /* Entry symbols are handled in the second pass */
    } else if (strncmp(line, ".extern", 7) == 0) {
        char extern_label[MAX_LABEL_LENGTH];
        sscanf(line + 7, "%s", extern_label);
        if (find_symbol(symbol_table, extern_label) == NULL) {
            add_to_symbol_table(symbol_table, extern_label, 0, true, false);
            printf("Added extern symbol: %s\n", extern_label); /* Debugging print */
        } else {
            printf("Symbol %s already exists, updating to extern\n", extern_label); /* Debugging print */
        }
    } else {
        fprintf(stderr, "Error in line %d: Unknown directive\n", line_number);
    }
}


void parse_data_directive(const char *line, DataImage *data_image, int *dc) {
    const char *ptr = line;
    char *endptr;
    int value;

    while (*ptr) {
        while (isspace(*ptr) || *ptr == ',') {
            ptr++;
        }
        if (*ptr == '\0') {
            break;
        }
        value = strtol(ptr, &endptr, 10);
        if (ptr == endptr) {
            fprintf(stderr, "Error: Invalid number in .data directive\n");
            return;
        }
        add_data(data_image, value);
        (*dc)++;
        ptr = endptr;
    }
}

void parse_string_directive(const char *line, DataImage *data_image, int *dc) {
    const char *ptr = line;
    char *start_quote, *end_quote;

    while (isspace(*ptr)) {
        ptr++;
    }
    start_quote = strchr(ptr, '"');
    if (!start_quote) {
        fprintf(stderr, "Error: Missing opening quote in .string directive\n");
        return;
    }
    end_quote = strchr(start_quote + 1, '"');
    if (!end_quote) {
        fprintf(stderr, "Error: Missing closing quote in .string directive\n");
        return;
    }
    for (ptr = start_quote + 1; ptr < end_quote; ptr++) {
        add_data(data_image, *ptr);
        (*dc)++;
    }
    add_data(data_image, '\0');
    (*dc)++;
}

