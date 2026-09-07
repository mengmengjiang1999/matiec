## MODIFIED Requirements

### Requirement: Validated access targets

Enabled access declarations SHALL resolve simple or hierarchical targets, check
every qualification and datatype selector, check direction and final type, remain
represented in the parsed syntax tree, and preserve required metadata for
downstream consumers. Validation SHALL operate on the primary AST without an
experimental pre-parse source scan.

#### Scenario: A valid hierarchical access declaration is parsed

- **WHEN** a configuration access name targets a configuration global, resource
  global, or resource program output through valid structure fields and constant
  array subscripts
- **THEN** its ordered selectors, canonical path, final type, and effective
  direction exist in the access AST and are exported to `ACCESS.csv`

#### Scenario: Hierarchical IEC is regenerated

- **WHEN** `iec2iec` processes a valid hierarchical experimental access declaration
- **THEN** the output contains an equivalent dotted and subscripted path

#### Scenario: A path component is invalid

- **WHEN** an access AST contains an unknown qualifier or field, an incompatible
  selector, or an out-of-range constant subscript
- **THEN** compilation fails with a source-ranged diagnostic before generation

#### Scenario: A hierarchical final type is invalid

- **WHEN** the declared access type does not match the resolved leaf datatype
- **THEN** compilation fails with a source-ranged type diagnostic before generation
