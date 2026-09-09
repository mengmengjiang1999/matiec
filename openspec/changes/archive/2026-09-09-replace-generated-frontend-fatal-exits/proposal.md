## Why

The generated Flex scanner still implements fatal allocation and input failures
by calling `exit()`. An embedded compiler must return control to its caller
instead of terminating the host process.

## What Changes

- Rewrite the generated scanner fatal hook to throw `CompilationAbort`.
- Make regeneration fail if the expected Flex fatal-exit shape changes.
- Audit generated frontend output for process-termination calls.
- Update architecture and README documentation.

## Capabilities

### New Capabilities

None.

### Modified Capabilities

- `compilation-context`: Generated frontend fatal failures unwind to the compiler
  boundary and become compilation results.

## Impact

Generated scanner post-processing and failure behavior change; successful
language behavior and public APIs do not.
