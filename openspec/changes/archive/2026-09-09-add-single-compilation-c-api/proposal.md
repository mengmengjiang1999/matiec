## Why

The versioned C ABI can report compatibility but cannot yet compile source.
Embedders need an opaque context and a minimal single-compilation workflow that
never exposes C++ implementation types or exceptions.

## What Changes

- Add opaque context lifecycle functions.
- Add file and byte-source input plus essential compilation options.
- Add a stable result struct and synchronous compile operation.
- Preserve API misuse details through a borrowed last-error string.

## Capabilities

### New Capabilities

None.

### Modified Capabilities

- `embedding-api`: API version 1.1 adds single-compilation operations.

## Impact

The public C ABI gains compatible symbols and increments its minor version.
Existing CLI and C++ callers are unchanged.
