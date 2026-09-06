## Context

The current `-r` implementation provides useful syntax, while `-R` intentionally
allows non-standard forms. Exact 2025 deltas are not yet publicly verified.

## Decisions

- Characterize existing `-r` and `-R` behavior before changing it.
- Keep standard evidence separate from implementation evidence.
- Gate any newly completed standard-oriented behavior through the experimental profile.

## Open Questions

- Initialization, assignment compatibility, generic targets, nullability, and lifetime rules.
