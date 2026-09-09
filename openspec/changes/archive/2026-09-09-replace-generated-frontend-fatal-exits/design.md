## Context

Handwritten frontend failures already throw `CompilationAbort`, which
`Compiler::compile()` catches. Flex's generated `yy_fatal_error()` is the last
frontend path that terminates the process. Bison memory exhaustion already
reports through `yyerror()` and returns a nonzero parser status.

## Goals / Non-Goals

**Goals:** eliminate generated frontend process termination, preserve a single
embedding-safe unwind boundary, and verify the generated shape.

**Non-Goals:** allocator injection, changing ordinary syntax diagnostics, or
changing the Flex scanner ownership model.

## Decisions

Extend the checked generated-frontend rewrite to replace the unique Flex
`exit(YY_EXIT_FAILURE)` statement with a thrown `CompilationAbort`. The scanner
grammar already includes the exception definition, so regenerated C++ can throw
it without a new public API. The rewrite remains idempotent and rejects missing
or duplicate source shapes.

Bison's `YYNOMEM` return path remains unchanged because it already returns
control to `stage1_2()`.

## Risks / Trade-offs

Flex versions may format the exit statement differently. A small set of checked
variants supports known generators, and an unknown version fails during the
build instead of silently restoring process termination.

## Migration Plan

Regenerate the frontend and verify that generated parser/scanner sources contain
no `exit()`, `_Exit()`, or `abort()` call.

## Open Questions

None.
