# modern-standard-library Specification

## Purpose
Define evidence, signature, runtime, test, and compatibility requirements for
profile-owned additions to MATIEC's standard-library surface.
## Requirements
### Requirement: Traceable library entries
Every new or changed experimental library function SHALL record its evidence,
signature, resolution rules, runtime implementation, and tests.

#### Scenario: A function appears in one vendor library
- **WHEN** no independent or official evidence supports it
- **THEN** it is not described as an IEC 61131-3:2025 requirement

### Requirement: Legacy library compatibility
Experimental library additions MUST NOT silently change legacy overload resolution
or generated runtime ABI.

#### Scenario: Legacy source is feature-neutral
- **WHEN** it is compiled after an experimental library addition
- **THEN** the selected overload and generated C remain unchanged

#### Scenario: Legacy source declares the same function name
- **WHEN** a legacy-compatible unit declares and calls its own expression-valued `ASSERT`
- **THEN** both profiles preserve normal user-function resolution and output

### Requirement: Experimental assertion contract
The experimental profile SHALL support standalone `ASSERT(IN : BOOL) : VOID` calls
with documented lowering and runtime behavior.

#### Scenario: An assertion condition is false
- **WHEN** an experimental assertion evaluates to false under the released runtime policy
- **THEN** execution continues without an externally visible notification

#### Scenario: An assertion is embedded in an expression
- **WHEN** experimental source attempts to consume an assertion result
- **THEN** compilation fails with a source-located diagnostic
