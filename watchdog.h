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

struct w_location {
    char *file;
    unsigned int line;
    char *function;
};

// DONE
extern struct w_location *w_location_create(struct w_location *w_location,
                                            const char *file, unsigned int line,
                                            const char *function);

// DONE
extern void w_location_destroy(struct w_location *w_location);

struct w_buffer {
    void *pointer;
    size_t size;
};

// DONE
extern struct w_buffer *w_buffer_create(struct w_buffer *buffer, size_t size);

// DONE
extern void w_buffer_destroy(struct w_buffer *buffer);

struct w_alloc_node {
    struct w_buffer *buffer;
    struct w_location *location;
    struct w_alloc_node *prev;
    struct w_alloc_node *next;
};

extern struct w_alloc_node *w_alloc_node_create(struct w_alloc_node *node);
extern void w_alloc_node_destroy(struct w_alloc_node *node);

struct w_freed_node {
    struct w_alloc_node *allocs;
    struct w_location *location;
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
extern void w_free(void);
extern void w_report(void);
extern void w_dump(void);
extern void w_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // WATCHDOG_H_
