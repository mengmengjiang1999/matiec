## MODIFIED Requirements

### Requirement: Thin executable boundary

The command-line executable SHALL translate arguments into compiler options,
invoke the compiler API, render diagnostics, and choose the final process status
without implementing compiler phases itself. Handwritten and generated lower
phases SHALL return or unwind failures to the compiler boundary and SHALL NOT
terminate the host process.

#### Scenario: Compilation fails in a lower phase

- **WHEN** parsing, semantic analysis, generation, or a generated scanner fatal
  path reports failure
- **THEN** the compiler returns a failed compilation result and the CLI or
  embedding host retains control
