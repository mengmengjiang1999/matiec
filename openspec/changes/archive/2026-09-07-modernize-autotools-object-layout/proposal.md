## Why

Automake warns on every bootstrap that source files from subdirectories are being
compiled without `subdir-objects`. The legacy fallback may change in a future
Automake release, and the in-memory generator test also compiles a context source
already supplied by `libcompiler.a`.

## What Changes

- Enable Automake's stable subdirectory object layout explicitly.
- Remove the duplicate `compilation_context.cc` test source and link its library
  implementation once.
- Validate clean and out-of-tree builds plus the full regression suite.

## Capabilities

### Modified Capabilities

- `build-modernization`: Generated object placement is explicit and test targets
  do not compile library implementations redundantly.

## Impact

This affects Autotools-generated object paths and one test link definition. It
does not change installed binaries, compiler behavior, or the generated C ABI.
