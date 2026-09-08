## Why

Flow and constant analysis are already exposed through typed `AnalysisStore`
records, but IL nodes and the AST base class still retain compatibility fields.
Those duplicate stores permit stale state, require a constant publication walk,
and keep semantic results coupled to AST lifetime.

## What Changes

- **BREAKING** Remove predecessor/successor vectors from IL AST nodes and remove
  the constant lattice value from `symbol_c`.
- Make flow consumers use only context-owned flow records, including temporary
  semantic helper nodes.
- Make constant propagation update live persistent or transient constant records
  in the active compilation context.
- Remove the whole-tree constant publication traversal and its build wiring.
- Update focused tests, architecture documentation, and the README to describe
  the new ownership boundary.

## Capabilities

### New Capabilities

None.

### Modified Capabilities

- `semantic-pass-pipeline`: Flow and constant working and completed results are
  context-owned without AST fallbacks or a publication traversal.
- `ast-lifecycle`: AST nodes contain no flow-edge or constant-analysis storage,
  and transient records are cleared with their compilation context.

## Impact

This affects AST definitions, AnalysisStore working-record APIs, flow and constant
semantic passes and consumers, Stage 3 build inputs, focused compiler tests, and
the analysis-ownership documentation. The source-language and CLI behavior do not
change.
