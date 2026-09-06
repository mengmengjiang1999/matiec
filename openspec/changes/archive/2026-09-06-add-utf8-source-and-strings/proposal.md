## Why

The IEC public product page identifies UTF-8 strings and related functions as a
2025 addition. MATIEC currently assumes legacy byte-oriented source and strings.

## What Changes

- Validate source encoding and define UTF-8 string storage and length semantics.
- Add only functions whose signatures and behavior have adequate public evidence.
- Gate new behavior behind `iec61131-3:2025-experimental` and preserve legacy output.

## Capabilities

### New Capabilities

- `utf8-source-and-strings`: Experimental UTF-8 source, literals, runtime, and functions.

## Impact

Lexer, AST literals, datatype analysis, C runtime, standard library, generators,
profile regressions, feature catalogue, and user documentation.
