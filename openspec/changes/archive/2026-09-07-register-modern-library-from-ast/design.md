## Context

The generated lexer classifies calls using a process-wide library-element symbol
table. The old normalizer satisfied that requirement by prepending an IEC
function to the source. The parser therefore produced a usable declaration, but
all source positions were displaced and compiler-owned text became
indistinguishable from user input.

## Goals / Non-Goals

**Goals:**

- Preserve ordinary ST call parsing for experimental `ASSERT`.
- Keep user source bytes unchanged through parsing.
- Make profile selection the only source of parser-name registration.
- Derive use, shadowing, metadata, and declaration placement from AST state.
- Preserve legacy user-defined `ASSERT` behavior and released/no-op generation.

**Non-Goals:**

- Add further Edition 4 library functions.
- Define a new runtime notification ABI.
- Replace the legacy parser's general two-pass symbol table.

## Decisions

### Register the name at the parser compatibility boundary

The experimental profile sets an explicit parser option. Stage 1/2 seeds
`ASSERT` as a previously declared function after clearing its symbol tables.
This is a name-classification bridge only; it does not create semantics or an
AST declaration. Legacy profiles do not seed the name.

### Construct the declaration after parsing

A post-parse visitor detects user `ASSERT` declarations and calls. When a call
requires the profile function and no user declaration owns the name, the
registry appends a compiler-owned `function_declaration_c` with BOOL input,
VOID result, and the documented no-op body. The same pass publishes
`ModernLibraryFunctionAst` metadata from the call location.

### Preserve user ownership

If the parsed unit declares `ASSERT`, no compiler-owned declaration or metadata
is added. This keeps the existing user overload and generated output unchanged
in both language profiles.

## Risks / Trade-offs

- The lexer still requires a compatibility symbol-table seed. Keeping it as a
  narrow profile option is less invasive than adding grammar-specific keywords
  and avoids reserving `ASSERT` in legacy mode.
- Constructed nodes must match parser-built declarations closely. Top-level focused and
  end-to-end tests cover semantic resolution, invalid expression use, IEC/C
  output, and sequential context reuse.

## Migration Plan

Replace the normalizer in one change, run the full regression and sanitizer
matrix, then archive. Reverting the commit restores source injection without a
data migration.
