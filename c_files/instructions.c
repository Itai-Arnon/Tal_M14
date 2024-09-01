#include <stdlib.h>
#include "instructions.h"
#include "symbol_table.h"
#include "data_image.h"
#include "directives.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_LABEL_LENGTH 31
#define LINE_LENGTH 80

extern const char *reserved_words[];
extern int reserved_words_count;

bool is_valid_opcode(const char *opcode) {
    const char *valid_opcodes[] = {
        "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec",
        "jmp", "bne", "red", "prn", "jsr", "rts", "stop"
    };
    int num_opcodes = sizeof(valid_opcodes) / sizeof(valid_opcodes[0]);
    int i;

    printf("Checking if '%s' is a valid opcode\n", opcode);

    for (i = 0; i < num_opcodes; i++) {
        if (strcmp(opcode, valid_opcodes[i]) == 0) {
            return true;
        }
    }
    return false;
}

void print_binary(int value, int bits) {
    int i;
    unsigned int mask;
    
    mask = 1u << (bits - 1);
    for (i = 0; i < bits; i++) {
        putchar(value & mask ? '1' : '0');
        mask >>= 1;
    }
    putchar('\n');
}

int get_opcode_value(const char *opcode) {
    if (strcmp(opcode, "mov") == 0) return 0x0;  /* 0000 */
    if (strcmp(opcode, "cmp") == 0) return 0x1;  /* 0001 */
    if (strcmp(opcode, "add") == 0) return 0x2;  /* 0010 */
    if (strcmp(opcode, "sub") == 0) return 0x3;  /* 0011 */
    if (strcmp(opcode, "lea") == 0) return 0x4;  /* 0100 */
    if (strcmp(opcode, "clr") == 0) return 0x5;  /* 0101 */
    if (strcmp(opcode, "not") == 0) return 0x6;  /* 0110 */
    if (strcmp(opcode, "inc") == 0) return 0x7;  /* 0111 */
    if (strcmp(opcode, "dec") == 0) return 0x8;  /* 1000 */
    if (strcmp(opcode, "jmp") == 0) return 0x9;  /* 1001 */
    if (strcmp(opcode, "bne") == 0) return 0xA;  /* 1010 */
    if (strcmp(opcode, "red") == 0) return 0xB;  /* 1011 */
    if (strcmp(opcode, "prn") == 0) return 0xC;  /* 1100 */
    if (strcmp(opcode, "jsr") == 0) return 0xD;  /* 1101 */
    if (strcmp(opcode, "rts") == 0) return 0xE;  /* 1110 */
    if (strcmp(opcode, "stop") == 0) return 0xF; /* 1111 */
    return -1; /* Invalid opcode */
}

int get_addressing_mode(const char *operand) {
    if (operand[0] == '#') return 0; /* Immediate */
    if (operand[0] == 'r' && isdigit((int)operand[1])) return 3; /* Register */
    if (operand[0] == '*') return 2; /* Indirect register */
    return 1; /* Direct */
}




void encode_opcode(DataImage *code_image, int opcode_value) {

printf("hereeeeeeeeeeeeeeeeeeeee ");
    int encoded_opcode;
    encoded_opcode = (opcode_value & 0xF) << 11; /* Shift left by 11 to position in 15-bit word */
    add_data(code_image, encoded_opcode);
    
    printf("22222222Debug: Encoding opcode value %d\n", opcode_value);
    printf("22222222Debug: Encoded opcode (full 15-bit word): ");
    print_binary(encoded_opcode, 15);
    printf("22222222Debug: Encoded opcode (only 4 opcode bits): ");
    print_binary(opcode_value, 4);
}
int encode_operand(char *operand, SymbolTable *symbol_table, DataImage *code_image) {
    int operand_size = 1;
    int addressing_mode;
    int value;
    int reg_num;
    Symbol *symbol;
    int encoded_value = 0;

    addressing_mode = get_addressing_mode(operand);
    printf("Debug: Encoding operand: %s\n", operand);
    printf("Debug: Addressing mode: %d\n", addressing_mode);

    if (addressing_mode == 0) { /* Immediate addressing */
        value = atoi(operand + 1);
        encoded_value = value & 0x7FFF; /* Ensure 15-bit value */
        encoded_value |= 4; /* Set A bit in A,R,E */
    } else if (addressing_mode == 3) { /* Register addressing */
        reg_num = operand[1] - '0';
        printf("Debug: reg_num: %d\n", reg_num);
        
        encoded_value = (reg_num << 6); /* Shift register number to bits 8-6 */
        printf("Debug: After shifting reg_num: ");
        print_binary(encoded_value, 15);
        
        encoded_value |= 0x80; /* Set bit 7 to indicate register direct mode */
        printf("Debug: After setting addressing mode: ");
        print_binary(encoded_value, 15);
        
        encoded_value |= 4; /* Set A bit in A,R,E */
        printf("Debug: Final encoded value: ");
        print_binary(encoded_value, 15);
    } else { /* Direct or indirect addressing */
        symbol = find_symbol(symbol_table, operand);
        if (symbol) {
            encoded_value = symbol->address;
            encoded_value |= 2; /* Set R bit in A,R,E */
        } else {
            /* Symbol not found, add a placeholder */
            encoded_value = 0;
        }
    }

    add_data(code_image, encoded_value);
    printf("Debug: Encoded operand %s as binary: ", operand);
    print_binary(encoded_value, 15);

    return operand_size;
}

