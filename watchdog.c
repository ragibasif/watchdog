/*
 * File: watchdog.c
 * Author: Ragib Asif
 * Email: ragib.asif30@myhunter.cuny.edu
 * GitHub: https://github.com/ragibasif
 * LinkedIn: https://www.linkedin.com/in/ragibasif/
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2025 Ragib Asif
 * Version 1.0.0
 */

#include "watchdog.h"

struct w_location *w_location_create(struct w_location *location,
                                     const char *file, unsigned int line,
                                     const char *function) {
    location = malloc(sizeof(*location));
    location->file = malloc((strlen(file) + 1) * sizeof(*location->file));
    memcpy(location->file, file, strlen(file));
    location->file[strlen(file)] = '\0';
    location->line = line;
    location->function =
        malloc((strlen(function) + 1) * sizeof(*location->function));
    memcpy(location->function, function, strlen(function));
    location->function[strlen(function)] = '\0';
    return location;
}

void w_location_destroy(struct w_location *location) {
    free(location->file);
    location->file = NULL;
    free(location->function);
    location->function = NULL;
    location->line = 0;
    free(location);
    location = NULL;
}

struct w_buffer *w_buffer_create(struct w_buffer *buffer, size_t size) {
    buffer = malloc(sizeof(*buffer));
    buffer->pointer = malloc(size * sizeof(*buffer->pointer));
    return buffer;
}

void w_buffer_destroy(struct w_buffer *buffer) {
    free(buffer->pointer);
    buffer->pointer = NULL;
    buffer->size = 0;
    free(buffer);
    buffer = NULL;
}

static struct watchdog vm;

void w_create(void) {
    vm.alloc_head = NULL;
    vm.alloc_head->prev = NULL;
    vm.alloc_head->next = NULL;
    vm.freed_head = NULL;
    vm.total_bytes_alloc = 0;
    vm.total_bytes_freed = 0;
    vm.total_allocations = 0;
    vm.total_frees = 0;
}

void *w_malloc(size_t size, const char *file, unsigned int line,
               const char *function) {
    struct w_buffer *buffer;
    buffer = w_buffer_create(buffer, size);
    struct w_location *location;
    location = w_location_create(location, file, line, function);

    struct w_alloc_node *alloc_node;
    alloc_node = w_alloc_node_create(alloc_node);
    alloc_node->buffer = buffer;
    alloc_node->location = location;

    return buffer->pointer;
}

struct w_alloc_node *w_alloc_node_create(struct w_alloc_node *node) {
    node = malloc(sizeof(*node));
    node->next = NULL;
    node->prev = NULL;
    return node;
}
void w_alloc_node_destroy(struct w_alloc_node *node) {
    free(node);
    node = NULL;
}

void *w_realloc(void);
void *w_calloc(void);
void w_free(void);
void w_report(void);
void w_dump(void);
void w_destroy(void);
