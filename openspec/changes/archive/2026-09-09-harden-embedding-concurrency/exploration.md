# Exploration: embedding concurrency hardening

## Current state

The suite proves two-context frontend isolation and a small mixed C++ batch.
It does not repeatedly reuse C contexts, combine callback failures with healthy
jobs, inject exceptions from C++ callbacks, or run a race detector in CI.

## Findings

- The existing opaque C API is the most representative stress boundary because
  it combines parser state, callbacks, diagnostics, limits, and batch workers.
- A deterministic multi-round matrix is more actionable than random-only tests:
  every run can assert result order and per-context callback isolation.
- GCC ThreadSanitizer is available on the Linux CI image and can reuse the
  out-of-tree sanitizer runner. Darwin's runtime support is toolchain-dependent,
  so the gate belongs in Linux CI.
- A useful coverage-guided fuzz target would require a smaller parser harness
  and a seed/corpus policy; adding an unbounded CI fuzzer here would make the
  change non-deterministic. Deterministic mutation/fault cases are included now.

## Decision

Add a C++ C-ABI stress test with repeated context reuse, mixed outcomes,
resolver/output exceptions, and shared atomic callback accounting. Extend the
sanitizer runner and Linux matrix with ThreadSanitizer.
