## Context

Diagnostics already live in a context-owned vector. Generated files are created
through `OutputManager`, but its file-sink type is currently fixed. Both are
natural context-scoped integration boundaries.

## Goals / Non-Goals

**Goals:** stable diagnostic views, deterministic callback delivery, in-memory
generated-output delivery, callback failure reporting, and C-safe lifetimes.

**Non-Goals:** callback serialization across contexts, caller-owned compiler
buffers, streaming input, or parallel batch scheduling.

## Decisions

Diagnostic getters fill a size-tagged view whose strings remain owned by the
context until the next compilation or destruction. The optional diagnostic
callback receives those same views in stored order after compilation completes.

An output callback installs a file-sink factory. Each generated artifact is
identified by its intended path and delivered in ordered byte chunks. Returning
zero rejects a chunk; the output manager records that as a compilation
diagnostic. Clearing the callback restores normal filesystem output.

Callbacks are invoked synchronously on the compiling thread. They must not call
back into the same context.

## Risks / Trade-offs

Generated content may arrive in multiple chunks, so hosts must append by path.
This avoids retaining an extra full copy inside the library.

## Migration Plan

Existing 1.1 callers require no changes. Hosts opt into either callback
independently and may continue using indexed diagnostics without callbacks.

## Open Questions

None.
