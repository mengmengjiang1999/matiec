## Context

`access_path_c` currently owns one global-name token. The analysis pass therefore
cannot distinguish qualification, field selection, or subscripting and only keeps
a flat map of configuration globals.

## Goals / Non-Goals

**Goals:** structural path AST nodes; deterministic spelling; resolution of
configuration globals, resource globals, and resource-qualified program outputs;
structure-field and constant integer array selection; source-ranged diagnostics.

**Non-Goals:** direct-address targets, dynamic subscripts, function-block member
traversal, runtime communication services, or a claim of complete IEC 61131-3:2025
conformance.

## Decisions

- Store a root token followed by ordered field/subscript selector nodes. This
  keeps punctuation semantics explicit and lets `iec2iec` reproduce the path.
- Interpret the leading components against the owning configuration: a direct
  global, `Resource.Global`, or `Resource.Program.Output`. Remaining selectors
  walk datatype declarations.
- Permit only integer-literal subscripts and validate rank and declared bounds.
- Expose selector kind and spelling in the context-owned experimental model while
  retaining the existing canonical `path` string and CSV schema.
- Resolve names case-insensitively, consistent with existing access aliases.

## Risks / Trade-offs

- A dotted path can be syntactically ambiguous; resolution order is explicit and
  diagnostics identify the first component that cannot be resolved.
- Supporting arbitrary expressions in brackets would require constant evaluation
  and is deferred.

## Migration Plan

Add the AST and grammar nodes, extend the resolver, update generators/tests/docs,
run normal and sanitizer suites plus strict OpenSpec validation, then archive,
commit, and push.

## Open Questions

Direct addresses and communication-service binding remain future work.
