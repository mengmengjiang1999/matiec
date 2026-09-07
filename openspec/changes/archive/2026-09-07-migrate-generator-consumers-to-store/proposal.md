## Why

Stage 4 currently exchanges generated implicit-type identifiers through a
misspelled generic AST annotation map, then publishes that map to the typed store
only after generation has finished. Making the context store the live Stage 4
annotation channel removes mutable generator state from the AST and makes the
existing generator records useful during generation.

## What Changes

- Add arena-checked named generator-symbol update and lookup operations.
- Give Stage 4 output contexts mutable access to their compilation analysis
  store while preserving read-only lookup access.
- Replace every Stage 4 generator annotation-map read and write with typed store
  access.
- Remove the post-generation AST publisher and materializer from production.
- Add regression coverage and update README/architecture documentation.

## Capabilities

### New Capabilities

None.

### Modified Capabilities

- `code-generation-boundaries`: Generator-local semantic identifiers are
  exchanged through the compilation-owned Analysis Store during Stage 4.

## Impact

This affects `AnalysisStore`, `stage4out_c`, C datatype generation helpers,
focused generator-store tests, Stage 4 completion, and architecture docs. The
legacy publisher/materializer API remains available for isolated compatibility.
