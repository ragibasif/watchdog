/*
 * File: common.h
 * Author: Ragib Asif
 * Email: 182296466+ragibasif@users.noreply.github.com
 * GitHub: https://github.com/ragibasif
 * LinkedIn: https://www.linkedin.com/in/ragibasif/
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2025 Ragib Asif
 * Version 1.0.0
 *
 */

#ifndef COMMON_H_
#define COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <assert.h>  // assert() - program diagnostics
#include <ctype.h>   // Character handling:
                     // isalpha(), isdigit(), toupper(), tolower()
#include <errno.h>   // Error numbers (errno)
#include <float.h>   // Floating-point limits (FLT_MAX, DBL_EPSILON, etc.)
#include <limits.h>  // Integer limits (INT_MAX, INT_MIN, etc.)
#include <locale.h>  // setlocale() - localization functions
#include <math.h>    // Math functions:
                     // sqrt(), sin(), cos(), pow(), ceil(), floor()
#include <setjmp.h>  // setjmp(), longjmp() - non-local jumps
#include <signal.h>  // signal(), raise() - signal handling
#include <stdarg.h>  // Variable arguments:
                     // va_start(), va_arg(), va_end(), va_list
#include <stdbool.h> // Boolean type (bool, true, false)
#include <stddef.h>  // NULL, offsetof(), size_t, ptrdiff_t
#include <stdint.h>  // Fixed-width integers (int32_t, uint64_t, etc.)
#include <stdio.h>   // Input/output:
                     // printf(), scanf(), fopen(), fclose(), fgets()
#include <stdlib.h>  // General utilities:
                     // malloc(), free(), exit(), atoi(), rand()
#include <string.h>  // String manipulation:
                     // strlen(), strcpy(), strcmp(), memset(), memmove()
#include <time.h>    // Time functions:
                     // time(), clock(), difftime(), strftime()

// POSIX/Unix extensions (may not be available on all systems)
#include <dirent.h>     // Directory operations (opendir(), readdir())
#include <fcntl.h>      // File control (open(), fcntl())
#include <pthread.h>    // Threads:
                        // pthread_create(), pthread_join(), mutex functions
#include <sys/stat.h>   // File status (stat(), fstat())
#include <unistd.h>     // POSIX API:
                        // read(), write(), close(), fork(), getpid()
#include <netdb.h>      // Network operations (gethostbyname(), getaddrinfo())
#include <sys/socket.h> // Socket programming (socket(), bind(), listen())

//------------------------------------------------------------------------------
// Utility Macros
//------------------------------------------------------------------------------

#define MOD(a, b) ((a) % (b) + (b)) % (b)

#define PI 3.14159265358979323846f
#define ABS(val) ((val) < 0) ? -(val) : (val)
#define AREA(l, w) (l * w)
#define AVERAGE(val1, val2) (((double)(val1) + (val2)) / 2.0)
#define BIT_CHK(bit, reg) ((reg >> (bit)) & 1)
#define BIT_CLR(value, bit) ((value) &= ~(1L << (bit)))
#define BIT_LSB(reg) ((reg) & 1)
#define BIT_SET(value, bit) ((value) |= (1L << (bit)))
#define CIRCUMFERENCE(radius) (PI * radious * radious) // circle circumference
#define IN_RANGE(x, lo, hi) (((x > lo) && (x < hi)) || (x == lo) || (x == hi))
#define IS_EVEN(x) !BIT_LSB(x)
#define IS_ODD(x) (x & 1)

#define MAX(a, b)                                                              \
    ({                                                                         \
        __typeof__(a) _a = (a);                                                \
        __typeof__(b) _b = (b);                                                \
        _a > _b ? _a : _b;                                                     \
    })

#define MIN(a, b)                                                              \
    ({                                                                         \
        __typeof__(a) _a = (a);                                                \
        __typeof__(b) _b = (b);                                                \
        _a < _b ? _a : _b;                                                     \
    })

#define PERCENT(val, per) ((val) * (per) / 100.0)
#define RAND_INT(min, max) ((rand() % (int)(((max) + 1) - (min))) + (min))
#define SIZE(x) (sizeof(x) / sizeof(x[0]))
#define SUM(a, b) (a + b)
#define XOR_SWAP(a, b)                                                         \
    do {                                                                       \
        (a) ^= (b);                                                            \
        (b) ^= (a);                                                            \
        (a) ^= (b);                                                            \
    } while (0)

