## ADDED Requirements

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
