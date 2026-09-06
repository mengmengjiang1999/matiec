# Reference semantics

## Purpose

Define traceable parsing, typing, diagnostics, and generated output for reference
forms while keeping MATIEC-specific extensions separate.

## Requirements

### Requirement: Reference extension boundary
Standard-oriented reference behavior and MATIEC non-standard reference extensions SHALL
remain separately identifiable and selectable.

#### Scenario: Broad reference extensions are enabled
- **WHEN** a user selects `-R`
- **THEN** the additional forms are reported and documented as MATIEC extensions

### Requirement: Reference semantic coverage
Every enabled reference form SHALL have defined typing, initialization, dereference,
diagnostic, and generated-output behavior.

#### Scenario: An invalid reference assignment is analyzed
- **WHEN** source assigns incompatible reference targets
- **THEN** compilation fails with a focused semantic diagnostic