#define PRINT_CHAR(x) printf("%c", x) // print char
#define PRINT_STR(x) printf("%s", x)  // print string of characters

// integer
#define INPUT_INT(x) scanf("%d", &x)     // int
#define INPUT_SHORT(x) scanf("%hd", &x)  // short int
#define INPUT_LINT(x) scanf("%ld", &x)   // lont int
#define INPUT_LLINT(x) scanf("%lld", &x) // long long int
#define INPUT_OCT(x) scanf("%o", &x)     // octal
#define INPUT_HEX(x) scanf("%x", &x)     // hexadecimal
#define INPUT_UINT(x) scanf("%u", &x)    // unsigned int

#define PRINT_INT(x) printf("%d", x)     // int
#define PRINT_SHORT(x) printf("%hd", x)  // short int
#define PRINT_LINT(x) printf("%ld", x)   // long int
#define PRINT_LLINT(x) printf("%lld", x) // long long int
#define PRINT_OCT(x) printf("%o", x)     // octal without leading zeros
#define PRINT_HEX(x) printf("%x", x)     // hexadecimal
#define PRINT_UINT(x) printf("%u", x)    // unsigned int

// floating point
#define INPUT_FLOAT(x) scanf("%f", &x)    // float
#define INPUT_DOUBLE(x) scanf("%lf", &x)  // double
#define INPUT_LDOUBLE(x) scanf("%Lf", &x) // long double
#define INPUT_FLOATEXP(x) scanf("%e", &x) // floating point exponent

#define PRINT_FLOAT(x) printf("%f", x)    // float
#define PRINT_DOUBLE(x) printf("%lf", x)  // double
#define PRINT_LDOUBLE(x) printf("%Lf", x) // long double
#define PRINT_FLOATEXP(x) printf("%e", x) // floating point exponent

// rounded integer division:

#define FLOOR_DIV(x, y) ((x) / (y))
#define CEIL_DIV(x, y) FLOOR_DIV((x) + (y - 1), y)
#define ROUND_DIV(x, y) FLOOR_DIV((x) + (y) / 2, y)

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))

#define SWAP(a, b)                                                             \
    do {                                                                       \
        __typeof__(a) _a = (a);                                                \
        __typeof__(b) _b = (b);                                                \
        _a < _b ? _a : _b;                                                     \
        (a) = _b;                                                              \
        (b) = _a;                                                              \
    } while (0)

#define STR(x) #x

typedef unsigned char BYTE; // name BYTE for one-byte numbers

// press any key (Linux & MS Windows)
#define PRESSKEY                                                               \
    printf("\n\t Press any key to continue...");                               \
    while (1) {                                                                \
        if (getc(stdin)) {                                                     \
            break;                                                             \
        }                                                                      \
    }

//------------------------------------------------------------------------------
// ANSI Escape Codes
//------------------------------------------------------------------------------

// Reset all attributes
#define AEC_RESET "\x1b[0m"

// Text colors
#define AEC_BLACK "\x1b[30m"
#define AEC_RED "\x1b[31m"
#define AEC_GREEN "\x1b[32m"
#define AEC_YELLOW "\x1b[33m"
#define AEC_BLUE "\x1b[34m"
#define AEC_MAGENTA "\x1b[35m"
#define AEC_CYAN "\x1b[36m"
#define AEC_WHITE "\x1b[37m"
#define AEC_DEFAULT "\x1b[39m"

// Background colors
#define AEC_BG_BLACK "\x1b[40m"
#define AEC_BG_RED "\x1b[41m"
#define AEC_BG_GREEN "\x1b[42m"
#define AEC_BG_YELLOW "\x1b[43m"
#define AEC_BG_BLUE "\x1b[44m"
#define AEC_BG_MAGENTA "\x1b[45m"
#define AEC_BG_CYAN "\x1b[46m"
#define AEC_BG_WHITE "\x1b[47m"
#define AEC_BG_DEFAULT "\x1b[49m"

