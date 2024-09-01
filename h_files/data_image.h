#ifndef DATA_IMAGE_H
#define DATA_IMAGE_H

#define MAX_DATA_SIZE 1000

typedef struct {
    int data[MAX_DATA_SIZE];
    int size;
} DataImage;

void init_data_image(DataImage *image);
int add_data(DataImage *image, int value);
int add_string(DataImage *image, const char *str);
int get_data_size(const DataImage *image);
int get_data_at(const DataImage *image, int index);
void set_data_at(DataImage *image, int index, int value);
void print_data_image(DataImage *data_image, const char *name);



#endif /* DATA_IMAGE_H */

