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

// TODO: implement thread safety
// FIX: add unit tests

struct w_alloc_node {
    void *ptr;
    size_t size;
    char *file;
    unsigned int line;
    char *func;
    bool freed;
    struct w_alloc_node *next;
};

struct w_freed_node {
    struct w_alloc_node *alloc;
    char *file;
    unsigned int line;
    char *func;
    struct w_freed_node *next;
};

struct watchdog {
    struct w_alloc_node *alloc_head;
    struct w_freed_node *freed_head;
    size_t total_bytes_alloc;
    size_t total_bytes_freed;
    size_t total_allocations;
    size_t total_frees;
    bool watchdog_initialized;
};

static struct watchdog watchdog;

void w_create(void) {
    watchdog.alloc_head = NULL;
    watchdog.freed_head = NULL;
    watchdog.total_bytes_alloc = 0;
    watchdog.total_bytes_freed = 0;
    watchdog.total_allocations = 0;
    watchdog.total_frees = 0;
    watchdog.watchdog_initialized = true;
}

static void w_initialized_check_internal(const char *file,
                                         const unsigned int line,
                                         const char *func) {
    if (!watchdog.watchdog_initialized) {
        fprintf(stderr, "[%s:%u:(%s)] w_create() must be called first.\n", file,
                line, func);
        exit(EXIT_FAILURE);
    }
}

static void w_alloc_check_internal(void *ptr, const char *file,
                                   const unsigned int line, const char *func) {
    if (!ptr) {
        fprintf(stderr, "[%s:%u:(%s)] Memory allocation error.\n", file, line,
                func);
        exit(EXIT_FAILURE);
    }
}

static struct w_alloc_node *
w_alloc_node_create_internal(const size_t size, const char *file,
                             const unsigned int line, const char *func) {
    struct w_alloc_node *node;
    node = malloc(sizeof(*node));
    w_alloc_check_internal(node, __FILE__, __LINE__, __func__);
    node->next = NULL;

    node->size = size;
    node->ptr = malloc(node->size);
    w_alloc_check_internal(node->ptr, __FILE__, __LINE__, __func__);

    node->file = malloc((strlen(file) + 1) * sizeof(*node->file));
    w_alloc_check_internal(node->file, __FILE__, __LINE__, __func__);
    memcpy(node->file, file, strlen(file));
    node->file[strlen(file)] = '\0';

    node->line = line;

    node->func = malloc((strlen(func) + 1) * sizeof(*node->func));
    w_alloc_check_internal(node->func, __FILE__, __LINE__, __func__);
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

    // FIX: Check if watchdog_initialized is true
    // FIX: Check if allocation size exceeds SIZE_MAX
    // FIX: Check if allocation size is greater than 0
    // TODO: Check if its already been allocated
    // TODO: Check if its already been freed
    struct w_alloc_node *node;
    node = w_alloc_node_create_internal(size, file, line, func);
    watchdog.total_allocations++;
    watchdog.total_bytes_alloc += size;

    if (!watchdog.alloc_head) {
        watchdog.alloc_head = node;
    } else {
        node->next = watchdog.alloc_head;
        watchdog.alloc_head = node;
    }

    return node->ptr;
}

static struct w_freed_node *
w_freed_node_create_internal(const char *file, const unsigned int line,
                             const char *func) {

    struct w_freed_node *node;
    node = malloc(sizeof(*node));
    w_alloc_check_internal(node, __FILE__, __LINE__, __func__);
    node->next = NULL;

    node->file = malloc((strlen(file) + 1) * sizeof(*node->file));
    w_alloc_check_internal(node->file, __FILE__, __LINE__, __func__);
    memcpy(node->file, file, strlen(file));
    node->file[strlen(file)] = '\0';

    node->line = line;

    node->func = malloc((strlen(func) + 1) * sizeof(*node->func));
    w_alloc_check_internal(node->func, __FILE__, __LINE__, __func__);
    memcpy(node->func, func, strlen(func));
    node->func[strlen(func)] = '\0';

    return node;
}

static void w_freed_node_destroy_internal(struct w_freed_node *node) {
    free(node->func);
    node->func = NULL;
    free(node->file);
    node->file = NULL;
    free(node);
    node = NULL;
}

// FIX: rewrite this function from scratch
void w_free(void *ptr, const char *file, const unsigned int line,
            const char *func) {

    if (!watchdog.freed_head) {

        struct w_freed_node *node;
        node = w_freed_node_create_internal(file, line, func);

        struct w_alloc_node *temp;
        temp = watchdog.alloc_head;
        // TODO: this should be a separate helper function
        while (temp) {
            if (temp->ptr == ptr) {
                node->alloc = temp;
                free(node->alloc->ptr);
            }
            temp = temp->next;
        }
        watchdog.freed_head = node;
    } else {
        struct w_freed_node *temp;
        struct w_freed_node *tail;
        temp = watchdog.freed_head;
        // FIX: double check and test this logic, might be broken
        // TODO: separate helper function to check for double free
        while (temp) {
            if (temp->alloc->ptr == ptr) {
                fprintf(stderr, "Error: Double free.\n");
                exit(EXIT_FAILURE);
            }
            if (!temp->next) {
                tail = temp;
            }
            temp = temp->next;
        }

        struct w_freed_node *node;
        node = w_freed_node_create_internal(file, line, func);

        struct w_alloc_node *temp2;
        temp2 = watchdog.alloc_head;
        while (temp2) {
            if (temp2->ptr == ptr) {
                node->alloc = temp2;
                free(node->alloc->ptr);
            }
            temp2 = temp2->next;
        }
        tail->next = node;
    }
}

// FIX: atm this is just a copy and paste of malloc,
//      rewrite this to take into account the pointer that is passed
void *w_realloc(void *ptr, size_t size, const char *file, unsigned int line,
                const char *func) {
    // FIX: check if pointer exists
    // FIX: check if pointer is freed
    // FIX: find existing allocation
    // FIX: handle special cases (size == 0, ptr == NULL)
    struct w_alloc_node *node;
    node = w_alloc_node_create_internal(size, file, line, func);
    watchdog.total_allocations++;
    watchdog.total_bytes_alloc += size;

    if (!watchdog.alloc_head) {
        watchdog.alloc_head = node;
    } else {
        node->next = watchdog.alloc_head;
        watchdog.alloc_head = node;
    }

    return node->ptr;
}

// FIX: atm this is just a copy and paste of malloc,
//      rewrite this to take into account the count number
void *w_calloc(size_t count, size_t size, const char *file, unsigned int line,
               const char *func) {
    struct w_alloc_node *node;
    node = w_alloc_node_create_internal(size, file, line, func);
    watchdog.total_allocations++;
    watchdog.total_bytes_alloc += size;

    if (!watchdog.alloc_head) {
        watchdog.alloc_head = node;
    } else {
        node->next = watchdog.alloc_head;
        watchdog.alloc_head = node;
    }

    return node->ptr;
}

void w_report(void) {
    // TODO: print total allocations, frees, and size
}
void w_dump(void) {
    // TODO: Verbose memory dump of all allocations and frees
}
void w_destroy(void) {
    // TODO: make sure all allocated memory if freed
}
