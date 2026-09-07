## Context

The mature constant propagation visitor performs many in-place operations on
`const_value`. Rewriting the algorithm and all consumers in one step would mix a
storage migration with semantic changes. A post-pass publisher captures every
completed result in the context store, which then owns the stable analysis
snapshot.

## Goals / Non-Goals

**Goals:**

- Store a typed constant result for each visited arena node.
- Preserve undefined, non-constant, constant, and overflow states exactly.
- Reset published results between compilations.
- Preserve existing downstream behavior through a derived AST view.

**Non-Goals:**

- Change the constant lattice or propagation algorithm.
- Migrate datatype, enumeration, or resolution annotations.
- Remove the compatibility field before all consumers use the store.

## Decisions

### Publish after propagation reaches its fixed point

An iterator copies final `const_value_c` objects into the store only after the
legacy algorithm completes. This prevents consumers from observing partial
intermediate states.

### Treat successful publication as a valid analysis record

The record status reports whether the pass completed. Individual scalar status
members retain the distinction among undefined, non-constant, overflow, and
concrete values.

### Keep compatibility materialization one-way

After publication, a compatibility visitor assigns AST fields from stored
records. Downstream passes therefore receive exactly the published snapshot.

## Risks / Trade-offs

The transition duplicates the compact constant record per node. It is bounded by
AST size and enables consumers to migrate independently without changing the
constant algorithm in the same commit.

## Migration Plan

Publish and validate constant snapshots in this change. Future consumer-focused
changes can replace compatibility reads and then remove the AST field.
