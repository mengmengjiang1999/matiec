# Design: cancellation and resource budgets

## Public contract

`matiec_limits_t` is size-tagged and uses zero for unlimited. The context copies
the structure. `matiec_context_cancel()` only sets an atomic flag and is the sole
context operation permitted concurrently with compilation. The flag remains set
until `matiec_context_reset_cancel()` succeeds.

## Enforcement

The compiler checks cancellation before source loading and between stages. The
scanner additionally polls once per input byte, which makes long parses
cooperatively interruptible. Primary and virtual include bytes are rejected
before parsing. `DiagnosticEngine` replaces its final retained item with a
terminal limit diagnostic on overflow, preserving the configured maximum.
`OutputManager` rejects the write that would exceed the aggregate byte budget.

## Result semantics

Cancellation and limit exhaustion are compilation failures, not API transport
failures: the compile call returns `MATIEC_STATUS_OK`, `succeeded` is false, and
the diagnostic stream explains the reason. Invalid limit structures remain API
argument errors.
