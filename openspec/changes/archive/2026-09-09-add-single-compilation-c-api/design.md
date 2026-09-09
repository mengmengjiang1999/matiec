## Context

The compiler already supports reusable `CompilationContext` objects, file or
owned-memory input, typed options, and structured results. The C ABI must wrap
those capabilities without leaking layouts or exceptions.

## Goals / Non-Goals

**Goals:** lifecycle, source selection, core options, synchronous compilation,
result counts, and deterministic argument validation.

**Non-Goals:** diagnostic enumeration, output callbacks, parallel batches,
installation, or ABI tooling.

## Decisions

`matiec_context_t` is incomplete in the header and owns one C++
`CompilationContext`. Setters copy all strings and source bytes. Functions return
a small status enum; compiler success remains in `matiec_result_t`. The last API
error is a borrowed context-owned string valid until the next call or destroy.

Every extern-C operation catches allocation and unexpected exceptions. Null
destroy is allowed; other null required arguments return invalid-argument.

## Risks / Trade-offs

Only essential options are exposed initially. Additive setters can follow under
minor API versions without growing a public options struct.

## Migration Plan

Callers create a context, configure input/options, compile, inspect the result,
and destroy the context.

## Open Questions

None.
