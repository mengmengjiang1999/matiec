# namespace-resolution Specification

## Purpose

Define the evidence and isolation rules for MATIEC's provisional experimental
namespace implementation without presenting vendor-inferred behavior as normative
IEC 61131-3:2025 semantics.
## Requirements
### Requirement: Evidence-backed namespace grammar
Namespace syntax SHALL NOT be frozen as IEC behavior until its rules have adequate
public evidence; provisional behavior MUST be labeled accordingly.

#### Scenario: Vendors disagree
- **WHEN** public implementations expose incompatible namespace syntax
- **THEN** the project records the disagreement instead of guessing a normative rule

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

### Requirement: Namespace structure follows post-parse AST analysis

The context-owned namespace model SHALL be populated only from accepted primary
AST namespace and USING nodes after parsing.

#### Scenario: Namespace metadata is recorded

- **WHEN** experimental source contains a valid namespace with visibility and imports
- **THEN** post-parse analysis records the namespace, visibility, imports, and source ranges from AST nodes

#### Scenario: A context is reused

- **WHEN** a context compiles namespaced source and then source without namespaces
- **THEN** no namespace metadata from the first compilation remains
