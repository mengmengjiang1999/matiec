## Why

The repository has isolated AddressSanitizer and UndefinedBehaviorSanitizer test
targets, but normal hosted validation never runs them. Parser and AST refactoring
can therefore introduce lifetime or undefined-behavior regressions that ordinary
GCC and Clang jobs do not detect.

## What Changes

- Add an Ubuntu sanitizer workflow with independent ASan and UBSan jobs.
- Run the existing isolated sanitizer targets for pushes, pull requests, and
  manual dispatches.
- Document the hosted sanitizer contract and keep the existing local commands.
- Make local ASan defaults platform-aware and preserve nested failure diagnostics.
- Remove the null-pointer arithmetic exposed by the first complete UBSan run.

## Capabilities

### Modified Capabilities

- `regression-validation`: Hosted validation continuously executes memory and
  undefined-behavior checks rather than merely exposing local targets.

## Impact

This adds one GitHub Actions workflow and documentation/spec changes. Compiler
behavior, dependencies, release artifacts, and the generated C ABI are unchanged.
