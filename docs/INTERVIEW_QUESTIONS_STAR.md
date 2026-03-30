# Watchdog Interview Questions and Answers

The answers below are written in STAR format so they can be used directly in
technical interviews, project walkthroughs, and portfolio discussions.

## 1. Tell me about a systems project where you improved reliability.

**Situation:** I built Watchdog, a C memory-tracking library for debugging heap
allocations in small native projects where memory issues were hard to diagnose
from crash logs alone.

**Task:** I needed to make allocation bugs visible at runtime without adding
external dependencies or requiring developers to rewrite their code.

**Action:** I wrapped `malloc`, `calloc`, `realloc`, and `free` behind macros,
tracked every allocation in metadata, added canary regions around user memory,
and emitted leak, overflow, double-free, and invalid-free diagnostics with
source location information.

**Result:** The project became a reusable debugging tool that surfaces the most
common heap misuse patterns early, making memory failures easier to reproduce
and explain during development.

## 2. Describe a time you found and fixed a subtle bug.

**Situation:** During review, I found that the custom `realloc` path could
mis-handle pointers when addresses were reused by later allocations.

**Task:** I needed to preserve correct `realloc` semantics while keeping the
tracking metadata accurate and avoiding false positives.

**Action:** I changed the lookup to search the most recent allocation record
first, rejected untracked pointers explicitly, and stopped freeing the original
block on nonzero resize failures.

**Result:** The implementation became much closer to standard `realloc`
behavior, removed a data-loss bug, and avoided misclassifying live allocations
as already freed.

## 3. Tell me about a time you improved thread safety.

**Situation:** Watchdog is intended to be used in multi-threaded C programs,
which means its internal tracking state must stay consistent under concurrent
allocations.

**Task:** I needed to make initialization and allocation bookkeeping safe even
when multiple threads touched the library at startup.

**Action:** I audited the locking strategy, ensured initialization happened once
under the mutex, and made the tracker state valid before other threads could
observe the library as initialized.

**Result:** The initialization path stopped exposing partially initialized
state, which reduced the risk of races around logging setup and the metadata
array.

## 4. Describe a time you handled unsafe edge cases in low-level code.

**Situation:** Low-level memory helpers are full of overflow and undefined-
behavior traps, especially around `calloc`, pointer ownership, and invalid free
paths.

**Task:** I needed to harden Watchdog so it would fail predictably instead of
introducing new bugs while trying to detect existing ones.

**Action:** I replaced an overflow-prone multiplication check with a division-
based guard, validated tracked pointers before copying memory, and cleaned up
logging and shutdown behavior around initialization and finalization.

**Result:** The library became safer under malformed inputs and more trustworthy
as a debugging aid because it reduced self-inflicted failure modes.

## 5. Tell me about a time you balanced usability with correctness.

**Situation:** A debugging library is only useful if it is easy to drop into an
existing codebase, but memory tooling also has to be precise enough to avoid
misleading engineers.

**Task:** I needed to make Watchdog lightweight to adopt while still producing
high-signal diagnostics.

**Action:** I kept integration simple through a header-based macro interface,
added optional colored and file-backed logging, and retained file/line/function
context in the allocation metadata for readable reports.

**Result:** The tool stayed low-friction for developers while still producing
actionable diagnostics that point directly to call sites.

## 6. Describe a time you wrote documentation for technical adoption.

**Situation:** The library solves debugging problems, but new users still need
examples, constraints, and setup instructions before they can apply it
confidently.

**Task:** I needed to document how to integrate, configure, and reason about
Watchdog without burying the reader in implementation details.

**Action:** I wrote a README with examples, build instructions, usage notes,
feature explanations, and visual demos for the main detection paths.

**Result:** The project became easier to evaluate quickly, which helps both
contributors and interviewers understand the technical choices and intended
usage.

## 7. Tell me about a project where testing influenced the design.

**Situation:** Watchdog is built to detect runtime memory problems, so the best
way to validate it is to trigger realistic misuse patterns intentionally.

**Task:** I needed a test setup that exercised allocation, reallocation, leak
detection, double frees, invalid frees, and buffer overflows.

**Action:** I added a test harness with focused misuse examples, compiled the
library separately from the instrumented test program, and used those scenarios
to verify the logging and shutdown report behavior.

**Result:** The tests acted as both regression coverage and executable
documentation for how the library behaves under common memory failures.

## 8. Describe a time you made a project more portfolio-ready.

**Situation:** Open-source projects often have solid code but weak presentation,
which makes them harder to discuss in resumes and interviews.

**Task:** I needed to package Watchdog in a way that highlighted both the
engineering value and the communication value of the work.

**Action:** I combined implementation cleanup with clearer documentation,
examples, and concise narratives that explain the problem, design, and outcome.

**Result:** The project became easier to present as evidence of systems
programming, debugging, API design, and engineering judgment.
