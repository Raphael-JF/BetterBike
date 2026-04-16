#ifndef BIN_MATRIX_H
#define BIN_MATRIX_H
#include <stdint.h>


struct bin_matrix {
    unsigned int width;
    unsigned int height;
    uint8_t* data; // 1 bit per pixel, stored in bytes
};

struct bin_matrix* create_bin_matrix(unsigned int width, unsigned int height);
void free_bin_matrix(struct bin_matrix* matrix);
void set_pixel_bin_matrix(struct bin_matrix* matrix, unsigned int x, unsigned int y, uint8_t value);
uint8_t get_pixel_bin_matrix(struct bin_matrix* matrix, unsigned int x, unsigned int y);
void fill_matrix(struct bin_matrix* matrix, uint8_t value);

#endif // BIN_MATRIX_H