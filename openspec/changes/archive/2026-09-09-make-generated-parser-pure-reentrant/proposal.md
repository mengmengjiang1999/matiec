## Why

Generated Bison lookahead, semantic/location values, and error counters are
currently rewritten into thread-local globals. They should instead belong to
each parser invocation and receive the owning parser session explicitly.

## What Changes

- Generate a pure Bison parser with explicit parser and lexer session parameters.
- Keep syntax-error accounting in `ParserState` across the library and source passes.
- Stop post-processing generated Bison globals; retain the checked Flex migration.
- Update tests and reentrancy documentation.

## Capabilities

### New Capabilities

None.

### Modified Capabilities

- `frontend-reentrancy`: Generated parser invocation state is automatic rather
  than thread-local global storage.

## Impact

Generated frontend signatures and build checks change; language and public APIs do not.
