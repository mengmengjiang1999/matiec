## Context

Fill and narrow remain the resolution producers. At the completed type-safety
boundary, records already preserve candidates, selected declarations, and extensible
parameter counts. Later lvalue and generator reads can therefore stop using AST fields.

## Goals / Non-Goals

**Goals:** explicit store injection, direct downstream reads, unchanged output and
diagnostics, and removal of production compatibility materialization.

**Non-Goals:** rewrite fill/narrow producer internals or delete resolution fields
from generated AST classes while those producers still use them.

## Decisions

- Keep publication after forced narrowing as the completed type-safety boundary.
- Give `lvalue_check_c` a const store reference.
- Attach an optional read-only store pointer to `stage4out_c`, which already flows
  through every C generator component; production Stage 4 always supplies it.
- Centralize Stage 4 resolution lookups in a small checked accessor.
- Stop calling the resolution materializer in production.

## Migration Plan

Implement direct consumers, validate normal and sanitizer suites, archive, commit,
and push independently.
