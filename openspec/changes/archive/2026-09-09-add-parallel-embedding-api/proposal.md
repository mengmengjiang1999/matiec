## Why

The compiler can execute independent C++ contexts concurrently, but C ABI
callers must currently schedule one context at a time. A bounded batch entry
point gives non-C++ hosts the same ordered parallelism without exposing threads
or implementation types.

## What Changes

- Add an ordered, bounded C batch compilation function.
- Validate context and result arrays before starting workers.
- Preserve independent diagnostics, callbacks, and last-error state.
- Define duplicate-context and callback-thread behavior.
- Add parallel success/failure regressions and update documentation.

## Capabilities

### Modified Capabilities

- `embedding-api`: expose bounded parallel compilation through the C ABI.

## Impact

The additive embedding API moves to version 1.3 and delegates scheduling to the
existing `Compiler::compile_parallel` implementation.
