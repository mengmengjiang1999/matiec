## Why

The existing embedding surface exposes C++ implementation types and is explicitly
source-level only. A stable binary boundary needs a versioned C contract before
compilation functions, callbacks, packaging, or compatibility CI are added.

## What Changes

- Define the public embedding API and ABI compatibility policy.
- Add a C-compatible public header with semantic API version macros.
- Export runtime API version and version-string queries.
- Test the header from a C translation unit and document the boundary.

## Capabilities

### New Capabilities

- `embedding-api`: Versioned, language-neutral discovery contract for the public
  embedding ABI.

### Modified Capabilities

None.

## Impact

A new public header and two query symbols are added. Existing C++ and CLI
interfaces remain unchanged.
