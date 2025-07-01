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
#include "common.h"

static struct watchdog vm;

void w_create(void) {
    vm.alloc_head = NULL;
    vm.freed_head = NULL;
    vm.total_bytes_alloc = 0;
    vm.total_bytes_freed = 0;
    vm.total_allocations = 0;
    vm.total_frees = 0;
}

static void w_alloc_check_internal(void *pointer, const char *file,
                                   unsigned int line, const char *function) {
    if (!pointer) {
        fprintf(stderr, "[%s:%u:(%s)] Memory allocation error.\n", file, line,
                function);
        exit(EXIT_FAILURE);
    }
}

static struct w_alloc_node *w_alloc_node_create(struct w_alloc_node *node,
                                                size_t size, const char *file,
                                                unsigned int line,
                                                const char *function) {
    node = malloc(sizeof(*node));
    w_alloc_check_internal(node, __FILE__, __LINE__, __func__);
    node->next = NULL;
    node->prev = NULL;

    node->size = size;
    dbg(node->pointer);
    node->pointer = malloc(node->size);
    dbg(node->pointer);
    w_alloc_check_internal(node->pointer, __FILE__, __LINE__, __func__);

    node->file = malloc((strlen(file) + 1) * sizeof(*node->file));
    w_alloc_check_internal(node->file, __FILE__, __LINE__, __func__);
    memcpy(node->file, file, strlen(file));
    node->file[strlen(file)] = '\0';

    node->line = line;

    node->function = malloc((strlen(function) + 1) * sizeof(*node->function));
    w_alloc_check_internal(node->function, __FILE__, __LINE__, __func__);
    memcpy(node->function, function, strlen(function));
    node->function[strlen(function)] = '\0';

    node->freed = false;

    return node;
}

void w_alloc_node_destroy(struct w_alloc_node *node) {
    free(node->function);
    node->function = NULL;
    free(node->file);
    node->file = NULL;
    if (!node->freed) {
        free(node->pointer);
        node->pointer = NULL;
    }
    free(node);
    node = NULL;
}

void *w_malloc(size_t size, const char *file, unsigned int line,
               const char *function) {

    // TODO: Check if its already been allocated
    // TODO: Check if its already been freed
    struct w_alloc_node *node;
    node = w_alloc_node_create(node, size, file, line, function);

    if (!vm.alloc_head) {
        vm.alloc_head = node;
    } else {
        struct w_alloc_node *temp;
        temp = vm.alloc_head;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = node;
        node->prev = temp;
    }

    return node->pointer;
}

void *w_realloc(void);
void *w_calloc(void);

void w_free(void *pointer, const char *file, unsigned int line,
            const char *function) {

    if (!vm.freed_head) {
        struct w_freed_node *node;
        node = malloc(sizeof(*node));
        w_alloc_check_internal(node, __FILE__, __LINE__, __func__);
        node->next = NULL;
        node->prev = NULL;

        node->file = malloc((strlen(file) + 1) * sizeof(*node->file));
        w_alloc_check_internal(node->file, __FILE__, __LINE__, __func__);
        memcpy(node->file, file, strlen(file));
        node->file[strlen(file)] = '\0';

        node->line = line;

        node->function =
            malloc((strlen(function) + 1) * sizeof(*node->function));
        w_alloc_check_internal(node->function, __FILE__, __LINE__, __func__);
        memcpy(node->function, function, strlen(function));
        node->function[strlen(function)] = '\0';

        struct w_alloc_node *temp;
        temp = vm.alloc_head;
        while (temp) {
            if (temp->pointer == pointer) {
                node->alloc = temp;
                free(node->alloc->pointer);
                dbg(temp->pointer);
                dbg(node->alloc->pointer);
                dbg(pointer);
            }
            temp = temp->next;
        }
        vm.freed_head = node;
    } else {
        struct w_freed_node *temp;
        struct w_freed_node *tail;
        temp = vm.freed_head;
        while (temp) {
            if (temp->alloc->pointer == pointer) {
                dbg(temp->alloc->pointer);
                dbg(pointer);
                dbg(EXIT_FAILURE);
            }
            if (!temp->next) {
                tail = temp;
            }
            temp = temp->next;
        }

        struct w_freed_node *node;
        node = malloc(sizeof(*node));
        w_alloc_check_internal(node, __FILE__, __LINE__, __func__);
        node->next = NULL;
        node->prev = NULL;

        node->file = malloc((strlen(file) + 1) * sizeof(*node->file));
        w_alloc_check_internal(node->file, __FILE__, __LINE__, __func__);
        memcpy(node->file, file, strlen(file));
        node->file[strlen(file)] = '\0';

        node->line = line;

        node->function =
            malloc((strlen(function) + 1) * sizeof(*node->function));
        w_alloc_check_internal(node->function, __FILE__, __LINE__, __func__);
        memcpy(node->function, function, strlen(function));
        node->function[strlen(function)] = '\0';

        struct w_alloc_node *temp2;
        temp2 = vm.alloc_head;
        while (temp2) {
            if (temp2->pointer == pointer) {
                node->alloc = temp2;
                dbg(temp2->pointer);
                dbg(node->alloc->pointer);
                dbg(pointer);
                free(node->alloc->pointer);
            }
            temp2 = temp2->next;
        }
        tail->next = node;
        node->prev = tail;
    }
}
void w_report(void);
void w_dump(void);
void w_destroy(void);
