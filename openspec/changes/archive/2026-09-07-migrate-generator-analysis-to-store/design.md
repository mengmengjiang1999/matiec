## Context

Stage 4 uses `symbol_c::anotations_map` to share generated implicit type identifiers
between generator components. These named symbol references are populated and
consumed during generation, so the correct publication point is after the complete
Stage 4 visitor finishes and output flush succeeds.

## Goals / Non-Goals

**Goals:** capture final generator annotations for every AST node, validate their
lifetime, preserve compatibility access, and accurately document all six store
record families as active.

**Non-Goals:** rewrite internal generator components to perform live store lookups,
rename the historical AST field, or remove compatibility annotations.

## Decisions

- Implement a full-AST adapter in the compiler library so both C and IEC Stage 4
  variants share one boundary.
- Publish only after output succeeds; failed generation does not claim a completed
  metadata result.
- Store empty maps to distinguish analyzed nodes from absent records.
- Allow current-arena and immutable unowned shared symbols, while rejecting nodes
  owned by another context.
- Materialize immediately to preserve the established compatibility contract.

## Risks / Trade-offs

Every AST node receives a small typed record, including nodes without annotations.
This matches datatype publication semantics and makes phase completion explicit.

## Migration Plan

Implement and test the Stage 4 boundary, run normal and sanitizer suites, validate
and archive the change, then commit and push independently.
