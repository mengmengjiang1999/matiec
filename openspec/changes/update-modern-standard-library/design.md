## Context

The public fourth-edition summary confirms related UTF-8 functions but does not expose
a complete normative library table.

## Decisions

- Use one record per function family with evidence, signature, profile, runtime, and tests.
- Keep existing legacy resolution and ABI stable.
- Do not infer a complete library from one vendor.
- The first entry is `ASSERT(IN : BOOL) : VOID`, supported by the official Edition
  4 preview table name and a public specialist description of the signature.
- Inject a profile-owned void declaration and enable MATIEC's existing void-call
  machinery only for compilation units that use the standard form. The experimental
  runtime implements the documented released/no-op policy.
- Keep the entry in the experimental normalization layer rather than the generated
  legacy declaration table. A user-defined expression-valued `ASSERT` remains
  unchanged in legacy and feature-neutral experimental source.

## Open Questions

- Complete added, changed, deprecated, and removed functions and exact overload rules.
- Development-mode notification transport and build-mode selection.
- Exact UTF-8 function names, signatures, indexing, capacity, and error behavior.
