#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "symbol_table.h"
#include "data_image.h"
#include "second_pass.h"

bool is_valid_opcode(const char *opcode);
int get_opcode_value(const char *opcode);
int encode_operand(char *operand, SymbolTable *symbol_table, DataImage *code_image);
int parse_instruction(const char *line, SymbolTable *symbol_table, DataImage *code_image, int *ic);
int is_register(const char *operand);

int get_opcode(const char *opcode);
int get_instruction_length(int opcode, const char *operands); 
void set_data_at(DataImage *image, int index, int value);


#endif

