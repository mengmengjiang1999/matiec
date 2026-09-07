## Why

The scanner supplies its own `yywrap()` implementation, but `configure.ac` calls
`AC_PROG_LEX` without declaring that policy. Current Autoconf warns that the
ambiguous form is obsolete and may perform an unnecessary lexer-library search.

## What Changes

- Declare the no-library yywrap policy to `AC_PROG_LEX`.
- Document that MATIEC's generated scanner owns the callback.
- Link shared compiler support from `libcompiler.a` so recursive directories own
  only their own generated objects and dependency files.
- Verify warning-free bootstrap plus clean build and regression behavior.

## Capabilities

### Modified Capabilities

- `build-modernization`: Lexer discovery has an explicit, reproducible yywrap
  ownership contract.

## Impact

Only Autoconf lexer discovery and build documentation change. The Flex source,
scanner behavior, binaries, and generated C ABI remain unchanged.
