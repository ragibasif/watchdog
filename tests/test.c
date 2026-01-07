#define WATCHDOG_ENABLE
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../watchdog.h"

static void malloc_test(void);
static void calloc_test(void);
static void realloc_test(void);
static void free_test(void);
static void leak_test(void);
static void double_free_test(void);
static void overflow_test(void);
static void invalid_free_test(void);

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
  bool enable_verbose_log = true;
  bool log_to_file = false;
  bool enable_color_output = true;

  w_init(enable_verbose_log, log_to_file, enable_color_output);

  malloc_test();
  calloc_test();
  realloc_test();
  free_test();
  leak_test();
  double_free_test();
  overflow_test();
  invalid_free_test();

  return EXIT_SUCCESS;
}

void malloc_test(void) {
  size_t count = 5;
  int* buffer = malloc(sizeof *buffer * 5);
  for (size_t i = 0; i < count; i++) {
    buffer[i] = -(i - count) * count;
  }

  free(buffer);
  buffer = NULL;
}

void realloc_test(void) {
  short* buffer = malloc(34222);
  buffer = realloc(buffer, 2342);
  buffer = realloc(buffer, 2342342);
  buffer = realloc(buffer, 2);
  buffer = realloc(buffer, 10);
  buffer = realloc(buffer, 0);
  free(buffer);
  buffer = NULL;
}

void calloc_test(void) {
  // Demonstrates correct usage of calloc
  size_t count = 5;
  int* buffer = calloc(count, sizeof *buffer);
  free(buffer);
  buffer = NULL;
}

void free_test(void) {
  // Demonstrates correct usage of free
  size_t count = 5;
  short int* buffer0 = malloc(sizeof *buffer0 * count);
  unsigned char* buffer1 = calloc(count, sizeof *buffer1);
  long double* buffer2 = realloc(buffer2, sizeof *buffer2 * count);
  free(buffer1);
  buffer1 = NULL;
  free(buffer2);
  buffer2 = NULL;
  free(buffer0);
  buffer1 = NULL;
}

void leak_test(void) {
  unsigned long long int* buffer = malloc(sizeof *buffer * 20);
  // intentionally not calling free
  // watchdog will detect this and report it
}

void double_free_test(void) {
  char** buffer = malloc(sizeof *buffer * 20);
  free(buffer);
  free(buffer);  // triggers a double-free error
}

void overflow_test(void) {
  char* buffer = malloc(sizeof *buffer * 10);
  strcpy(buffer, "This will overflow");  // out-of-bounds write
  // of buffer overflows will be detected with using canary values
}

void invalid_free_test(void) {
  float* buffer;
  free(buffer);  // will trigger an error since buffer wasn't allocated
}
