## Why

Experimental namespace, method, access-variable, and modern-library recognizers
produce structured declarations, but the compiler currently keeps those results
in short-lived local variables. Later parser and semantic migrations therefore
cannot consume a stable per-compilation model without repeating source scans.

## What Changes

- Introduce one context-owned experimental syntax model.
- Move the existing structured declaration records into that shared model.
- Populate and reset the model at the compiler boundary for every compilation.
- Preserve all existing source lowering, diagnostics, generated output, and CLI
  behavior while establishing the ownership boundary for later native parsing.

## Capabilities

### Modified Capabilities

- `compilation-context`: The context owns structured experimental syntax metadata
  and never leaks it between compilations.

## Impact

This changes compiler-only value types, `CompilationContext`, experimental
normalizer result types, the driver pipeline, unit tests, and architecture
documentation. It does not change accepted syntax or the generated C ABI.
