## ADDED Requirements

### Requirement: Compilation work is cooperatively cancellable

The compilation context SHALL retain an atomic cancellation request and the
compiler SHALL observe it during source scanning and between major stages.

#### Scenario: Cancellation is requested before parsing

- **WHEN** a context has a pending cancellation request
- **THEN** compilation fails before producing output

### Requirement: Resource limits are context-local

The compilation context SHALL own independent source-byte, diagnostic-count,
and output-byte limits, with zero representing unlimited.

#### Scenario: Parallel contexts use different budgets

- **WHEN** batch members have different resource limits
- **THEN** exhausting one member's budget does not change another member's result
