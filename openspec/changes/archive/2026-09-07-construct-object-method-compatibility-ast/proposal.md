## Why

Function-block methods and their invocations now have native primary-AST nodes, but
the experimental normalizer still appends generated `FUNCTION` text before parsing.
That hidden source mutation makes parser input differ from user source and leaves the
declaration side of method compatibility outside the explicit AST pipeline.

## What Changes

- Stop appending lowered method-function declarations to normalized source.
- Construct the established compatibility function representation from each native
  method AST after parsing and attach it explicitly to the compilation library.
- Preserve the existing deterministic method ABI, hidden owner-field references,
  semantic behavior, generated C, and native IEC method output.
- Keep legacy-profile behavior and the bounded public/static-dispatch subset unchanged.

## Capabilities

### Modified Capabilities

- `object-oriented-elements`: method declaration compatibility becomes an AST pass
  instead of generated parser input.
- `code-generation-boundaries`: native IEC output no longer contains injected
  compatibility source while C retains the established lowered ABI.

## Impact

This affects the object-method normalizer, the experimental AST compatibility
pipeline, library/symbol initialization, method semantic delegation, C and IEC
generation, focused tests, CLI regressions, and architecture/user documentation.
