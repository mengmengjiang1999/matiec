## Why

Datatype candidate sets now cross the fill/narrow boundary through the context-owned
Analysis Store, but narrowing results (`datatype` and `scope`) still exist only as
mutable AST annotations. Publishing the completed selection closes the next
semantic lifetime gap without forcing all downstream consumers to migrate at once.

## What Changes

- Publish selected datatype and scope references after narrowing completes.
- Materialize those records for legacy Stage 3 and generator consumers.
- Cover canonical datatype, arena ownership, compatibility, and reset behavior.
- Update README and architecture documentation to describe the completed boundary.

## Capabilities

### Modified Capabilities

- `semantic-pass-pipeline`: completed datatype selections become context-owned records.
- `compilation-context`: selected datatype and scope records obey context lifetime rules.

## Impact

Analysis Store validation, Stage 3 datatype publication, focused tests, README, and
architecture documentation.
