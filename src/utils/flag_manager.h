#ifndef FLAG_MANAGER_H
#define FLAG_MANAGER_H

#include "utils/binary_matrix.h"

struct flag_manager {
    struct bin_matrix* flags;
};


struct flag_manager* create_flag_manager(uint8_t num_flags);
void set_flag(struct flag_manager* fm, uint8_t flag_index);
void clear_flag(struct flag_manager* fm, uint8_t flag_index);
void clear_all_flags(struct flag_manager* fm);
uint8_t is_flag_set(struct flag_manager* fm, uint8_t flag_index);
void free_flag_manager(struct flag_manager* fm);
void print_flags(struct flag_manager* fm, uint8_t num_flags);


#endif // FLAG_MANAGER_H