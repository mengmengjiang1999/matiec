## Why

Experimental `ASSERT` support currently scans and rewrites the user's source by
prepending a synthetic function declaration. That makes diagnostics, source
ranges, and the parsed compilation unit depend on text that the user never
wrote. The last roadmap step should make the profile-owned library explicit in
the parser boundary and construct its declaration as compiler-owned AST.

## What Changes

- Seed profile-owned function names through the stage 1/2 symbol-table boundary.
- Discover `ASSERT` use and user declarations from the parsed AST.
- Add the experimental `ASSERT(IN : BOOL) : VOID` declaration as compiler-owned
  AST only when it is required and not shadowed by a user declaration.
- Remove the source-scanning modern-library normalizer and its tests.
- Keep legacy parsing, user-defined `ASSERT`, generated C, and diagnostics
  compatible.

## Capabilities

### Modified Capabilities

- `modern-standard-library`: define profile-aware registration and AST-owned
  declarations for experimental functions.
- `code-generation-boundaries`: prohibit source injection for profile-owned
  library declarations.

## Impact

The compiler orchestration, stage 1/2 compatibility options, modern-library
implementation, tests, architecture documentation, and feature traceability are
updated. No command-line interface or legacy runtime ABI changes.
