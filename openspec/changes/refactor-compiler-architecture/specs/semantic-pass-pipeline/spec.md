## ADDED Requirements

### Requirement: Explicit semantic pass order
Semantic checks SHALL be registered and executed through a pipeline whose order and prerequisites are visible in one location.

#### Scenario: The semantic pipeline executes
- **WHEN** a parsed AST enters semantic analysis
- **THEN** flow, constant, declaration, type, lvalue, range, case, and dependency passes execute in their declared order

### Requirement: Structured pass results
Each semantic pass SHALL return a structured result and report user-facing findings through the shared diagnostic service.

#### Scenario: A pass reports an error
- **WHEN** a semantic pass detects invalid IEC source
- **THEN** the pipeline records the diagnostic, prevents unsafe downstream generation, and returns failure without terminating the process

### Requirement: Pass isolation
Individual passes SHALL be invocable in tests with an explicit context and their declared prerequisite analyses.

#### Scenario: A pass is tested independently
- **WHEN** a test supplies a valid AST and required prior analysis state
- **THEN** the selected pass can execute without invoking the CLI or filesystem output layer
