/*
 * File: watchdog.c
 * Author: Ragib Asif
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
    bool freed;
    struct w_alloc_node *next;
};

struct watchdog {
    struct w_alloc_node *alloc_head;
};

// Internals

static void w_create_internal(void);
static void w_destroy_internal(void);
static void w_alloc_check_internal(void *ptr, const size_t size,
                                   const char *file, const int line,
                                   const char *func);
static struct w_alloc_node *w_alloc_node_create_internal(const size_t size,
                                                         const char *file,
                                                         const int line,
                                                         const char *func);
static void w_alloc_node_destroy_internal(struct w_alloc_node *node);
static void w_alloc_node_list_print_internal(struct w_alloc_node *head);

static struct watchdog *watchdog = NULL;

static void w_create_internal(void) {
    if (watchdog) {
        return;
    }
    watchdog = malloc(sizeof *watchdog);
    w_alloc_check_internal(watchdog, sizeof *watchdog, __FILE__, __LINE__,
                           __func__);
    watchdog->alloc_head = NULL;
    atexit(w_destroy_internal);
}

static void w_destroy_internal(void) {
    if (!watchdog) {
        return;
    }
    if (watchdog->alloc_head) {
        printf("MEMORY LEAKS DETECTED...\n");
        w_alloc_node_list_print_internal(watchdog->alloc_head);
        printf("...MEMORY LEAKS DETECTED\n");
    }
    while (watchdog->alloc_head) {
        // printf("[%s:%u(%s)] MEMORY LEAK: At memory address %p of size "
        //        "%zu bytes.\n",
        //        watchdog->alloc_head->file, watchdog->alloc_head->line,
        //        watchdog->alloc_head->func, (void *)watchdog->alloc_head->ptr,
        //        watchdog->alloc_head->size);
        struct w_alloc_node *temp = watchdog->alloc_head->next;
        w_alloc_node_destroy_internal(watchdog->alloc_head);
        watchdog->alloc_head = temp;
    }
    free(watchdog);
    watchdog = NULL;
}

static void w_alloc_check_internal(void *ptr, const size_t size,
                                   const char *file, const int line,
                                   const char *func) {
    if (!ptr) {
        fprintf(stderr,
                "[%s:%u:(%s)] Memory allocation error. Failed to allocate %lu "
                "bytes to memory address %p.\n",
                file, line, func, size, (void *)ptr);
        exit(EXIT_FAILURE);
    }
}

static struct w_alloc_node *w_alloc_node_create_internal(const size_t size,
                                                         const char *file,
                                                         const int line,
                                                         const char *func) {
    struct w_alloc_node *node;
    node = malloc(sizeof *node);
    w_alloc_check_internal(node, sizeof *node, __FILE__, __LINE__, __func__);
    node->next = NULL;

    node->size = size;
    node->ptr = malloc(sizeof *node->ptr * node->size);
    w_alloc_check_internal(node->ptr, node->size, __FILE__, __LINE__, __func__);

    node->file = malloc(sizeof *node->file * (strlen(file) + 1));
    w_alloc_check_internal(node->file, (strlen(file) + 1) * sizeof *node->file,
                           __FILE__, __LINE__, __func__);
    memcpy(node->file, file, strlen(file));
    node->file[strlen(file)] = '\0';

    node->line = line;

    node->func = malloc(sizeof *node->func * (strlen(func) + 1));
    w_alloc_check_internal(node->func, (strlen(func) + 1) * sizeof *node->func,
                           __FILE__, __LINE__, __func__);
    memcpy(node->func, func, strlen(func));
    node->func[strlen(func)] = '\0';

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

static void w_alloc_node_list_print_internal(struct w_alloc_node *head) {
    if (!head) {
        return;
    }

    w_alloc_node_list_print_internal(head->next);
    fputs("\n--------------------", stderr);
    fprintf(stderr, "\nLocation: %s %d %s()\n", head->file, head->line,
            head->func);
    fprintf(stderr, "Memory Address: %p\n", (void *)head->ptr);
    fprintf(stderr, "Size (Bytes): %zu\n", head->size);
    fprintf(stderr, "Freed: ");
    fputs(head->freed ? "true\n" : "false\n", stderr);
    fputs("--------------------\n", stderr);
}

// API

void *w_malloc(const size_t size, const char *file, const int line,
               const char *func) {
    w_create_internal();
    if (!size) {
        return NULL;
    }
    if (size == SIZE_MAX) {
        fprintf(stderr,
                "[%s:%u:(%s)] Out of memory error. %zu bytes exceeds maximum "
                "allowed memory allocation.\n",
                file, line, func, size);
        exit(EXIT_FAILURE);
    }
    struct w_alloc_node *node;
    node = w_alloc_node_create_internal(size, file, line, func);
    node->freed = false;

    if (!watchdog->alloc_head) {
        watchdog->alloc_head = node;
    } else {
        node->next = watchdog->alloc_head;
        watchdog->alloc_head = node;
    }

    return node->ptr;
}

void *w_realloc(void *ptr, size_t size, const char *file, const int line,
                const char *func) {

    w_create_internal();
    if (!size) {
        w_free(ptr, file, line, func);
        return NULL;
    }
    if (size == SIZE_MAX) {
        fprintf(stderr,
                "[%s:%u:(%s)] Out of memory error. %zu bytes exceeds maximum "
                "allowed memory allocation.\n",
                file, line, func, size);
        exit(EXIT_FAILURE);
    }
    struct w_alloc_node *node;
    node = w_alloc_node_create_internal(size, file, line, func);

    if (!watchdog->alloc_head) {
        watchdog->alloc_head = node;
    } else {
        node->next = watchdog->alloc_head;
        watchdog->alloc_head = node;
    }

    return node->ptr;
}

void *w_calloc(size_t count, size_t size, const char *file, const int line,
               const char *func) {

    w_create_internal();
    struct w_alloc_node *node;
    node = w_alloc_node_create_internal(size, file, line, func);

    if (!watchdog->alloc_head) {
        watchdog->alloc_head = node;
    } else {
        node->next = watchdog->alloc_head;
        watchdog->alloc_head = node;
    }

    return node->ptr;
}

// TODO: implement
void w_free(void *ptr, const char *file, const int line, const char *func) {
    w_create_internal();
}
