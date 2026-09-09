## MODIFIED Requirements

### Requirement: Installed embedding consumer smoke test

The regression suite SHALL stage installation and execute both dynamically and
statically linked C API consumers. It SHALL validate runtime shared-library
lookup, ABI-major naming, pkg-config discovery, and uninstall cleanup on each
supported host platform.

#### Scenario: Shared package smoke test runs

- **WHEN** package regressions run on Linux or macOS
- **THEN** both installed linkage modes compile, link, execute, and uninstall cleanly

### Requirement: Dedicated API compatibility CI gate

The API compatibility gate SHALL compare the shared library's exported API with
the maintained public-symbol allowlist in addition to checking the static
archive and header signatures.

#### Scenario: An internal symbol becomes dynamically visible

- **WHEN** the shared library exports a compiler implementation symbol
- **THEN** the API compatibility gate fails
