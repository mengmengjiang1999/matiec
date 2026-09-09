# Design: diagnostic metadata evolution

## Codes and phases

Codes use the stable `MATIEC-{N,W,E,F}dddd` namespace. Generic phase codes are
available for legacy emitters; cancellation, resource limits, include failures,
and output failures use dedicated codes. Messages remain human-readable and may
change without changing the code.

## Range contract

`range_valid` explicitly controls file/line/column fields. `offset_valid`
controls zero-based half-open byte offsets independently because legacy AST
locations do not always retain offsets.

## ABI compatibility

The new fields are appended. `matiec_context_get_diagnostic` accepts structures
through the end of the original 1.2 prefix and guards every extension write by
the supplied size. Callback views always use the current full layout.
