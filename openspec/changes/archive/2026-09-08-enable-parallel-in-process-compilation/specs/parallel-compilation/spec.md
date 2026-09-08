## ADDED Requirements

### Requirement: Independent contexts compile in parallel

The embedding API SHALL compile distinct caller-owned `CompilationContext`
instances with bounded worker concurrency and without a process-wide compiler
lock.

#### Scenario: Multiple full-pipeline jobs execute

- **WHEN** a caller submits distinct contexts with a concurrency limit greater
  than one
- **THEN** parsing, semantic analysis, and generation can overlap across worker
  threads

#### Scenario: Automatic concurrency is requested

- **WHEN** the caller supplies a zero concurrency limit
- **THEN** the compiler selects a positive worker count and completes every job

### Requirement: Batch results preserve input order

The parallel compilation API SHALL return exactly one `CompilationResult` per
input context in the same order as the submitted contexts.

#### Scenario: Jobs complete in a different order

- **WHEN** parallel jobs finish independently
- **THEN** each result remains at the index of its input context

#### Scenario: One job fails

- **WHEN** one context contains invalid source while other contexts are valid
- **THEN** only that result reports failure and the valid jobs complete normally

### Requirement: Parallel jobs remain isolated

Each parallel job SHALL retain its diagnostics, parser classifications, AST
allocations, analysis records, and generated outputs in its own context.

#### Scenario: Valid jobs use different output directories

- **WHEN** distinct contexts generate code concurrently to independent output
  directories
- **THEN** each directory contains only the output for its corresponding source

#### Scenario: A context is submitted more than once

- **WHEN** one mutable context appears in multiple batch positions
- **THEN** the compiler rejects those positions without compiling that context
  concurrently

### Requirement: Single-context compilation remains available

The existing `Compiler::compile()` entry point SHALL retain its current behavior
and SHALL be the operation scheduled for each accepted batch item.

#### Scenario: Existing embedder compiles one context

- **WHEN** an embedder continues to call `compile()` directly
- **THEN** no batch setup or API migration is required
