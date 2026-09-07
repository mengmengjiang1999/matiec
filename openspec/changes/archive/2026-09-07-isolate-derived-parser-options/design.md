## Context

`allow_void_datatype` is a legacy parser switch. The experimental library
recognizer needs it only when it injects the built-in `ASSERT : VOID`
declaration. Mutating the context's public option object conflates configured
intent with one compilation's derived requirements.

## Decision

Create an effective `CompilerOptions` snapshot at the parser boundary, enable
`allow_void_datatype` only in that snapshot when required, and pass it explicitly
to `LegacyGlobalStateAdapter`. The adapter continues to use the context for source
and symbol initialization services.

## Invariant

`Compiler::compile()` does not modify any caller-supplied option, regardless of
success, failure, profile, or context reuse.
