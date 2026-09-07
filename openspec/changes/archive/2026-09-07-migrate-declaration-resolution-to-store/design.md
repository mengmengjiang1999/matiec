## Context

Three function-call node types carry `candidate_functions`,
`called_function_declaration`, and `extensible_param_count`. Function-block calls
and ten implicit IL FB operators carry `called_fb_declaration`.

## Goals / Non-Goals

**Goals:** publish every specialized resolution field after narrowing, preserve
ordering and parameter counts, reject cross-context declarations, and retain
legacy Stage 4 behavior.

**Non-Goals:** change overload selection, eliminate compatibility fields, or
migrate enumeration and generator metadata.

## Decisions

- Use explicit visitor overrides for only the AST node classes that own resolution
  annotations.
- Publish after forced narrowing, alongside final datatype selection.
- Preserve shared immutable compiler declarations with no arena owner and reject
  declarations owned by another compilation arena.
- Materialize immediately for legacy consumers.

## Risks / Trade-offs

The explicit node list must track future AST additions. Its narrow scope makes
missing resolution-bearing nodes visible in review and tests.

## Migration Plan

Implement the typed boundary and focused tests, run normal and sanitizer suites,
validate and archive the change, then commit and push independently.
