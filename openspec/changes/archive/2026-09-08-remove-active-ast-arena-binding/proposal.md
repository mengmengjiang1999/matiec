## Why

AST nodes and retained lexer strings still discover ownership through a separate
thread-local `ActiveAstArenaScope`. That hidden binding is the last allocation
boundary that prevents one parser session from carrying all state needed for a
compilation.

## What Changes

- Remove `ActiveAstArenaScope`, `active_ast_arena()`, and their dedicated
  thread-local pointer.
- Attach the context arena to `ParserState`, so legacy parser actions obtain
  allocation ownership from the already-scoped parser session.
- Make `AstArena::make()` explicitly attach AST-derived objects to the requested
  arena, independent of any active session.
- Update component tests, lifecycle coverage, README, and architecture docs.

## Capabilities

### Modified Capabilities

- `ast-lifecycle`: AST ownership is established by explicit arena construction
  or the context-owned parser session, without a separate active-arena binding.
- `compilation-context`: The parser session carries the owning AST arena for
  legacy parser and synthetic-node allocation.

## Impact

This removes an internal compatibility API and changes parser-state plumbing and
AST test setup. IEC behavior, generated C ABI, and the embedding compile entry
point remain unchanged.
