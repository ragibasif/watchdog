/*
 * File: main.c
 * Author: Ragib Asif
 * GitHub: https://github.com/ragibasif
 * LinkedIn: https://www.linkedin.com/in/ragibasif/
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2025 Ragib Asif
 * Version 1.0.0
 *
 */

#include "watchdog.h"

static void test_malloc(void);
static void test_realloc(void);
static void test_calloc(void);
static void test_free(void);

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {

    bool enable_verbose_log = true;
    bool log_to_file = false;
    bool enable_color_output = true;

    w_init(enable_verbose_log, log_to_file, enable_color_output);
    int *a = malloc(34232);
    free(a);
    int *b = malloc(4232);
    b = realloc(b, 243);
    // free(b);
    int *c = realloc(b, 243);
    free(c);

    // int *f1 = w_malloc(1 * sizeof(int), __FILE__, __LINE__, __func__);
    // f1[1] = 34;
    // w_free(f1, __FILE__, __LINE__, __func__);
    //
    // int *f2 = w_malloc(2 * sizeof(int), __FILE__, __LINE__, __func__);
    // w_free(f1, __FILE__, __LINE__, __func__);
    // int *f3 = w_malloc(3 * sizeof(int), __FILE__, __LINE__, __func__);
    // w_free(f3, __FILE__, __LINE__, __func__);
    // f3 = w_malloc(4 * sizeof(int), __FILE__, __LINE__, __func__);
    // f3 = w_malloc(5 * sizeof(int), __FILE__, __LINE__, __func__);
    // f3 = w_malloc(6 * sizeof(int), __FILE__, __LINE__, __func__);
    // w_free(f2, __FILE__, __LINE__, __func__);
    // f3 = w_malloc(SIZE_MAX, __FILE__, __LINE__, __func__);

    // test_malloc();
    // test_realloc();
    // test_calloc();
    // test_free();

    return EXIT_SUCCESS;
}

// Summary of Malloc Test Cases:
// Case     Description
// 1	Normal allocation
// 2	Allocation with size = 0
// 3	Huge allocation (OOM)
// 4	Leak detection (intentional no free)
static void test_malloc(void) {

    printf("%s %u %s\n", __FILE__, __LINE__, __func__);

    printf("Case 1: normal malloc\n");
    int *a1 = w_malloc(10 * sizeof(int), __FILE__, __LINE__, __func__);
    if (a1) {
        a1[0] = 123;
        printf("Value stored in malloc'd memory: %d\n", a1[0]);
        w_free(a1, __FILE__, __LINE__, __func__);
    }

    printf("Case 2: malloc 0 bytes (should return NULL or valid unique ptr)\n");
    int *a2 = w_malloc(0, __FILE__, __LINE__, __func__);
    if (a2) {
        printf("malloc(0) returned non-NULL\n");
        w_free(a2, __FILE__, __LINE__, __func__);
    }

    printf("Case 3: malloc large size to simulate OOM\n");
    void *a3 = w_malloc((size_t)-1, __FILE__, __LINE__, __func__);
    if (!a3) {
        printf("malloc failed as expected for large size\n");
    }

    printf("Case 4: malloc without free (intentional leak)\n");
    void *a4 = w_malloc(64, __FILE__, __LINE__, __func__);
    (void)a4; // intentionally not freeing

    printf("Finished test_malloc()\n");
}

