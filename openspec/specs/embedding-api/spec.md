# embedding-api Specification

## Purpose
TBD - created by archiving change define-versioned-embedding-api. Update Purpose after archive.
## Requirements
### Requirement: Versioned public embedding ABI

The project SHALL install the versioned C embedding header, a self-contained
static `libmatiec` containing the C code generator, compiler IEC library data,
generated-C runtime headers, and `pkg-config` metadata. An external C source
consumer SHALL compile against the staged header, link through a C++ linker, and
execute without source-tree include or archive paths.

#### Scenario: A consumer installs and links

- **WHEN** a caller stages installation and queries `pkg-config` from that prefix
- **THEN** it can compile a C translation unit, link `libmatiec`, and execute API
  version discovery

#### Scenario: Compiler library data is installed

- **WHEN** an installed embedding host configures the documented shared-data
  include directory
- **THEN** the compiler can load the same IEC library definitions as a source
  tree build

#### Scenario: The package is uninstalled

- **WHEN** the caller runs the generated uninstall target for the same staging
  root
- **THEN** the public header, metadata, library, and installed data are removed

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
