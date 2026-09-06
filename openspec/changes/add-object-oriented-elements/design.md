## Context

OO elements have broad cross-vendor evidence but exact normative behavior remains
to be verified. Generated C needs a stable and documented ABI.

## Decisions

- Freeze no grammar or ABI until an evidence matrix and object model are reviewed.
- Land declarations, type rules, and dispatch in independently testable increments.
- Keep all new syntax out of legacy unless a separate compatibility decision is made.

## Open Questions

- Visibility, overriding, overloads, interface conformance, layout, and dispatch rules.
