## Why

Classes, interfaces, methods, inheritance, and dispatch require coordinated AST,
semantic, and ABI design and cannot safely be added as isolated grammar tokens.

## What Changes

- Define an evidence-gated OO subset and object model.
- Add experimental syntax, type checking, dispatch, layout, and C generation.

## Capabilities

### New Capabilities

- `object-oriented-elements`: Classes, interfaces, methods, inheritance, and dispatch.

## Impact

AST ownership, grammar, symbols, type analysis, code generation ABI, tests, and docs.
