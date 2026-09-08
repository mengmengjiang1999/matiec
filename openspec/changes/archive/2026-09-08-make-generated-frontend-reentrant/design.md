## Context

The checked-in Flex and Bison sources generate C++ scanners/parsers with classic
global interfaces. In addition, scanner helpers and Stage 1/2 classification
tables retain mutable file-scope state. The project currently uses Flex 2.6 and
Bison 3.x in CI, while the grammar still targets the traditional impure Bison
interface.

## Goals / Non-Goals

**Goals:**

- Permit generated frontend executions for independent contexts to overlap on
  separate threads without a global lock.
- Make parser classification entries owned and reset by `CompilationContext`.
- Keep generated sources reproducible across clean and sanitizer builds.

**Non-Goals:**

- Supporting recursive parsing twice on the same thread.
- Rewriting the complete grammar to Bison parse/lex parameters in this change.
- Exposing the parallel batch compiler API; that follows after arena binding is
  removed.

## Decisions

- Extend `ParserState` with the three classification tables. Existing grammar
  names become compatibility macros that resolve through the active context.
- Mark all mutable scanner/parser session variables `thread_local`. Handwritten
  declarations carry the qualifier directly; a checked post-generation script
  qualifies the small, version-stable set emitted by Flex/Bison skeletons.
- Make generated object files depend on a post-processing stamp so parallel
  builds cannot compile unqualified generated sources.
- Verify the boundary with two threads that parse different identifiers and
  inspect separate AST arenas and classification tables.

The alternative pure Bison/reentrant Flex API would require threading scanner
handles and parse parameters through thousands of existing grammar actions and
scanner helpers. That remains a possible future cleanup, but is not required to
isolate simultaneous thread sessions.

## Risks / Trade-offs

- [Generator skeleton declarations change] → The post-generation script fails
  if an expected declaration is absent and CI regenerates the frontend on every
  supported platform.
- [Thread-local state is not recursive on one thread] → The capability and docs
  explicitly guarantee separate-thread concurrency only.
- [Compatibility macros remain ambient] → Their backing storage is context-owned
  and the later frontend parameter migration can remove the macros without an
  ownership change.

## Migration Plan

Land the ownership and generation changes together, run normal and sanitizer
regressions, then archive the OpenSpec change. Reverting the commit restores the
previous generated-source path without a data migration.

## Open Questions

None for this milestone.
