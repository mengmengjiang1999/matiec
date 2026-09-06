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

#### Scenario: A qualified public declaration is used
- **WHEN** experimental source refers to a public declaration with its dot-qualified name
- **THEN** the declaration and reference are lowered to the same deterministic legacy identifier

#### Scenario: An internal declaration is used outside its namespace tree
- **WHEN** experimental source refers to an `INTERNAL` namespace declaration from an unrelated scope
- **THEN** compilation fails with an inaccessible-declaration diagnostic

### Requirement: Profile isolation
The provisional namespace grammar MUST be enabled only by
`iec61131-3:2025-experimental` and MUST NOT change legacy parsing.

#### Scenario: Namespace syntax is compiled as legacy source
- **WHEN** a source containing `NAMESPACE` is compiled with the legacy profile
- **THEN** the legacy parser rejects it

### Requirement: Provisional ABI is explicit
Qualified declarations SHALL use an injective, deterministic internal spelling and
documentation MUST identify that spelling as an unstable experimental ABI.

#### Scenario: Generated C uses a qualified declaration
- **WHEN** a namespaced type or POU reaches C generation
- **THEN** generated declarations and uses contain the same length-prefixed lowered identifier
