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

////////////////////////////////////////////////////////////////////////////////
// Watchdog Internals Declarations
////////////////////////////////////////////////////////////////////////////////

typedef struct {
    void *ptr;
    size_t size;
    const char *file;
    unsigned int line;
    const char *func;
    bool freed;
} WAllocationMetadata;

static void w_finalize(void);
static void w_alloc_check_internal(void *ptr, const size_t size,
                                   const char *file, const int line,
                                   const char *func);
static void w_alloc_max_size_check_internal(const size_t size, const char *file,
                                            const int line, const char *func);
static void w_allocation_metadata_create_internal(void *ptr, const size_t size,
                                                  const char *file,
                                                  const int line,
                                                  const char *func);
static void
w_reallocation_metadata_create_internal(void *old_ptr, void *new_ptr,
                                        const size_t new_size, const char *file,
                                        const int line, const char *func);
static void w_deallocation_metadata_update_internal(void *ptr, const char *file,
                                                    const int line,
                                                    const char *func);
static void w_check_initialization_internal(void);
static void w_report_memory_leak_internal(void);

// static void w_alloc_node_destroy_internal(struct w_alloc_node *node);
// static void w_alloc_node_list_print_internal(struct w_alloc_node *head);

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Watchdog Internal Dynamic Array Declarations
////////////////////////////////////////////////////////////////////////////////

#define WDA_DEFAULT_BUFFER_SIZE 10
#define WDA_GROWTH_FACTOR 2

typedef struct {
    WAllocationMetadata **buffer;
    size_t size;
    size_t capacity;
} WDynamicArray;

static void w_da_init(WDynamicArray *w_da);
static void w_da_push(WDynamicArray *w_da, void *ptr);
static void w_da_pop(WDynamicArray *w_da);
static void w_da_insert(WDynamicArray *w_da, size_t index, void *ptr);
static void w_da_remove(WDynamicArray *w_da, size_t index);
static void w_da_print(WDynamicArray *w_da);
static void w_da_cleanup(WDynamicArray *w_da);
static bool w_da_index_in_bounds_check_internal(WDynamicArray *w_da,
                                                size_t index);
static void w_da_expand_capacity_internal(WDynamicArray *w_da);

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Watchdog Global Variables
////////////////////////////////////////////////////////////////////////////////

static pthread_mutex_t w_mutex = PTHREAD_MUTEX_INITIALIZER;
static FILE *w_log_file = NULL;
static WDynamicArray watchdog;

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Watchdog API Definitions
////////////////////////////////////////////////////////////////////////////////

void w_init(const char *file) {
    dbg(__func__);
    pthread_mutex_lock(&w_mutex);
    if (file) {
        w_log_file = fopen(file, "a");
        if (!w_log_file) {
            fprintf(stderr, "Failed to open log file: %s\n", file);
            pthread_mutex_unlock(&w_mutex);
            exit(EXIT_FAILURE);
        }
    } else {
        w_log_file = stdout; // default to standard output
    }
    w_da_init(&watchdog);
    atexit(w_finalize);
    pthread_mutex_unlock(&w_mutex);
}

void w_finalize(void) {
    dbg(__func__);
    pthread_mutex_lock(&w_mutex);
    if (w_log_file && w_log_file != stdout) {
        fclose(w_log_file);
        w_log_file = NULL;
    }

    w_report_memory_leak_internal();
    w_da_cleanup(&watchdog);

    pthread_mutex_unlock(&w_mutex);
}

// API

void *w_malloc(const size_t size, const char *file, const int line,
               const char *func) {

    w_check_initialization_internal();

    w_alloc_max_size_check_internal(size, file, line, func);

    pthread_mutex_lock(&w_mutex);
    if (!size) {
        pthread_mutex_unlock(&w_mutex);
        return NULL;
    }

    void *ptr = malloc(size);
    w_alloc_check_internal(ptr, size, __FILE__, __LINE__, __func__);

    w_allocation_metadata_create_internal(ptr, size, file, line, func);
    pthread_mutex_unlock(&w_mutex);

    return ptr;
}

