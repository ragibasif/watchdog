/*
 * File: main.c
 * Author: Ragib Asif
 * Email: 182296466+ragibasif@users.noreply.github.com
 * GitHub: https://github.com/ragibasif
 * LinkedIn: https://www.linkedin.com/in/ragibasif/
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2025 Ragib Asif
 * Version 1.0.0
 *
 *
 */

// #include "third_party_c/internal_debug.h"
#include "watchdog.h"

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    w_create();
    int *a = w_malloc(10 * sizeof(*a), __FILE__, __LINE__, __func__);
    a = w_malloc(20 * sizeof(*a), __FILE__, __LINE__, __func__);
    int *b = w_malloc(1024 * sizeof(*a), __FILE__, __LINE__, __func__);
    int *c = w_malloc(1024 * sizeof(*a), __FILE__, __LINE__, __func__);
    int *d = w_malloc(1024 * sizeof(*a), __FILE__, __LINE__, __func__);
    int *e = w_malloc(1024 * sizeof(*a), __FILE__, __LINE__, __func__);
    int *f = w_malloc(1024 * sizeof(*a), __FILE__, __LINE__, __func__);
    w_free(a, __FILE__, __LINE__, __func__);
    w_free(b, __FILE__, __LINE__, __func__);
    w_free(c, __FILE__, __LINE__, __func__);
    w_free(d, __FILE__, __LINE__, __func__);
    w_free(e, __FILE__, __LINE__, __func__);
    w_free(f, __FILE__, __LINE__, __func__);

    // forge_run();
    return 0;
}
