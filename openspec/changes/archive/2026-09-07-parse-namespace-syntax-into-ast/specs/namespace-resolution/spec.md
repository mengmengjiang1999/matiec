## MODIFIED Requirements

### Requirement: Deterministic qualified lookup

Enabled namespace lookup SHALL resolve qualified names deterministically, diagnose
missing, ambiguous, or inaccessible declarations, and preserve validated namespace
structure in the primary compiler AST.

#### Scenario: A qualified name is ambiguous

- **WHEN** more than one visible declaration matches
- **THEN** compilation fails with a diagnostic identifying the lookup conflict

#### Scenario: A qualified public declaration is parsed

- **WHEN** experimental source refers to a public declaration with its dot-qualified name
- **THEN** its namespace wrapper exists in the primary AST and the declaration and reference use the same deterministic lowered identifier

#### Scenario: An internal declaration is used outside its namespace tree

- **WHEN** experimental source refers to an `INTERNAL` namespace declaration from an unrelated scope
- **THEN** compilation fails with an inaccessible-declaration diagnostic

#### Scenario: Normalized IEC is generated

- **WHEN** `iec2iec` processes a valid experimental namespace and `USING` directive
- **THEN** the output structurally contains the namespace boundaries and directive

### Requirement: Profile isolation

The provisional namespace grammar MUST be enabled only by
`iec61131-3:2025-experimental` and MUST NOT change legacy parsing.

#### Scenario: Namespace syntax is compiled as legacy source

- **WHEN** a source containing `NAMESPACE` is compiled with the legacy profile
- **THEN** the legacy parser rejects it
