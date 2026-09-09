## Why

Embedders can provide a primary source from memory but include pragmas still
require disk files. IDEs and services need a hermetic source graph owned by the
host application.

## What Changes

- Add a size-tagged borrowed source view and context include-resolver callback.
- Resolve nested includes without temporary files or filesystem fallback while
  the callback is configured.
- Preserve existing include-directory behavior when no resolver is installed.
- Add C API, nesting, error, ownership, and compatibility tests and documentation.

## Capabilities

### New Capabilities

None.

### Modified Capabilities

- `embedding-api`: allow hosts to supply included source bytes on demand.
- `source-management`: make included inputs context-owned and memory-backed.
- `frontend-reentrancy`: route include resolution through the active parser session.

## Impact

The public C ABI gains additive version 1.4 symbols and types. Parser state,
generated scanner integration, API fixtures, tests, README, and architecture
documentation are affected.
