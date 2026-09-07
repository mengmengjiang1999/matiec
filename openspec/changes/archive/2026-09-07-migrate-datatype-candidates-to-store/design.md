## Context

`fill_candidate_datatypes_c` has hundreds of legacy AST reads/writes, and the
narrowing pass immediately consumes those vectors. Flow and constants established
a safe migration pattern: publish typed store records at a phase boundary and
materialize the compatibility view for unmigrated consumers.

## Goals / Non-Goals

**Goals:** capture every visited node's ordered candidate vector in the owning
context, validate arena ownership, preserve legacy narrowing behavior, and document
the current architecture accurately.

**Non-Goals:** migrate selected datatypes, scope, resolution, enumeration, or
generator metadata; remove `symbol_c::candidate_datatypes` before all consumers are
migrated; change type inference behavior.

## Decisions

- Traverse the complete AST after fill-candidates and publish one datatype record
  per node, including empty vectors so “analyzed with no candidates” is distinct
  from “absent”.
- Accept immutable compiler canonical datatypes that intentionally have no arena
  owner, while rejecting candidates owned by another compilation context.
- Preserve any selected/scope fields already present in a datatype record.
- Materialize candidate vectors from the store before legacy narrowing executes.
- Add `datatype_size()` for lifecycle and isolation assertions.
- Mark the deferral ADR as superseded by incremental migrations rather than erase
  its historical rationale.

## Risks / Trade-offs

- The compatibility copy temporarily duplicates vectors. It keeps this change
  reviewable; removal is deferred until narrowing and diagnostics use store access.

## Migration Plan

Publish, materialize, test normal and sanitizer builds, validate and archive the
OpenSpec change, then commit and push independently.
