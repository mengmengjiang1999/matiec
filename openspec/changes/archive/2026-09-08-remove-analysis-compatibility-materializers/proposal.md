## Why

All production consumers now use typed Analysis Store records, but obsolete
copy-back visitors and declarations remain reachable from tests. Keeping those
APIs preserves a misleading second result path and allows future code to regress
to mutable AST annotations.

## What Changes

- Remove constant, datatype, resolution, enumeration, and generator
  materialization visitors and public declarations.
- Update record tests to assert store authority without copying values back onto
  AST fields.
- Remove stale specification scenarios that still describe compatibility
  materialization after publication.
- Update README and architecture documentation to state that no materialization
  API remains.

## Capabilities

### New Capabilities

None.

### Modified Capabilities

- `semantic-pass-pipeline`: Completed analysis records have no AST copy-back API.
- `code-generation-boundaries`: Remove the superseded post-generation annotation
  publication/materialization contract.

## Impact

Stage 3 analysis-store adapters, Stage 4 generator adapters, focused tests,
OpenSpec requirements, README, and architecture documentation change. Compiler
output and supported language behavior remain unchanged.
