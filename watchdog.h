/*
 * File: watchdog.h
 * Author: Ragib Asif
 * Email: ragib.asif30@myhunter.cuny.edu
 * GitHub: https://github.com/ragibasif
 * LinkedIn: https://www.linkedin.com/in/ragibasif/
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2025 Ragib Asif
 * Version 1.0.0
 */

#ifndef WATCHDOG_H_
#define WATCHDOG_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "common.h"

struct w_alloc_node {
    void *pointer;
    size_t size;
    char *file;
    unsigned int line;
    char *function;
    bool freed;
    struct w_alloc_node *prev;
    struct w_alloc_node *next;
};

struct w_freed_node {
    struct w_alloc_node *alloc;
    char *file;
    unsigned int line;
    char *function;
    struct w_freed_node *prev;
    struct w_freed_node *next;
};

struct watchdog {
    struct w_alloc_node *alloc_head;
    struct w_freed_node *freed_head;
    size_t total_bytes_alloc;
    size_t total_bytes_freed;
    size_t total_allocations;
    size_t total_frees;
};

extern void w_create(void);
extern void *w_malloc(size_t size, const char *file, unsigned int line,
                      const char *function);
extern void *w_realloc(void);
extern void *w_calloc(void);
extern void w_free(void *pointer, const char *file, unsigned int line,
                   const char *function);
extern void w_report(void);
extern void w_dump(void);
extern void w_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // WATCHDOG_H_
