# Experimental function-block method semantics

This document defines the first bounded object-oriented increment in
`iec61131-3:2025-experimental`. It is a provisional MATIEC behavior, not a claim of
complete IEC 61131-3:2025 object-oriented conformance. See
[`object-oriented-evidence.md`](object-oriented-evidence.md) for the evidence boundary.

## Supported syntax

```iecst
FUNCTION_BLOCK Counter
  VAR Count : INT; END_VAR
  Count := Count;

  METHOD PUBLIC Increment : INT
    VAR_INPUT Delta : INT; END_VAR
    Count := Count + Delta;
    Increment := Count;
  END_METHOD
END_FUNCTION_BLOCK

Result := CounterInstance.Increment(2);
```

- A method is declared directly inside a `FUNCTION_BLOCK`.
- The optional access word is `PUBLIC`; omitting it has the same meaning.
- Parameters, locals, return type, expressions, and statements follow existing
  MATIEC function rules.
- An unqualified owner field in the method body refers to the existing caller-owned
  function-block instance. Method local variables take precedence over owner fields.
- Calls use `instance.method(arguments)` and dispatch statically from the declared
  type of `instance`.

## Ownership and ABI

The compiler lowers each method to a legacy function. Each owner field is passed
after ordinary method parameters through a hidden `VAR_IN_OUT` parameter named
`MATIECSELF<field>`. The fields remain owned by the caller and are not copied, so
updates remain visible after the call. Passing fields individually also avoids
introducing a recursive function-block value into the legacy function ABI. A method
such as `Counter.Increment` receives a deterministic length-prefixed name:

```text
MATIECMETHOD7COUNTER9INCREMENT
```

This spelling is visible in generated C and `iec2iec` output but is an unstable
experimental ABI. Method overloads are not supported, so an owner cannot declare
the same case-insensitive method name twice.

The provisional normalizer currently recognizes owner declarations whose type is a
single named type token. More complex declaration forms remain outside this first
increment.

## Unsupported constructs

`CLASS`, `INTERFACE`, `EXTENDS`, `IMPLEMENTS`, `OVERRIDE`, `ABSTRACT`, `FINAL`,
properties, `THIS`, `SUPER`, non-public methods, interface/base references, and
dynamic dispatch are outside this increment. Method declarations split across
include files are also unsupported by the normalization stage.
