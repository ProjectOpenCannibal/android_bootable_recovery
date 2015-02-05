/*
 * If you need to access functions outside of libcot that aren't already
 * defined in the included headers, define them here instead of modifying
 * core recovery code.
 */

int get_menu_selection(const char* const * headers, const char* const * items, int menu_only, int initial_selection, Device* device);