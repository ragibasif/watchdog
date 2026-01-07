#define WATCHDOG_INTERNAL
#include "watchdog.h"

////////////////////////////////////////////////////////////////////////////////
// Watchdog Global Variables
////////////////////////////////////////////////////////////////////////////////

static const char* log_file_name = "watchdog.log";
static pthread_mutex_t w_mutex = PTHREAD_MUTEX_INITIALIZER;
static FILE* w_log_file = NULL;
static bool verbose_log = true;
static bool log_to_file = false;
static bool color_output = false;

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Watchdog Internals Declarations
////////////////////////////////////////////////////////////////////////////////

typedef unsigned char BYTE;

#define WATCHDOG_LOG(prefix, ptr, size, file, line, func)                      \
  do {                                                                         \
    time_t now = time(NULL);                                                   \
    char* time_str = ctime(&now);                                              \
    time_str[strlen(time_str) - 1] = '\0';                                     \
    if (color_output && w_log_file == stdout) {                                \
      fprintf(w_log_file,                                                      \
              "%s%s[%s]%s %s%s [%s:%d (%s)]:%s %s%s%p%s = %s%zu%s Bytes\n",    \
              AEC_BOLD, AEC_MAGENTA, prefix, AEC_RESET, AEC_DIM, time_str,     \
              file, line, func, AEC_RESET, AEC_CYAN, AEC_BOLD, ptr, AEC_RESET, \
              AEC_BOLD, size, AEC_RESET);                                      \
    } else {                                                                   \
      fprintf(w_log_file, "[%s] %s [%s:%d (%s)]: %p = %zu Bytes\n", prefix,    \
              time_str, file, line, func, ptr, size);                          \
    }                                                                          \
    fflush(w_log_file);                                                        \
  } while (0)

#define WATCHDOG_LOG_ERROR(msg, file, line, func)                          \
  do {                                                                     \
    time_t now = time(NULL);                                               \
    char* time_str = ctime(&now);                                          \
    time_str[strlen(time_str) - 1] = '\0';                                 \
    if (color_output && w_log_file == stdout) {                            \
      fprintf(w_log_file, "%s%s[ERROR]%s %s%s [%s:%d (%s)]:%s %s%s%s\n",   \
              AEC_BOLD, AEC_RED, AEC_RESET, AEC_DIM, time_str, file, line, \
              func, AEC_RESET, AEC_BOLD, msg, AEC_RESET);                  \
    } else {                                                               \
      fprintf(w_log_file, "[ERROR] %s [%s:%d (%s)]: %s\n", time_str, file, \
              line, func, msg);                                            \
    }                                                                      \
    fflush(w_log_file);                                                    \
  } while (0)

#define CANARY_SIZE 64
#define CANARY_VALUE 0x7E

typedef struct WatchdogAllocationMetadata WAM;

struct WatchdogAllocationMetadata {
  void* ptr;
  size_t size;
  const char* file;
  unsigned int line;
  const char* func;
  bool freed;
};

static void w_report(void);
static void w_finalize(void);
static void w_alloc_check_internal(void* ptr, const size_t size,
                                   const char* file, const int line,
                                   const char* func);
static bool w_alloc_max_size_check_internal(const size_t size, const char* file,
                                            const int line, const char* func);
static void WAM_alloc_create_internal(void* ptr, const size_t size,
                                      const char* file, const int line,
                                      const char* func);
static void WAM_realloc_update_internal(void* old_ptr, void* new_ptr,
                                        const size_t new_size, const char* file,
                                        const int line, const char* func);
static void w_check_initialization_internal(void);

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Watchdog Internal Dynamic Array Declarations
////////////////////////////////////////////////////////////////////////////////

#define WDA_DEFAULT_BUFFER_SIZE 10
#define WDA_GROWTH_FACTOR 2

typedef struct WatchdogDynamicArray WDA;

struct WatchdogDynamicArray {
  WAM** buffer;
  size_t size;
  size_t capacity;
};

