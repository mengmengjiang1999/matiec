## ADDED Requirements

### Requirement: Traceable language feature catalogue
The project SHALL maintain stable feature records containing an identifier, language family, implementation status, applicable profile or extension, evidence level, evidence references, tests, and unresolved questions.

#### Scenario: An implemented feature is catalogued
- **WHEN** a maintained lexer, parser, semantic, library, or generator feature is listed
- **THEN** its record identifies repository evidence and at least one relevant test location or explicitly records a test gap

#### Scenario: A planned feature is catalogued
- **WHEN** a feature is known but not implemented
- **THEN** its record says `not-implemented` or `planned` rather than implying partial conformance

### Requirement: Evidence and implementation are independent
The catalogue MUST distinguish proof that MATIEC implements a behavior from proof that the behavior belongs to a particular IEC edition.

#### Scenario: Existing IL support is recorded
- **WHEN** the catalogue describes IL
- **THEN** it may identify repository implementation evidence while leaving its exact 2025 status unverified

### Requirement: Evidence levels are explicit
Every standards-related claim SHALL use `implementation-verified`, `official-public`, `cross-vendor-provisional`, or `unverified`, with source links where applicable.

#### Scenario: Vendor documentation is the only source
- **WHEN** a feature is inferred from multiple vendor implementations without normative public text
- **THEN** the feature is classified as `cross-vendor-provisional` and is not described as an IEC requirement

### Requirement: Copyrighted publications stay outside source control
The repository MUST NOT contain an IEC publication, substantial copied tables or grammar, or a reconstructed replacement for a copyrighted standard.

#### Scenario: An authorized contributor uses a paid publication
- **WHEN** the contributor records an implementation decision derived from material they are licensed to read
- **THEN** the repository stores original implementation notes, clause references, and tests without storing or reproducing the publication
