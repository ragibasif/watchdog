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

#define WATCHDOG_LOG(output, prefix, ptr, size, file, line, func)              \
    do {                                                                       \
        time_t now = time(NULL);                                               \
        char *time_str = ctime(&now);                                          \
        time_str[strlen(time_str) - 1] = '\0';                                 \
        if (output) {                                                          \
            FILE *log_file;                                                    \
            log_file = fopen("debug.log", "a");                                \
            fprintf(log_file, "%s [%s:%d (%s)] [%s]: %p = %zu\n", time_str,    \
                    file, line, func, prefix, ptr, size);                      \
            fclose(log_file);                                                  \
        } else {                                                               \
            fprintf(stdout, "%s [%s:%d (%s)] [%s]: %p = %zu\n", time_str,      \
                    file, line, func, prefix, ptr, size);                      \
        }                                                                      \
    } while (0) // print log

typedef struct WatchdogAllocationMetadata WAM;
struct WatchdogAllocationMetadata {
    void *ptr;
    size_t size;
    const char *file;
    unsigned int line;
    const char *func;
    bool freed;
};

static void w_finalize(void);
static void w_alloc_check_internal(void *ptr, const size_t size,
                                   const char *file, const int line,
                                   const char *func);
static bool w_alloc_max_size_check_internal(const size_t size, const char *file,
                                            const int line, const char *func);
static void WAM_malloc_create_internal(void *ptr, const size_t size,
                                       const char *file, const int line,
                                       const char *func);
static void WAM_realloc_update_internal(void *old_ptr, void *new_ptr,
                                        const size_t new_size, const char *file,
                                        const int line, const char *func);
static void WAM_free_update_internal(void *ptr, const char *file,
                                     const int line, const char *func);
static void w_check_initialization_internal(void);

// static void w_alloc_node_destroy_internal(struct w_alloc_node *node);
// static void w_alloc_node_list_print_internal(struct w_alloc_node *head);

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Watchdog Internal Dynamic Array Declarations
////////////////////////////////////////////////////////////////////////////////

#define WDA_DEFAULT_BUFFER_SIZE 10
#define WDA_GROWTH_FACTOR 2

typedef struct WatchdogDynamicArray WDA;
struct WatchdogDynamicArray {
    WAM **buffer;
    size_t size;
    size_t capacity;
};

static void WDA_init(void);
static void WDA_push(void *ptr);
static void WDA_pop(void);
static void WDA_insert(size_t index, void *ptr);
static void WDA_remove(size_t index);
static void WDA_print(void);
static void WDA_cleanup(void);
static bool WDA_index_in_bounds_check_internal(size_t index);
static void WDA_expand_capacity_internal(void);

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Watchdog Global Variables
////////////////////////////////////////////////////////////////////////////////

static pthread_mutex_t w_mutex = PTHREAD_MUTEX_INITIALIZER;
static FILE *w_log_file = NULL;
static WDA watchdog;

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
    WDA_init();
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

    w_report();
    WDA_cleanup();

    pthread_mutex_unlock(&w_mutex);
}

// API

void *w_malloc(const size_t size, const char *file, const int line,
               const char *func) {

    w_check_initialization_internal();

    if (!w_alloc_max_size_check_internal(size, file, line, func)) {
        pthread_mutex_unlock(&w_mutex);
        return NULL;
    }
    pthread_mutex_lock(&w_mutex);
    if (!size) {
        pthread_mutex_unlock(&w_mutex);
        return NULL;
    }

    void *ptr = malloc(size);
    w_alloc_check_internal(ptr, size, __FILE__, __LINE__, __func__);

    WAM_malloc_create_internal(ptr, size, file, line, func);
    // printf("[Malloc] %s %u %s\n", file, line, func);

    WATCHDOG_LOG(NULL, "Malloc", ptr, size, file, line, func);
    WATCHDOG_LOG(1, "Malloc", ptr, size, file, line, func);
    pthread_mutex_unlock(&w_mutex);

    return ptr;
}

void *w_realloc(void *old_ptr, size_t size, const char *file, const int line,
                const char *func) {

    w_check_initialization_internal();

    if (!w_alloc_max_size_check_internal(size, file, line, func)) {
        return NULL;
    }
    if (!size) {
        w_free(old_ptr, file, line, func);
        return NULL;
    }

    pthread_mutex_lock(&w_mutex);
    void *new_ptr = realloc(old_ptr, size);
    WAM_realloc_update_internal(old_ptr, new_ptr, size, file, line, func);

    // printf("[Realloc] %s %u %s\n", file, line, func);

    WATCHDOG_LOG(NULL, "Realloc", new_ptr, size, file, line, func);
    WATCHDOG_LOG(1, "Realloc", new_ptr, size, file, line, func);
    pthread_mutex_unlock(&w_mutex);

    return new_ptr;
}

void *w_calloc(size_t count, size_t size, const char *file, const int line,
               const char *func) {
    w_check_initialization_internal();

    if (!w_alloc_max_size_check_internal(size, file, line, func)) {
        return NULL;
    }

    pthread_mutex_lock(&w_mutex);
    if (!size) {
        pthread_mutex_unlock(&w_mutex);
        return NULL;
    }

    void *ptr = calloc(count, size);
    w_alloc_check_internal(ptr, size, __FILE__, __LINE__, __func__);

    WAM_malloc_create_internal(ptr, size, file, line, func);

    // printf("[Calloc] %s %u %s\n", file, line, func);

    WATCHDOG_LOG(NULL, "Calloc", ptr, size, file, line, func);
    WATCHDOG_LOG(1, "Calloc", ptr, size, file, line, func);
    pthread_mutex_unlock(&w_mutex);

    return ptr;
}

