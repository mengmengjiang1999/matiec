## ADDED Requirements

### Requirement: Source bytes can be bounded

The compiler SHALL reject a primary source or virtual include whose byte count
exceeds the context's configured source-byte limit before parsing those bytes.

#### Scenario: Virtual include exceeds its budget

- **WHEN** a resolver returns more bytes than the configured source limit
- **THEN** compilation fails with a resource-limit diagnostic
