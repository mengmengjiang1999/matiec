## Why

MATIEC's checked library is based on legacy types and functions. A 2025-oriented
profile needs traceable additions without silently changing legacy overloads.

## What Changes

- Inventory current declarations, generators, and runtime implementations.
- Add only publicly evidenced functions and overloads to the experimental profile.
- Version generated declarations and runtime ABI where compatibility requires it.

## Capabilities

### New Capabilities

- `modern-standard-library`: Profile-aware standard functions, blocks, and runtime ABI.

## Impact

Library generation scripts, type resolution, runtime headers, generated C, tests, docs.
