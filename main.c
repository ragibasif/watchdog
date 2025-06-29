/*
 * File: main.c
 * Author: Ragib Asif
 * Email: ragib.asif30@myhunter.cuny.edu
 * GitHub: https://github.com/ragibasif
 * LinkedIn: https://www.linkedin.com/in/ragibasif/
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2025 Ragib Asif
 * Version 1.0.0
 *
 *
 */

#include "third_party_c/internal_debug.h"
#include "watchdog.h"

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    int *a = w_malloc(1024 * sizeof(*a), __FILE__, __LINE__, __func__);

    forge_run();
    return 0;
}
