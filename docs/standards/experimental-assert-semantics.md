# Experimental `ASSERT` semantics

The source form is enabled only by `--std=iec61131-3:2025-experimental`:

```iecst
ASSERT(Speed <= MaximumSpeed);
```

The first increment requires exactly one standalone call per source line. The input
must type-check as `BOOL`. The function has no source-level return value and cannot
be embedded in an expression.

## Runtime and lowering

MATIEC evaluates the condition once. Both true and false results then continue
execution without an externally visible notification. This is the deterministic
"released/no-op" policy for the experimental runtime; a future development-mode
diagnostic hook can be added without changing the source signature.

The experimental front end injects a profile-owned `VOID` declaration and enables
the existing MATIEC void-call machinery only for a compilation unit that uses the
standard form. The resulting C symbol is an unstable internal ABI and must not be
called directly by applications.

Comments and string literals are ignored while recognizing calls. Inline block
comments inside an `ASSERT` call and multiline calls are rejected with
source-located diagnostics. If the compilation unit declares its own function named
`ASSERT`, experimental recognition is disabled for that unit and normal legacy name
resolution applies.

Legacy mode is not rewritten. Existing projects that declare and use their own
expression-valued function named `ASSERT` therefore retain their previous overload
and generated output.
