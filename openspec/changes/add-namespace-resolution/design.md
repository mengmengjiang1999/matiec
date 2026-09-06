## Context

Available vendor behavior is provisional and must not be represented as normative.

## Decisions

- Complete the evidence matrix before selecting concrete grammar.
- Keep names and symbols context-owned and deterministic.
- Reject ambiguous and inaccessible names with source-located diagnostics.

## Open Questions

- Declaration syntax, nesting, imports, aliases, visibility, and collision rules.

## Evidence review (2026-09-06)

The completed public-source comparison is recorded in
`docs/standards/namespace-evidence.md`. It confirms the capability family but does
not resolve lookup, collision, visibility, or ABI rules. Per the first design
decision, concrete parser and resolver behavior remains blocked rather than guessed.
