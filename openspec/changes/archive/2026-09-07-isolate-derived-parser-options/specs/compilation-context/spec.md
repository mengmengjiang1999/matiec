## MODIFIED Requirements

### Requirement: Per-compilation state

Each compilation SHALL use an explicit context that contains compiler options,
diagnostics, source identity and optional source bytes, structured experimental
syntax metadata, symbol state, AST storage, and output configuration for that
compilation. Derived phase permissions SHALL NOT mutate caller-provided options.

#### Scenario: A context is reused across profiles

- **WHEN** an experimental compilation derives permission for a built-in `VOID`
  function and the same context is then used for legacy source
- **THEN** the configured option remains unchanged and legacy parsing does not
  inherit the derived permission

#### Scenario: Two contexts recognize experimental declarations

- **WHEN** separate contexts compile sources containing different declarations
- **THEN** each context exposes only the declaration metadata from its own source
