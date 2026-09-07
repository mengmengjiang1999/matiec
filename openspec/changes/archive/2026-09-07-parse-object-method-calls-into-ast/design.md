## Context

Function-block method declarations are native AST children, but the method
normalizer still rewrites instance calls in source text before parsing and appends
synthetic functions for the established semantic and C ABI. The declaration-side
native node therefore coexists with a call side that is invisible to the parser.

The existing method subset is intentionally narrow: public methods, statically
resolved from a declared FB instance, with owner fields passed as hidden `VAR_IN_OUT`
arguments. This change must preserve that behavior while moving only the call
rewrite boundary into the AST pipeline.

## Goals / Non-Goals

**Goals:**

- represent `instance.method(...)` as a primary AST node;
- preserve source ranges and normalized IEC spelling;
- bind each supported call to the existing deterministic lowered function and hidden
  owner-field arguments before ordinary stage-3 function-call analysis;
- remove regular-expression replacement of method calls from source normalization;
- preserve generated C and runtime behavior.

**Non-Goals:**

- removing the synthetic lowered function declarations;
- directly type-checking method declarations as methods;
- overloads, named method arguments beyond existing function-call behavior, nested
  receiver expressions, inheritance, interfaces, dynamic dispatch, or new visibility;
- changing the provisional lowered symbol ABI.

## Decisions

- Add `object_method_invocation_c` with receiver, method name, formal arguments, and
  non-formal arguments. A non-owning compatibility annotation points at a synthesized
  `function_invocation_c`; it is not traversed by generic AST iterators.
- Extend the experimental ST grammar only. Legacy grammar behavior and keyword rules
  remain unchanged.
- Keep declaration validation and synthetic function construction in the existing
  normalizer for this increment, but expose the already discovered owner/method,
  owner-field, and instance-type metadata needed by the AST lowering pass.
- Run a dedicated compatibility pass after parsing and before symbol-table
  initialization. It binds native calls, appends hidden `instance.field` arguments,
  and installs the compatibility invocation annotation.
- Semantic function-call passes and C generation delegate to the compatibility
  invocation. IEC generation prints the native invocation so structural output does
  not expose the lowering.
- Unknown instances or methods produce source-ranged diagnostics in the compatibility
  pass instead of becoming misleading unknown-function errors.

Alternatives considered were retaining regex rewriting, which does not advance AST
ownership, and implementing complete direct method semantics in one change, which
would couple parser, lookup, type checking, ABI, and generation changes too tightly
for a reviewable increment.

## Risks / Trade-offs

- **Grammar ambiguity with structured variables** → Restrict the new production to
  the invocation shape and cover expression and statement forms in parser regressions.
- **Compatibility annotations accidentally traversed twice** → Keep the synthesized
  invocation outside the macro-declared child references and add explicit visitor
  delegation only where function-call semantics are required.
- **Metadata lookup preserves existing global limitations** → Keep the same bounded
  instance/type contract and document scope-aware resolution as a later change.
- **Two call representations temporarily coexist** → Treat the native node as source
  authority and the compatibility invocation as a transient semantic annotation.

## Migration Plan

Create and validate the OpenSpec artifacts, add the AST/parser/pass changes, remove
source call rewriting, add focused and CLI regressions, update documentation and the
feature catalogue, run focused/full/sanitizer validation, archive the change, then
commit and push the single completed change to `origin/main`.

Rollback is the single change commit; the provisional external method ABI remains
unchanged.

## Open Questions

None for the bounded static-dispatch subset.
