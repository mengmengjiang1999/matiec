## ADDED Requirements

### Requirement: Release and API versions are aligned

The 1.6 release line SHALL use the same semantic version for the package,
runtime embedding API string, public API macros, and shared-library real name.

#### Scenario: A host inspects installed metadata

- **WHEN** version 1.6.0 is installed
- **THEN** pkg-config, the runtime API, and the shared-library filename report 1.6.0
