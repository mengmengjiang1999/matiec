# Design

## Native ownership

`object_method_declaration_c` owns an optional semantic callable projection and
`object_method_invocation_c` owns an optional resolved call projection. These are
analysis attachments, not library members, and are cleared with the AST arena.

The binder first indexes FB declarations, fields, methods, and instances. It then
builds callable projections and resolves calls. Owner-field references are mapped
while constructing the projection without preprocessing or replacing parser input.
Nested call expressions are bound recursively.

## Downstream use

Existing function-call type algorithms operate on the attached callable view.
Native visitors transfer analysis results between the method node and that view.
C generation emits a method declaration's attached callable once and emits calls
through the resolved projection. IEC output continues to print native syntax.

## Failure behavior

Unknown receivers, unknown methods, unsupported receiver shapes, and duplicate
methods produce deterministic semantic diagnostics at the native source range.
