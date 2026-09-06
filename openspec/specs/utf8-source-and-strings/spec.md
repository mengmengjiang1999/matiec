# UTF-8 source and strings

## Purpose

Define the evidence boundary, compatibility contract, and diagnostics for
experimental UTF-8 source and string support.

## Requirements

### Requirement: Evidence-gated UTF-8 behavior
The experimental profile SHALL implement only UTF-8 behavior whose semantics are
supported by recorded evidence and SHALL identify unresolved behavior explicitly.

#### Scenario: Public evidence gives no indexing rule
- **WHEN** UTF-8 support is designed before an authoritative indexing rule is known
- **THEN** the project does not label a guessed code-unit or code-point rule as IEC behavior

### Requirement: Profile-compatible source handling
UTF-8 support SHALL be isolated to its declared profile without changing legacy
handling of feature-neutral source.

#### Scenario: Feature-neutral source is compiled
- **WHEN** the same legacy-compatible program is compiled in either profile
- **THEN** its acceptance and generated output remain equivalent

### Requirement: Invalid encoding diagnostics
Malformed UTF-8 SHALL produce a deterministic diagnostic with a source location.

#### Scenario: A malformed byte sequence is read
- **WHEN** experimental source contains invalid UTF-8
- **THEN** compilation fails before unsafe literal or identifier processing
