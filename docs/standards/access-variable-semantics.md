# Experimental `VAR_ACCESS` semantics

`VAR_ACCESS` is available only with `--std=iec61131-3:2025-experimental`. This is a
bounded MATIEC contract, not a complete IEC 61131-3:2025 conformance claim.

## Supported form

```iecst
CONFIGURATION Controller
  VAR_GLOBAL
    Setpoint : INT;
    Samples : ARRAY [1..4] OF INT;
  END_VAR

  (* resource and program configurations *)

  VAR_ACCESS
    RemoteSetpoint : Setpoint : INT READ_WRITE;
    MonitorSetpoint : Setpoint : INT;
    MonitorSample : Samples[2] : INT;
  END_VAR
END_CONFIGURATION
```

Each declaration has the form:

```text
exported-name : access-path : named-type [READ_ONLY | READ_WRITE];
```

- The path root may be a same-configuration global, or a resource name followed by
  a resource global or a program instance and one of its outputs.
- Structure fields use `.Field`; array elements use constant integer `[index]`
  selectors, whose rank and declared bounds are checked.
- Identifiers and type matching are ASCII case-insensitive.
- The default direction is `READ_ONLY`.
- `READ_WRITE` cannot target a `VAR_GLOBAL CONSTANT` declaration.
- Access names must be unique within their configuration.
- The declaration exports metadata; it does not create a second runtime variable.
- Validated declarations remain as explicit access nodes in the compiler AST.
- Validation and CSV metadata are derived from those nodes after parsing; the
  experimental frontend does not rescan or rewrite `VAR_ACCESS` source text.
- `iec2iec` preserves the block and writes an explicit `READ_ONLY` direction when
  the source omits one.

## Output contract

After successful semantic analysis and code generation, `iec2c` or `iec2iec` writes
`ACCESS.csv` in the `-T` directory (or the current directory when `-T` is omitted):

```csv
configuration,name,path,type,direction
Controller,RemoteSetpoint,Setpoint,INT,READ_WRITE
Controller,MonitorSetpoint,Setpoint,INT,READ_ONLY
Controller,MonitorSample,Samples[2],INT,READ_ONLY
```

The header and column order are the provisional export ABI. No file is produced by
syntax-only mode or when validation fails.

## Deferred forms

Direct addresses, dynamic array subscripts, function-block member traversal,
program variables other than outputs, access blocks inside a POU, and runtime
communication-service binding are not implemented. Legacy mode continues to reject
`VAR_ACCESS`.
