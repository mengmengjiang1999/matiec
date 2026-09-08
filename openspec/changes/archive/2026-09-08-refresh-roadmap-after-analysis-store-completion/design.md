## Context

The README roadmap predates the completed typed Analysis Store migration. The
standards evolution document similarly retains a numbered implementation sequence
whose first four foundations are now implemented or intentionally bounded.

## Goals / Non-Goals

**Goals:**

- Make completed and remaining roadmap work visibly distinct.
- Base status claims on maintained tests, feature catalogue records, and archived
  OpenSpec changes.
- Preserve the documented uncertainty around normative IEC 61131-3:2025 details.

**Non-Goals:**

- Change compiler behavior or feature status.
- Promise dates or turn roadmap boundaries into scheduled milestones.
- Claim normative conformance beyond available evidence.

## Decisions

The README will replace the completed Analysis Store boundary with the remaining
cleanup of legacy AST compatibility annotations. The standards document will use
separate completed-foundation and remaining-work lists rather than silently
renumbering history. Architecture and ADR text will identify AST field removal as
the active follow-up.

## Risks / Trade-offs

- [Roadmap language may be mistaken for a delivery promise] → Retain the explicit
  statement that boundaries are unscheduled.
- [Documentation may drift again] → Add a traceability requirement tying roadmap
  status to maintained implementation evidence.

## Migration Plan

Update the reader-facing documents, validate all OpenSpec specifications, and
archive this documentation-only change. Reverting the commit fully restores the
previous wording.

## Open Questions

None.
