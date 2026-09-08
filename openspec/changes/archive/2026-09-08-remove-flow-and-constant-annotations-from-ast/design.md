## Context

Flow analysis already inserts reciprocal edges directly into `AnalysisStore`, but
typed accessors still fall back to vectors embedded in two IL AST classes.
Constant propagation still writes `symbol_c::const_value`, after which a separate
visitor copies every arena node into constant records. Datatype analysis established
the intended migration pattern: persistent arena nodes update typed records in
place, while stack-local/shared helper nodes use a context-scoped transient table.

## Goals / Non-Goals

**Goals:**

- Remove flow and constant semantic storage from AST node layouts.
- Preserve the existing ordered flow-edge and constant-lattice behavior.
- Let constant producers update persistent and transient context records through
  one working API.
- Delete the redundant constant publication traversal.

**Non-Goals:**

- Removing the active-analysis compatibility scope; that is handled by the later
  explicit-dependencies change.
- Changing constant-folding rules, diagnostics, or generated output.
- Removing resolution, enumeration, or generator annotations in this change.

## Decisions

1. Flow accessors return an immutable empty vector when no typed record exists.
   Existing semantic algorithms that synthesize stack-local IL nodes write their
   temporary predecessor state through a context-owned transient flow record.

2. `AnalysisStore::constant_working` returns the persistent record value for an
   arena node and a context-owned transient value for other helper nodes. A mutable
   compatibility accessor and `symbol_c::const_value()` bridge existing constant
   code until analysis dependencies become explicit. This mirrors the proven
   datatype migration and avoids rewriting folding semantics at the same time.

3. Persistent working records are immediately canonical. The compiler removes the
   constant publisher from Stage 3 instead of retaining a second synchronization
   phase. Focused tests verify that values are readable before any tree walk.

4. Transient flow and constant records are cleared by `AnalysisStore::clear()` and
   destroyed with the compilation context. They are never counted as completed
   persistent records or accepted as cross-context output.

## Risks / Trade-offs

- [A missed direct field access fails compilation] → Search all compiler and test
  sources, then build normal, ASan, and UBSan configurations.
- [A stack-local constant helper loses state] → Route mutable compatibility
  access through the transient constant table and cover it in a focused test.
- [Missing flow data becomes silently empty] → Preserve existing generic-accessor
  behavior and test that completed arena records still contain reciprocal edges.

## Migration Plan

Create the working-record API and focused tests, migrate constant and flow users,
remove AST fields and publication build wiring, run the full validation matrix,
then archive this change. Reverting the single change commit restores the previous
compatibility fields and publisher.

## Open Questions

None.