void w_free(void *ptr, const char *file, const int line, const char *func) {
    dbg(__func__);
    for (size_t i = 0; i < watchdog.size; i++) {
        if (watchdog.buffer[i]->ptr == ptr) {
            if (!watchdog.buffer[i]->freed) {
                free(watchdog.buffer[i]->ptr);
                watchdog.buffer[i]->freed = true;
                // printf("[Free] %s %u %s\n", file, line, func);

                WATCHDOG_LOG(NULL, "Free", ptr, watchdog.buffer[i]->size, file,
                             line, func);
                WATCHDOG_LOG(1, "Free", ptr, watchdog.buffer[i]->size, file,
                             line, func);
                return;
            } else {
                printf("[WARNING] Double free detected at %p (allocated at "
                       "%s:%d)\n",
                       ptr, watchdog.buffer[i]->file, watchdog.buffer[i]->line);

                return;
            }
        }
    }
    printf("[WARNING] Attempt to free untracked memory at %p\n", ptr);
}

void w_report(void) {
    for (int i = 0; i < watchdog.size; i++) {
        if (!watchdog.buffer[i]->freed) {
            printf("[LEAK] %zu bytes at %p (allocated at %s:%d)\n",
                   watchdog.buffer[i]->size, watchdog.buffer[i]->ptr,
                   watchdog.buffer[i]->file, watchdog.buffer[i]->line);
        }
    }
}

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

static bool w_alloc_max_size_check_internal(const size_t size, const char *file,
                                            const int line, const char *func) {
    if (size == SIZE_MAX) {
        fprintf(stderr,
                "[%s:%u:(%s)] Out of memory error. %zu bytes exceeds maximum "
                " allowed memory allocation.\n ",
                file, line, func, size);
        pthread_mutex_unlock(&w_mutex);
        return false;
    }
    return true;
}

static void WAM_malloc_create_internal(void *ptr, const size_t size,
                                       const char *file, const int line,
                                       const char *func) {
    WAM *data = malloc(sizeof *data);
    w_alloc_check_internal(data, sizeof *data, __FILE__, __LINE__, __func__);

    data->ptr = ptr;
    data->size = size;
    data->file = file;
    data->line = line;
    data->func = func;
    data->freed = false;

    WDA_push(data);
    return;
}

static void WAM_realloc_update_internal(void *old_ptr, void *new_ptr,
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
    WAM_malloc_create_internal(new_ptr, new_size, file, line, func);
}

static void w_check_initialization_internal(void) {
    if (!w_log_file) {
        w_init(NULL);
    }
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Watchdog Internal Dynamic Array API Definitions
////////////////////////////////////////////////////////////////////////////////

void WDA_init(void) {
    watchdog.size = 0;
    watchdog.capacity = WDA_DEFAULT_BUFFER_SIZE;
    watchdog.buffer = malloc(sizeof *watchdog.buffer * watchdog.capacity);
    w_alloc_check_internal(watchdog.buffer,
                           sizeof *watchdog.buffer * watchdog.capacity,
                           __FILE__, __LINE__, __func__);
}

void WDA_push(void *ptr) {
    if (watchdog.size == watchdog.capacity) {
        WDA_expand_capacity_internal();
    }
    watchdog.buffer[watchdog.size++] = ptr;
}

void WDA_pop(void) {
    if (!(watchdog.size > 0)) {
        return;
    }
    watchdog.size--;
    free(watchdog.buffer[watchdog.size]);
    watchdog.buffer[watchdog.size] = NULL;
}

void WDA_insert(size_t index, void *ptr) {
    if (!WDA_index_in_bounds_check_internal(index)) {
        exit(EXIT_FAILURE);
    }
    if (watchdog.size + 1 >= watchdog.capacity) {
        WDA_expand_capacity_internal();
    }
    for (size_t i = watchdog.size; i < index; i++) {
        watchdog.buffer[i] = watchdog.buffer[i - 1];
    }
    watchdog.buffer[index] = ptr;
}

void WDA_remove(size_t index) {
    if (!WDA_index_in_bounds_check_internal(index)) {
        return;
    }
    free(watchdog.buffer[index]);
    for (size_t i = index; i < watchdog.size - 1; i++) {
        watchdog.buffer[i] = watchdog.buffer[i + 1];
    }
    watchdog.size--;
}

void WDA_print(void) {
    for (size_t i = 0; i < watchdog.size; i++) {
        printf("[%zu] %p\n", i, (WAM *)watchdog.buffer[i]);
    }
}

void WDA_cleanup(void) {
    if (watchdog.buffer) {
        for (size_t i = 0; i < watchdog.size; i++) {
            WDA_pop();
        }
        free(watchdog.buffer);
        watchdog.buffer = NULL;
    }
    watchdog.size = 0;
    watchdog.capacity = 0;
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Watchdog Internal Dynamic Array Internals Definitions
////////////////////////////////////////////////////////////////////////////////

static bool WDA_index_in_bounds_check_internal(size_t index) {
    if (index >= 0 && index < watchdog.size) {
        return true;
    }
    fprintf(stderr, "Index Out Of Bounds Error: %zu is out of bounds of %zu.\n",
            index, watchdog.size);
    return false;
}

static void WDA_expand_capacity_internal(void) {
    watchdog.capacity *= WDA_GROWTH_FACTOR;
    WAM **buffer =
        realloc(watchdog.buffer, sizeof *watchdog.buffer * watchdog.capacity);
    w_alloc_check_internal(buffer, sizeof **watchdog.buffer * watchdog.capacity,
                           __FILE__, __LINE__, __func__);
    watchdog.buffer = buffer;
}

////////////////////////////////////////////////////////////////////////////////
