# Experimental MATIEC namespace semantics

This document defines a MATIEC-specific provisional namespace subset. It is enabled
only by `--std=iec61131-3:2025-experimental` and is **not** a claim about the exact
IEC 61131-3:2025 grammar or lookup rules. The evidence boundary is recorded in
[`namespace-evidence.md`](namespace-evidence.md).

## Syntax

```iecst
NAMESPACE Factory.Motion
TYPE Speed : INT; END_TYPE
END_NAMESPACE

USING Factory.Motion;
```

- A declaration starts with `NAMESPACE`, an optional `INTERNAL`, and a dot-qualified
  ASCII identifier. It ends with `END_NAMESPACE`.
- One namespace block may contain type declarations, functions, function blocks,
  programs, and configurations.
- Namespace blocks cannot be lexically nested or reopened.
- `USING A.B;` is allowed at file scope or within a namespace block.
- Names are case-insensitive, matching the existing MATIEC identifier model.

## Lookup and visibility

Qualified lookup uses dot notation. Unqualified lookup first checks the current
namespace, then all applicable `USING` imports. Exactly one imported declaration
must match; two or more matches produce an ambiguity diagnostic.

`INTERNAL` makes every declaration in that namespace visible only from the same
namespace or a dot-qualified descendant. Missing qualified declarations, duplicate
declarations, unknown imports, ambiguous imports, and inaccessible declarations are
compile errors with source locations where available.

Namespace declarations reserve their unqualified names in the applicable scope.
Local shadowing of an imported or current-namespace declaration is not supported in
this first subset.

## Lowering and generated output

The main parser creates native nodes for namespace declarations, visibility,
qualified namespace names, contained declarations, and `USING` directives. A
post-parse analysis validates that structure and exclusively populates the
context-owned namespace metadata. Semantic visitors and `-p` dependency ordering
retain those wrappers. During the native parser prepass, the context-owned
namespace registry classifies declarations and lookups, including declarations
loaded through filesystem or virtual includes. The lexer lowers resolved names to
a deterministic identifier made from length-prefixed segments. For example,
`Factory.Motion.Speed` becomes:

```text
MATIECNS7FACTORY6MOTION5SPEED
```

Generated C emits no namespace wrapper, but recursively consumes its declarations;
`iec2iec` preserves the wrapper and `USING` structure. Both outputs currently expose
the lowered spelling. It is an experimental implementation detail, not a stable
external ABI. No source text is rescanned or rewritten: native tokens and AST nodes
remain the structural authority, and diagnostics retain the original source ranges.

## Unsupported behavior

Aliases, namespace reopening, local shadowing, nested block syntax, import
transitivity, and a stable public ABI spelling are not implemented. Namespace
declarations and references may cross filesystem or virtual include boundaries.
