/*
 * File: main.c
 * Author: Ragib Asif
 * GitHub: https://github.com/ragibasif
 * LinkedIn: https://www.linkedin.com/in/ragibasif/
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2025 Ragib Asif
 * Version 1.0.0
 */

#define WATCHDOG_ENABLE
#include "../src/watchdog.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void malloc_example(void);
static void calloc_example(void);
static void realloc_example(void);
static void free_example(void);
static void leak_example(void);
static void double_free_example(void);
static void overflow_example(void);
static void invalid_free_example(void);

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {

    bool enable_verbose_log  = true;
    bool log_to_file         = false;
    bool enable_color_output = true;

    w_init(enable_verbose_log, log_to_file, enable_color_output);

    malloc_example();
    calloc_example();
    realloc_example();
    free_example();
    leak_example();
    double_free_example();
    overflow_example();
    invalid_free_example();

    return EXIT_SUCCESS;
}

void malloc_example(void) {

    size_t count  = 5;
    int   *buffer = malloc(sizeof *buffer * 5);
    for (size_t i = 0; i < count; i++) {
        buffer[i] = -(i - count) * count;
    }
    for (size_t i = 0; i < count; i++) {
        printf("%d ", buffer[i]);
    }

    putchar('\n');
    free(buffer);
    buffer = NULL;
}

void realloc_example(void) {
    short *buffer = malloc(34222);
    buffer        = realloc(buffer, 2342);
    buffer        = realloc(buffer, 2342342);
    buffer        = realloc(buffer, 2);
    buffer        = realloc(buffer, 10);
    buffer        = realloc(buffer, 0);
    free(buffer);
    buffer = NULL;
}

void calloc_example(void) {
    // Demonstrates correct usage of calloc
    size_t count  = 5;
    int   *buffer = calloc(count, sizeof *buffer);
    for (size_t i = 0; i < count; i++) {
        printf("%d ", buffer[i]); // should output 0
    }
    putchar('\n');
    free(buffer);
    buffer = NULL;
}

void free_example(void) {
    // Demonstrates correct usage of free
    size_t         count   = 5;
    short int     *buffer0 = malloc(sizeof *buffer0 * count);
    unsigned char *buffer1 = calloc(count, sizeof *buffer1);
    long double   *buffer2 = realloc(buffer2, sizeof *buffer2 * count);
    free(buffer1);
    buffer1 = NULL;
    free(buffer2);
    buffer2 = NULL;
    free(buffer0);
    buffer1 = NULL;
}

void leak_example(void) {
    unsigned long long int *buffer = malloc(sizeof *buffer * 20);
    // intentionally not calling free
    // watchdog will detect this and report it
}

void double_free_example(void) {
    char **buffer = malloc(sizeof *buffer * 20);
    free(buffer);
    free(buffer); // triggers a double-free error
}

void overflow_example(void) {
    char *buffer = malloc(sizeof *buffer * 10);
    strcpy(buffer, "This will overflow"); // out-of-bounds write
    // of buffer overflows will be detected with using canary values
}

void invalid_free_example(void) {
    float *buffer;
    free(buffer); // will trigger an error since buffer wasn't allocated
}
