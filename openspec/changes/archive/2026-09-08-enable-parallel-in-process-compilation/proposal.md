## Why

The compiler's per-compilation state and generated frontend are now isolated,
but embedders still have to create and coordinate their own threads. A supported
batch entry point and full-pipeline concurrency regression are needed to make the
parallel capability explicit and dependable.

## What Changes

- Add a public `Compiler::compile_parallel()` API for distinct compilation
  contexts with bounded worker concurrency and result-order preservation.
- Reject duplicate context entries without racing the same mutable context.
- Add end-to-end concurrent coverage across parsing, semantic analysis, code
  generation, diagnostics, and AST teardown.
- Update README and architecture documentation to describe supported parallel
  in-process compilation and its context-isolation contract.

## Capabilities

### New Capabilities

- `parallel-compilation`: Public bounded parallel compilation of independent
  contexts with ordered, isolated results.

### Modified Capabilities

None.

## Impact

The C++ embedding API gains a batch method and uses the standard C++ thread
library. Build metadata gains a threaded full-pipeline regression executable.
The CLI, generated C ABI, and single-context `compile()` behavior are unchanged.