// Bright text colors
#define AEC_BRIGHT_BLACK "\x1b[90m"
#define AEC_BRIGHT_RED "\x1b[91m"
#define AEC_BRIGHT_GREEN "\x1b[92m"
#define AEC_BRIGHT_YELLOW "\x1b[93m"
#define AEC_BRIGHT_BLUE "\x1b[94m"
#define AEC_BRIGHT_MAGENTA "\x1b[95m"
#define AEC_BRIGHT_CYAN "\x1b[96m"
#define AEC_BRIGHT_WHITE "\x1b[97m"

// Bright background colors
#define AEC_BG_BRIGHT_BLACK "\x1b[100m"
#define AEC_BG_BRIGHT_RED "\x1b[101m"
#define AEC_BG_BRIGHT_GREEN "\x1b[102m"
#define AEC_BG_BRIGHT_YELLOW "\x1b[103m"
#define AEC_BG_BRIGHT_BLUE "\x1b[104m"
#define AEC_BG_BRIGHT_MAGENTA "\x1b[105m"
#define AEC_BG_BRIGHT_CYAN "\x1b[106m"
#define AEC_BG_BRIGHT_WHITE "\x1b[107m"

// Text attributes
#define AEC_BOLD "\x1b[1m"
#define AEC_DIM "\x1b[2m"
#define AEC_ITALIC "\x1b[3m"
#define AEC_UNDERLINE "\x1b[4m"
#define AEC_BLINK "\x1b[5m"
#define AEC_REVERSE "\x1b[7m"
#define AEC_HIDDEN "\x1b[8m"
#define AEC_STRIKETHROUGH "\x1b[9m"

// Reset specific attributes
#define AEC_NO_BOLD "\x1b[21m" // or "\x1b[22m" (both work)
#define AEC_NO_ITALIC "\x1b[23m"
#define AEC_NO_UNDERLINE "\x1b[24m"
#define AEC_NO_BLINK "\x1b[25m"
#define AEC_NO_REVERSE "\x1b[27m"
#define AEC_NO_HIDDEN "\x1b[28m"
#define AEC_NO_STRIKETHROUGH "\x1b[29m"

// 256-color support
#define AEC_COLOR256(n) "\033[38;5;" #n "m"
#define AEC_BG_COLOR256(n) "\033[48;5;" #n "m"

// Truecolor (RGB) support
#define AEC_RGB(r, g, b) "\033[38;2;" #r ";" #g ";" #b "m"
#define AEC_BG_RGB(r, g, b) "\033[48;2;" #r ";" #g ";" #b "m"

// Cursor movement
#define AEC_CURSOR_UP(n) "\x1b[" #n "A"
#define AEC_CURSOR_DOWN(n) "\x1b[" #n "B"
#define AEC_CURSOR_FORWARD(n) "\x1b[" #n "C"
#define AEC_CURSOR_BACK(n) "\x1b[" #n "D"
#define AEC_CURSOR_NEXT_LINE(n) "\x1b[" #n "E"
#define AEC_CURSOR_PREV_LINE(n) "\x1b[" #n "F"
#define AEC_CURSOR_COLUMN(n) "\x1b[" #n "G"
#define AEC_CURSOR_POSITION(row, col) "\x1b[" #row ";" #col "H"

// Save and restore cursor position
#define AEC_CURSOR_SAVE "\x1b[s"
#define AEC_CURSOR_RESTORE "\x1b[u"

// Cursor visibility
#define AEC_CURSOR_HIDE "\x1b[?25l"
#define AEC_CURSOR_SHOW "\x1b[?25h"

// Screen operations
#define AEC_SCREEN_CLEAR "\x1b[2J"
#define AEC_SCREEN_CLEAR_LINE "\x1b[2K"
#define AEC_SCREEN_CLEAR_TO_END "\x1b[0J"
#define AEC_SCREEN_CLEAR_TO_START "\x1b[1J"
#define AEC_LINE_CLEAR_TO_END "\x1b[0K"
#define AEC_LINE_CLEAR_TO_START "\x1b[1K"
#define AEC_SCROLL_UP(n) "\x1b[" #n "S"
#define AEC_SCROLL_DOWN(n) "\x1b[" #n "T"

// Terminal modes
#define AEC_ALTERNATE_BUFFER "\x1b[?1049h"
#define AEC_MAIN_BUFFER "\x1b[?1049l"
#define AEC_LINE_WRAP_ON "\x1b[?7h"
#define AEC_LINE_WRAP_OFF "\x1b[?7l"

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // COMMON_H_
