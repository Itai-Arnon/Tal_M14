#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include "symbol_table.h"
#include "data_image.h"
#include "instructions.h"

#define MAX_LINE_LENGTH 80
#define MAX_LABEL_LENGTH 31

typedef struct {
    int address;
    char label[MAX_LABEL_LENGTH];
    int opcode;
    int source_addressing;
    int target_addressing;
} IncompleteInstruction;

typedef struct {
    IncompleteInstruction instructions[MAX_LINE_LENGTH];
    int count;
} IncompleteInstructionTable;

/* Main function for the second pass */
int second_pass(const char *filename, SymbolTable *symbol_table, DataImage *data_image, DataImage *code_image, IncompleteInstructionTable *incomplete_instructions);

/* Helper functions */
int process_second_pass_line(char *line, SymbolTable *symbol_table, DataImage *code_image, IncompleteInstructionTable *incomplete_instructions, int *ic, int *dc);
int handle_entry_directive(char *operands, SymbolTable *symbol_table);
int complete_instruction_encoding(char *operation, char *operands, SymbolTable *symbol_table, DataImage *code_image, IncompleteInstructionTable *incomplete_instructions, int *ic);
int complete_incomplete_instruction(IncompleteInstruction *instr, SymbolTable *symbol_table, DataImage *code_image, int *ic);
void resolve_incomplete_labels(IncompleteInstructionTable *table, SymbolTable *symbol_table, DataImage *code_image);

/* Utility functions for incomplete instructions */
void init_incomplete_instruction_table(IncompleteInstructionTable *table);
void add_incomplete_instruction(IncompleteInstructionTable *table, int address, const char *label, int opcode, int source_addressing, int target_addressing);
IncompleteInstruction* find_incomplete_instruction(IncompleteInstructionTable *table, int address);

#endif /* SECOND_PASS_H */

