## Context

Narrowing and forced narrowing assign `symbol_c::datatype`; candidate filling also
assigns `symbol_c::scope` on variable expressions. Downstream checks and generators
still read those annotations directly.

## Goals / Non-Goals

**Goals:** publish final selected datatype and scope references in the active
context, preserve candidate records, validate reference lifetime, and retain
behavior for legacy consumers.

**Non-Goals:** remove AST annotations, migrate declaration-resolution fields, or
change inference and diagnostic behavior.

## Decisions

- Publish after forced narrowing so records contain the final values observed by
  later passes.
- Preserve each node's previously published candidate vector.
- Treat compiler canonical datatypes with no arena owner as immutable shared
  references; require scope nodes to belong to the active arena.
- Materialize selected datatype and scope fields immediately after publication.

## Risks / Trade-offs

The compatibility materialization retains duplicate pointer fields until all
downstream consumers use store lookups. It avoids a broad behavioral rewrite.

## Migration Plan

Implement and test the phase boundary, run normal and sanitizer suites, validate
and archive the OpenSpec change, then commit and push independently.
