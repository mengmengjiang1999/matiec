## Why

Recognizing the experimental `ASSERT` helper currently sets
`CompilationContext::options().allow_void_datatype` permanently. Reusing that
context for legacy input can therefore expose an internal experimental parser
extension that the caller never requested.

## What Changes

- Derive parser-only options in a local value instead of mutating caller options.
- Let the legacy parser adapter consume that effective option snapshot.
- Add experimental-to-legacy context reuse coverage, including legacy `VOID`
  rejection after a successful assertion compilation.

## Capabilities

### Modified Capabilities

- `compilation-context`: Caller-provided options remain stable and derived parser
  permissions cannot leak between compilations.

## Impact

This affects the compiler driver, the legacy frontend adapter, sequential API
tests, and architecture documentation. CLI behavior and generated output remain
unchanged.
