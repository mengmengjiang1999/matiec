# Decision 0002: Defer the context-owned semantic analysis store

Status: Accepted

## Context

Stage 3 currently writes analysis results directly onto AST nodes. The shared
`symbol_c` base stores candidate datatypes, the selected datatype, declaration
scope, and constant-folding values. Specialized nodes additionally store flow
predecessors/successors, resolved function and function-block declarations,
enumeration tables, and generator annotations.

Moving those fields into `CompilationContext` would improve AST immutability and
make pass inputs and outputs more explicit. It is not a contained change,
however. A source inventory for the current tree found these direct references:

| Annotation family | References |
| --- | ---: |
| Candidate datatypes | 755 |
| Selected datatype | 421 |
| Constant value | 135 |
| IL predecessor links | 284 |
| IL successor links | 19 |
| Resolved function declaration | 77 |
| Resolved function-block declaration | 100 |
| Enumeration symbol tables | 75 |
| Generator annotation map | 29 |

The references span AST definitions, every major semantic visitor, datatype
utilities, and both code generators. Migrating them while the generator is also
being split would combine two high-churn architectural changes and make output
regressions difficult to localize.

## Decision

The semantic annotation store is deferred to a dedicated OpenSpec change.
`refactor-compiler-architecture` keeps annotations on compilation-owned AST
nodes, whose lifetime is already bounded by `CompilationContext` and `AstArena`.
No new annotation fields may be added to `symbol_c` during the remainder of this
change.

The follow-up change is scoped to:

1. Introduce an identity-keyed `AnalysisStore` owned by `CompilationContext`.
2. Define typed records for datatype, constant, flow, resolution, enumeration,
   and generator analysis instead of one untyped map.
3. Add read/write accessors that distinguish missing analysis from invalid
   analysis and verify that referenced nodes belong to the same AST arena.
4. Migrate one annotation family at a time in dependency order: flow, constants,
   candidates and selected types, declaration resolution, enumeration data, then
   generator annotations.
5. Remove each AST field only after every producer and consumer uses the store.

The follow-up must preserve pass isolation: tests provide a `CompilationContext`,
an AST, and only the declared prerequisite records. It must not introduce a
process-wide or thread-local active analysis store.

## Entry criteria

Work may begin after the code-generation extraction in tasks 6.1 through 6.6 is
complete and characterized output is stable. Before the first field migration,
the follow-up must add:

- focused producer/consumer tests for each annotation family;
- a sequential-context test proving records do not leak between compilations;
- a memory and compile-time baseline for representative IEC inputs; and
- a compatibility test that compares generated C and IEC output byte-for-byte.

## Consequences

This change gains an explicit pass pipeline without simultaneously rewriting
more than a thousand annotation access sites. AST nodes remain mutable during
analysis for now, but their state is confined to one compilation lifetime. The
later migration has a bounded sequence, measurable entry criteria, and an
explicit prohibition on compatibility globals.
