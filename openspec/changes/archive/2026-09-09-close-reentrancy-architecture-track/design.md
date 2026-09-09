## Context

Separate compilation contexts already parse and compile concurrently without a
global lock. Bison state is invocation-local, Flex state is thread-isolated,
context data is owned explicitly, and generated fatal paths return control. The
remaining `fallback_parser_state` can silently accept legacy access outside a
compilation and weakens that boundary.

## Goals / Non-Goals

**Goals:** remove the mutable fallback, encode the completed cross-thread
invariants in the test suite, and synchronize roadmap documentation.

**Non-Goals:** recursive parser/scanner entry on one thread or converting Flex
to its reentrant scanner API.

## Decisions

Expose an optional active-session query for allocation code that legitimately
constructs standalone AST nodes. Required legacy access continues through
`active_parser_state()` but throws `logic_error` when no scope is installed.
This distinguishes optional ownership attachment from accidental use of an
ambient fallback.

Add a portable shell audit to the normal Automake suite. It checks the
source-of-truth grammar, adapter, generated rewrite, and documentation markers;
runtime concurrent and parallel compilation tests remain the behavioral proof.

Rename the stamp variable and file to generated frontend isolation because the
checked transformation now covers scanner TLS and fatal-exit removal.

## Risks / Trade-offs

Previously hidden out-of-scope compatibility calls will throw. This is intended
fail-fast behavior, and normal plus sanitizer suites cover known call paths.

## Migration Plan

Run the full normal and sanitizer matrices, archive the change, and treat future
same-thread recursive scanning as a new capability rather than unfinished work.

## Open Questions

None.
