## Why

Constant propagation computes a compilation-wide set of derived values, but
those results are only observable through mutable AST fields. Publishing the
completed analysis into the context store gives consumers a typed, resettable
source of truth and continues the migration sequence after flow analysis.

## What Changes

- Publish constant-folding results for arena-owned AST nodes into
  `ConstantAnalysisRecord` entries after propagation completes.
- Make the store explicit in the constant-propagation pass boundary.
- Materialize AST constant fields from stored results as a compatibility view.
- Add focused literal evaluation, store publication, and reset tests.

## Capabilities

### Modified Capabilities

- `semantic-pass-pipeline`: publish constant results into typed context storage.
- `compilation-context`: own constant results for one compilation lifetime.

## Impact

Stage 3 gains a constant-result publication boundary and focused tests. Constant
semantics, diagnostics, generated output, CLI, and runtime ABI do not change.
