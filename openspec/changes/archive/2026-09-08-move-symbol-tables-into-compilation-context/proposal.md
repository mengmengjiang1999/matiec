## Why

Function, function-block, program, and datatype declarations are currently kept
in mutable process-wide tables populated after parsing. Sequential resets avoid
most stale-state failures, but separate compilation contexts still share storage
and cannot safely progress toward concurrent compilation.

## What Changes

- Add context-owned declaration symbol tables and clear them at compilation start.
- Populate the owning context rather than global `absyntax_utils` variables.
- Preserve legacy visitor call shapes through a nested thread-local access scope
  whose pointer selects context-owned storage.
- Add context isolation tests and update the reentrancy inventory and README.

## Capabilities

### Modified Capabilities

- `compilation-context`: Declaration lookup state belongs to each compilation and
  cannot leak entries between contexts.

## Impact

This changes internal symbol-table ownership and adds one compiler service. The
CLI, language behavior, generated C ABI, and public embedding entry point remain
unchanged.
