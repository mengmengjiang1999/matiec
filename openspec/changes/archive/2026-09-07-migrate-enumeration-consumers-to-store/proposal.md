## Why

Enumeration checking already publishes every scope-specific enum lookup table to
the compilation-owned `AnalysisStore`, but Stage 3 immediately copies those
records back to AST compatibility fields even though no later production pass
reads them. Removing that redundant materialization makes the typed store the
completed-phase boundary and prevents stale duplicate state.

## What Changes

- Keep enumeration declaration checking as the producer of scope lookup tables.
- Retain the arena-checked typed enumeration records as the authoritative output.
- Remove production AST materialization after enumeration publication.
- Add regression coverage proving records remain usable while AST compatibility
  tables stay empty.
- Update README and architecture documentation to reflect the direct boundary.

## Capabilities

### New Capabilities

None.

### Modified Capabilities

- `semantic-pass-pipeline`: Completed enumeration analysis is retained in the
  context store without a production AST copy-back step.

## Impact

This affects the Stage 3 enumeration boundary, its focused store test, and the
documented semantic-state ownership model. The optional compatibility
materializer remains available to isolated legacy callers and tests.
