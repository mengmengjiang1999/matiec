## ADDED Requirements

### Requirement: Evidence-backed namespace grammar
Namespace syntax SHALL NOT be frozen as IEC behavior until its rules have adequate
public evidence; provisional behavior MUST be labeled accordingly.

#### Scenario: Vendors disagree
- **WHEN** public implementations expose incompatible namespace syntax
- **THEN** the project records the disagreement instead of guessing a normative rule

### Requirement: Deterministic qualified lookup
Enabled namespace lookup SHALL resolve qualified names deterministically and diagnose
missing, ambiguous, or inaccessible declarations.

#### Scenario: A qualified name is ambiguous
- **WHEN** more than one visible declaration matches
- **THEN** compilation fails with a diagnostic identifying the lookup conflict
