## ADDED Requirements

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