// void *w_realloc(void *ptr, size_t size, const char *file, const int line,
//                 const char *func) {
//
//     w_create_internal();
//     if (!size) {
//         w_free(ptr, file, line, func);
//         return NULL;
//     }
//     if (size == SIZE_MAX) {
//         fprintf(stderr,
//                 "[%s:%u:(%s)] Out of memory error. %zu bytes exceeds
// maximum
//                 " "allowed memory allocation.\n", file, line, func,
//                 size);
//         exit(EXIT_FAILURE);
//     }
//     struct w_alloc_node *node;
//     node = w_alloc_node_create_internal(size, file, line, func);
//
//     if (!watchdog->alloc_head) {
//         watchdog->alloc_head = node;
//     } else {
//         node->next = watchdog->alloc_head;
//         watchdog->alloc_head = node;
//     }
//
//     return node->ptr;
// }
//
// void *w_calloc(size_t count, size_t size, const char *file, const int
// line,
//                const char *func) {
//
//     w_create_internal();
//     struct w_alloc_node *node;
//     node = w_alloc_node_create_internal(size, file, line, func);
//
//     if (!watchdog->alloc_head) {
//         watchdog->alloc_head = node;
//     } else {
//         node->next = watchdog->alloc_head;
//         watchdog->alloc_head = node;
//     }
//
//     return node->ptr;
// }
//
// // TODO: implement
// void w_free(void *ptr, const char *file, const int line, const char *func)
// {
//     w_create_internal();
// }
//

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Watchdog Internals Definitions
////////////////////////////////////////////////////////////////////////////////

static void w_alloc_check_internal(void *ptr, const size_t size,
                                   const char *file, const int line,
                                   const char *func) {
    if (!ptr) {
        fprintf(stderr,
                "[%s:%u:(%s)] Memory allocation error. Failed to allocate %lu "
                "bytes to memory address %p.\n",
                file, line, func, size, (void *)ptr);
        pthread_mutex_unlock(&w_mutex);
        exit(EXIT_FAILURE);
    }
}

static void w_alloc_max_size_check_internal(const size_t size, const char *file,
                                            const int line, const char *func) {
    if (size == SIZE_MAX) {
        fprintf(stderr,
                "[%s:%u:(%s)] Out of memory error. %zu bytes exceeds maximum "
                " allowed memory allocation.\n ",
                file, line, func, size);
        pthread_mutex_unlock(&w_mutex);
        exit(EXIT_FAILURE);
    }
}

static void w_allocation_metadata_create_internal(void *ptr, const size_t size,
                                                  const char *file,
                                                  const int line,
                                                  const char *func) {
    WAllocationMetadata *data = malloc(sizeof *data);
    w_alloc_check_internal(data, sizeof *data, __FILE__, __LINE__, __func__);

    data->ptr = ptr;
    data->size = size;
    data->file = file;
    data->line = line;
    data->func = func;
    data->freed = false;

    w_da_push(&watchdog, data);
}

static void
w_reallocation_metadata_create_internal(void *old_ptr, void *new_ptr,
                                        const size_t new_size, const char *file,
                                        const int line, const char *func) {
    for (size_t i = 0; i < watchdog.size; i++) {
        if (watchdog.buffer[i]->ptr == old_ptr && !watchdog.buffer[i]->freed) {
            watchdog.buffer[i]->ptr = new_ptr;
            watchdog.buffer[i]->size = new_size;
            watchdog.buffer[i]->file = file;
            watchdog.buffer[i]->line = line;
            watchdog.buffer[i]->func = func;
            return;
        }
    }

    // If old address is not found, treat as a new allocation
    w_allocation_metadata_create_internal(new_ptr, new_size, file, line, func);
}

static void w_deallocation_metadata_update_internal(void *ptr, const char *file,
                                                    const int line,
                                                    const char *func) {
    for (size_t i = 0; i < watchdog.size; i++) {
        if (watchdog.buffer[i]->ptr == ptr && !watchdog.buffer[i]->freed) {
            watchdog.buffer[i]->freed = true;
            return;
        }
    }
    printf(
        "[WARNING] Attempt to free unallocated or already freed memory: %p\n",
        ptr);
}

static void w_check_initialization_internal(void) {
    if (!w_log_file) {
        w_init(NULL);
    }
}

