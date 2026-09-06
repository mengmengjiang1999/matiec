## Why

`VAR_ACCESS` grammar is currently commented out and lacks verified semantic and
backend behavior.

## What Changes

- Establish evidence for access declarations and external paths.
- Implement profile-gated parsing, validation, and export metadata.

## Capabilities

### New Capabilities

- `access-variables`: Access declarations, paths, validation, and generated metadata.

## Impact

Configuration grammar, AST, symbol resolution, output interfaces, tests, and docs.
