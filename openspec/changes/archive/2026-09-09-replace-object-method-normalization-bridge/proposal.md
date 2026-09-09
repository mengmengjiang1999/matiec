# Proposal: Replace object-method normalization bridge

## Why

Object methods parse into native AST nodes, but compilation still depends on a
post-parse compatibility normalization that mutates method bodies and the library.
That weakens AST ownership and blocks reliable nested and included method support.

## What changes

- Replace compatibility construction/lowering passes with native method semantic
  binding.
- Keep callable ABI projections attached to native method nodes rather than adding
  synthetic top-level AST elements.
- Resolve owner fields and nested method receivers without identifier rewriting.
- Generate method implementations and calls through native AST visitors.
- Add nested and virtual-include regressions and update documentation.

## Compatibility

The provisional lowered C symbol spelling remains unchanged. Legacy profile syntax
and output remain unchanged.
