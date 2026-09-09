## Why

The supported compiler owns a `ParserState`, but the stage 1/2 API does not name
that dependency. It relies on an adapter-installed ambient session, making the
frontend contract unclear and preventing the generated interface from later
receiving a reentrant scanner/parser handle.

## What Changes

- Require an explicit `ParserState&` in every stage 1/2 entry point.
- Initialize parser classifications and options through that named session.
- Limit the active-session compatibility scope to the generated frontend call.
- Update adapter calls, tests, and architecture documentation.

## Capabilities

### New Capabilities

None.

### Modified Capabilities

- `frontend-reentrancy`: Handwritten frontend entry points explicitly receive
  the parser session they operate on.

## Impact

Internal stage 1/2 function signatures change. The public `Compiler` API, CLI,
language behavior, and generated output remain unchanged.
