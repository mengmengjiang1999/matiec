## Context

Several established IL passes still read `prev_il_instruction` and
`next_il_instruction`. Moving the producer first gives flow facts one validated
owner while a derived compatibility view keeps the migration independently
reversible.

## Goals / Non-Goals

**Goals:**

- Record every fallthrough and jump edge in `AnalysisStore`.
- Make store access explicit at the flow-pass construction boundary.
- Preserve edge order and downstream behavior.
- Prove records do not survive context reuse.

**Non-Goals:**

- Migrate datatype and constant annotations.
- Redesign IL control-flow semantics.
- Remove legacy vector members before their consumers are migrated.

## Decisions

### Store edges atomically

`add_flow_edge` validates both endpoints, then updates predecessor and successor
records together. It preserves the old distinction between front insertion for
fallthrough and append for jump targets.

### Derive the compatibility view after production

A small visitor copies completed store records to the old AST vectors once. The
flow algorithm never writes those vectors, so the store remains authoritative
and consumers cannot create divergent edges.

### Keep pass wiring explicit

The stage 3 pass runner takes `AnalysisStore&` from `CompilationContext` and
passes it into the flow visitor. No process-global or thread-local store is used.

## Risks / Trade-offs

The compatibility view temporarily duplicates pointer vectors. This costs a
small amount of memory but isolates the producer migration and can be deleted
when remaining IL consumers move to typed lookups.

## Migration Plan

Land flow ownership and regression coverage as one change. Subsequent semantic
migrations may consume typed flow records directly and retire the derived view.
