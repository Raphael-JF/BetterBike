#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "binary_matrix.h"

struct bin_matrix* create_bin_matrix(unsigned int width,unsigned int height){
    struct bin_matrix* matrix = (struct bin_matrix*)malloc(sizeof(struct bin_matrix));
    if (!matrix) return NULL;

    matrix->width = width;
    matrix->height = height;
    unsigned int byte_width = (width + 7) / 8; // nombre de bytes nécessaires pour stocker une ligne
    matrix->data = (uint8_t*)calloc(byte_width * height, sizeof(uint8_t)); // alloue et initialise à 0
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

void set_pixel(struct bin_matrix* matrix, unsigned int x, unsigned int y, uint8_t value){
    if (x >= matrix->width || y >= matrix->height) return;
    unsigned int byte_index = (y * matrix->width + x) / 8;
    unsigned int bit_index = (y * matrix->width + x) % 8;
    if (value) {
        matrix->data[byte_index] |= (1 << bit_index);
    } else {
        matrix->data[byte_index] &= ~(1 << bit_index);
    }
}

uint8_t get_pixel(struct bin_matrix* matrix, unsigned int x, unsigned int y){
    if (x >= matrix->width || y >= matrix->height) return 0;
    unsigned int byte_index = (y * matrix->width + x) / 8;
    unsigned int bit_index = (y * matrix->width + x) % 8;
    return (matrix->data[byte_index] >> bit_index) & 1;
}

int main() {
    struct bin_matrix* matrix = create_bin_matrix(16, 15);
    set_pixel(matrix, 3, 4, 1);
    set_pixel(matrix, 5, 6, 1);
    printf("Pixel (3,4): %d\n", get_pixel(matrix, 3, 4)); // devrait afficher 1
    printf("Pixel (5,6): %d\n", get_pixel(matrix, 5, 6)); // devrait afficher 1
    printf("Pixel (0,0): %d\n", get_pixel(matrix, 0, 0)); // devrait afficher 0
    free_bin_matrix(matrix);
    return 0;
}