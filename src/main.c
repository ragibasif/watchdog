#define WATCHDOG_ENABLE
#include "watchdog.h"

#include <stdio.h>
#include <stdlib.h>

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {

    bool enable_verbose_log  = true;
    bool log_to_file         = false;
    bool enable_color_output = true;

    w_init(enable_verbose_log, log_to_file, enable_color_output);
    int *a = malloc(34);
    free(a);
    a      = NULL;
    int *b = calloc(34, 43);
    free(b);
    b      = NULL;
    int *c = realloc(c, 3432);
    free(c);
    c      = NULL;
    int *d = malloc(34);
    free(d);
    d      = NULL;
    int *e = calloc(34, 43);
    free(e);
    e      = NULL;
    int *f = realloc(f, 3432);
    free(f);
    f      = NULL;
    int *g = malloc(34222);
    g      = realloc(g, 2342);
    g      = realloc(g, 2342342);
    g      = realloc(g, 2);
    g      = realloc(g, 0);
    g      = realloc(g, 10);
    free(g);

    return EXIT_SUCCESS;
}
