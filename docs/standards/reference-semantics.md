# Reference semantics inventory

## Evidence boundary

MATIEC's source and AST identify `REF_TO`, `REF`, `DREF`, postfix `^`, and `NULL`
as IEC 61131-3 Edition 3-oriented forms. Public vendor documentation confirms the
general model—references store an object's address, compatible target types are
checked, and invalid/null references must not be dereferenced—but vendor syntax
and extensions differ. These sources are provisional interoperability evidence,
not a substitute for the normative standard:

- <https://product-help.schneider-electric.com/Machine%20Expert/V1.1/en/SoMProg/SoMProg/Data_Types/Data_Types-7.htm>
- <https://infosys.beckhoff.com/content/1033/tc3_plc_intro/2529165707.html>

Exact IEC 61131-3:2025 initialization, nullability, lifetime, generic-reference,
and variance rules remain unresolved.

## Implemented forms

| Form | Activation | Type and output behavior |
| --- | --- | --- |
| `REF_TO T` | `-r` or `-R` | Typed reference represented as a generated C pointer to `T` |
| `REF(variable)` | `-r` or `-R` | Produces a reference to an addressable symbolic variable |
| `DREF(reference)` | `-r` or `-R` | Produces the referenced `T` value |
| `reference^` | `-r` or `-R` | Postfix spelling of dereference |
| `NULL` assignment | `-r` or `-R` | Compatible with any `REF_TO T` |
| `REF_TO T := NULL` declaration | `-r`/`-R` plus experimental profile | Explicit null initialization; generated as a null C pointer |

Assignments compare the referenced base types. For example, `REF_TO INT` cannot
receive `REF(REAL-variable)` and produces a focused assignment type diagnostic.

## MATIEC-only broad forms

`-R` includes `-r` and separately enables forms the source marks as non-standard:

- `REF_TO ANY`;
- direct `REF_TO` members in structures;
- direct `REF_TO` elements in arrays.

They are never enabled merely by selecting the experimental language profile.

## Lifetime and safety contract

Generated references are raw C pointers. MATIEC validates types but does not add
ownership, borrow tracking, null checks, or runtime lifetime checks. A reference
is usable only while its target's generated C storage is alive. Local references
used during the same function invocation are covered by regression tests;
returning, retaining, or dereferencing a reference after a shorter-lived target
has ceased to exist is not diagnosed and must not be treated as safe behavior.
Dereferencing `NULL` has undefined generated-C behavior and is not made safe by
the compiler.

This limitation is documented rather than guessed into an IEC rule. Stronger
lifetime or nullability diagnostics require authoritative semantics and a
dedicated flow-sensitive analysis.
