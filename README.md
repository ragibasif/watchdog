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

Then `#include watchdog.h` in a source/header file and pass flag `-DWATCHDOG` to
the CFLAGS of your build system to enable the debugger.

![./docs/include_file.svg](./docs/include_file.svg)

The following line is an example of a possible build instruction. The project
requires at least C11 to be used as intended.

```sh
gcc -std=c11 -DWATCHDOG -lpthread main.c watchdog.c -o program
```

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

### Malloc

![./docs/malloc.svg](./docs/malloc.svg)

![./docs/malloc.gif](./docs/malloc.gif)

### Realloc

![./docs/realloc.svg](./docs/realloc.svg)

![./docs/realloc.gif](./docs/realloc.gif)

### Calloc

![./docs/calloc.svg](./docs/calloc.svg)

![./docs/calloc.gif](./docs/calloc.gif)

### Free

![./docs/free.svg](./docs/free.svg)

![./docs/free.gif](./docs/free.gif)

### Out of Bounds Checking

![./docs/out-of-bounds.svg](./docs/out-of-bounds.svg)

![./docs/out-of-bounds.gif](./docs/out-of-bounds.gif)

Disclaimer: **For educational and recreational purposes only.**
