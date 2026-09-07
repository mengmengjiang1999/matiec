## Why

Resolution records are published after datatype analysis, but lvalue checking and
C generation still consume compatibility fields copied back onto the AST. Direct
store reads are required before that compatibility layer can be retired.

## What Changes

- Pass the Analysis Store explicitly to lvalue and Stage 4 consumers.
- Read resolved declarations and extensible parameter counts from typed records.
- Remove resolution compatibility materialization from the production pipeline.
- Add focused no-materialization coverage and update architecture documentation.

## Capabilities

### Modified Capabilities

- `semantic-pass-pipeline`: dependent passes consume completed resolution records.
- `code-generation-boundaries`: C generators receive read-only analysis through
  their existing output boundary.

## Impact

Stage 3 orchestration/lvalue checks, Stage 4 output and C generators, tests, README,
and architecture documentation.
