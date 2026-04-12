#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "binary_matrix.h"

struct bin_matrix* create_bin_matrix(unsigned int width, unsigned int height){
    struct bin_matrix* matrix = (struct bin_matrix*)malloc(sizeof(struct bin_matrix));
    if (!matrix) return NULL;

    matrix->width = width;
    matrix->height = height;
    unsigned int nb_bytes = (height*width + 7) / 8; // nombre de bytes nécessaires pour stocker une ligne
    matrix->data = (uint8_t*)calloc(nb_bytes, sizeof(uint8_t)); // alloue et initialise à 0
    if (!matrix->data) {
        free(matrix);
        return NULL;
    }

    return matrix;
}

void free_bin_matrix(struct bin_matrix* matrix){
    if (matrix) {
        free(matrix->data);
        free(matrix);
    }
}

void set_pixel_bin_matrix(struct bin_matrix* matrix, unsigned int x, unsigned int y, uint8_t value){
    if (x >= matrix->width || y >= matrix->height) return;
    unsigned int byte_index = (y * matrix->width + x) / 8;
    unsigned int bit_index = (y * matrix->width + x) % 8;
    if (value) {
        matrix->data[byte_index] |= (1 << bit_index);
    } else {
        matrix->data[byte_index] &= ~(1 << bit_index);
    }
}

uint8_t get_pixel_bin_matrix(struct bin_matrix* matrix, unsigned int x, unsigned int y){
    if (x >= matrix->width || y >= matrix->height) return 0;
    unsigned int byte_index = (y * matrix->width + x) / 8;
    unsigned int bit_index = (y * matrix->width + x) % 8;
    return (matrix->data[byte_index] >> bit_index) & 1;
}

void fill_matrix(struct bin_matrix* matrix, uint8_t value) {
    unsigned int nb_bytes = (matrix->width * matrix->height + 7) / 8;
    for (unsigned int i = 0; i < nb_bytes; i++) {
        matrix->data[i] = value ? 0xFF : 0x00;
    }
}