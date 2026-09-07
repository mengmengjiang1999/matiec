## Why

The experimental access-variable validator currently deletes every `VAR_ACCESS`
block before parsing, so the compiler AST and normalized IEC output cannot
represent a feature the compiler claims to support.

## What Changes

- Add AST nodes for access blocks, declarations, paths, and directions.
- Enable the bounded configuration-level grammar only after profile validation.
- Preserve validated access syntax through parsing and normalized IEC emission.
- Keep `ACCESS.csv`, legacy rejection, and the existing bounded semantics stable.

## Capabilities

### New Capabilities

None.

### Modified Capabilities

- `access-variables`: Valid access declarations become first-class syntax-tree
  elements rather than metadata extracted from deleted source.

## Impact

This affects the AST definition, Bison grammar, access validator, IEC generator,
tests, and access-variable documentation. Generated C and metadata ABI remain
unchanged.
