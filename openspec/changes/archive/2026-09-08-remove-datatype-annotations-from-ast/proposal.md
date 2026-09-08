## Why

Datatype consumers use typed records, but candidate filling and narrowing still
mutate three public fields on every AST node and then traverse the tree to publish
them. This duplicates state and prevents the AST from becoming a structural model.

## What Changes

- Replace public candidate, selected datatype, and declaration-scope AST fields
  with working-record access backed by the active Analysis Store.
- Keep transient and immutable shared helper-node working values in a
  context-owned transient table for the duration of a compilation.
- Remove datatype publisher visitors because arena-node records are updated live.
- Validate completed datatype records before dependent semantic passes run.
- Update focused tests, README, architecture documentation, and ownership notes.

## Capabilities

### New Capabilities

None.

### Modified Capabilities

- `semantic-pass-pipeline`: Candidate filling and narrowing update context-owned
  datatype records directly.
- `ast-lifecycle`: AST nodes no longer own datatype-analysis fields.

## Impact

The AST base class, Analysis Store, Stage 3 datatype producers and diagnostics,
small Stage 4 helper writes, datatype tests, build source lists, and architecture
documentation change. This intentionally changes internal C++ AST interfaces but
does not change IEC behavior or generated output.
