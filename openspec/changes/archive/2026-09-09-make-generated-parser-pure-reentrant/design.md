## Context

Bison state is made thread-local by a generated-file rewrite. Stage 1/2 now
already supplies `ParserState&`, enabling native pure-parser parameters.

## Goals / Non-Goals

**Goals:** pure parser state, explicit session flow, preserved diagnostics.

**Non-Goals:** a fully reentrant Flex scanner or language changes.

## Decisions

Use `%define api.pure full`, `%parse-param`, and `%lex-param`. Flex's `YY_DECL`
accepts Bison value/location pointers and the named session. ParserState retains
the externally observable recovered-error total. The generated rewrite checks
only remaining scanner globals.

## Risks / Trade-offs

Older Bison interfaces differ, so clean generation is covered on Linux and
macOS. Scanner objectification remains a later boundary.

## Migration Plan

Regenerate from the grammar; no persisted data changes.

## Open Questions

None.
