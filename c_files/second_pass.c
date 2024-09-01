#include "second_pass.h"
#include "first_pass.h"
#include "instructions.h"
#include "directives.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 80
#define MAX_FILENAME 256

int process_second_pass_line(char *line, SymbolTable *symbol_table, DataImage *code_image, IncompleteInstructionTable *incomplete_instructions, int *ic, int *dc);
int handle_directive(char *directive, char *operands, SymbolTable *symbol_table);
int handle_entry_directive(char *operands, SymbolTable *symbol_table);
int complete_instruction_encoding(char *operation, char *operands, SymbolTable *symbol_table, DataImage *code_image, IncompleteInstructionTable *incomplete_instructions, int *ic);
int complete_incomplete_instruction(IncompleteInstruction *instr, SymbolTable *symbol_table, DataImage *code_image, int *ic);
int generate_output_files(const char *input_filename, SymbolTable *symbol_table, DataImage *code_image, DataImage *data_image);
int generate_object_file(const char *base_filename, DataImage *code_image, DataImage *data_image);
int generate_externals_file(const char *base_filename, SymbolTable *symbol_table, const char *input_filename);


int generate_entries_file(const char *base_filename, SymbolTable *symbol_table);

int complete_incomplete_instruction(IncompleteInstruction *instr, SymbolTable *symbol_table, DataImage *code_image, int *ic) {
    Symbol *symbol;
    int instruction_word;
    int address;

    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Debug: Starting completion of instruction at IC = %d\n", *ic);

    symbol = find_symbol(symbol_table, instr->label);
    if (!symbol) {
        fprintf(stderr, "Error: Symbol %s not found in second pass\n", instr->label);
        return 0;
    }
    printf("Debug: Found symbol %s with address %d\n", instr->label, symbol->address);

    /* עדכון המילה של ההוראה עם כתובת הסמל */
    instruction_word = get_data_at(code_image, *ic);
    printf("Debug: Current instruction word before update: ");
    print_binary(instruction_word, 15);
    
    address = symbol->address & 0x3FFF; /* כתובת בסיביות 14-3 */
    instruction_word |= (address << 2);  /* הנחת הכתובת בסיביות 14-3 */
    printf("Debug: Updated instruction word with symbol address: ");
    print_binary(instruction_word, 15);

    /* עדכון ביטי ה-A,R,E */
    if (symbol->is_extern) {
        instruction_word |= 0x1;  /* Set E bit */
        printf("Debug: Marked as external symbol, updated instruction word: ");
        print_binary(instruction_word, 15);
    } else {
        instruction_word |= 0x2;  /* Set R bit */
        printf("Debug: Marked as relocatable symbol, updated instruction word: ");
        print_binary(instruction_word, 15);
    }

    set_data_at(code_image, *ic, instruction_word);
    printf("Debug: Final instruction word saved: ");
    print_binary(instruction_word, 15);

    *ic += get_instruction_length(instr->opcode, NULL);  /* NULL because we don't need operands here */
    return 1;
}

void resolve_incomplete_labels(IncompleteInstructionTable *table, SymbolTable *symbol_table, DataImage *code_image) {

    int i;
    for (i = 0; i < table->count; i++) {
        IncompleteInstruction *instr = &table->instructions[i];
        complete_incomplete_instruction(instr, symbol_table, code_image, &(instr->address));
    }
}



int second_pass(const char *input_filename, SymbolTable *symbol_table, DataImage *data_image, DataImage *code_image, IncompleteInstructionTable *incomplete_instructions) {
    FILE *file;
    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    int ic = 100;  /* Initialize instruction counter */
    int dc = 0;    /* Initialize data counter */

    file = fopen(input_filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Unable to open file %s\n", input_filename);
        return 0;
    }
     printf("Debug: Starting second pass with initial IC = %d\n", ic);

    while (fgets(line, sizeof(line), file)) {
        char *trimmed_line = line;
        line_number++;

        /* Remove leading whitespace */
        while (*trimmed_line == ' ' || *trimmed_line == '\t') {
            trimmed_line++;
        }

        /* Skip empty lines and comments */
        if (*trimmed_line == '\n' || *trimmed_line == ';') {
            continue;
        }

        /* Process the line */
        if (!process_second_pass_line(trimmed_line, symbol_table, code_image, incomplete_instructions, &ic, &dc)) {
            fprintf(stderr, "Error in line %d: %s", line_number, line);
            fclose(file);
            return 0;
        }
       /*printf("Debug: After processing line '%s', IC = %d\n", trimmed_line, ic);*/

   /* השלמת ההוראות הלא שלמות */
    resolve_incomplete_labels(incomplete_instructions, symbol_table, code_image);
    }

    fclose(file);
    /*printf("Debug: Final IC after second pass = %d\n", ic);*/

    /* אין לעדכן את כתובות הסמלים, השאר את הכתובות כמו שהן במעבר הראשון */
    
    /* Generate output files */
    if (!generate_output_files(input_filename, symbol_table, code_image, data_image)) {
        return 0;
    }

    return 1;  /* Successful completion */
}

