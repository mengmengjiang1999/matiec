# Experimental `ASSERT` semantics

The source form is enabled only by `--std=iec61131-3:2025-experimental`:

```iecst
ASSERT(Speed <= MaximumSpeed);
```

The call uses ordinary Structured Text function-call grammar, including whitespace,
newlines, and block comments between tokens. The input must type-check as `BOOL`.
The function has no source-level return value and cannot be embedded in an
expression; Stage 3 reports that `VOID` type error.

## Runtime and lowering

MATIEC evaluates the condition once. Both true and false results then continue
execution without an externally visible notification. This is the deterministic
"released/no-op" policy for the experimental runtime; a future development-mode
diagnostic hook can be added without changing the source signature.

The experimental parser registers the profile-owned function name without
rewriting source bytes. After parsing, an AST registry discovers the call and adds
a compiler-owned `VOID` declaration before semantic analysis. The resulting C
symbol is an unstable internal ABI and must not be called directly by applications.

Because calls are discovered from parsed nodes, comments and string literals are
never mistaken for calls. If the compilation unit declares its own function named
`ASSERT`, no compiler-owned declaration is added and normal user-function name
resolution applies.

Legacy mode is not rewritten. Existing projects that declare and use their own
expression-valued function named `ASSERT` therefore retain their previous overload
and generated output.
