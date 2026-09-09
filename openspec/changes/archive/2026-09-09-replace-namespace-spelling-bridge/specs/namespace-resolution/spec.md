## MODIFIED Requirements

### Requirement: Deterministic qualified lookup

Enabled namespace lookup SHALL resolve qualified names deterministically,
diagnose missing, ambiguous, or inaccessible declarations, and preserve
validated namespace structure in the primary compiler AST. Namespace discovery
and lookup SHALL operate on the native parser/scanner stream without rewriting
source bytes before parsing.

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

#### Scenario: A declaration is supplied by an include

- **WHEN** a filesystem or virtual include declares a namespace used by the main source
- **THEN** the native prepass registers it and the definitive parser resolves the reference while preserving the included namespace AST node
