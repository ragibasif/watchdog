/*
 * File: watchdog.h
 * Author: Ragib Asif
 * Email: ragibasif@tuta.io
 * GitHub: https://github.com/ragibasif
 * LinkedIn: https://www.linkedin.com/in/ragibasif/
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2025 Ragib Asif
 * Version 1.1.0
 */

#ifndef WATCHDOG_H_
#define WATCHDOG_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Option which clones all strings (file and func names) to prevent broken
// references (useful in case when dynamic libraries are involved). This may
// have performance and memory usage impact. Another option is to keep all
// loaded libraries till the program ends (see RTLD_NODELETE and
// GET_MODULE_HANDLE_EX_FLAG_PIN (nb: FreeLibrary does not work with
// GET_MODULE_HANDLE_EX_FLAG_PIN flag)).
#ifndef WATCHDOG_COPY_STRINGS
#define WATCHDOG_COPY_STRINGS 0
#endif // WATCHDOG_COPY_STRINGS

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//------------------------------------------------------------------------------
// ANSI Escape Codes
//------------------------------------------------------------------------------

// Reset all text colors and attributes
#define AEC_RESET "\x1b[0m"

// Text colors
#define AEC_RED     "\x1b[91m"
#define AEC_MAGENTA "\x1b[95m"
#define AEC_CYAN    "\x1b[96m"

// Text attributes
#define AEC_BOLD "\x1b[1m"
#define AEC_DIM  "\x1b[2m"

extern void  w_init(bool enable_verbose_log, bool log_to_file,
                    bool enable_color_output);
extern void *w_malloc(size_t size, const char *file, const int line,
                      const char *func);
extern void *w_realloc(void *old_ptr, size_t size, const char *file,
                       const int line, const char *func);
extern void *w_calloc(size_t count, size_t size, const char *file,
                      const int line, const char *func);
extern void  w_free(void *ptr, const char *file, const int line,
                    const char *func);

#if !defined(WATCHDOG_INTERNAL) && defined(WATCHDOG_ENABLE)
#define malloc(size)        w_malloc(size, __FILE__, __LINE__, __func__)
#define realloc(ptr, size)  w_realloc(ptr, size, __FILE__, __LINE__, __func__)
#define calloc(count, size) w_calloc(count, size, __FILE__, __LINE__, __func__)
#define free(ptr)           w_free(ptr, __FILE__, __LINE__, __func__)
#else
#undef malloc
#undef realloc
#undef calloc
#undef free
#endif // !defined(WATCHDOG_DISABLE) && defined(WATCHDOG_ENABLE)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // WATCHDOG_H_
