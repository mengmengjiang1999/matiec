# access-variables Specification

## Purpose
Define the evidence boundary, validation rules, exported metadata, and legacy
isolation for the bounded experimental access-variable subset.
## Requirements
### Requirement: Evidence-backed access declarations
Access-variable syntax and semantics SHALL be enabled only after their source and
uncertainties are recorded.

#### Scenario: Only a historical commented rule is available
- **WHEN** no adequate public semantic evidence exists
- **THEN** the experimental profile continues to reject the construct

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

### Requirement: Legacy access-variable isolation
Access-variable syntax MUST remain unavailable in the legacy profile.

#### Scenario: A legacy source declares an access block
- **WHEN** source containing `VAR_ACCESS` is compiled with `--std=legacy`
- **THEN** compilation terminates with a profile-specific diagnostic
