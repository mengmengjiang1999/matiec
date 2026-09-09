## MODIFIED Requirements

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