int process_second_pass_line(char *line, SymbolTable *symbol_table, DataImage *code_image, IncompleteInstructionTable *incomplete_instructions, int *ic, int *dc) {
    char label[MAX_LABEL_LENGTH] = {0};
    char operation[MAX_LABEL_LENGTH] = {0};
    char operands[MAX_LINE_LENGTH] = {0};
    int result;
    char *colon;

    /* Extract label if exists */
    colon = strchr(line, ':');
    if (colon) {
        strncpy(label, line, colon - line);
        label[colon - line] = '\0';
        line = colon + 1;
        while (*line == ' ' || *line == '\t') line++;
    }

    /* Extract operation and operands */
    sscanf(line, "%s %[^\n]", operation, operands);
    /*printf("Debug: Processing line - Label: '%s', Operation: '%s', Operands: '%s', Current IC: %d\n", 
           label, operation, operands, *ic);*/

    /* Handle directives */
    if (operation[0] == '.') {
        return handle_directive(operation, operands, symbol_table);
    }

    /* Handle instructions */
    result = complete_instruction_encoding(operation, operands, symbol_table, code_image, incomplete_instructions, ic);
    if (result == 0) {
        fprintf(stderr, "Error: Failed to complete instruction encoding\n");
        return 0;
    }
     /*printf("Debug: After processing, IC updated to: %d\n", *ic);*/
    return 1;
}

int handle_directive(char *directive, char *operands, SymbolTable *symbol_table) {
    if (strcmp(directive, ".entry") == 0) {
        return handle_entry_directive(operands, symbol_table);
    }
    /* We ignore .extern directives in the second pass as they were handled in the first pass */
    return 1;
}

int handle_entry_directive(char *operands, SymbolTable *symbol_table) {
    Symbol *symbol = find_symbol(symbol_table, operands);
    if (!symbol) {
        fprintf(stderr, "Error: Symbol %s not found for .entry directive\n", operands);
        return 0;
    }
    symbol->is_entry = 1;
    return 1;
}

int complete_instruction_encoding(char *operation, char *operands, SymbolTable *symbol_table, DataImage *code_image, IncompleteInstructionTable *incomplete_instructions, int *ic) {
    int opcode;
    IncompleteInstruction *instr;

    opcode = get_opcode(operation);
    if (opcode == -1) {
        fprintf(stderr, "Error: Invalid operation %s\n", operation);
        return 0;
    }

    instr = find_incomplete_instruction(incomplete_instructions, *ic);
    if (instr) {
        printf("Debug [complete_instruction_encoding]: Found incomplete instruction at IC = %d, completing...\n", *ic);
        return complete_incomplete_instruction(instr, symbol_table, code_image, ic);
    }

    printf("Debug [complete_instruction_encoding]: No incomplete instruction at IC = %d, advancing IC by instruction length.\n", *ic);
    *ic += get_instruction_length(opcode, operands);
    return 1;
}




int generate_output_files(const char *input_filename, SymbolTable *symbol_table, DataImage *code_image, DataImage *data_image) {
    char base_filename[MAX_FILENAME];
    char *dot_position;

    /* Remove .am extension if present */
    strncpy(base_filename, input_filename, MAX_FILENAME - 1);
    base_filename[MAX_FILENAME - 1] = '\0';
    dot_position = strrchr(base_filename, '.');
    if (dot_position && strcmp(dot_position, ".am") == 0) {
        *dot_position = '\0';
    }

    fprintf(stdout, "Base filename: %s\n", base_filename); /* Debugging print */

    if (!generate_object_file(base_filename, code_image, data_image)) return 0;
    if (!generate_externals_file(base_filename, symbol_table, input_filename)) return 0;

    if (!generate_entries_file(base_filename, symbol_table)) return 0;

    return 1;
}

int generate_object_file(const char *base_filename, DataImage *code_image, DataImage *data_image) {
    char filename[MAX_FILENAME];
    FILE *ob_file;
    int i, ic, dc;

    sprintf(filename, "%s.ob", base_filename);
    fprintf(stdout, "Attempting to create object file: %s\n", filename); /* Debugging print */

    ob_file = fopen(filename, "w");
    if (!ob_file) {
        perror("Error opening .ob file"); /* Debugging print */
        fprintf(stderr, "Error: Unable to create object file %s\n", filename);
        return 0;
    }

    fprintf(stdout, "Creating object file: %s\n", filename); /* Debugging print */

    ic = code_image->size;
    dc = data_image->size;
    fprintf(ob_file, "%d %d\n", ic, dc);

    for (i = 0; i < ic; i++) {
        fprintf(ob_file, "%04d %05o\n", i + 100, code_image->data[i]);
    }
    for (i = 0; i < dc; i++) {
        fprintf(ob_file, "%04d %05o\n", i + ic + 100, data_image->data[i]);
    }
    fclose(ob_file);

    fprintf(stdout, "Object file created successfully: %s\n", filename); /* Debugging print */

    return 1;
}

