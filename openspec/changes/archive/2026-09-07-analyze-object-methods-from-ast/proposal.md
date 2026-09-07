## Why

Function-block methods and invocations are native AST nodes, but a regex source scan
still constructs method, owner-field, and receiver metadata before parsing.

## What Changes

- Analyze method declarations, visibility, owner fields, locals, and FB receivers
  from the primary AST.
- Remove the experimental method source scanner.
- Preserve the deterministic compatibility ABI and static dispatch behavior.

## Capabilities

### Modified Capabilities

- `object-oriented-elements`: method compatibility metadata originates from native AST nodes.
- `code-generation-boundaries`: method metadata is produced only after parsing.

## Impact

Compiler orchestration, method analysis and compatibility passes, tests,
documentation, and catalogue evidence are affected.
