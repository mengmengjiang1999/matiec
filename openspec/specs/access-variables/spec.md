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
and preserve required metadata for downstream consumers.

#### Scenario: An access path is unresolved
- **WHEN** its target does not exist or is not visible
- **THEN** compilation fails with a source-located diagnostic

#### Scenario: A valid access declaration is generated
- **WHEN** a configuration access name targets a same-configuration global with a matching type
- **THEN** its configuration, name, path, type, and effective direction are exported to `ACCESS.csv`

#### Scenario: Writable access targets a constant
- **WHEN** a `READ_WRITE` access name targets a constant global
- **THEN** compilation fails with a source-located diagnostic

### Requirement: Legacy access-variable isolation
Access-variable syntax MUST remain unavailable in the legacy profile.

#### Scenario: A legacy source declares an access block
- **WHEN** source containing `VAR_ACCESS` is compiled with `--std=legacy`
- **THEN** compilation terminates with a profile-specific diagnostic
