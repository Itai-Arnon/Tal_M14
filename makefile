# Compiler and compilation flags
CC = gcc
CFLAGS = -Wall -ansi -pedantic -Ih_files

# Directories
C_DIR = c_files
H_DIR = h_files
IO_DIR = input_and_output

# Object files
OBJFILES = $(C_DIR)/main.o $(C_DIR)/first_pass.o $(C_DIR)/second_pass.o \
           $(C_DIR)/instructions.o $(C_DIR)/directives.o $(C_DIR)/symbol_table.o \
           $(C_DIR)/pre_assembler.o $(C_DIR)/data_image.o

# Target executable
TARGET = assembler

# Default rule to build everything
.PHONY: all clean

all: $(TARGET)

# Link the target executable
$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES)  -g

# Compile object files
$(C_DIR)/main.o: $(C_DIR)/main.c $(H_DIR)/first_pass.h $(H_DIR)/second_pass.h \
                 $(H_DIR)/instructions.h $(H_DIR)/directives.h \
                 $(H_DIR)/symbol_table.h $(H_DIR)/pre_assembler.h $(H_DIR)/data_image.h
	$(CC) $(CFLAGS) -c $(C_DIR)/main.c -o $(C_DIR)/main.o

$(C_DIR)/first_pass.o: $(C_DIR)/first_pass.c $(H_DIR)/first_pass.h \
                       $(H_DIR)/directives.h $(H_DIR)/instructions.h \
                       $(H_DIR)/symbol_table.h $(H_DIR)/data_image.h
	$(CC) $(CFLAGS) -c $(C_DIR)/first_pass.c -o $(C_DIR)/first_pass.o

$(C_DIR)/second_pass.o: $(C_DIR)/second_pass.c $(H_DIR)/second_pass.h \
                        $(H_DIR)/directives.h $(H_DIR)/instructions.h \
                        $(H_DIR)/symbol_table.h $(H_DIR)/data_image.h
	$(CC) $(CFLAGS) -c $(C_DIR)/second_pass.c -o $(C_DIR)/second_pass.o

$(C_DIR)/instructions.o: $(C_DIR)/instructions.c $(H_DIR)/instructions.h \
                         $(H_DIR)/symbol_table.h $(H_DIR)/data_image.h
	$(CC) $(CFLAGS) -c $(C_DIR)/instructions.c -o $(C_DIR)/instructions.o

$(C_DIR)/directives.o: $(C_DIR)/directives.c $(H_DIR)/directives.h \
                       $(H_DIR)/symbol_table.h $(H_DIR)/data_image.h
	$(CC) $(CFLAGS) -c $(C_DIR)/directives.c -o $(C_DIR)/directives.o

$(C_DIR)/symbol_table.o: $(C_DIR)/symbol_table.c $(H_DIR)/symbol_table.h
	$(CC) $(CFLAGS) -c $(C_DIR)/symbol_table.c -o $(C_DIR)/symbol_table.o

$(C_DIR)/pre_assembler.o: $(C_DIR)/pre_assembler.c $(H_DIR)/pre_assembler.h
	$(CC) $(CFLAGS) -c $(C_DIR)/pre_assembler.c -o $(C_DIR)/pre_assembler.o

$(C_DIR)/data_image.o: $(C_DIR)/data_image.c $(H_DIR)/data_image.h
	$(CC) $(CFLAGS) -c $(C_DIR)/data_image.c -o $(C_DIR)/data_image.o

# Clean generated files
clean:
	rm -f $(OBJFILES) $(TARGET)
	rm -f $(IO_DIR)/*/*.ob $(IO_DIR)/*/*.ent $(IO_DIR)/*/*.ext $(IO_DIR)/*/*.am
	# Delete empty output directories, without deleting input files
	find $(IO_DIR)/* -type d -empty -delete

