## Why

Datatype candidates, selected datatypes, and declaration scopes are already
published to `AnalysisStore`, but production still copies them back to AST fields
for downstream consumers. This final annotation-family migration removes the
remaining production materializers and makes the store the completed datatype
source of truth.

## What Changes

- Add typed accessors for candidate datatypes, selected datatypes, and scopes.
- Route post-publication semantic checks and Stage 4 generators through those
  accessors, while retaining AST fallback for producer-local transient nodes.
- Remove production candidate and selected-datatype materialization.
- Add empty-AST regression coverage and synchronize README and architecture docs.

## Capabilities

### New Capabilities

None.

### Modified Capabilities

- `semantic-pass-pipeline`: Completed datatype consumers use context-owned typed
  records without AST copy-back.
- `code-generation-boundaries`: Generators obtain selected datatypes and scopes
  through the active compilation analysis boundary.

## Impact

This affects the analysis-store access API, datatype semantic visitors and
utilities, Stage 4 generators, focused tests, and architecture documentation.
Candidate filling and narrowing may continue using AST fields as producer-local
scratch until their algorithms are redesigned.
