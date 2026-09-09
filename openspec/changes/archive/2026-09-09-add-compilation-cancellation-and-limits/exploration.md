# Exploration: compilation cancellation and resource limits

## Current state

Compilation contexts own parser, diagnostic, source, and output state, but a host
cannot interrupt an in-flight parse or bound work produced by untrusted input.
The scanner is the narrowest hot-path cancellation point. `DiagnosticEngine`,
`SourceManager`, and `OutputManager` are the existing ownership boundaries for
the requested limits.

## Findings

- Cancellation must be an atomic, sticky request because another thread may set
  it while the owning thread compiles.
- Resetting cancellation must remain an explicit host action so an accidentally
  reused context does not silently resume work.
- A zero limit can consistently mean unlimited, preserving current behavior.
- The primary source and each virtual include can be checked before parsing;
  output and diagnostic counts can be enforced at their manager boundaries.
- A terminal diagnostic must identify cancellation or limit exhaustion while
  remaining within the configured diagnostic count.

## Decision

Add a size-tagged limits structure and cancel/reset APIs in C API 1.5.0. Store
policy in the compilation context, poll cancellation in the scanner and between
major compiler stages, and make each manager enforce its own bound.
