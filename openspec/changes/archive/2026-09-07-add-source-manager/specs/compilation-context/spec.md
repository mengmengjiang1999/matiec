## MODIFIED Requirements

### Requirement: Per-compilation state

Each compilation SHALL use an explicit context that contains compiler options,
diagnostics, source identity and optional source bytes, symbol state, AST storage,
and output configuration for that compilation.

#### Scenario: Two compilations use different source storage

- **WHEN** one context names a filesystem source and another owns memory source bytes
- **THEN** each compilation reads only its configured input and reports its own name
