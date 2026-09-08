## Context

The incremental migration retained whole-tree materializers so old consumers and
early record tests could copy completed results back to AST fields. Production no
longer calls any materializer, and focused tests already exercise direct record
access for most families.

## Goals / Non-Goals

**Goals:**

- Make typed records the only completed-result interface.
- Delete dead traversal code and declarations.
- Preserve publication behavior and validation.

**Non-Goals:**

- Remove producer-local AST fields; later changes migrate producers first.
- Remove the active-store accessor bridge.
- Change pass order or generated output.

## Decisions

Materializer implementations will be deleted rather than hidden behind a test
namespace. Tests will clear compatibility fields and prove that the typed record
remains authoritative. Existing publisher visitors remain until producer-native
store writes are introduced by later changes.

The obsolete generator requirement is removed because the newer live-record
requirement already defines the current Stage 4 boundary.

## Risks / Trade-offs

- [An untracked caller still uses a materializer] → Search the complete source
  tree and require a clean build before delivery.
- [Tests lose copy-direction coverage] → Retain arena-ownership, invalid-record,
  clearing, and direct-access assertions.

## Migration Plan

Delete callers first, then declarations and visitor implementations. Run the
normal and sanitizer suites before archiving. Reverting the single commit restores
the compatibility APIs.

## Open Questions

None.
