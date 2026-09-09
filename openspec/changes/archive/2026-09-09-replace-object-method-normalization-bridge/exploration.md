# Exploration

## Current path

The grammar already produces native `object_method_declaration_c` and
`object_method_invocation_c` nodes. After parsing, two compatibility passes mutate
method bodies, append synthetic functions to the library, and attach synthetic
function invocations. Stage 3 and C generation then delegate through those
compatibility nodes.

This duplicates structure, changes the top-level AST after parsing, and makes
nested calls sensitive to aliasing order. Included declarations work only
incidentally because the synthetic library walk happens after include expansion.

## Direction

Keep method declarations and calls as the only structural nodes. A normal semantic
binding phase may attach a private callable view needed by the legacy type checker
and C ABI, but it must not append declarations to the library or rewrite source
identifiers. Stage 3 and stage 4 consume the binding through native method nodes.

Regression coverage must include nested calls, calls declared in virtual includes,
formal/non-formal arguments, unknown receivers, and context reuse.
