## Context

The parser already owns method declaration and invocation structure. The remaining
normalizer independently rescans declarations and variables to build a side model.

## Goals / Non-Goals

**Goals:** derive the bounded public-FB-method model from AST nodes, retain source
ranges and deterministic lowered names, and preserve compatibility lowering.

**Non-Goals:** classes, interfaces, inheritance, dynamic dispatch, or a broader ABI.

## Decisions

- Run method analysis immediately after parsing and before compatibility AST construction.
- Collect FB-typed receiver declarations with an AST visitor.
- Collect owner fields and method locals from their declaration subtrees so local
  shadowing remains deterministic.
- Keep the context-owned model as compatibility data, not a structural authority.

## Risks / Trade-offs

- Declaration nodes have several wrappers; focused runtime and sequential tests
  guard field and receiver collection.
- Parser diagnostics now precede bounded-subset diagnostics for structurally invalid input.

## Migration Plan

Add AST analysis, remove the scanner, update tests/docs, validate, archive, commit,
and push.

## Open Questions

None for the bounded public function-block method subset.
