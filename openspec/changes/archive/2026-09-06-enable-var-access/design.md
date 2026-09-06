## Context

The commented parser rule is implementation evidence of an old intent, not proof of
current IEC semantics.

## Decisions

- Public Beckhoff and OPC Foundation material is sufficient for a
  cross-vendor-provisional typed alias and direction model; exact 2025 rules remain
  unverified.
- The first increment accepts only configuration-level declarations targeting a
  simple `VAR_GLOBAL` in the same configuration. Hierarchical and direct paths are
  deferred until the compiler owns a corresponding resolver.
- Resolve targets case-insensitively, require exact named-type equality, default an
  omitted direction to `READ_ONLY`, reject duplicate access names, and reject
  `READ_WRITE` access to constants.
- Represent access declarations explicitly in the experimental front end and export
  `configuration,name,path,type,direction` records through `ACCESS.csv`; the access
  alias does not allocate runtime storage.
- Preserve legacy rejection until compatibility is deliberately changed.

## Open Questions

- Resource/program/function-block path resolution, structured and array elements,
  direct addresses, externally connected inputs, and communication-service runtime
  integration remain deferred.
