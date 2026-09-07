## Context

The compiler's visitors and static datatype utilities predate
`CompilationContext`, so many do not carry an explicit analysis reference. Flow
records are complete before constant/type analysis; constant records are complete
before type analysis. Both are currently materialized onto AST fields solely to
serve these legacy call shapes.

## Goals / Non-Goals

**Goals:**

- Provide nested, compilation-scoped access to the active `AnalysisStore`.
- Route completed flow and constant reads through typed records.
- Retain AST fallback only for stack/synthetic nodes created inside producers or
  Stage 4 and therefore absent from the store.
- Remove production copy-back of both record families.

**Non-Goals:**

- Rewrite constant folding to produce records incrementally.
- Remove flow/constant fields from AST node layouts.
- Enable parallel compilation; legacy global adapters still prohibit it.

## Decisions

- Add `ActiveAnalysisStoreScope`, mirroring `ActiveAstArenaScope`, with a
  thread-local pointer restored on scope exit. The compiler activates it for the
  whole parsed-AST lifetime, preserving nested compilation behavior.
- Central accessors prefer a matching typed record and fall back to the node's
  AST field only when no record exists. Thus completed arena-owned nodes use the
  store while transient synthetic nodes keep working.
- Consumer macros and direct reads call these accessors. Producer macros continue
  writing their local AST scratch until publication.
- Remove the Stage 3 flow materializer class and constant materializer call only
  after all repository production reads are routed through accessors.

## Risks / Trade-offs

- [A consumer runs without an active scope] → explicit unit tests activate the
  scope; fallback preserves standalone legacy utilities.
- [A completed node unexpectedly lacks a record] → focused tests clear AST fields
  and full regression suites exercise the pipeline.
- [Nested compilation selects the wrong store] → RAII restoration mirrors the
  tested arena/parser scope pattern.

## Migration Plan

Add scoped access and tests, migrate flow then constant read sites, remove both
materializers, update docs, and run all gates. Rollback restores the two
materialization calls while leaving accessors harmless.

## Open Questions

None.
