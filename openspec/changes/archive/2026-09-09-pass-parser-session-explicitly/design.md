## Context

`LegacyGlobalStateAdapter` currently installs an `ActiveParserStateScope` for its
entire lifetime and then calls stage 1/2 functions without a session argument.
The handwritten entry points clear classification tables through compatibility
macros before invoking generated Flex/Bison code.

## Goals / Non-Goals

**Goals:**

- Make parser-session dependency visible in stage 1/2 signatures.
- Perform handwritten setup directly against the supplied session.
- Narrow ambient selection to the generated call that still needs it.

**Non-Goals:**

- Convert Flex to `%option reentrant` or Bison to a pure parser.
- Change generated token, grammar, or include behavior.
- Remove compatibility callbacks used by generated code.

## Decisions

- All file and memory entry points take `matiec::ParserState&` as their first
  argument.
- A shared implementation resets the supplied session's classification tables
  and registers experimental library entries from `state.options`.
- The stage 1/2 implementation installs a nested `ActiveParserStateScope` only
  while generated code runs. This preserves current generated callbacks and
  nested restoration while moving the API contract to explicit dependency
  injection.
- `LegacyGlobalStateAdapter` no longer owns a long-lived scope; semantic and
  generation declaration access is moved to a compiler-owned narrow scope until
  those visitors are parameterized.

## Risks / Trade-offs

- Generated callbacks remain ambient for one more milestone. The next change
  replaces their compatibility globals with generated reentrant parameters.
- Stage 3/4 legacy declaration lookup still needs a selected session; keeping
  that scope at the compiler boundary makes the remaining dependency visible.

## Migration Plan

Update every internal caller atomically. Reverting restores the old overloads;
there is no persisted format migration.

## Open Questions

None.