int generate_externals_file(const char *base_filename, SymbolTable *symbol_table, const char *input_filename) {
    char filename[MAX_FILENAME];
    FILE *ext_file, *input_file;
    char line[MAX_LINE_LENGTH];
    int ic = 100;  /* Instruction counter initialization */
    Symbol *symbol;
    int found_extern = 0;
    char operation[MAX_LABEL_LENGTH];
    char operands[MAX_LINE_LENGTH];
    char *operand1, *operand2;
    char *trimmed_line;
    int instruction_length;
    int opcode;
    int offset;
    int is_two_register_op;

    sprintf(filename, "%s.ext", base_filename);
    ext_file = fopen(filename, "w");
    if (!ext_file) {
        fprintf(stderr, "Error: Unable to create externals file %s\n", filename);
        return 0;
    }

    input_file = fopen(input_filename, "r");
    if (!input_file) {
        fclose(ext_file);
        return 0;
    }

    while (fgets(line, sizeof(line), input_file)) {
        trimmed_line = line;
        while (*trimmed_line == ' ' || *trimmed_line == '\t') trimmed_line++;
        if (*trimmed_line == '\n' || *trimmed_line == ';' || trimmed_line[0] == '.') {
            continue;  /* Skip empty lines, comments, and directives */
        }

        sscanf(trimmed_line, "%s %[^\n]", operation, operands);
        
        opcode = get_opcode(operation);
        instruction_length = get_instruction_length(opcode, operands);

        /*printf("Debug: Processing line - Operation: '%s', Operands: '%s', Current IC: %d\n", operation, operands, ic);*/

        operand1 = strtok(operands, ",");
        operand2 = strtok(NULL, ",");

        offset = 1; /* Default offset */
        is_two_register_op = 0;

        if (operand1 && operand2) {
            /* Check if both operands are registers */
            if ((*operand1 == 'r' && isdigit(*(operand1+1))) && 
                (*operand2 == 'r' && isdigit(*(operand2+1)))) {
                is_two_register_op = 1;
            }
        }

        if (operand1) {
            while (*operand1 == ' ' || *operand1 == '\t') operand1++;
            symbol = find_symbol(symbol_table, operand1);
            if (symbol && symbol->is_extern) {
                found_extern = 1;
                fprintf(ext_file, "%s %04d\n", symbol->name, ic + offset);
                printf("Debug: Found external symbol '%s' at address %04d\n", symbol->name, ic + offset);
                if (!is_two_register_op) offset++;
            }
            if (*operand1 == '*' && !is_two_register_op) offset++; /* Indirect addressing takes an extra word */
        }

        if (operand2) {
            while (*operand2 == ' ' || *operand2 == '\t') operand2++;
            symbol = find_symbol(symbol_table, operand2);
            if (symbol && symbol->is_extern) {
                found_extern = 1;
                fprintf(ext_file, "%s %04d\n", symbol->name, ic + offset);
                printf("Debug: Found external symbol '%s' at address %04d\n", symbol->name, ic + offset);
            }
        }

        ic += instruction_length;
        /*printf("Debug: After processing, IC updated to: %d\n", ic);*/
    }

    fclose(input_file);
    fclose(ext_file);

    if (!found_extern) {
        remove(filename);  /* Remove empty .ext file */
    }

    return 1;
}

int generate_entries_file(const char *base_filename, SymbolTable *symbol_table) {
    char filename[MAX_FILENAME];
    FILE *ent_file;
    int i;
    int found_entry = 0;

    sprintf(filename, "%s.ent", base_filename);
    fprintf(stdout, "Attempting to create entries file: %s\n", filename); /* Debugging print */

    ent_file = fopen(filename, "w");
    if (!ent_file) {
        perror("Error opening .ent file"); /* Debugging print */
        fprintf(stderr, "Error: Unable to create entries file %s\n", filename);
        return 0;
    }

    fprintf(stdout, "Creating entries file: %s\n", filename); /* Debugging print */

    for (i = 0; i < symbol_table->count; i++) {
        if (symbol_table->symbols[i].is_entry) {
            found_entry = 1;
            fprintf(stdout, "Writing entry symbol: %s %04d\n", symbol_table->symbols[i].name, symbol_table->symbols[i].address); /* Debugging print */
            fprintf(ent_file, "%s %04d\n", symbol_table->symbols[i].name, symbol_table->symbols[i].address);
        }
    }
    fclose(ent_file);

    if (!found_entry) {
        fprintf(stdout, "No entry symbols found.\n"); /* Debugging print */
    } else {
        fprintf(stdout, "Entries file created successfully: %s\n", filename); /* Debugging print */
    }

    return 1;
}



