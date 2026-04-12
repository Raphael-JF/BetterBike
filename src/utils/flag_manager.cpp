#include <stdlib.h>
#include <Arduino.h>

#include "utils/binary_matrix.h"
#include "utils/flag_manager.h"


struct flag_manager* create_flag_manager(uint8_t num_flags) {
    struct flag_manager* fm = (struct flag_manager*)malloc(sizeof(struct flag_manager));
    fm->flags = create_bin_matrix(1, num_flags);
    return fm;
}

void set_flag(struct flag_manager* fm, uint8_t flag_index){
    set_pixel_bin_matrix(fm->flags, 0, flag_index, 1);
}

void clear_flag(struct flag_manager* fm, uint8_t flag_index){
    set_pixel_bin_matrix(fm->flags, 0, flag_index, 0);
}

void clear_all_flags(struct flag_manager* fm){
    fill_matrix(fm->flags, 0);
}

uint8_t is_flag_set(struct flag_manager* fm, uint8_t flag_index){
    return get_pixel_bin_matrix(fm->flags, 0, flag_index);
}

void free_flag_manager(struct flag_manager* fm) {
    free_bin_matrix(fm->flags);
    free(fm);
}


void print_flags(struct flag_manager* fm, uint8_t num_flags) {
    for (uint8_t i = 0; i < num_flags; i++) {
        Serial.print(i);
        Serial.print(":");
        Serial.print(is_flag_set(fm, i));
        Serial.print(" | ");
    }
    Serial.println();
}