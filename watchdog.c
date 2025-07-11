/*
 * File: watchdog.c
 * Author: Ragib Asif
 * Email: 182296466+ragibasif@users.noreply.github.com
 * GitHub: https://github.com/ragibasif
 * LinkedIn: https://www.linkedin.com/in/ragibasif/
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2025 Ragib Asif
 * Version 1.0.0
 */

#include "watchdog.h"
#include "common.h"

struct w_alloc_node {
    void *ptr;
    size_t size;
    char *file;
    unsigned int line;
    char *func;
    bool mallocd;
    bool reallocd;
    bool callocd;
    bool freed;
    struct w_alloc_node *next;
};

struct watchdog {
    struct w_alloc_node *alloc_head;
    size_t total_allocations;
    size_t total_bytes_alloc;
    size_t total_frees;
    size_t total_bytes_freed;
};

static void w_alloc_check_internal(void *ptr, const size_t size,
                                   const char *file, const unsigned int line,
                                   const char *func) {
    if (!ptr) {
        fprintf(stderr,
                "[%s:%u:(%s)] Memory allocation error. Failed to allocate %lu "
                "bytes to memory address %p.\n",
                file, line, func, size, (void *)ptr);
        exit(EXIT_FAILURE);
    }
}

static struct watchdog *watchdog = NULL;

void w_create(void) {
    if (!watchdog) {
        watchdog = malloc(sizeof *watchdog);
        w_alloc_check_internal(watchdog, sizeof *watchdog, __FILE__, __LINE__,
                               __func__);
    } else {
        return;
    }
    watchdog->alloc_head = NULL;
    watchdog->total_bytes_alloc = 0;
    watchdog->total_bytes_freed = 0;
    watchdog->total_allocations = 0;
    watchdog->total_frees = 0;
}

static struct w_alloc_node *
w_alloc_node_create_internal(const size_t size, const char *file,
                             const unsigned int line, const char *func) {
    struct w_alloc_node *node;
    node = malloc(sizeof(*node));
    w_alloc_check_internal(node, sizeof(*node), __FILE__, __LINE__, __func__);
    node->next = NULL;

    node->size = size;
    node->ptr = malloc(node->size);
    w_alloc_check_internal(node->ptr, node->size, __FILE__, __LINE__, __func__);

    node->file = malloc((strlen(file) + 1) * sizeof(*node->file));
    w_alloc_check_internal(node->file, (strlen(file) + 1) * sizeof(*node->file),
                           __FILE__, __LINE__, __func__);
    memcpy(node->file, file, strlen(file));
    node->file[strlen(file)] = '\0';

    node->line = line;

    node->func = malloc((strlen(func) + 1) * sizeof(*node->func));
    w_alloc_check_internal(node->func, (strlen(func) + 1) * sizeof(*node->func),
                           __FILE__, __LINE__, __func__);
    memcpy(node->func, func, strlen(func));
    node->func[strlen(func)] = '\0';

    node->freed = false;

    return node;
}

static void w_alloc_node_destroy_internal(struct w_alloc_node *node) {
    free(node->func);
    node->func = NULL;
    free(node->file);
    node->file = NULL;
    if (!node->freed) {
        free(node->ptr);
        node->ptr = NULL;
    }
    free(node);
    node = NULL;
}

void *w_malloc(const size_t size, const char *file, const unsigned int line,
               const char *func) {
    w_create();
    struct w_alloc_node *node;
    node = w_alloc_node_create_internal(size, file, line, func);
    watchdog->total_allocations++;
    watchdog->total_bytes_alloc += size;

    if (!watchdog->alloc_head) {
        watchdog->alloc_head = node;
    } else {
        node->next = watchdog->alloc_head;
        watchdog->alloc_head = node;
    }

    return node->ptr;
}

void *w_realloc(void *ptr, size_t size, const char *file, unsigned int line,
                const char *func) {

    w_create();
    struct w_alloc_node *node;
    node = w_alloc_node_create_internal(size, file, line, func);
    watchdog->total_allocations++;
    watchdog->total_bytes_alloc += size;

    if (!watchdog->alloc_head) {
        watchdog->alloc_head = node;
    } else {
        node->next = watchdog->alloc_head;
        watchdog->alloc_head = node;
    }

    return node->ptr;
}

void *w_calloc(size_t count, size_t size, const char *file, unsigned int line,
               const char *func) {

    w_create();
    struct w_alloc_node *node;
    node = w_alloc_node_create_internal(size, file, line, func);
    watchdog->total_allocations++;
    watchdog->total_bytes_alloc += size;

    if (!watchdog->alloc_head) {
        watchdog->alloc_head = node;
    } else {
        node->next = watchdog->alloc_head;
        watchdog->alloc_head = node;
    }

    return node->ptr;
}

void w_free(void *ptr, const char *file, const unsigned int line,
            const char *func) {
    w_create();
}

// FIX: consistent formatting of output
void w_report(void) {
    w_create();
    printf("Watchdog Report\n");
    printf("Allocations: %zu\n", watchdog->total_allocations);
    printf("Allocated Bytes: %zu\n", watchdog->total_bytes_alloc);
    printf("Frees: %zu\n", watchdog->total_frees);
    printf("Freed Bytes: %zu\n", watchdog->total_bytes_freed);
}

void w_dump(void) {}

void w_destroy(void) {
    if (!watchdog) {
        return;
    }
    while (watchdog->alloc_head) {
        printf("[%s:%u(%s)] MEMORY LEAK DETECTED: At memory address %p of size "
               "%zu bytes.\n",
               watchdog->alloc_head->file, watchdog->alloc_head->line,
               watchdog->alloc_head->func, (void *)watchdog->alloc_head->ptr,
               watchdog->alloc_head->size);
        struct w_alloc_node *temp = watchdog->alloc_head->next;
        w_alloc_node_destroy_internal(watchdog->alloc_head);
        watchdog->alloc_head = temp;
    }
    watchdog->total_bytes_alloc = 0;
    watchdog->total_bytes_freed = 0;
    watchdog->total_allocations = 0;
    watchdog->total_frees = 0;
    free(watchdog);
    watchdog = NULL;
}
