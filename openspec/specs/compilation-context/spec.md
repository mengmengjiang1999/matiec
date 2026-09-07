# compilation-context Specification

## Purpose
TBD - created by archiving change refactor-compiler-architecture. Update Purpose after archive.
## Requirements
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
### Requirement: Thin executable boundary
The command-line executable SHALL translate arguments into compiler options, invoke the compiler API, render diagnostics, and choose the final process status without implementing compiler phases itself.

#### Scenario: Compilation fails in a lower phase
- **WHEN** parsing, semantic analysis, or generation reports failure
- **THEN** the CLI returns failure without the lower phase terminating the process directly

### Requirement: No new process-wide compilation state
New compiler functionality MUST NOT introduce mutable process-wide state outside an explicitly documented compatibility adapter.

#### Scenario: A component needs shared compilation data
- **WHEN** a compiler component requires options, symbols, diagnostics, or output configuration
- **THEN** it receives that data through the compilation context or a context-owned service
