## Why

Function and function-block invocation resolution is still passed from Stage 3 to
Stage 4 through specialized mutable AST fields. Moving the completed candidate and
declaration results into the Analysis Store gives them the same compilation
lifetime and ownership checks as other semantic results.

## What Changes

- Publish function candidates, resolved function/FB declarations, and extensible
  parameter counts after datatype narrowing.
- Materialize compatibility fields for existing diagnostics and generators.
- Add focused record, ownership, compatibility, and reset coverage.
- Update README and architecture documentation.

## Capabilities

### Modified Capabilities

- `semantic-pass-pipeline`: declaration resolution crosses Stage 3 boundaries in
  typed context-owned records.
- `compilation-context`: resolution records obey arena and reset rules.

## Impact

Analysis Store APIs, Stage 3 orchestration/build files, compiler tests, README, and
architecture documentation.
