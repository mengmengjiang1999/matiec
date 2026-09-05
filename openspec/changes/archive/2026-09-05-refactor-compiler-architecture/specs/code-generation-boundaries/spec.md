## ADDED Requirements

### Requirement: Injectable output destinations
Code generators SHALL emit through an output abstraction that supports filesystem and in-memory destinations with consistent error propagation.

#### Scenario: In-memory generation
- **WHEN** a test invokes a generator with an in-memory output sink
- **THEN** generated text is available for assertions without creating files

#### Scenario: Output write fails
- **WHEN** an output destination rejects a write or flush
- **THEN** generation returns failure and records an actionable diagnostic

### Requirement: Independently compiled generator components
C, IEC, ST, IL, SFC, declaration, and configuration generator components SHALL have explicit interfaces and MUST NOT depend on including implementation `.cc` files.

#### Scenario: A generator component changes
- **WHEN** one independently compiled component is modified
- **THEN** the build resolves dependencies through declarations rather than textual inclusion of its implementation

### Requirement: Generated behavior preservation
Component extraction SHALL preserve generated output for existing characterized inputs unless a separately tested correctness change is specified.

#### Scenario: A generator component is extracted
- **WHEN** regression inputs are compiled before and after extraction
- **THEN** stable generated artifacts match the approved baseline and compile successfully
