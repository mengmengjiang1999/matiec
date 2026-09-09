## ADDED Requirements

### Requirement: Release versions remain consistent

The regression gates SHALL compare package, public API, runtime API, shared
library, and optional release-tag versions and SHALL fail on drift.

#### Scenario: Package and API versions differ

- **WHEN** authoritative version declarations are inconsistent
- **THEN** the release version check fails with the mismatched values
