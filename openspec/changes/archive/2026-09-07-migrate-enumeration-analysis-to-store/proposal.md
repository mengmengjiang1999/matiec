## Why

Enumeration checking builds scope-specific, case-insensitive multimaps on six AST
node types. Those symbol tables are semantic results and should follow the active
compilation context rather than exist only as mutable annotations.

## What Changes

- Preserve enumeration tables as case-insensitive multimaps in typed store records.
- Publish the six enumeration scopes after enumeration checking and materialize
  compatibility tables for existing consumers.
- Add duplicate-key, case-insensitive, ownership, compatibility, and reset tests.
- Update README and architecture documentation.

## Capabilities

### Modified Capabilities

- `semantic-pass-pipeline`: enumeration scope tables become context-owned results.
- `compilation-context`: enumeration records obey arena and reset rules.

## Impact

Analysis Store record shape/API, Stage 3 orchestration/build files, compiler tests,
README, and architecture documentation.
