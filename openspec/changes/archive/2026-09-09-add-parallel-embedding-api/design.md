## Context

`Compiler::compile_parallel` already guarantees bounded workers, input-order
results, failure isolation, and duplicate-context rejection. The C layer must
validate size-tagged outputs and dispatch callbacks without weakening those
properties.

## Goals / Non-Goals

**Goals:** ordered batch results, bounded concurrency, automatic worker count,
per-context diagnostics, and exception containment.

**Non-Goals:** cancellation, asynchronous handles, shared contexts, or a global
thread pool.

## Decisions

`matiec_compile_batch` accepts parallel arrays of context pointers and initialized
result structures. Zero contexts is a successful no-op; zero concurrency selects
an implementation-defined positive worker count. All arguments are validated
before compilation starts.

Each context is cleared and compiled once. Duplicate context pointers are passed
to the existing scheduler, which records an error for every duplicate result.
Configured callbacks execute on worker threads, so user data shared between
contexts must be synchronized by the caller.

## Risks / Trade-offs

A synchronous batch call still occupies its calling thread, but avoids adding an
asynchronous lifetime model to the ABI.

## Migration Plan

Existing callers remain valid. Batch users create and configure independent
contexts, initialize one result per context, call the batch function, then
inspect and destroy each context normally.

## Open Questions

None.
