## Why

Namespace structure is native AST, but context metadata still comes from the
pre-parse compatibility resolver instead of parsed namespace nodes.

## What Changes

- Analyze namespace declarations, visibility, and imports from the primary AST.
- Make post-parse AST analysis authoritative for context namespace metadata.
- Retain the bounded lookup, visibility, ambiguity, and deterministic C ABI behavior.
- Narrow the remaining pre-parse component to a documented legacy parser spelling bridge.

## Capabilities

### Modified Capabilities

- `namespace-resolution`: structural metadata and validation originate after parsing.
- `code-generation-boundaries`: the side model mirrors accepted AST nodes.

## Impact

Compiler orchestration, namespace analysis, sequential and CLI tests,
architecture documentation, and catalogue evidence are affected.
