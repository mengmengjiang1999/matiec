# regression-validation Specification

## Purpose
Define the maintained regression entry points, artifact isolation, evidence
coverage, and cross-platform hosted validation contract.
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

The project SHALL validate clean builds and tests on supported GCC/Linux and
Clang/macOS environments, and hosted validation SHALL run independent address,
leak, undefined-behavior, and thread sanitizer jobs without known sanitizer
findings.

#### Scenario: A change is validated

- **WHEN** the automated validation workflows run for a push or pull request from
  a clean checkout
- **THEN** supported compiler and sanitizer jobs execute the maintained regression
  entry points and report independent successful statuses without leaks or races

#### Scenario: A sanitizer job stalls

- **WHEN** a hosted sanitizer build or regression exceeds its configured limit
- **THEN** the workflow terminates that job and reports a failure

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

### Requirement: Reentrancy architecture audit

The normal regression suite SHALL audit the source-of-truth reentrancy
invariants alongside behavioral concurrent-compilation tests.

#### Scenario: A completed invariant regresses

- **WHEN** pure parser parameters, generated failure isolation, or fallback-free
  compatibility access is removed
- **THEN** the normal test suite fails with the missing invariant

### Requirement: ThreadSanitizer gates concurrent regressions

Linux continuous integration SHALL build and run the regression suite with
ThreadSanitizer in addition to address and undefined-behavior sanitizers.

#### Scenario: A data race is introduced

- **WHEN** a concurrent regression triggers ThreadSanitizer
- **THEN** the dedicated sanitizer check fails

### Requirement: Release versions remain consistent

The regression gates SHALL compare package, public API, runtime API, shared
library, and optional release-tag versions and SHALL fail on drift.

#### Scenario: Package and API versions differ

- **WHEN** authoritative version declarations are inconsistent
- **THEN** the release version check fails with the mismatched values

### Requirement: Parser fuzz harnesses use production entry points

The project SHALL provide libFuzzer and AFL-compatible entry points that exercise
filesystem input, memory input, virtual includes, cancellation, and compiler
resource limits through the public compilation API.

#### Scenario: A corpus input selects an embedding boundary

- **WHEN** a fuzzer supplies a mode byte and arbitrary input bytes
- **THEN** the harness invokes the selected production API path in an isolated context and releases all resources

### Requirement: Maintained seed corpus

The project SHALL distribute compact deterministic seeds for every fuzz mode and
MUST keep generated findings outside the tracked seed directory.

#### Scenario: A fresh checkout runs fuzzing

- **WHEN** the fuzz runner starts without prior findings
- **THEN** each API mode has at least one valid or near-valid seed available

### Requirement: Bounded hosted fuzz gates

Hosted validation SHALL run a short libFuzzer smoke campaign for pushes and pull
requests and a longer time-bounded campaign on a schedule.

#### Scenario: A push changes parser behavior

- **WHEN** hosted validation runs for the push
- **THEN** the smoke campaign terminates within its configured bound and fails on unexpected termination, sanitizer findings, or hangs

#### Scenario: A scheduled campaign finds a failing input

- **WHEN** periodic fuzzing produces a minimized failure sample
- **THEN** the job fails and preserves the sample as a workflow artifact
