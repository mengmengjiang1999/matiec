## ADDED Requirements

### Requirement: Compilation context exposes explicit analysis access
`CompilationContext` SHALL own the analysis store and SHALL provide the explicit dependency passed to semantic passes and generators without installing ambient analysis state.

#### Scenario: Two compilations execute sequentially
- **WHEN** separate compilation contexts execute on one thread
- **THEN** each pass and generator accesses only the store supplied by its own context
