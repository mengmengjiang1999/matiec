# embedding-api Specification

## Purpose
TBD - created by archiving change define-versioned-embedding-api. Update Purpose after archive.
## Requirements
### Requirement: Versioned public embedding ABI

The project SHALL provide a C-compatible public embedding header with semantic
major, minor, and patch API version macros, a packed numeric version, and runtime
queries for the numeric and display forms. The public embedding ABI SHALL keep
implementation objects opaque.

#### Scenario: A C caller checks compatibility

- **WHEN** a C translation unit includes the public header and calls the runtime
  version queries
- **THEN** it compiles without C++ dependencies and observes the same version as
  the header macros

#### Scenario: Compatibility policy is reviewed

- **WHEN** a public ABI change is proposed
- **THEN** documentation identifies breaking changes by a major increment,
  compatible additions by a minor increment, and fixes by a patch increment
