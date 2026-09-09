## MODIFIED Requirements

### Requirement: Versioned public embedding ABI

The project SHALL provide a C-compatible public embedding header with semantic
major, minor, and patch API version macros and runtime queries. API version 1.1
SHALL expose an opaque context lifecycle, copied file or byte-source input,
essential profile/output options, synchronous compilation, stable result counts,
and a borrowed last-API-error view. No C++ exception SHALL cross the C ABI.

#### Scenario: A C caller compiles memory source

- **WHEN** a caller creates a context, supplies named source bytes and an include
  directory, selects syntax-only compilation, and compiles
- **THEN** it receives a successful result without observing C++ types

#### Scenario: A required argument is missing

- **WHEN** a C caller passes a null required argument
- **THEN** the function returns invalid-argument and preserves a descriptive API
  error where a context is available

#### Scenario: Compatibility policy is reviewed

- **WHEN** the single-compilation functions are added compatibly
- **THEN** the API minor version increases while its major version remains one
