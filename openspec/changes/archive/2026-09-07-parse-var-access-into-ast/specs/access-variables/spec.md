## MODIFIED Requirements

### Requirement: Validated access targets

Enabled access declarations SHALL resolve their targets, check direction and type,
remain represented in the parsed syntax tree, and preserve required metadata for
downstream consumers.

#### Scenario: A valid access declaration is parsed

- **WHEN** a configuration access name targets a same-configuration global with a matching type
- **THEN** its name, path, type, and effective direction exist in the access AST and are exported to `ACCESS.csv`

#### Scenario: Normalized IEC is generated

- **WHEN** `iec2iec` processes a valid experimental access declaration
- **THEN** the output contains an equivalent `VAR_ACCESS` block
