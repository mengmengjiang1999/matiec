## Why

The experimental `VAR_ACCESS` implementation resolves only a simple global name
in the surrounding configuration. Real configurations also need stable aliases
for resource globals, program outputs, structure fields, and array elements.

## What Changes

- Represent access paths as a root plus typed field and constant-subscript AST
  selectors.
- Resolve configuration globals, resource globals, and resource program outputs.
- Walk declared structure and array types while validating each selector.
- Preserve the canonical path spelling in normalized IEC and `ACCESS.csv`.
- Diagnose unknown path components, invalid selectors, out-of-range subscripts,
  and final type mismatches before generation.

## Capabilities

### Modified Capabilities

- `access-variables`: hierarchical targets are parsed, resolved, validated, and
  exported from the primary AST.

## Impact

Parser and AST definitions, access-variable analysis, IEC regeneration, CLI and
sequential compilation tests, user documentation, and the feature catalogue.
