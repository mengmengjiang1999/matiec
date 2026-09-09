## Context

`CompilationContext` owns both `ParserState` and `DeclarationSymbolTables`.
Compilation currently creates an `ActiveDeclarationSymbolTablesScope` in
addition to the parser-state scope created by `LegacyGlobalStateAdapter`.
Legacy lookup macros then call a free function backed by a separate TLS pointer.

## Goals / Non-Goals

**Goals:**

- Delete the independent declaration-table TLS and fallback table.
- Make nested parser sessions select their matching declaration tables.
- Preserve all existing lookup behavior and context isolation.

**Non-Goals:**

- Remove the parser-session compatibility selector in this change.
- Redesign legacy stage 3 and stage 4 visitor APIs.
- Change declaration collection or lookup semantics.

## Decisions

- `ParserState` holds a non-owning pointer to its context's declaration tables,
  matching its existing non-owning AST-arena binding.
- `CompilationContext` binds both services during construction.
- The legacy lookup adapter reads the declaration tables from
  `active_parser_state()` and fails loudly if a caller bypasses the compiler
  session instead of silently using process-local fallback state.
- The compiler removes the declaration scope because
  `LegacyGlobalStateAdapter` already selects the correct parser session for the
  full parse, semantic, and generation lifetime.

## Risks / Trade-offs

- Direct legacy visitor calls without a parser session will no longer receive an
  empty fallback table. Tests and supported entry points must construct a
  context session; silent fallback behavior was not a supported API.
- This is an intermediate compatibility shape. The next change passes the
  parser session explicitly and removes the ambient selector.

## Migration Plan

Update internal tests to bind a parser state rather than a declaration-only
scope. Reverting restores the independent selector without changing stored data.

## Open Questions

None.
