#ifndef FLAG_MANAGER_H
#define FLAG_MANAGER_H

#include "utils/bin_matrix.h"

struct flag_manager {
    /** @brief Binary matrix storing flag states (0/1). */
    struct bin_matrix* flags;
};


/**
 * @brief Create a flag manager able to hold a given number of flags.
 * @param num_flags Number of flags to allocate.
 * @return Pointer to the allocated manager, or nullptr on failure.
 */
struct flag_manager* create_flag_manager(uint8_t num_flags);

/**
 * @brief Set a flag to 1.
 * @param fm Flag manager instance.
 * @param flag_index Index of the flag to set.
 */
void set_flag(struct flag_manager* fm, uint8_t flag_index);

/**
 * @brief Clear a flag to 0.
 * @param fm Flag manager instance.
 * @param flag_index Index of the flag to clear.
 */
void clear_flag(struct flag_manager* fm, uint8_t flag_index);

/**
 * @brief Clear all flags in the manager.
 * @param fm Flag manager instance.
 */
void clear_all_flags(struct flag_manager* fm);


/**
 * @brief Check whether a flag is set.
 * @param fm Flag manager instance.
 * @param flag_index Index of the flag to read.
 * @return 1 if the flag is set, 0 otherwise.
 */
uint8_t is_flag_set(struct flag_manager* fm, uint8_t flag_index);

/**
 * @brief Free the manager and its internal resources.
 * @param fm Flag manager instance.
 */
void free_flag_manager(struct flag_manager* fm);

/**
 * @brief Print the state of the first N flags (debug output).
 * @param fm Flag manager instance.
 * @param num_flags Number of flags to print.
 */
void print_flags(struct flag_manager* fm, uint8_t num_flags);


#endif // FLAG_MANAGER_H