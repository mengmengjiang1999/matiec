## ADDED Requirements

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
