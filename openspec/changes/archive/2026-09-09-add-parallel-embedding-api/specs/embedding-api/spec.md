## MODIFIED Requirements

### Requirement: Versioned public embedding ABI

The project SHALL provide a C-compatible public embedding header with semantic
major, minor, and patch API version macros and runtime queries. API version 1.3
SHALL expose opaque contexts, stable diagnostic and output callbacks, and a
synchronous bounded batch function whose result indexes match its context
indexes. No C++ exception SHALL cross the C ABI.

#### Scenario: Independent contexts compile concurrently

- **WHEN** a caller submits configured independent contexts with a positive
  concurrency bound
- **THEN** at most that many workers compile them and results retain input order

#### Scenario: Automatic concurrency is selected

- **WHEN** a caller submits a batch with a zero concurrency bound
- **THEN** the implementation selects a positive bounded worker count

#### Scenario: A context is duplicated

- **WHEN** the same context appears more than once in a batch
- **THEN** each duplicate position fails with a context diagnostic while other
  independent contexts can complete

#### Scenario: Compatibility policy is reviewed

- **WHEN** batch compilation is added compatibly
- **THEN** the API minor version increases while its major version remains one