static void WDA_init(void);
static void WDA_push(void* ptr);
static void WDA_pop(void);
static void WDA_cleanup(void);
static void WDA_expand_capacity_internal(void);

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Watchdog Benchmarking Helpers
////////////////////////////////////////////////////////////////////////////////

typedef struct {
  size_t total_allocations;
  size_t total_frees;
  size_t current_usage;
  size_t peak_usage;
  double total_time_spent;  // in seconds
} WatchdogStats;

static WatchdogStats w_stats = {0};

// Helper for high-resolution timing
static double w_get_time() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec + ts.tv_nsec * 1e-9;
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Watchdog API Definitions
////////////////////////////////////////////////////////////////////////////////

static WDA watchdog;

void w_init(bool enable_verbose_log, bool enable_file_log,
            bool enable_color_output) {
  pthread_mutex_lock(&w_mutex);
  verbose_log = enable_verbose_log;
  log_to_file = enable_file_log;
  color_output = enable_color_output;
  if (log_to_file) {
    w_log_file = fopen(log_file_name, "a");
    if (!w_log_file) {
      fprintf(stderr, "Failed to open log file: %s\n", log_file_name);
      pthread_mutex_unlock(&w_mutex);
      exit(EXIT_FAILURE);
    }
  } else {
    w_log_file = stdout;  // default to standard output
  }
  atexit(w_finalize);
  pthread_mutex_unlock(&w_mutex);
  WDA_init();
}

void w_finalize(void) {
  pthread_mutex_unlock(&w_mutex);
  w_report();
  WDA_cleanup();

  if (w_log_file && w_log_file != stdout) {
    fclose(w_log_file);
    w_log_file = NULL;
  }
}

void* w_malloc(const size_t size, const char* file, const int line,
               const char* func) {
  double start_time = w_get_time();
  w_check_initialization_internal();

  pthread_mutex_lock(&w_mutex);

  w_stats.total_allocations++;
  w_stats.current_usage += size;
  if (w_stats.current_usage > w_stats.peak_usage) {
    w_stats.peak_usage = w_stats.current_usage;
  }

  if (!w_alloc_max_size_check_internal(size, file, line, func)) {
    pthread_mutex_unlock(&w_mutex);
    return NULL;
  }
  if (!size) {
    pthread_mutex_unlock(&w_mutex);
    return NULL;
  }

  void* ptr = malloc(size + (2 * CANARY_SIZE));
  w_alloc_check_internal(ptr, size, __FILE__, __LINE__, __func__);

  WAM_alloc_create_internal(ptr, size, file, line, func);
  memset(ptr, CANARY_VALUE, CANARY_SIZE);
  memset((BYTE*)ptr + size + CANARY_SIZE, CANARY_VALUE, CANARY_SIZE);

  if (verbose_log) {
    WATCHDOG_LOG("MALLOC", (void*)((BYTE*)ptr + CANARY_SIZE), size, file, line,
                 func);
  }

  w_stats.total_time_spent += (w_get_time() - start_time);
  pthread_mutex_unlock(&w_mutex);

  return (BYTE*)ptr + CANARY_SIZE;
}

