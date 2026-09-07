## Context

The C datatype generator assigns implicit type identifiers while traversing the
AST and later generator visitors consume those identifiers. Today both sides use
`symbol_c::anotations_map`; only after all output succeeds does Stage 4 copy the
maps into `AnalysisStore`, then redundantly copy them back. Unlike completed
semantic passes, this family needs a live mutable store channel during Stage 4.

## Goals / Non-Goals

**Goals:**

- Make typed generator records the live producer/consumer channel.
- Preserve arena ownership checks for both record keys and stored symbols.
- Ensure every derived Stage 4 output shares the same analysis context.
- Leave AST generator annotation maps untouched in production.

**Non-Goals:**

- Rename the historical implicit-type annotation key.
- Remove generic AST annotation storage from the public node layout.
- Redesign implicit datatype generation.

## Decisions

- Add `set_generator_symbol` and `generator_symbol` operations to
  `AnalysisStore`. A named update preserves existing symbols in the node's record
  and validates ownership before committing the updated record.
- `stage4out_c` owns a non-owning mutable store pointer and exposes both mutable
  and const accessors. Its child file outputs inherit the same pointer.
- Small Stage 4 helper functions centralize missing-store behavior and named
  lookup, keeping generator visitors independent of record layout.
- Remove production `publish_generator_analysis` and materialization after
  output. The compatibility functions remain available for tests and embedders.

## Risks / Trade-offs

- [A generated helper node is not arena-owned] → store validation rejects the
  update and Stage 4 treats that as an internal generation error.
- [A child output drops the context pointer] → constructor propagation is covered
  by generated-C, initialization, CLI, and sanitizer regression suites.
- [Partial generation leaves records] → records have compilation lifetime and
  failed compilation is not reused as a successful result.

## Migration Plan

Introduce the store operations and Stage 4 helpers, migrate every annotation
site, remove the completion copy steps, verify all output modes, then archive.
Rollback restores AST accesses and the final publisher/materializer calls.

## Open Questions

None.