static void w_report_memory_leak_internal(void) {
    for (int i = 0; i < watchdog.size; i++) {
        if (!watchdog.buffer[i]->freed) {
            printf("[LEAK] %zu bytes at %p (allocated at %s:%d)\n",
                   watchdog.buffer[i]->size, watchdog.buffer[i]->ptr,
                   watchdog.buffer[i]->file, watchdog.buffer[i]->line);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

//
// // Internals

// static void w_alloc_node_destroy_internal(struct w_alloc_node *node);
// static void w_alloc_node_list_print_internal(struct w_alloc_node *head);
//

// static void w_alloc_node_destroy_internal(struct w_alloc_node *node) {
//     if (!node->freed) {
//         free(node->ptr);
//         node->ptr = NULL;
//     }
//     free(node);
//     node = NULL;
// }

// // Internals
//
// static void w_alloc_node_list_print_internal(struct w_alloc_node *head) {
//     if (!head) {
//         return;
//     }
//
//     w_alloc_node_list_print_internal(head->next);
//     fputs("\n--------------------", stderr);
//     fprintf(stderr, "\nLocation: %s %d %s()\n", head->file, head->line,
//             head->func);
//     fprintf(stderr, "Memory Address: %p\n", (void *)head->ptr);
//     fprintf(stderr, "Size (Bytes): %zu\n", head->size);
//     fprintf(stderr, "Freed: ");
//     fputs(head->freed ? "true\n" : "false\n", stderr);
//     fputs("--------------------\n", stderr);
// }

////////////////////////////////////////////////////////////////////////////////
// Watchdog Internal Dynamic Array API Definitions
////////////////////////////////////////////////////////////////////////////////

void w_da_init(WDynamicArray *w_da) {
    w_da->size = 0;
    w_da->capacity = WDA_DEFAULT_BUFFER_SIZE;
    w_da->buffer = malloc(sizeof *w_da->buffer * w_da->capacity);
    w_alloc_check_internal(w_da->buffer, sizeof *w_da->buffer * w_da->capacity,
                           __FILE__, __LINE__, __func__);
}

void w_da_push(WDynamicArray *w_da, void *ptr) {
    if (w_da->size == w_da->capacity) {
        w_da_expand_capacity_internal(w_da);
    }
    w_da->buffer[w_da->size++] = ptr;
}

void w_da_pop(WDynamicArray *w_da) {
    if (!(w_da->size > 0)) {
        return;
    }
    w_da->size--;
    free(w_da->buffer[w_da->size]);
    w_da->buffer[w_da->size] = NULL;
}

void w_da_insert(WDynamicArray *w_da, size_t index, void *ptr) {
    if (!w_da_index_in_bounds_check_internal(w_da, index)) {
        exit(EXIT_FAILURE);
    }
    if (w_da->size + 1 >= w_da->capacity) {
        w_da_expand_capacity_internal(w_da);
    }
    for (size_t i = w_da->size; i < index; i++) {
        w_da->buffer[i] = w_da->buffer[i - 1];
    }
    w_da->buffer[index] = ptr;
}

void w_da_remove(WDynamicArray *w_da, size_t index) {
    if (!w_da_index_in_bounds_check_internal(w_da, index)) {
        exit(EXIT_FAILURE);
    }
    free(w_da->buffer[index]);
    for (size_t i = index; i < w_da->size - 1; i++) {
        w_da->buffer[i] = w_da->buffer[i + 1];
    }
    w_da->size--;
}

void w_da_print(WDynamicArray *w_da) {
    for (size_t i = 0; i < w_da->size; i++) {
        printf("[%zu] %p\n", i, (WAllocationMetadata *)w_da->buffer[i]);
    }
}

void w_da_cleanup(WDynamicArray *w_da) {
    dbg(__func__);
    if (w_da->buffer) {
        free(w_da->buffer);
        w_da->buffer = NULL;
    }
    w_da->size = 0;
    w_da->capacity = 0;
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Watchdog Internal Dynamic Array Internals Definitions
////////////////////////////////////////////////////////////////////////////////

static bool w_da_index_in_bounds_check_internal(WDynamicArray *w_da,
                                                size_t index) {
    if (index >= 0 && index < w_da->size) {
        return true;
    }
    fprintf(stderr, "Index Out Of Bounds Error: %zu is out of bounds of %zu.\n",
            index, w_da->size);
    return false;
}

static void w_da_expand_capacity_internal(WDynamicArray *w_da) {
    w_da->capacity *= WDA_GROWTH_FACTOR;
    WAllocationMetadata **buffer =
        realloc(w_da->buffer, sizeof *w_da->buffer * w_da->capacity);
    w_alloc_check_internal(buffer, sizeof **w_da->buffer * w_da->capacity,
                           __FILE__, __LINE__, __func__);
    w_da->buffer = buffer;
}

////////////////////////////////////////////////////////////////////////////////
