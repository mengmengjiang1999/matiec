## Context

`object_method_normalizer` is currently both a validator and a source-structure
lowerer. It removes method declarations and appends equivalent top-level functions,
so downstream semantic passes work but the primary AST loses the accepted language
construct. `ExperimentalSyntaxModel` records summary metadata but cannot substitute
for syntax-tree ownership.

## Goals / Non-Goals

**Goals:** represent the already supported method subset natively beneath its owning
FB, preserve source locations and method components, emit equivalent IEC structure,
keep current diagnostics/runtime behavior/C ABI, and retain legacy-profile isolation.

**Non-Goals:** replacing compatibility semantic lowering, direct native method type
checking or C emission, overloads, inheritance, interfaces, overrides, dynamic
dispatch, additional visibility, properties, include-spanning methods, or normative
fourth-edition conformance claims.

## Decisions

- A function block owns a method-declaration list. Each method node owns a visibility
  node, method name, return type, function-style variable declarations, and body.
- `METHOD`, `END_METHOD`, `PUBLIC`, `PRIVATE`, and `PROTECTED` are lexer keywords only
  in `iec61131-3:2025-experimental`; the existing experimental `INTERNAL` token is
  reused. Legacy identifier behavior remains unchanged.
- The normalizer retains original method blocks in the parser input and separately
  appends compatibility lowered functions. It still performs bounded validation,
  owner-field rewriting, and instance-call rewriting.
- Stage 3 continues analyzing the compatibility functions. Existing explicit FB
  visitors do not treat native method nodes as executable FB body statements.
- Dependency ordering preserves methods automatically as fields of the retained FB
  node. IEC generation prints them beneath the owner. C generation deliberately
  ignores the native list because the lowered functions are the current semantic
  and ABI boundary.
- Normalized IEC may still contain compatibility lowered functions after the native
  FB. This increment guarantees preservation of method structure, not whole-output
  round-trip equivalence or removal of all lowering artifacts.

## Risks / Trade-offs

The legacy scanner uses state transitions to distinguish POU declarations from
ST/IL bodies. Method boundaries require explicit experimental transitions so a
method header and its declarations/body are tokenized in the correct states.
Retaining both native nodes and lowered functions also creates two representations;
tests must ensure IEC emits the native node once and C generation does not emit it
twice. Semantic authority remains temporarily split: syntax ownership is native,
while method execution semantics use compatibility lowering.

## Migration Plan

Validate this specification before implementation, add the AST/parser and scanner
transitions, retain structure in the normalizer, adapt generation, add focused
regressions and documentation, run full and sanitizer validation, archive the
change, strictly validate all specs, then create and push one commit to
`origin/main`.
