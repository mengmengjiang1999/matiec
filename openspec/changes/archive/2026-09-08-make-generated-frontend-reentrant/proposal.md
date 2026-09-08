## Why

The generated Flex/Bison frontend and its classification tables still keep
mutable process-wide state, so otherwise independent compilation contexts race
as soon as parsing overlaps. Isolating that state is the prerequisite for a
supported parallel in-process compiler API.

## What Changes

- Move parser classification tables into each compilation context's parser
  state.
- Make generated scanner/parser variables and handwritten scanner session data
  thread-local through a checked, repeatable post-generation step.
- Add a concurrent frontend regression that parses distinct sources on separate
  threads and proves their state and ASTs remain isolated.
- Update the architecture inventory and README to describe the remaining arena
  compatibility binding accurately.

## Capabilities

### New Capabilities

- `frontend-reentrancy`: Independent compilation contexts can execute the
  generated frontend concurrently on separate threads without sharing parser
  state.

### Modified Capabilities

- `compilation-context`: Parser classification storage becomes context-owned
  and is reset when the owning context is reused.

## Impact

This changes internal parser-state ownership, generated-source build rules, and
frontend tests. It does not change IEC language behavior, generated C ABI, or
the existing synchronous `Compiler::compile` entry point.
