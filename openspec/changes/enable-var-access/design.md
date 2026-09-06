## Context

The commented parser rule is implementation evidence of an old intent, not proof of
current IEC semantics.

## Decisions

- Verify syntax and visibility rules before re-enabling grammar.
- Represent access metadata explicitly rather than discarding it during C generation.
- Preserve legacy rejection until compatibility is deliberately changed.

## Open Questions

- Path grammar, direction, visibility, type compatibility, and runtime integration.
