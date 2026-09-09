## ADDED Requirements

### Requirement: Parser fuzz harnesses use production entry points

The project SHALL provide libFuzzer and AFL-compatible entry points that exercise
filesystem input, memory input, virtual includes, cancellation, and compiler
resource limits through the public compilation API.

#### Scenario: A corpus input selects an embedding boundary

- **WHEN** a fuzzer supplies a mode byte and arbitrary payload
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
- **THEN** the smoke campaign terminates within its configured bound and fails on crashes, sanitizer findings, or hangs

#### Scenario: A scheduled campaign finds a crash

- **WHEN** periodic fuzzing produces a reproducer
- **THEN** the job fails and preserves the reproducer as a workflow artifact
