## Why

Flow control is the first semantic producer in the planned annotation migration,
but its predecessor and successor edges are currently created directly on IL AST
nodes. The context cannot identify, reset, or validate those results as a unit.

## What Changes

- Make the context-owned analysis store the producer and canonical owner of IL
  predecessor/successor edges.
- Pass the store explicitly into flow analysis and validate every recorded edge.
- Materialize the existing AST vectors from the store as a read-only compatibility
  view for semantic and generator consumers not yet migrated.
- Add end-to-end flow population and context-reset coverage.

## Capabilities

### Modified Capabilities

- `semantic-pass-pipeline`: make flow results context-owned and explicitly passed.
- `compilation-context`: retain flow records for the active compilation only.

## Impact

Stage 3 flow analysis and analysis-store APIs change internally. IL diagnostics
and generated IEC/C output remain compatible.
