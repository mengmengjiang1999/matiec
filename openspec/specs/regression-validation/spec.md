# regression-validation Specification

## Purpose
TBD - created by archiving change refactor-compiler-architecture. Update Purpose after archive.
## Requirements
### Requirement: Automated regression entry point
The project SHALL provide a single build-system test command that runs maintained compiler regression suites and returns a non-zero status when any suite fails.

#### Scenario: All regression suites pass
- **WHEN** a developer runs `make check` after a successful build
- **THEN** the initialization and CLI regression suites execute and the command succeeds

#### Scenario: A regression fails
- **WHEN** any maintained regression case produces an unexpected exit status, diagnostic, or output
- **THEN** the test command reports the failing case and returns a non-zero status

### Requirement: Isolated test artifacts
Regression tests MUST create transient output outside tracked source locations and MUST remove that output when the test process exits.

#### Scenario: Tests complete
- **WHEN** a regression suite succeeds or fails normally
- **THEN** generated compiler output and captured streams do not dirty the source worktree

### Requirement: Cross-environment validation
The project SHALL validate clean builds and tests on supported GCC/Linux and Clang/macOS environments, with sanitizer coverage available for memory and undefined-behavior checks.

#### Scenario: A change is validated
- **WHEN** the automated validation workflow runs for a branch
- **THEN** each configured environment builds the project from generated build files and executes the regression entry point

### Requirement: Generated output validation
The regression baseline SHALL verify both that valid IEC input produces expected output artifacts and that generated C can be compiled for representative programs.

#### Scenario: Representative IEC source is compiled
- **WHEN** `iec2c` processes a valid representative source file
- **THEN** the expected C and header artifacts are non-empty and compile successfully with the configured C compiler

### Requirement: Language-profile regression coverage
The regression suite SHALL exercise language behavior under the profile that owns that behavior and SHALL guard legacy compatibility when experimental features are added.

#### Scenario: Profile infrastructure is introduced
- **WHEN** the legacy and experimental profile selectors are implemented before any experimental syntax is enabled
- **THEN** representative accepted inputs, diagnostics intent, and generated output remain equivalent

#### Scenario: An experimental feature is added
- **WHEN** a later change enables a construct only in the experimental profile
- **THEN** positive experimental coverage and legacy rejection or compatibility coverage are both present

### Requirement: Evidence-linked tests
Every feature catalogue entry marked implemented SHALL link to maintained test evidence or explicitly identify an unresolved coverage gap.

#### Scenario: Catalogue validation is reviewed
- **WHEN** a language feature changes implementation status
- **THEN** its test references are added or updated in the same change
