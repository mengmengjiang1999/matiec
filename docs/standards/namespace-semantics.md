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

The experimental front end parses namespace declarations into a context-owned
namespace AST and resolves references before invoking the legacy parser. It lowers
each qualified declaration to a deterministic identifier made from length-prefixed
segments. For example, `Factory.Motion.Speed` becomes:

```text
MATIECNS7FACTORY6MOTION5SPEED
```

The generated C and `iec2iec` output currently expose that spelling. It is an
experimental implementation detail, not a stable external ABI. The original source
filename and line are retained for parser diagnostics, although columns after a
length-changing lowered name may differ from the original source column.

## Unsupported behavior

Aliases, namespace reopening, local shadowing, nested block syntax, import
transitivity, and a stable public ABI spelling are not implemented. Includes are
still handled by the legacy parser; namespace normalization currently analyzes the
entry source file, so namespace declarations must not be split across include files.
