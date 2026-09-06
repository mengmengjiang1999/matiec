# ADR 0004: Defer the IEC 61131-3:2025 IL migration policy

## Status

Accepted

## Decision

MATIEC keeps Instruction List available as a legacy, implementation-verified
capability. The experimental profile initially behaves the same way.

No removal, warning, deprecation, or automatic migration policy will be attributed
to IEC 61131-3:2025 until authoritative public evidence establishes IL's precise
fourth-edition status and applicable transition rules. The public edition summary
and inaccessible Annex B are not enough to choose one of those behaviors.

When that evidence becomes available, a dedicated OpenSpec change must record the
source, compatibility impact, diagnostic policy, tests, and user migration guidance.

## Consequences

- Existing IL programs remain accepted in both profiles for now.
- Documentation describes current implementation and unknown standard status
  independently.
- Vendor deprecation messages may inform research but cannot establish IEC policy.
