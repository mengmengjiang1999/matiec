## Context

The parser owns access declaration nodes, while the normalizer independently scans
the same source for scope, target, type, direction, and duplicate checks.

## Goals / Non-Goals

**Goals:** make the AST authoritative, retain source-ranged diagnostics and CSV
behavior, and remove the experimental pre-parse scan.

**Non-Goals:** hierarchical/resource/direct paths, runtime aliases, or broader
normative claims.

## Decisions

- Run a dedicated AST analysis immediately after parsing.
- Collect configuration-scope globals and validate access nodes in their owning
  configuration.
- Store successful declarations in the existing context-owned model/result.
- Retain the small legacy-profile rejection scan because legacy parsing must fail
  before experimental grammar is enabled.

## Risks / Trade-offs

- AST datatype spelling may differ from source text; use token structure and cover
  elementary/derived types in tests.
- Scope mistakes could change diagnostics; retain negative regressions.

## Migration Plan

Add the pass, remove the experimental scan, update tests/docs, run full and sanitizer
validation, archive, commit, and push.

## Open Questions

None for the existing subset.
