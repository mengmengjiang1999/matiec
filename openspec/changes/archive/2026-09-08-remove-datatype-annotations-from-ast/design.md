## Context

Candidate filling and narrowing perform hundreds of fine-grained reads and writes
through `symbol_c` fields. Completed records are already consumed from the
Analysis Store, so a final whole-tree publication pass only copies producer state.
Some compiler helpers are stack-local or immutable shared symbols and cannot be
persistent record keys under the arena-ownership invariant.

## Goals / Non-Goals

**Goals:**

- Remove datatype-analysis storage from `symbol_c`.
- Preserve the existing producer algorithm and ordering.
- Update arena-owned typed records live and preserve transient helper behavior.
- Reject invalid completed arena records before later passes execute.

**Non-Goals:**

- Rewrite the datatype inference algorithm.
- Remove other AST annotation families.
- Remove the active-store scope; that is the final change in this batch.

## Decisions

`symbol_c` exposes transitional working-value methods instead of public fields.
Those methods delegate to the active Analysis Store: arena-owned keys update the
persistent datatype table, while non-arena helper keys use a transient table owned
and cleared by that same store. This keeps the large legacy algorithm mechanically
stable without putting result storage back on AST objects.

The persistent table is validated after forced narrowing. Publisher visitors and
their build units are removed because there is no copy boundary left.

## Risks / Trade-offs

- [Mechanical call-site migration changes evaluation] → Preserve lvalue-reference
  return types and run characterization plus sanitizer suites.
- [Transient pointers outlive their helpers] → Keep transient records private to
  the context, clear them on compilation reset, and never expose them through the
  persistent `datatype()` query.
- [Direct mutation bypasses validation] → Validate every persistent datatype
  record before resolution publication and downstream passes.

## Migration Plan

Add working-record APIs, migrate producer call sites, remove AST fields and
publisher visitors, then execute all regressions. Reverting the single commit
restores the old public fields and publication boundary.

## Open Questions

None.
