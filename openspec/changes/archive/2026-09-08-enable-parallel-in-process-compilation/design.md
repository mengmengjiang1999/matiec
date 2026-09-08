## Context

`CompilationContext` owns options, diagnostics, parser state, declaration
tables, AST storage, analysis records, and output management. Generated
Flex/Bison mutable state is thread-local, and AST allocation follows the active
context's parser session. The internal pipeline can therefore execute on
separate threads, but the API exposes only one-context-at-a-time compilation.

## Goals / Non-Goals

**Goals:**

- Publish a bounded parallel batch API using independent caller-owned contexts.
- Preserve input order in returned results regardless of completion order.
- Prevent accidental concurrent mutation when one context appears twice.
- Verify valid and invalid full-pipeline jobs together without cross-talk.

**Non-Goals:**

- Making one `CompilationContext` safe for simultaneous calls.
- Recursive parsing on a single thread.
- Adding a persistent executor, cancellation, priorities, or progress callbacks.
- Changing CLI behavior or generated output names within one output directory.

## Decisions

- Add `Compiler::compile_parallel()` accepting a vector of
  `std::reference_wrapper<CompilationContext>` plus an optional maximum
  concurrency. References express caller ownership without requiring contexts
  to become movable.
- Treat zero concurrency as automatic selection from hardware concurrency,
  falling back to one worker; clamp the worker count to the batch size.
- Use a short-lived worker pool with an atomic next-index counter. This bounds
  resource use, keeps result slots aligned with input order, and adds no
  long-lived global state.
- Detect duplicate context addresses before starting workers. Every duplicate
  slot receives a failure result and its context receives one diagnostic; unique
  jobs continue normally.
- Let `compile()` remain the single-job implementation. The batch method only
  schedules calls, so sequential and parallel behavior share one pipeline.

Launching one unbounded `std::async` per input was rejected because a large
batch could create excessive threads and offers weak control over execution.
A new executor abstraction was rejected as unnecessary for this milestone.

## Risks / Trade-offs

- [Callers share output paths between contexts] → Document that each parallel
  job must use an independent output destination; the regression verifies this
  arrangement.
- [A worker encounters an unexpected C++ exception] → Capture exceptions in the
  worker, join all threads, then rethrow the first exception on the caller.
- [Hardware concurrency is unavailable] → Fall back deterministically to one
  worker.

## Migration Plan

This is an additive C++ API. Existing `compile()` callers require no changes.
Reverting the commit removes the batch method and its test without changing file
formats or persisted data.

## Open Questions

None.
