## MODIFIED Requirements

### Requirement: Versioned public embedding ABI

The project SHALL provide a C-compatible public embedding header with semantic
major, minor, and patch API version macros and runtime queries. API version 1.2
SHALL expose opaque single-compilation contexts, stable indexed diagnostic
views, optional ordered diagnostic callbacks, and optional generated-output
callbacks that can replace filesystem artifacts. No C++ exception SHALL cross
the C ABI.

#### Scenario: A host consumes diagnostics

- **WHEN** a compilation records diagnostics
- **THEN** the host can inspect each diagnostic in order by index or receive it
  through the configured callback

#### Scenario: A host captures generated files

- **WHEN** an output callback is configured and code generation succeeds
- **THEN** every intended output path and its ordered byte chunks are delivered
  to the callback without creating those generated files

#### Scenario: An output callback rejects data

- **WHEN** the output callback returns zero for a chunk
- **THEN** compilation fails with an output diagnostic instead of throwing

#### Scenario: Compatibility policy is reviewed

- **WHEN** diagnostic and output callbacks are added compatibly
- **THEN** the API minor version increases while its major version remains one
