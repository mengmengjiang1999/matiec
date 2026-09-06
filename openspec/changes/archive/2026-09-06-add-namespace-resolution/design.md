## Context

Available vendor behavior is provisional and must not be represented as normative.

## Decisions

- Complete the evidence matrix before selecting concrete grammar.
- Keep names and symbols context-owned and deterministic.
- Reject ambiguous and inaccessible names with source-located diagnostics.
- After explicit project approval, implement the following MATIEC provisional
  contract only in `iec61131-3:2025-experimental`:
  - `NAMESPACE [INTERNAL] A.B` ... `END_NAMESPACE` declares one non-reopenable
    namespace block; namespace and symbol names are ASCII case-insensitive.
  - `USING A.B;` imports public declarations for unqualified lookup. The current
    namespace wins; otherwise exactly one imported match is required.
  - `INTERNAL` declarations are visible only from the same namespace tree.
  - Qualified names use dot notation and are lowered to deterministic,
    collision-checked legacy identifiers before the existing parser runs.
  - Namespace declarations reserve matching unqualified names in their scope;
    local shadowing of imported namespace declarations is not supported yet.
  - The lowering model is a context-owned front-end AST/resolver. The legacy
    parser and generators receive a flattened library AST, keeping the feature
    gated and avoiding new process-global symbol state.

## Open Questions

- Normative fourth-edition syntax and behavior remain unknown. Aliases, namespace
  reopening, local shadowing, nested block syntax, and public ABI spelling are
  intentionally outside the provisional subset.

## Evidence review (2026-09-06)

The completed public-source comparison is recorded in
`docs/standards/namespace-evidence.md`. It confirms the capability family but does
not resolve lookup, collision, visibility, or ABI rules. Per the first design
decision, concrete parser and resolver behavior was initially blocked rather than
guessed. The project owner subsequently authorized the explicitly labeled MATIEC
provisional contract above so implementation can continue without presenting it
as IEC-defined behavior.
