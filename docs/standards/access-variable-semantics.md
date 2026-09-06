# Experimental `VAR_ACCESS` semantics

`VAR_ACCESS` is available only with `--std=iec61131-3:2025-experimental`. This is a
bounded MATIEC contract, not a complete IEC 61131-3:2025 conformance claim.

## Supported form

```iecst
CONFIGURATION Controller
  VAR_GLOBAL
    Setpoint : INT;
  END_VAR

  (* resource and program configurations *)

  VAR_ACCESS
    RemoteSetpoint : Setpoint : INT READ_WRITE;
    MonitorSetpoint : Setpoint : INT;
  END_VAR
END_CONFIGURATION
```

Each declaration has the form:

```text
exported-name : configuration-global : named-type [READ_ONLY | READ_WRITE];
```

- The target must be a simple `VAR_GLOBAL` declared in the same configuration.
- Identifiers and type matching are ASCII case-insensitive.
- The default direction is `READ_ONLY`.
- `READ_WRITE` cannot target a `VAR_GLOBAL CONSTANT` declaration.
- Access names must be unique within their configuration.
- The declaration exports metadata; it does not create a second runtime variable.

## Output contract

After successful semantic analysis and code generation, `iec2c` or `iec2iec` writes
`ACCESS.csv` in the `-T` directory (or the current directory when `-T` is omitted):

```csv
configuration,name,path,type,direction
Controller,RemoteSetpoint,Setpoint,INT,READ_WRITE
Controller,MonitorSetpoint,Setpoint,INT,READ_ONLY
```

The header and column order are the provisional export ABI. No file is produced by
syntax-only mode or when validation fails.

## Deferred forms

Resource/program/function-block qualification, direct addresses, fields, array
elements, and access blocks inside a POU are not implemented. Legacy mode continues
to reject `VAR_ACCESS`.
