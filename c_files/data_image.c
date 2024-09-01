#include "data_image.h"
#include <string.h>
#include <stdio.h>

#define WORD_SIZE 15

void init_data_image(DataImage *image) {
    image->size = 0;
    memset(image->data, 0, sizeof(image->data));
}

int add_data(DataImage *image, int value) {
    if (image->size >= MAX_DATA_SIZE) {
        return 0; /* Data image is full */
    }

    /* Convert to two's complement if the value is negative */
    if (value < 0) {
        value = (1 << WORD_SIZE) + value;
    }

    image->data[image->size++] = value & ((1 << WORD_SIZE) - 1);
    return 1; /* Success */
}

int add_string(DataImage *image, const char *str) {
    while (*str && image->size < MAX_DATA_SIZE) {
        image->data[image->size++] = *str++ & 0x7F;
    }
    if (image->size < MAX_DATA_SIZE) {
        image->data[image->size++] = '\0'; /* Null terminator */
        return 1; /* Success */
    }
    return 0; /* Data image is full */
}

int get_data_size(const DataImage *image) {
    return image->size;
}

int get_data_at(const DataImage *image, int index) {
    if (index >= 0 && index < image->size) {
        return image->data[index];
    }
    return 0; /* Invalid index */
}

void print_data_image(DataImage *data_image, const char *name) {
    int i;
    printf("%s:\n", name);
    for (i = 0; i < data_image->size; i++) {
        printf("%04d: %05o\n", i, data_image->data[i]);
    }

    printf("\n");
}
