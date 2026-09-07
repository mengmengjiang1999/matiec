## Why

`VAR_ACCESS` declarations now enter the primary AST, but a regex pre-scan still
duplicates their validation and produces `ACCESS.csv` metadata. The AST should be
the structural authority.

## What Changes

- Remove experimental pre-parse `VAR_ACCESS` normalization.
- Analyze configuration globals and access declarations from the parsed AST.
- Populate context metadata and preserve existing diagnostics and CSV output.
- Keep legacy rejection and the bounded simple-global subset unchanged.

## Capabilities

### Modified Capabilities

- `access-variables`: validation and metadata originate from primary AST nodes.
- `code-generation-boundaries`: access metadata is derived after parsing.

## Impact

Compiler orchestration, access-variable analysis, tests, documentation, and the
feature catalogue are affected.
