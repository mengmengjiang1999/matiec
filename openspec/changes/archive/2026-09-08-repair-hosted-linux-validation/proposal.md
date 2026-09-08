## Why

Hosted Linux validation is red even though the corresponding macOS build passes.
The GCC job exposes a clean parallel-build race hidden by local dependency
directories, while both sanitizer jobs invoke top-level Make targets before the
Autotools build files exist.

## What Changes

- Build the shared stage 4 implementation once in its owning directory and link
  it explicitly into both compiler executables.
- Invoke the self-bootstrapping sanitizer regression script directly in hosted
  jobs instead of requiring a pre-generated top-level Makefile.
- Remove copied dependency directories from sanitizer workspaces so local runs
  exercise the same clean-build conditions as hosted validation.

## Capabilities

### Modified Capabilities

- `build-modernization`: Clean parallel builds no longer rely on a pre-existing
  parent dependency directory.
- `regression-validation`: Hosted sanitizer jobs start from a clean checkout by
  using their maintained self-bootstrapping entry point.

## Impact

This changes Automake target ownership, static-library link inputs, the sanitizer
workflow entry point, and clean-copy preparation. Compiler behavior and the
generated C ABI are unchanged.
