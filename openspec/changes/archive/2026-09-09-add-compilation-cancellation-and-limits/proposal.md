# Proposal: bounded and cancellable compilation

## Why

Embedded and multi-tenant hosts need predictable control over malformed or
oversized jobs. Today they can neither stop a running parse nor cap retained
diagnostics and generated output.

## What changes

- Add thread-safe cancellation request and explicit reset APIs.
- Add per-context primary/include input, diagnostic, and output byte/count limits.
- Fail compilation deterministically with a retained diagnostic when a request is
  cancelled or a configured limit is exceeded.
- Add C API, reuse, batch, and asynchronous cancellation coverage.
- Update public API documentation and compatibility fixtures for version 1.5.0.

## Compatibility

All limits default to unlimited and cancellation defaults to clear. Existing
callers therefore retain current behavior until they opt in.
