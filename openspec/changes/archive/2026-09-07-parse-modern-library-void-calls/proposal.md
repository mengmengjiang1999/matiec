## Why

Experimental `ASSERT` recognition currently re-parses one source line with a
regular expression, rejecting valid formatting and duplicating ST expression
rules. The compiler's parser and type system should own call syntax and `VOID`
result validation.

## What Changes

- Replace line-based call validation with a comment/string-aware token scan used
  only to decide whether the experimental declaration is needed.
- Accept ordinary multiline and commented standalone calls through the ST parser.
- Let semantic type checking reject use of the `VOID` result in expressions.
- Preserve user-defined `ASSERT` functions and legacy behavior.

## Capabilities

### New Capabilities

None.

### Modified Capabilities

- `modern-standard-library`: `ASSERT` uses ordinary ST call grammar instead of a
  one-call-per-line preprocessing restriction.

## Impact

This affects the modern-library recognizer, unit/CLI tests, semantics docs, and
the experimental library requirement. Runtime behavior and generated ABI stay
unchanged.
