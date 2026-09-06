## Context

Public evidence establishes the feature family, not every normative representation
detail. Implementation must therefore begin with an evidence inventory and must
not infer code-point, code-unit, indexing, truncation, or normalization rules.

## Decisions

- Reject malformed UTF-8 in the experimental profile with source locations.
- Define an explicit internal string representation before changing the runtime ABI.
- Treat each library function as separately evidence-gated.
- Keep legacy literals and generated C byte-for-byte stable where the feature is unused.

## Risks / Trade-offs

- Vendor behavior may conflict; such behavior remains provisional.
- Runtime ABI changes require generated-C compile and execution coverage.

## Open Questions

- Normative length, indexing, slicing, conversion, and normalization behavior.
- The complete set and signatures of fourth-edition UTF-8 functions.
