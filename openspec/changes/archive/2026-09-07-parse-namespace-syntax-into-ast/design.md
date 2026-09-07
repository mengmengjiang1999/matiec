## Context

Namespace declarations are represented today only by `ExperimentalSyntaxModel`.
The source normalizer removes their lines, then lowers names so the historical
grammar and generators can continue. That side model remains useful for provisional
resolution, but it must no longer be the only structural representation.

## Goals / Non-Goals

**Goals:** parse the already validated subset into explicit nodes, preserve source
locations and namespace nesting through semantic processing, emit equivalent IEC,
and keep generated C and existing diagnostics stable.

**Non-Goals:** namespace aliases, reopening, nested block syntax, import
transitivity, local shadowing, include-spanning declarations, or normative claims
about IEC 61131-3 fourth-edition details that are not publicly verified.

## Decisions

- A namespace node owns its qualified name, visibility marker, contained library
  elements, and local `USING` nodes. File-scope `USING` is a library element.
- Lexer keywords are gated by `iec61131-3:2025-experimental`; legacy parsing keeps
  treating the spellings as ordinary identifiers and rejects the construct.
- The existing normalizer continues resolving and lowering declared/referenced
  names, but stops blanking structural syntax.
- Dependency ordering retains namespace wrappers. It orders contained declarations
  through the same global dependency state and emits wrappers deterministically.
- IEC generation prints namespace wrappers and directives. C generation recursively
  visits wrapper contents during each generation pass and emits no namespace syntax;
  deterministic lowered identifiers remain the provisional ABI.

## Risks / Trade-offs

Preserving wrappers while globally reordering declarations is more complex than a
flat library. Tests must cover `-p`, namespaced declarations, top-level consumers,
and structural IEC output. Resolution remains a pre-parse step in this increment, so the
main AST is structural rather than yet being the sole semantic authority.

## Migration Plan

Add and validate the specification first, implement AST/parser support and
consumers, run focused and full regressions, update documentation, strictly
revalidate, then archive and commit the completed change.