void* w_realloc(void* old_ptr, size_t size, const char* file, const int line,
                const char* func) {
  double start_time = w_get_time();
  w_check_initialization_internal();

  pthread_mutex_lock(&w_mutex);

  w_stats.total_allocations++;
  w_stats.current_usage += size;
  if (w_stats.current_usage > w_stats.peak_usage) {
    w_stats.peak_usage = w_stats.current_usage;
  }

  if (!old_ptr) {
    w_stats.total_time_spent += (w_get_time() - start_time);
    pthread_mutex_unlock(&w_mutex);
    void* temp = w_malloc(size, file, line, func);
    return temp;
  }

  size_t old_ptr_size;

  void* original_ptr = (BYTE*)old_ptr - CANARY_SIZE;
  for (size_t i = 0; i < watchdog.size; i++) {
    if (watchdog.buffer[i]->ptr == original_ptr) {
      old_ptr_size = watchdog.buffer[i]->size;
      if (watchdog.buffer[i]->freed) {
        WATCHDOG_LOG_ERROR("Attempt to reallocate a freed pointer.", file, line,
                           func);
        w_stats.total_time_spent += (w_get_time() - start_time);
        pthread_mutex_unlock(&w_mutex);
        return NULL;
      } else {
        break;
      }
    }
  }

  if (!w_alloc_max_size_check_internal(size, file, line, func)) {
    w_stats.total_time_spent += (w_get_time() - start_time);
    pthread_mutex_unlock(&w_mutex);
    w_free(old_ptr, file, line, func);
    return NULL;
  }
  if (!size) {
    w_stats.total_time_spent += (w_get_time() - start_time);
    pthread_mutex_unlock(&w_mutex);
    w_free(old_ptr, file, line, func);
    return NULL;
  }

  void* new_ptr = malloc(size + (2 * CANARY_SIZE));
  w_alloc_check_internal(new_ptr, size, __FILE__, __LINE__, __func__);
  memset(new_ptr, CANARY_VALUE, size + (2 * CANARY_SIZE));
  size_t move_size;
  if (old_ptr_size > size || !old_ptr_size) {
    move_size = size;
  } else {
    move_size = old_ptr_size;
  }
  memcpy((BYTE*)new_ptr + CANARY_SIZE, old_ptr, move_size);
  WAM_realloc_update_internal(old_ptr, new_ptr, size, file, line, func);
  if (!new_ptr) {
    w_stats.total_time_spent += (w_get_time() - start_time);
    pthread_mutex_unlock(&w_mutex);
    return NULL;
  }
  old_ptr = NULL;

  if (verbose_log) {
    WATCHDOG_LOG("REALLOC", (void*)((BYTE*)new_ptr + CANARY_SIZE), size, file,
                 line, func);
  }
  w_stats.total_time_spent += (w_get_time() - start_time);
  pthread_mutex_unlock(&w_mutex);

  return (BYTE*)new_ptr + CANARY_SIZE;
}

void* w_calloc(size_t count, size_t size, const char* file, const int line,
               const char* func) {
  double start_time = w_get_time();
  w_check_initialization_internal();

  pthread_mutex_lock(&w_mutex);

  w_stats.total_allocations++;
  w_stats.current_usage += size;
  if (w_stats.current_usage > w_stats.peak_usage) {
    w_stats.peak_usage = w_stats.current_usage;
  }

  if (!count) {
    w_stats.total_time_spent += (w_get_time() - start_time);
    pthread_mutex_unlock(&w_mutex);
    return NULL;
  }

  if (!w_alloc_max_size_check_internal(size, file, line, func)) {
    w_stats.total_time_spent += (w_get_time() - start_time);
    pthread_mutex_unlock(&w_mutex);
    return NULL;
  }

  if (!size) {
    w_stats.total_time_spent += (w_get_time() - start_time);
    pthread_mutex_unlock(&w_mutex);
    return NULL;
  }

  if (count * size > (SIZE_MAX - 2 * CANARY_SIZE)) {
    WATCHDOG_LOG_ERROR("Calloc parameter overflow.", file, line, func);
    w_stats.total_time_spent += (w_get_time() - start_time);
    pthread_mutex_unlock(&w_mutex);
    return NULL;
  }

  void* ptr = malloc(count * size + (2 * CANARY_SIZE));
  w_alloc_check_internal(ptr, count * size, __FILE__, __LINE__, __func__);
  WAM_alloc_create_internal(ptr, count * size, file, line, func);
  memset((BYTE*)ptr, 0, count * size + (2 * CANARY_SIZE));
  memset((BYTE*)ptr, CANARY_VALUE, CANARY_SIZE);
  memset((BYTE*)ptr + (count * size) + CANARY_SIZE, CANARY_VALUE, CANARY_SIZE);

  if (verbose_log) {
    WATCHDOG_LOG("CALLOC", (void*)((BYTE*)ptr + CANARY_SIZE), count * size,
                 file, line, func);
  }

  w_stats.total_time_spent += (w_get_time() - start_time);
  pthread_mutex_unlock(&w_mutex);

  return (BYTE*)ptr + CANARY_SIZE;
}

