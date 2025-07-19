#define WATCHDOG_ENABLE
#include "watchdog.h"

#include <stdio.h>
#include <stdlib.h>

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {

    bool enable_verbose_log  = true;
    bool log_to_file         = false;
    bool enable_color_output = true;

    w_init(enable_verbose_log, log_to_file, enable_color_output);

    size_t ccount = 5;

    int *a = calloc(ccount, sizeof *a);

    for (size_t i = 0; i < ccount; i++) {
        printf("%d ", a[i]);
    }
    putchar('\n');
    free(a);
}
