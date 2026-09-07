## Why

Semantic passes currently write derived facts directly onto AST nodes. That
couples syntax lifetime to analysis lifetime and prevents a compilation context
from owning, clearing, and validating its semantic state independently.

## What Changes

- Add a typed, identity-keyed `AnalysisStore` owned by `CompilationContext`.
- Define records for flow, constants, datatypes, declaration resolution,
  enumeration, and generator metadata.
- Reject keys and record references that do not belong to the context AST arena.
- Clear analysis state at the start of every compilation.
- Add focused ownership, validity, isolation, and lifecycle tests.

## Capabilities

### Modified Capabilities

- `compilation-context`: own and isolate semantic analysis state.
- `ast-lifecycle`: expose node ownership checks needed by context-owned state.
- `semantic-pass-pipeline`: establish typed storage for later pass migrations.

## Impact

Compiler context and arena APIs gain internal analysis-storage facilities. No
language syntax, command-line interface, generated output, or runtime ABI changes.
