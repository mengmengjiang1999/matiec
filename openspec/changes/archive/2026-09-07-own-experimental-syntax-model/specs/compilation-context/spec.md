## MODIFIED Requirements

### Requirement: Per-compilation state

Each compilation SHALL use an explicit context that contains compiler options,
diagnostics, source identity and optional source bytes, structured experimental
syntax metadata, symbol state, AST storage, and output configuration for that
compilation.

#### Scenario: A context is reused

- **WHEN** a context begins another compilation after an experimental or failed run
- **THEN** its experimental syntax model is cleared before new source is inspected

#### Scenario: Two contexts recognize experimental declarations

- **WHEN** separate contexts compile sources containing different declarations
- **THEN** each context exposes only the declaration metadata from its own source
