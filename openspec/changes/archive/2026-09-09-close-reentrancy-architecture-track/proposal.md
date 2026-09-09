## Why

The supported cross-thread compilation path is now isolated, but the roadmap
still describes reentrancy as an active architecture track and the parser
selector retains a mutable fallback session. The completed guarantees need a
final executable audit and an honest boundary statement.

## What Changes

- Remove the mutable fallback parser session and make missing required scope fail
  explicitly while preserving optional arena attachment for standalone ASTs.
- Add a source-level architecture regression audit for the completed invariants.
- Rename the generated frontend build stamp to describe isolation rather than
  only thread-local conversion.
- Mark the supported cross-thread reentrancy track complete in README and
  architecture documentation, retaining same-thread recursive Flex scanning as
  an explicit non-goal.

## Capabilities

### New Capabilities

None.

### Modified Capabilities

- `frontend-reentrancy`: Compatibility access has no mutable fallback, and the
  supported architecture invariants are continuously audited.
- `regression-validation`: The normal suite includes the reentrancy architecture
  audit.

## Impact

Unsupported compatibility calls without an active session now fail fast;
standalone AST construction remains valid and successful compilation behavior
does not change.
