## Why

The experimental namespace front end accepts and resolves namespace syntax but
deletes its structural tokens before parsing. Consequently the compiler's primary
AST and normalized IEC output disagree with the language surface it supports.

## What Changes

- Add native AST nodes and experimental grammar for namespace declarations,
  visibility, qualified names, and `USING` directives.
- Preserve validated namespace syntax for the parser while retaining the existing
  deterministic lowering and diagnostics.
- Preserve namespace nodes through semantic dependency ordering and teach IEC and
  C generation how to consume them.
- Add parser/output/profile regressions and update the provisional documentation.

## Capabilities

### New Capabilities

None.

### Modified Capabilities

- `namespace-resolution`: validated namespace structure becomes part of the main
  compiler AST and normalized IEC output.
- `code-generation-boundaries`: generators consume namespace wrappers without
  changing the provisional lowered C ABI.

## Impact

This affects the AST definition, lexer, parser, namespace normalizer, dependency
ordering, IEC/C generators, regression tests, and namespace documentation. The
experimental profile gate and lowered C identifiers remain compatible.
