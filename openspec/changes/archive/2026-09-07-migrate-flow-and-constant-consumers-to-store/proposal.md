## Why

Flow analysis is already produced directly in `AnalysisStore`, and constant
folding publishes completed values there, but the pipeline copies both families
back onto AST nodes for later passes. Migrating their consumers removes two more
mutable compatibility channels and lets downstream semantics and generators use
one context-owned source of truth.

## What Changes

- Add a compilation-scoped analysis access bridge for legacy visitor utilities.
- Read flow predecessors/successors from typed records in semantic and generator
  consumers, with AST fallback only for transient producer-local nodes.
- Read completed constants from typed records in datatype, range, case, and code
  generation consumers.
- Remove production flow and constant materialization.
- Add no-materialization coverage and update README/architecture documentation.

## Capabilities

### New Capabilities

None.

### Modified Capabilities

- `semantic-pass-pipeline`: Flow and constant consumers use context-owned typed
  records after their respective completion boundaries.
- `code-generation-boundaries`: Generators obtain completed flow and constant
  data from the active compilation analysis store.

## Impact

This affects the analysis-store access API, compiler context activation, IL
semantic visitors, constant consumers, Stage 4 generators, focused tests, and
architecture documentation. Producer-local AST scratch remains supported until
each producer itself is redesigned.
