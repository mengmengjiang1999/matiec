## Context

Native namespace wrappers preserve names, visibility, members, and imports. The
legacy parser still needs deterministic spellings to classify qualified symbols.

## Goals / Non-Goals

**Goals:** make parsed namespace nodes authoritative for structural metadata,
validate declaration/import invariants after parsing, and isolate the remaining
legacy spelling bridge.

**Non-Goals:** aliases, reopening, nested blocks, transitive imports, or changing
the provisional flattened ABI.

## Decisions

- Collect top-level namespace and USING nodes after parsing.
- Validate duplicate namespaces and unknown imports from collected AST data.
- Populate `CompilationContext::experimental_syntax()` only from the AST result.
- Keep the existing parser bridge temporarily for token classification and lookup
  spelling; it is not a second structural model.

## Risks / Trade-offs

- Full removal of the spelling bridge requires broader legacy grammar and symbol
  table changes; the boundary remains explicitly documented.
- Existing CLI ambiguity and visibility tests protect lookup behavior.

## Migration Plan

Add AST analysis, detach context metadata from the bridge, update evidence and
tests, validate, archive, commit, and push.

## Open Questions

None for this structural-authority increment.
