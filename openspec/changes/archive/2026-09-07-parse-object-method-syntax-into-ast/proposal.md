## Why

The experimental front end accepts function-block methods but deletes their source
structure before parsing. Consequently the compiler's primary AST cannot represent
method ownership, visibility, interface, or body, and normalized IEC output exposes
only an implementation-specific lowered function.

## What Changes

- Add native function-block method AST nodes and experimental lexer/parser rules.
- Preserve validated `METHOD` structure while retaining compatibility lowering for
  current semantics, static dispatch, and generated C ABI.
- Preserve method nodes through dependency ordering, emit them structurally from
  `iec2iec`, and avoid duplicate C generation.
- Add AST/IEC/C/profile regressions and update provisional documentation.

## Capabilities

### New Capabilities

None.

### Modified Capabilities

- `object-oriented-elements`: supported methods become FB-owned primary AST nodes
  and normalized IEC preserves their structure.
- `code-generation-boundaries`: IEC and C generators consume the native method
  boundary while compatibility lowering keeps generated C behavior stable.

## Impact

This affects the AST definition, lexer, parser, method normalizer, IEC/C generator
boundaries, regression tests, feature catalogue, and method documentation. The
experimental profile gate and deterministic provisional C ABI remain compatible.
