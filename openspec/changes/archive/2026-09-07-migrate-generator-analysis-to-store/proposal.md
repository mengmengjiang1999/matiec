## Why

Stage 4's generator-specific symbol annotations are the last typed Analysis Store
record family without a production boundary. Publishing them after successful
generation completes the planned incremental migration while preserving legacy
generator behavior.

## What Changes

- Publish every AST node's named generator annotations after Stage 4 succeeds.
- Provide compatibility materialization and enforce compilation ownership.
- Add focused key/value, ownership, compatibility, and reset tests.
- Update README, architecture documentation, and the migration decision to reflect
  the completed set of record-family boundaries.

## Capabilities

### Modified Capabilities

- `code-generation-boundaries`: generator annotations become context-owned results.
- `compilation-context`: generator records obey arena and reset rules.

## Impact

Analysis Store API, compiler/store adapter, Stage 4 completion boundary, tests,
README, architecture documentation, and migration decision status.
