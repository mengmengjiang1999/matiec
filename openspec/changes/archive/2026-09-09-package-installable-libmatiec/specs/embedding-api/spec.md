## MODIFIED Requirements

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
