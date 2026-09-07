# semantic-pass-pipeline Specification

## Purpose
TBD - created by archiving change refactor-compiler-architecture. Update Purpose after archive.
## Requirements
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

### Requirement: Analysis records are typed and stateful

The analysis store SHALL provide distinct record types for flow, constants,
datatypes, declaration resolution, enumerations, and generator metadata, and
SHALL distinguish absent records from invalid records.

#### Scenario: A pass records a negative result

- **WHEN** a semantic pass stores an invalid analysis entry for an arena node
- **THEN** consumers observe a present invalid entry rather than an absent entry

### Requirement: Flow edges are context-owned

Flow control analysis SHALL record ordered IL predecessor and successor edges in
the active compilation context's analysis store and SHALL NOT produce canonical
edges directly on AST nodes.

#### Scenario: Fallthrough and jump edges are analyzed

- **WHEN** an IL body contains sequential instructions and a jump target
- **THEN** the store contains reciprocal ordered predecessor and successor records for those instructions

#### Scenario: Legacy consumers execute

- **WHEN** a downstream pass still reads legacy IL flow vectors
- **THEN** it observes a compatibility view derived from the completed store records
