# embedding-api Specification

## Purpose
TBD - created by archiving change define-versioned-embedding-api. Update Purpose after archive.
## Requirements
### Requirement: Versioned public embedding ABI

The public C API SHALL retain its context-owned authoritative include resolver,
resource limits, and thread-safe sticky cancellation contract. It SHALL expose
stable diagnostic codes, compiler phases, severity, messages, and explicit
source-range validity. It SHALL append this metadata after the existing
size-tagged diagnostic prefix and SHALL accept a caller that provides the
complete legacy prefix.

#### Scenario: A memory source includes another memory source

- **WHEN** an embedder compiles a memory source whose include is resolved by the callback
- **THEN** the full source graph compiles without materializing source files

#### Scenario: A resolver misses an include

- **WHEN** the installed resolver returns not found
- **THEN** compilation fails with an include diagnostic and does not search disk

#### Scenario: The resolver is cleared

- **WHEN** the host clears the resolver callback
- **THEN** the existing include-directory filesystem behavior is restored

#### Scenario: Host cancels an in-flight compilation

- **WHEN** a host requests cancellation from another thread
- **THEN** compilation stops cooperatively, reports failure, and retains a cancellation diagnostic

#### Scenario: Host reuses a cancelled context

- **WHEN** a host resets cancellation before compiling again
- **THEN** the context can compile normally with its existing source and options

#### Scenario: An old host retrieves a new diagnostic

- **WHEN** the host supplies the legacy diagnostic structure size
- **THEN** the compiler fills only the supported prefix without overwriting the extension

#### Scenario: An IDE classifies a diagnostic

- **WHEN** a compilation reports a diagnostic
- **THEN** its stable code and phase can be consumed without parsing the English message

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
### Requirement: Callback failures are context-local

The embedding adapter SHALL contain resolver and output callback failures within
their owning compilation and SHALL NOT stop independent batch jobs.

#### Scenario: A callback throws

- **WHEN** a C++ host callback throws while another context compiles
- **THEN** the owning result fails with a diagnostic and the independent result completes normally
