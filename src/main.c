#include "watchdog.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {

    bool enable_verbose_log = true;
    bool log_to_file = false;
    bool enable_color_output = true;

    w_init(enable_verbose_log, log_to_file, enable_color_output);

    return EXIT_SUCCESS;
}
