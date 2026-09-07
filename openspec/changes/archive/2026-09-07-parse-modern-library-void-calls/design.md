## Context

The existing recognizer masks comments and strings line by line, then uses a
greedy regular expression for `ASSERT(...)`. It injects a `VOID` function
declaration that the normal parser already resolves and type-checks.

## Goals / Non-Goals

**Goals:** detect calls without parsing expressions, support normal ST whitespace
and comments, preserve user declarations, and rely on Stage 3 for result typing.

**Non-Goals:** change assertion runtime policy, introduce a general intrinsic
registry, or claim additional normative signatures.

## Decisions

- A small scanner emits identifiers and punctuation while skipping IEC block
  comments and quoted strings with `$` escapes.
- `FUNCTION ASSERT` disables the built-in for the whole compilation unit.
- An unqualified `ASSERT` followed by `(` activates the experimental declaration.
- After declaration injection, Bison parses the call and Stage 3 rejects a `VOID`
  result wherever an expression value is required.

## Risks / Trade-offs

The declaration is still injected before parsing. Full library ownership by the
frontend requires a later profile-aware library registry.

## Migration Plan

Replace the line recognizer, extend unit and CLI fixtures, update documentation,
run full regressions, and archive the change.