/* Check if the operand is a register (direct or indirect) */
int is_register(const char *operand) {
    const char *registers[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
    int num_registers = sizeof(registers) / sizeof(registers[0]);
    int i;

    printf("Checking if operand '%s' is a register\n", operand);

    for (i = 0; i < num_registers; i++) {
        if (strcmp(operand, registers[i]) == 0) {
            printf("Operand '%s' is a register\n", operand);
            return 1;
        }
    }
    printf("Operand '%s' is not a register\n", operand);
    return 0;
}

int parse_instruction(const char *line, SymbolTable *symbol_table, DataImage *code_image, int *ic) {
printf("start_prase_instruction ");
     char opcode[LINE_LENGTH] = {0};
    char operands[LINE_LENGTH] = {0};
    int instruction_size = 1; /* Start with 1 for the opcode word */
    int opcode_value;
    char *operand1;
    char *operand2;
    int encoded_operands;
    int source_addressing;
    int dest_addressing;
    int instruction_word = 0;




    /* Extract opcode and operands */
    sscanf(line, "%s %[^\n]", opcode, operands);

    if (!is_valid_opcode(opcode)) {
        fprintf(stderr, "Error: Invalid opcode '%s'\n", opcode);
        return 0;
    }

    opcode_value = get_opcode_value(opcode);
    instruction_word |= (opcode_value << 11); /* Place opcode in bits 11-14 */

    printf("11111Debug: Opcode '%s' value: %d\n", opcode, opcode_value);
    printf("11111Debug: Encoded opcode (4 bits): ");
    print_binary(opcode_value, 4);
    printf("11111Debug: Instruction word after opcode encoding: ");
    print_binary(instruction_word, 15);

    /* Process operands */
    operand1 = strtok(operands, ",");
    operand2 = strtok(NULL, ",");

    /* Trim leading spaces from operands */
       if (operand1) {
        while (isspace((unsigned char)*operand1)) operand1++;
        source_addressing = get_addressing_mode(operand1);
        instruction_word |= (1 << (7 + source_addressing));
    }
      if (operand2) {
        while (isspace((unsigned char)*operand2)) operand2++;
        dest_addressing = get_addressing_mode(operand2);
        instruction_word |= (1 << (3 + dest_addressing));
    }

    /* Set ARE bits for the first word */
    instruction_word |= 0x4; /* Set A bit */

    printf("Debug: Final instruction word: ");
    print_binary(instruction_word, 15);
    add_data(code_image, instruction_word);
    /* Debugging prints */
    printf("Opcode: %s, Operand1: %s, Operand2: %s\n", opcode, operand1 ? operand1 : "NULL", operand2 ? operand2 : "NULL");

    /* Handle any opcode with two register operands */
   if (operand1 && operand2 && is_register(operand1) && (is_register(operand2) || operand2[0] == '*')) {
        instruction_size = 2;
        printf("Instruction: %s with two register operands, Size: %d\n", opcode, instruction_size);
        encoded_operands = ((operand1[1] - '0') << 6) | ((operand2[1] - '0') << 3);
        instruction_word |= (3 << 7) | (3 << 3); /* Set source and destination addressing modes to 11 (register) */
        add_data(code_image, instruction_word);
        add_data(code_image, encoded_operands);
    } else {
        if (operand1) {
            instruction_size += encode_operand(operand1, symbol_table, code_image);
        }
        if (operand2) {
            instruction_size += encode_operand(operand2, symbol_table, code_image);
        }
        add_data(code_image, instruction_word);
    }

        printf("Instruction: %s, Size: %d\n", opcode, instruction_size);
    printf("Encoded instruction word: ");
    print_binary(instruction_word, 15);

    return instruction_size;
}


int get_opcode(const char *opcode) {
    return get_opcode_value(opcode);
}

int get_instruction_length(int opcode, const char *operands) {
    int length = 1; /* Opcode itself takes one word */
    
    char operands_copy[LINE_LENGTH];
    char *operand1, *operand2;

    strcpy(operands_copy, operands);
    operand1 = strtok(operands_copy, ",");
    operand2 = strtok(NULL, ",");

    printf("Debug: get_instruction_length - operands: %s | operand1: %s | operand2: %s\n", operands, operand1, operand2);

/* Debugging information */
    printf("Debug: Checking operands - operand1: '%s', operand2: '%s'\n", operand1, operand2);

   
     /* Trim leading spaces from operand1 */
    if (operand1) {
        while (isspace(*operand1)) operand1++;
    }
    
    /* Trim leading spaces from operand2 */
    if (operand2) {
        while (isspace(*operand2)) operand2++;
    }

    /* Debugging information 
    printf("Debug: Checking operands - operand1: '%s', operand2: '%s'\n", operand1, operand2);*/

    /* Check if operand2 is an immediate value (e.g., #-6) */
    if (operand2 && operand2[0] == '#') {
        printf("Debug: Operand2 '%s' is recognized as an immediate value\n", operand2);
        length += 2; /* Immediate value requires two extra words */
    } else if (operand1 && operand2 && is_register(operand1)) {
        if (is_register(operand2) || (operand2[0] == '*' && is_register(operand2 + 1))) {
            printf("Debug: Both operands are registers\n");
            length += 1; /* Both operands are registers, consider as a single word */
        } else {
            printf("Debug: Operand2 '%s' is not a register\n", operand2);
            length += 1; /* Operand 2 takes an extra word */
        }
    } else {
        if (operand1) {
            printf("Debug: Operand1 '%s' requires an extra word\n", operand1);
            length += 1; /* Operand 1 might take an extra word */
        }
        if (operand2) {
            printf("Debug: Operand2 '%s' requires an extra word\n", operand2);
            length += 1; /* Operand 2 might take an extra word */
        }
    }


    printf("Debug: Final calculated length for opcode %d with operands '%s': %d\n", opcode, operands, length);
    



    return length;
}

void set_data_at(DataImage *image, int index, int value) {
    if (index >= 0 && index < image->size) {
        image->data[index] = value;
    }
}


void print_instruction_debug(int instruction)
{
    int opcode, source_addressing, destination_addressing, are;
    
    printf("Instruction: ");
    {
        int mask = 0x4000; /* 100000000000000 in binary */
        int i;
        for (i = 0; i < 15; i++) {
            printf("%d", (instruction & mask) ? 1 : 0);
            mask >>= 1;
        }
        printf("\n");
    }
    
    opcode = (instruction >> 11) & 0xF;
    source_addressing = (instruction >> 7) & 0xF;
    destination_addressing = (instruction >> 3) & 0xF;
    are = instruction & 0x7;
    
    printf("Opcode: %d\n", opcode);
    printf("Source addressing: %d\n", source_addressing);
    printf("Destination addressing: %d\n", destination_addressing);
    printf("A,R,E: %d\n", are);
}

void init_incomplete_instruction_table(IncompleteInstructionTable *table) {
    table->count = 0;
}

IncompleteInstruction* find_incomplete_instruction(IncompleteInstructionTable *table, int address) {
    int i;
    for (i = 0; i < table->count; i++) {
        if (table->instructions[i].address == address) {
            return &table->instructions[i];
        }
    }
    return NULL;
}


