## Why

Declaration entries are owned by each `CompilationContext`, but legacy visitors
still select them through a second thread-local pointer and a mutable fallback
table. That redundant ambient binding complicates nesting and obscures the fact
that parsing and later compiler stages operate on one context-owned session.

## What Changes

- Remove `ActiveDeclarationSymbolTablesScope`, its independent thread-local
  pointer, and the fallback declaration table.
- Bind the context-owned declaration tables to its context-owned `ParserState`.
- Route legacy declaration lookup through the selected parser session until that
  session is made explicit by the next migration.
- Update regression coverage and architecture documentation.

## Capabilities

### New Capabilities

None.

### Modified Capabilities

- `compilation-context`: Declaration lookup no longer maintains a separate
  ambient binding or fallback storage.
- `frontend-reentrancy`: The selected parser session consistently identifies
  parser state, AST allocation, and declaration lookup state.

## Impact

The internal compatibility API changes. Public compiler behavior, generated C,
the CLI, and context ownership remain unchanged.
