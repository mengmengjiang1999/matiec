## MODIFIED Requirements

### Requirement: Validated access targets

Enabled access declarations SHALL resolve their targets, check direction and type,
remain represented in the parsed syntax tree, and preserve required metadata for
downstream consumers. Validation SHALL operate on the primary AST without an
experimental pre-parse source scan.

#### Scenario: A valid access declaration is parsed

- **WHEN** a configuration access name targets a same-configuration global with a matching type
- **THEN** its name, path, type, and effective direction exist in the access AST and are exported to `ACCESS.csv`

#### Scenario: Normalized IEC is generated

- **WHEN** `iec2iec` processes a valid experimental access declaration
- **THEN** the output contains an equivalent `VAR_ACCESS` block

#### Scenario: Access metadata is derived

- **WHEN** a configuration contains a valid simple-global access declaration
- **THEN** a post-parse AST pass records its configuration, name, path, type, direction, and source range

#### Scenario: An invalid target is diagnosed

- **WHEN** an access AST refers to a missing, mismatched, or forbidden target
- **THEN** compilation fails with a source-ranged diagnostic before generation
