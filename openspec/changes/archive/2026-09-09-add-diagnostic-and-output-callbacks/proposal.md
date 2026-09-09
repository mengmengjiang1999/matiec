## Why

Embedding callers can run a compilation but cannot yet consume individual
diagnostics or generated artifacts without parsing process output or reading
files. Native callbacks and stable diagnostic views let hosts integrate the
compiler into editors, build systems, and services.

## What Changes

- Add stable diagnostic severity and view types with indexed access.
- Add an optional diagnostic callback delivered after each compilation.
- Add an optional generated-output callback that replaces filesystem writes.
- Define callback failure and borrowed-string lifetime behavior.
- Update the embedding API documentation and examples.

## Capabilities

### Modified Capabilities

- `embedding-api`: expose diagnostics and generated output through the C ABI.

## Impact

The additive C API moves to version 1.2. The internal output manager gains a
configurable file-sink factory; CLI behavior remains unchanged.
