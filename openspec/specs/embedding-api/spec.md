# embedding-api Specification

## Purpose
TBD - created by archiving change define-versioned-embedding-api. Update Purpose after archive.
## Requirements
### Requirement: Versioned public embedding ABI

The project SHALL install the public C header, a self-contained static archive,
and a platform-versioned shared library whose ABI major version matches
`MATIEC_API_VERSION_MAJOR`. The shared library SHALL export the declared
`matiec_*` API and SHALL NOT expose compiler implementation symbols.

#### Scenario: A consumer links dynamically

- **WHEN** a staged-install consumer links with `pkg-config matiec`
- **THEN** it loads the versioned shared library and successfully compiles IEC source

#### Scenario: A consumer links statically

- **WHEN** a consumer explicitly links the installed `libmatiec.a`
- **THEN** the same public API remains usable

#### Scenario: The package is uninstalled

- **WHEN** `make uninstall` runs for the staged prefix
- **THEN** the static archive, shared artifact, version links, header, metadata, and data files are removed

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