// Summary of Realloc Test Cases:
// Case     Description
// 1        Realloc to a larger size
// 2        Realloc to a smaller size
// 3        Realloc from NULL (acts like malloc)
// 4        Realloc to size 0 (acts like free)
// 5        Realloc after free (UB, but test if it’s handled)
// 6        Realloc to extremely large size to simulate OOM
static void test_realloc(void) {
    printf("%s %u %s\n", __FILE__, __LINE__, __func__);

    int *arr1 = w_malloc(10 * sizeof(int), __FILE__, __LINE__, __func__);
    if (arr1) {
        printf("Case 1: realloc to larger size\n");
        arr1 = w_realloc(arr1, 20 * sizeof(int), __FILE__, __LINE__, __func__);
        if (!arr1) {
            printf("Case 1 failed: realloc returned NULL\n");
        }
        w_free(arr1, __FILE__, __LINE__, __func__);
    }

    int *arr2 = w_malloc(20 * sizeof(int), __FILE__, __LINE__, __func__);
    if (arr2) {
        printf("Case 2: realloc to smaller size\n");
        arr2 = w_realloc(arr2, 5 * sizeof(int), __FILE__, __LINE__, __func__);
        if (!arr2) {
            printf("Case 2 failed: realloc returned NULL\n");
        }
        w_free(arr2, __FILE__, __LINE__, __func__);
    }

    printf("Case 3: realloc with NULL pointer (malloc behavior)\n");
    int *arr3 = w_realloc(NULL, 15 * sizeof(int), __FILE__, __LINE__, __func__);
    if (!arr3) {
        printf("Case 3 failed: realloc returned NULL\n");
    } else {
        w_free(arr3, __FILE__, __LINE__, __func__);
    }

    int *arr4 = w_malloc(10 * sizeof(int), __FILE__, __LINE__, __func__);
    if (arr4) {
        printf("Case 4: realloc to size 0 (should free memory)\n");
        arr4 = w_realloc(arr4, 0, __FILE__, __LINE__, __func__);
        if (arr4 != NULL) {
            printf("Case 4 failed: realloc(size=0) did not return NULL\n");
        }
    }

    printf("Case 5: realloc a freed pointer (should not be allowed)\n");
    int *arr5 = w_malloc(10 * sizeof(int), __FILE__, __LINE__, __func__);
    if (arr5) {
        w_free(arr5, __FILE__, __LINE__, __func__);
        arr5 = w_realloc(arr5, 20 * sizeof(int), __FILE__, __LINE__, __func__);
        if (arr5) {
            printf("Case 5 warning: realloc on freed pointer succeeded (UB)\n");
            w_free(arr5, __FILE__, __LINE__,
                   __func__); // avoid leak if somehow worked
        }
    }

    printf("Case 6: realloc with large size to simulate failure\n");
    int *arr6 = w_malloc(100 * sizeof(int), __FILE__, __LINE__, __func__);
    if (arr6) {
        arr6 = w_realloc(arr6, (size_t)-1, __FILE__, __LINE__, __func__);
        if (!arr6) {
            printf("Case 6 passed: realloc failed gracefully\n");
        } else {
            printf("Case 6 warning: large realloc unexpectedly succeeded\n");
            w_free(arr6, __FILE__, __LINE__, __func__);
        }
    }

    printf("Finished test_realloc()\n");
}

// Summary of Calloc Test Cases:
// Case     Description
// 1	Normal zero-initialized memory
// 2	calloc(0, size)
// 3	calloc(n, 0)
// 4	Overflow detection
static void test_calloc(void) {
    printf("%s %u %s\n", __FILE__, __LINE__, __func__);

    printf("Case 1: normal calloc\n");
    int *c1 = w_calloc(10, sizeof(int), __FILE__, __LINE__, __func__);
    if (c1) {
        int initialized = 1;
        for (int i = 0; i < 10; i++) {
            if (c1[i] != 0)
                initialized = 0;
        }
        printf("Calloc initialized to zero: %s\n", initialized ? "Yes" : "No");
        w_free(c1, __FILE__, __LINE__, __func__);
    }

    printf("Case 2: calloc with 0 elements\n");
    int *c2 = w_calloc(0, sizeof(int), __FILE__, __LINE__, __func__);
    if (c2) {
        printf("calloc(0, size) returned non-NULL\n");
        w_free(c2, __FILE__, __LINE__, __func__);
    }

    printf("Case 3: calloc with 0 size\n");
    int *c3 = w_calloc(10, 0, __FILE__, __LINE__, __func__);
    if (c3) {
        printf("calloc(n, 0) returned non-NULL\n");
        w_free(c3, __FILE__, __LINE__, __func__);
    }

    printf("Case 4: calloc with size overflow\n");
    size_t huge = (size_t)1 << (sizeof(size_t) * 4); // simulate overflow
    void *c4 = w_calloc(huge, huge, __FILE__, __LINE__, __func__);
    if (!c4) {
        printf("Calloc failed as expected due to overflow\n");
    }

    printf("Finished test_calloc()\n");
}

// Summary of Free Test Cases:
// Case     Description
// 1	Normal free
// 2	Double free
// 3	Free NULL
// 4	Free invalid pointer
static void test_free(void) {
    printf("%s %u %s\n", __FILE__, __LINE__, __func__);

    printf("Case 1: malloc then free\n");
    int *f1 = w_malloc(20 * sizeof(int), __FILE__, __LINE__, __func__);
    if (f1) {
        w_free(f1, __FILE__, __LINE__, __func__);
    }

    printf("Case 2: double free (should be detected)\n");
    int *f2 = w_malloc(32, __FILE__, __LINE__, __func__);
    if (f2) {
        w_free(f2, __FILE__, __LINE__, __func__);
        w_free(f2, __FILE__, __LINE__,
               __func__); // UB, but we test detection
    }

    printf("Case 3: free NULL pointer (safe)\n");
    int *f3 = NULL;
    w_free(f3, __FILE__, __LINE__, __func__);

    printf("Case 4: free unallocated pointer (intentional bad free)\n");
    int dummy = 0;
    w_free(&dummy, __FILE__, __LINE__,
           __func__); // should be detected as invalid

    printf("Finished test_free()\n");
}
