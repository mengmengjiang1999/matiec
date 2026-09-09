# embedding-api Specification

## Purpose
TBD - created by archiving change define-versioned-embedding-api. Update Purpose after archive.
## Requirements
### Requirement: Versioned public embedding ABI

The public C API SHALL allow a context to install an include resolver callback.
For each include pragma, the callback SHALL return a borrowed, size-tagged source
view that remains valid until the next resolver call and which the compiler
copies before making that call. A configured resolver SHALL be authoritative
and SHALL NOT fall back to filesystem lookup on misses or errors; filesystem
lookup SHALL require an explicit callback result.

#### Scenario: A memory source includes another memory source

- **WHEN** an embedder compiles a memory source whose include is resolved by the callback
- **THEN** the full source graph compiles without materializing source files

#### Scenario: A resolver misses an include

- **WHEN** the installed resolver returns not found
- **THEN** compilation fails with an include diagnostic and does not search disk

#### Scenario: The resolver is cleared

- **WHEN** the host clears the resolver callback
- **THEN** the existing include-directory filesystem behavior is restored

### Requirement: Public API compatibility contract

The project SHALL maintain a reviewed allowlist of defined public `matiec_*`
symbols and compile the complete published function signatures as both C11 and
C++17. Intentional additive changes SHALL update the allowlist and minor API
version; incompatible changes SHALL require a major API version change.

#### Scenario: A public symbol disappears

- **WHEN** the built library no longer defines an allowlisted symbol
- **THEN** the API compatibility check fails with a symbol-list difference

#### Scenario: An undeclared public symbol appears

- **WHEN** the built library defines a new `matiec_*` symbol without updating the
  reviewed contract
- **THEN** the API compatibility check fails

#### Scenario: A header signature changes

- **WHEN** a published function is incompatible with its C11 signature fixture
- **THEN** compilation of the API contract fails

#### Scenario: A C++ host includes the header

- **WHEN** a C++17 translation unit includes the installed public header
- **THEN** it compiles with C linkage declarations and no C++ implementation
  types exposed
