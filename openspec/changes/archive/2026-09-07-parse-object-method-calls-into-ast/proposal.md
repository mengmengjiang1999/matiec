## Why

The experimental method implementation preserves declarations in the primary AST,
but rewrites every `instance.method(...)` expression with regular expressions before
the parser sees it. This loses the source-level call boundary and prevents later
semantic and code-generation passes from taking ownership of method dispatch.

## What Changes

- Add a primary AST node for supported function-block method invocations.
- Parse `instance.method(...)` directly in the experimental profile while retaining
  the existing bounded public/static-dispatch behavior.
- Move compatibility call lowering from source-text rewriting to an explicit AST
  compatibility pass before ordinary datatype analysis.
- Preserve existing diagnostics, generated C behavior, legacy-profile rejection,
  and the provisional method ABI.
- Add parser, normalized IEC, semantic, generated-C, runtime, and negative tests.

## Capabilities

### New Capabilities

None.

### Modified Capabilities

- `object-oriented-elements`: supported method invocations become native AST nodes
  and are resolved through an explicit compiler pass.
- `code-generation-boundaries`: method-call compatibility lowering operates on AST
  structure rather than pre-parser source text.

## Impact

This affects the AST definition, experimental parser grammar, method compatibility
lowering, semantic pipeline boundary, IEC/C generation, focused compiler tests, CLI
runtime regressions, architecture documentation, and the language feature catalogue.
