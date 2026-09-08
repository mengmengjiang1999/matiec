## Context

`symbol_c` currently registers itself by reading a dedicated thread-local arena
pointer installed around the whole compile. Parser state already has the correct
nested scope and lifetime, so the second ambient pointer duplicates session
selection. `AstArena::make()` also relies on that ambient pointer to set a
symbol's back-reference even though the caller already names the target arena.

## Goals / Non-Goals

**Goals:**

- Remove the active-arena TLS variable and scope API completely.
- Preserve automatic ownership for thousands of legacy parser `new` actions.
- Make direct `AstArena::make()` ownership independent of parser state.
- Preserve safe direct deletion and cross-arena transfer semantics.

**Non-Goals:**

- Rewriting every grammar action to call `AstArena::make()`.
- Removing the active parser-state compatibility scope in this change.
- Publishing the parallel batch API.

## Decisions

- Store a non-owning `AstArena*` in `ParserState`; the legacy adapter binds the
  context arena before parsing and keeps that parser state active through
  parser-adjacent synthetic construction.
- Have `symbol_c` and `retain_ast_string()` consult the active parser session's
  arena instead of a dedicated allocation TLS variable.
- Teach `AstArena::make()` to attach AST-derived objects explicitly. If a
  constructor was invoked under another parser session, transfer registration
  to the arena named by the caller before returning.
- Remove active-arena scopes from focused tests. Tests that exercise legacy
  direct `new` helpers install an explicit parser session instead.

Passing an arena argument through every generated constructor call was rejected
for this milestone because it combines allocation ownership with a grammar-wide
API rewrite. The parser session is already context-owned and thread-isolated.

## Risks / Trade-offs

- [AST trait requires complete derived types] → Instantiate `AstArena::make()`
  only at call sites where the requested type is complete, as existing callers
  already do.
- [Direct legacy `new` outside a parser session is unowned] → Component code
  uses `AstArena::make()`; tests of legacy helpers install `ActiveParserStateScope`.
- [Parser state retains a non-owning arena pointer] → It always points to the
  arena in the same `CompilationContext`, whose member lifetime encloses parser
  state use.

## Migration Plan

Remove the old API and convert every repository caller in one commit. Normal and
sanitizer suites verify construction, direct deletion, failed parses, and context
teardown. Reverting the commit restores the old scope with no persisted data
migration.

## Open Questions

None.
