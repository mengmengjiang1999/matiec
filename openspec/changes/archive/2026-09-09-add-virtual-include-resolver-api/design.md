## Context

The scanner owns a nested include stack but each entry assumes a `FILE *`.
ParserState is already the explicit context-selected bridge used by generated
frontend code, so it is the appropriate owner of resolver behavior.

## Goals / Non-Goals

**Goals:** on-demand virtual includes, nested resolution, copied callback data,
deterministic diagnostics, and unchanged disk behavior when disabled.

**Non-Goals:** replacing the MATIEC include pragma, defining URI semantics, or
making callbacks reentrant on the same context.

## Decisions

The public callback returns `RESOLVED`, `NOT_FOUND`, or `ERROR` and fills a
size-tagged borrowed view. The compiler validates and copies both display name
and exact byte length before the callback returns. An installed callback is
authoritative so hermetic hosts never accidentally read disk. An explicit
`USE_FILESYSTEM` result permits compiler library data or host-approved paths to
use the configured include directory.

ParserState stores a C++ resolver function. The generated scanner passes its
explicit parser session into include handling. Tracking entries can read either
a file or owned memory bytes, allowing the normal nested Flex buffer stack to be
reused without `tmpfile`.

## Risks / Trade-offs

- [Borrowed callback data expires] -> synchronously validate and copy it.
- [Silent disk fallback breaks hermeticity] -> treat resolver misses as errors.
- [Nested buffers leak] -> centralize tracking cleanup for file and memory entries.
- [ABI growth] -> append size-tagged types/functions and increment the minor version.

## Migration Plan

Existing callers need no changes. Hosts opt in by setting a resolver and clear it
with a null callback to restore filesystem includes.

## Open Questions

None.