void w_free(void* ptr, const char* file, const int line, const char* func) {
  double start_time = w_get_time();

  pthread_mutex_lock(&w_mutex);

  if (!ptr) {
    pthread_mutex_unlock(&w_mutex);
    return;
  }
  void* original_ptr = (BYTE*)ptr - CANARY_SIZE;

  // Search from the end to prefer the most-recent allocation record for this
  // address. This avoids false double-free errors when the allocator reuses
  // the same addresses.
  for (size_t i = watchdog.size; i-- > 0;) {
    if (watchdog.buffer[i]->ptr == original_ptr) {
      if (!watchdog.buffer[i]->freed) {
        for (int j = 0; j < CANARY_SIZE; j++) {
          if (((BYTE*)original_ptr)[j] != CANARY_VALUE ||
              ((BYTE*)original_ptr + watchdog.buffer[i]->size +
               CANARY_SIZE)[j] != CANARY_VALUE) {
            WATCHDOG_LOG_ERROR("Out of bounds access.", file, line, func);
            break;
          }
        }

        free(watchdog.buffer[i]->ptr);
        watchdog.buffer[i]->freed = true;

        if (verbose_log) {
          WATCHDOG_LOG("FREE",
                       (void*)((BYTE*)watchdog.buffer[i]->ptr + CANARY_SIZE),
                       watchdog.buffer[i]->size, file, line, func);
        }
        w_stats.current_usage -= watchdog.buffer[i]->size;
        w_stats.total_frees++;
        w_stats.total_time_spent += (w_get_time() - start_time);
        pthread_mutex_unlock(&w_mutex);
        return;
      } else {
        WATCHDOG_LOG_ERROR("Double free error.", file, line, func);
        w_stats.total_time_spent += (w_get_time() - start_time);
        pthread_mutex_unlock(&w_mutex);
        return;
      }
    }
  }
  WATCHDOG_LOG_ERROR("Attempt to free unallocated/untracked memory.", file,
                     line, func);
  w_stats.total_time_spent += (w_get_time() - start_time);
  pthread_mutex_unlock(&w_mutex);
  return;
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Watchdog Internals Definitions
////////////////////////////////////////////////////////////////////////////////

static void w_alloc_check_internal(void* ptr, const size_t size,
                                   const char* file, const int line,
                                   const char* func) {
  if (!ptr) {
    fprintf(stderr,
            "[%s:%u:(%s)] Memory allocation error. Failed to allocate %lu "
            "bytes to memory address %p.\n",
            file, line, func, size, (void*)ptr);
    pthread_mutex_unlock(&w_mutex);
    exit(EXIT_FAILURE);
  }
}

static bool w_alloc_max_size_check_internal(const size_t size, const char* file,
                                            const int line, const char* func) {
  if (size > SIZE_MAX - 2 * CANARY_SIZE) {
    WATCHDOG_LOG_ERROR("Out of memory error.", file, line, func);
    pthread_mutex_unlock(&w_mutex);
    return false;
  }
  return true;
}

static void WAM_alloc_create_internal(void* ptr, const size_t size,
                                      const char* file, const int line,
                                      const char* func) {
  WAM* data = malloc(sizeof *data);
  w_alloc_check_internal(data, sizeof *data, __FILE__, __LINE__, __func__);

#if WATCHDOG_COPY_STRINGS
  file = file ? strdup(file) : NULL;
  func = func ? strdup(func) : NULL;
#endif

  data->ptr = ptr;
  data->size = size;
  data->file = file;
  data->line = line;
  data->func = func;
  data->freed = false;

  WDA_push(data);
  return;
}

static void WAM_realloc_update_internal(void* old_ptr, void* new_ptr,
                                        const size_t new_size, const char* file,
                                        const int line, const char* func) {
  void* original_ptr = (BYTE*)old_ptr - CANARY_SIZE;
  for (size_t i = 0; i < watchdog.size; i++) {
    if (watchdog.buffer[i]->ptr == original_ptr) {
      if (!watchdog.buffer[i]->freed) {
        pthread_mutex_unlock(&w_mutex);
        w_free(old_ptr, watchdog.buffer[i]->file, watchdog.buffer[i]->line,
               watchdog.buffer[i]->func);
        WAM_alloc_create_internal(new_ptr, new_size, file, line, func);

        return;
      }
    }
  }

  WAM_alloc_create_internal(new_ptr, new_size, file, line, func);
}

static void w_check_initialization_internal(void) {
  pthread_mutex_lock(&w_mutex);
  if (!w_log_file) {
    pthread_mutex_unlock(&w_mutex);
    w_init(true, false, false);
  }
  pthread_mutex_unlock(&w_mutex);
}

static void w_report(void) {
  verbose_log = false;
  for (size_t i = 0; i < watchdog.size; i++) {
    if (!watchdog.buffer[i]->freed) {
      WATCHDOG_LOG("LEAK",
                   (void*)((BYTE*)watchdog.buffer[i]->ptr + CANARY_SIZE),
                   watchdog.buffer[i]->size, watchdog.buffer[i]->file,
                   watchdog.buffer[i]->line, watchdog.buffer[i]->func);
      w_free((BYTE*)watchdog.buffer[i]->ptr + CANARY_SIZE,
             watchdog.buffer[i]->file, watchdog.buffer[i]->line,
             watchdog.buffer[i]->func);
    }
  }

  fprintf(w_log_file, "\nWatchdog Report:\n");
  fprintf(w_log_file, "Total Allocations:  %zu\n", w_stats.total_allocations);
  fprintf(w_log_file, "Total Frees:        %zu\n", w_stats.total_frees);
  fprintf(w_log_file, "Peak Memory Usage:  %zu Bytes (%.2f MB)\n",
          w_stats.peak_usage, w_stats.peak_usage / 1024.0 / 1024.0);
  fprintf(w_log_file, "Total Tool Latency: %.6f seconds\n",
          w_stats.total_time_spent);
  fprintf(w_log_file, "Avg Latency/Alloc:  %.6f ms\n",
          (w_stats.total_allocations > 0)
              ? (w_stats.total_time_spent / w_stats.total_allocations) * 1000
              : 0);
  fprintf(w_log_file, "\n\n");
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Watchdog Internal Dynamic Array API Definitions
////////////////////////////////////////////////////////////////////////////////

static void WDA_init(void) {
  pthread_mutex_lock(&w_mutex);
  watchdog.size = 0;
  watchdog.capacity = WDA_DEFAULT_BUFFER_SIZE;
  watchdog.buffer = malloc(sizeof *watchdog.buffer * watchdog.capacity);
  w_alloc_check_internal(watchdog.buffer,
                         sizeof *watchdog.buffer * watchdog.capacity, __FILE__,
                         __LINE__, __func__);
  pthread_mutex_unlock(&w_mutex);
}

static void WDA_push(void* ptr) {
  if (watchdog.size == watchdog.capacity) {
    WDA_expand_capacity_internal();
  }
  watchdog.buffer[watchdog.size++] = ptr;
}

static void WDA_pop(void) {
  if (!(watchdog.size > 0)) {
    return;
  }
  watchdog.size--;
#if WATCHDOG_COPY_STRINGS
  if (watchdog.buffer[watchdog.size]) {
    free(watchdog.buffer[watchdog.size]->file);
    free(watchdog.buffer[watchdog.size]->func);
  }
#endif
  free(watchdog.buffer[watchdog.size]);
  watchdog.buffer[watchdog.size] = NULL;
}

static void WDA_cleanup(void) {
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

static void WDA_expand_capacity_internal(void) {
  watchdog.capacity *= WDA_GROWTH_FACTOR;
  WAM** buffer =
      realloc(watchdog.buffer, sizeof *watchdog.buffer * watchdog.capacity);
  w_alloc_check_internal(buffer, sizeof **watchdog.buffer * watchdog.capacity,
                         __FILE__, __LINE__, __func__);
  watchdog.buffer = buffer;
}

////////////////////////////////////////////////////////////////////////////////
