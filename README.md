<div align="center">
<h1>
 Watchdog
</h1>
</div>

<div align="center">
<p>
  <a href="https://github.com/ragibasif/watchdog">
    <img alt="GitHub" src="https://img.shields.io/badge/GitHub-0D1117?style=for-the-badge&logo=github&logoColor=C9D1D9">
  </a>
  <a href="https://github.com/ragibasif/watchdog/pulse">
    <img alt="Last Commit" src="https://img.shields.io/github/last-commit/ragibasif/watchdog?style=for-the-badge&logo=github&color=58A6FF&logoColor=C9D1D9&labelColor=0D1117">
  </a>
  <a href="https://github.com/ragibasif/watchdog/stargazers">
    <img alt="Stars" src="https://img.shields.io/github/stars/ragibasif/watchdog?style=for-the-badge&logo=apachespark&color=D29922&logoColor=C9D1D9&labelColor=0D1117">
  </a>
  <a href="https://github.com/ragibasif/watchdog/network/members">
    <img alt="Forks" src="https://img.shields.io/github/forks/ragibasif/watchdog?style=for-the-badge&logo=github&color=3FB950&logoColor=C9D1D9&labelColor=0D1117">
  </a>
  <a href="https://github.com/ragibasif/watchdog/actions">
    <img alt="Build Status" src="https://img.shields.io/github/actions/workflow/status/ragibasif/watchdog/build.yml?branch=master&style=for-the-badge&label=build&logo=githubactions&color=58A6FF&logoColor=C9D1D9&labelColor=0D1117">
  </a>
  <a href="https://visitorbadge.io/status?path=https%3A%2F%2Fgithub.com%2Fragibasif%2Fwatchdog">
    <img alt="Visitors" src="https://api.visitorbadge.io/api/visitors?path=https%3A%2F%2Fgithub.com%2Fragibasif%2Fwatchdog&label=visitors&labelColor=%230D1117&countColor=%2358A6FF">
  </a>
  <a href="https://github.com/ragibasif/watchdog/issues">
    <img alt="Issues" src="https://img.shields.io/github/issues/ragibasif/watchdog?style=for-the-badge&logo=bilibili&color=F85149&logoColor=C9D1D9&labelColor=0D1117">
  </a>
  <a href="https://github.com/ragibasif/watchdog/blob/master/LICENSE">
    <img alt="License" src="https://img.shields.io/github/license/ragibasif/watchdog?style=for-the-badge&logo=starship&color=8957E5&logoColor=C9D1D9&labelColor=0D1117">
  </a>
</p>
</div>

Watchdog wraps dynamic memory functions (`malloc`, `calloc`, `realloc`, `free`) and
tracks all memory activity at runtime:

- Memory Leak Detection
- Buffer Overflows / Out-of-Bounds Access
- Double Free Detection
- Thread Safe
- Verbose Logging with Optional File Output
- Minimal Integration – Just One Header and One C File

![./docs/demo_0.svg](./docs/demo_0.svg)

![./docs/demo_0.gif](./docs/demo_0.gif)

## Usage

### Installation

Include `watchdog.h` and `watchdog.c` in your project.

![./docs/project_dir.svg](./docs/project_dir.svg)

Then `#include watchdog.h` in a source/header file and pass flag `-DWATCHDOG_ENABLE` to
the CFLAGS of your build system to enable the debugger or add `#define WATCHDOG_ENABLE`
to a file.

![./docs/include_file.svg](./docs/include_file.svg)

### Defaults

Verbose logging is on by default, log to file is off by default, and color
output is off by default.

![./docs/defaults.svg](./docs/defaults.svg)

To customize the defaults, pass appropriate boolean to `w_init`.

![./docs/change_defaults.svg](./docs/change_defaults.svg)

Enabling `log_to_file` will direct log output to a log file named `watchdog.log`.
Color output is turned off if `log_to_file` is enabled regardless of the
`enable_color_output` variable value.

If `enable_verbose_log` is set to false, only errors will be logged.

## Examples

### Malloc

```c
void malloc_example(void) {

    size_t count  = 5;
    int   *buffer = malloc(sizeof *buffer * 5);
    for (size_t i = 0; i < count; i++) {
        buffer[i] = -(i - count) * count;
    }
    for (size_t i = 0; i < count; i++) {
        printf("%d ", buffer[i]);
    }

    putchar('\n');
    free(buffer);
    buffer = NULL;
}
```

![./docs/malloc.gif](./docs/malloc.gif)

### Realloc

```c
void realloc_example(void) {
    short *buffer = malloc(34222);
    buffer        = realloc(buffer, 2342);
    buffer        = realloc(buffer, 2342342);
    buffer        = realloc(buffer, 2);
    buffer        = realloc(buffer, 10);
    buffer        = realloc(buffer, 0);
    free(buffer);
    buffer = NULL;
}
```

![./docs/realloc.gif](./docs/realloc.gif)

### Calloc

```c
void calloc_example(void) {
    // Demonstrates correct usage of calloc
    size_t count  = 5;
    int   *buffer = calloc(count, sizeof *buffer);
    for (size_t i = 0; i < count; i++) {
        printf("%d ", buffer[i]); // should output 0
    }
    putchar('\n');
    free(buffer);
    buffer = NULL;
}
```

![./docs/calloc.gif](./docs/calloc.gif)

### Free

```c
void free_example(void) {
    // Demonstrates correct usage of free
    size_t         count   = 5;
    short int     *buffer0 = malloc(sizeof *buffer0 * count);
    unsigned char *buffer1 = calloc(count, sizeof *buffer1);
    long double   *buffer2 = realloc(buffer2, sizeof *buffer2 * count);
    free(buffer1);
    buffer1 = NULL;
    free(buffer2);
    buffer2 = NULL;
    free(buffer0);
    buffer1 = NULL;
}
```

![./docs/free.gif](./docs/free.gif)

### Leak Check

```c
void leak_example(void) {
    unsigned long long int *buffer = malloc(sizeof *buffer * 20);
    // intentionally not calling free
    // watchdog will detect this and report it
}
```

![./docs/leak_check.gif](./docs/leak_check.gif)

### Overflow Check

```c
void overflow_example(void) {
    char *buffer = malloc(sizeof *buffer * 10);
    strcpy(buffer, "This will overflow"); // out-of-bounds write
    // of buffer overflows will be detected with using canary values
}
```

![./docs/overflow_check.gif](./docs/overflow_check.gif)

### Double Free Check

```c
void double_free_example(void) {
    double *buffer = malloc(sizeof *buffer * 20);
    free(buffer);
    free(buffer); // triggers a double-free error
}
```

![./docs/double_free_check.gif](./docs/double_free_check.gif)

### Invalid Free Check

```c
void invalid_free_example(void) {
    float *buffer;
    free(buffer); // will trigger an error since buffer wasn't allocated
}
```

![./docs/invalid_free_check.gif](./docs/invalid_free_check.gif)
