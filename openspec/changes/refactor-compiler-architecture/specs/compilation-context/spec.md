## ADDED Requirements

### Requirement: Per-compilation state
Each compilation SHALL use an explicit context that contains compiler options, diagnostics, source state, symbol state, AST storage, and output configuration for that compilation.

#### Scenario: Two compilations use different options
- **WHEN** two compilation operations run sequentially with different option sets
- **THEN** each operation observes only its own options and results

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
