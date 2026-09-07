## Why

Parser options and lexer transition flags currently live in process-wide static
storage. Sequential contexts overwrite each other, and the generated parser can
observe stale transient state after a failed or nested invocation.

## What Changes

- Add a `ParserState` owned by each `CompilationContext`.
- Move legacy runtime options and stage 1/2 transition flags into that state.
- Bind generated-parser compatibility access through a scoped active-state
  adapter and restore an outer binding after nested scopes.
- Reset transient parser controls for every parser invocation.
- Add context isolation, nested binding, and reset coverage.

## Capabilities

### Modified Capabilities

- `compilation-context`: own parser configuration and transient controls.
- `source-management`: bind parsing to the requesting context's state.

## Impact

Legacy parser option access changes from a global object to a scoped reference.
Generated Flex/Bison source syntax and ABI remain unchanged.
