# Proposal: make Flex scanner fully reentrant

## Why

Thread-local scanner globals permit cross-thread concurrency but do not support
nested parsing on one thread. Embedders can legitimately invoke a compilation
from an include callback, so each parse needs an independent Flex session.

## What changes

- Generate Flex with its reentrant scanner skeleton.
- Allocate and destroy one scanner session for each library or main-source parse.
- Move include, tracking, filename, body-buffer, and token-order state into the
  scanner session.
- Pass the scanner handle through the pure Bison parser and scanner helpers.
- Remove Flex global-to-thread-local generation rewrites and reject regressions.
- Add nested, repeated, parallel, and sanitizer-backed validation.
