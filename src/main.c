#include "watchdog.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {

    bool enable_verbose_log = true;
    bool log_to_file = false;
    bool enable_color_output = true;

    w_init(enable_verbose_log, log_to_file, enable_color_output);

    // out of bounds checking
    int *a = malloc(4 * sizeof(int));
    a[2] = 23; // fine
    a[4] = 14; // out of bounds error
    free(a);

    return EXIT_SUCCESS;
}
