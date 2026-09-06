## Why

MATIEC has optional reference syntax but incomplete coverage and mixes standard-
oriented `-r` behavior with broader non-standard `-R` forms.

## What Changes

- Inventory parsing, typing, initialization, lifetime, and C generation gaps.
- Complete only evidence-backed reference semantics in the experimental profile.
- Preserve `-R` as a separately named MATIEC extension.

## Capabilities

### New Capabilities

- `reference-semantics`: Traceable reference parsing, typing, diagnostics, and output.

## Impact

Reference grammar, AST, semantic passes, C generator, runtime, CLI compatibility,
catalogue, and regression fixtures.
