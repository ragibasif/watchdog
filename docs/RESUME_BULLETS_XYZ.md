# Watchdog Resume Bullets in X-Y-Z Format

These bullets follow the X-Y-Z pattern:

- Accomplished X
- as measured by Y
- by doing Z

## General Software Engineering

- Built a lightweight C memory-debugging library that detects leaks,
  invalid frees, double frees, and heap overflows, improving debugging
  coverage for core allocation paths by wrapping `malloc`, `calloc`,
  `realloc`, and `free` with source-aware instrumentation.

- Improved allocation safety and standards compliance in a custom memory
  tracker by fixing `realloc` ownership and metadata edge cases, preventing
  false positives and data-loss scenarios through stricter pointer validation
  and corrected tracking semantics.

- Increased the reliability of a native debugging tool in concurrent programs
  by hardening thread-safe initialization and shutdown behavior, ensuring the
  tracker state is fully initialized before multi-threaded allocation activity.

- Reduced the risk of integer-overflow bugs in heap instrumentation by adding
  defensive `calloc` size validation, protecting the tracker from overflow-
  prone arithmetic in high-risk allocation paths.

## Systems / C / Low-Level Roles

- Engineered a POSIX-thread-safe heap tracker in C that records allocation
  metadata and emits file, line, and function diagnostics, making memory
  failures easier to root-cause by correlating runtime errors with call sites.

- Implemented canary-based overflow detection around heap allocations, enabling
  runtime identification of out-of-bounds writes without introducing external
  runtime dependencies.

- Designed a macro-based instrumentation layer for native codebases, allowing
  developers to opt into memory tracking with a compile flag instead of
  rewriting allocator call sites manually.

- Strengthened low-level memory tooling correctness by auditing pointer
  lifecycles, reused addresses, and shutdown reporting behavior, improving the
  trustworthiness of diagnostics in debug builds.

## Open Source / Portfolio Variants

- Created and documented an open-source C debugging library that demonstrates
  systems programming, concurrency control, and runtime instrumentation through
  a reusable tool with examples, tests, and visual demos.

- Packaged a systems project for technical storytelling by pairing production-
  style implementation details with README guidance, interview-ready STAR
  narratives, and recruiter-friendly X-Y-Z resume bullets.

## Shorter Variants

- Built a C heap-debugging library that detects leaks and invalid memory usage
  by instrumenting standard allocation APIs with metadata tracking and canary
  checks.

- Improved correctness in a custom allocator wrapper by fixing `realloc`,
  overflow, and initialization edge cases, reducing misleading diagnostics in
  memory-debug builds.

- Developed a thread-safe memory instrumentation tool for C programs, enabling
  faster debugging through source-level allocation logs and end-of-run reports.
