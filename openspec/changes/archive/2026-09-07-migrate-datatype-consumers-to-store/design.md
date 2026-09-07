## Context

Candidate filling and narrowing still write AST fields as working state, then
publish completed snapshots to the context-owned datatype record. The pipeline
currently materializes those snapshots back to the same fields before later
passes and generation, leaving the ownership boundary unenforced.

## Goals / Non-Goals

**Goals:**

- Make typed datatype records authoritative after each completed publication
  boundary.
- Remove production candidate and selected datatype/scope copy-back.
- Preserve producer-local and synthetic-node behavior through scoped fallback.

**Non-Goals:**

- Rewrite candidate filling or narrowing to update records incrementally.
- Remove datatype, scope, or candidate fields from AST layouts.
- Add parallel compilation support.

## Decisions

- Extend the active-store bridge with read-only accessors for candidates,
  selected datatype, and scope. Explicitly threading a store through hundreds of
  legacy visitor and utility call shapes was rejected for this final migration
  because it would add churn without changing ownership semantics.
- Accessors prefer a present datatype record and fall back only when a node has
  no record. This supports stack and compiler-owned canonical datatype nodes
  while making completed arena-node snapshots authoritative.
- Candidate filling remains producer-local. Narrowing reads the published
  candidate snapshot, writes selected AST scratch, and publishes the completed
  selection. Passes after narrowing and Stage 4 read the final record.
- Migrate reusable datatype utility predicates as well as direct visitor reads,
  so generator call sites cannot bypass the boundary indirectly.

## Risks / Trade-offs

- [A producer mutates candidates after publication] → keep candidate mutation
  inside filling and publish only when filling finishes; narrowing treats records
  as immutable inputs.
- [Canonical or transient nodes have no record] → accessors retain the existing
  AST field as an explicit missing-record fallback.
- [A missed direct field read masks the migration] → repository-wide inventory,
  empty-AST focused tests, and all characterization/sanitizer gates enforce the
  boundary.

## Migration Plan

Add accessors and focused tests, migrate narrowing/downstream semantic consumers,
migrate generators and shared utilities, remove materializer calls, update docs,
and run all validation gates. Rollback restores the two materializer calls while
leaving the accessors compatible.

## Open Questions

None.
