## ADDED Requirements

### Requirement: Explicit language profiles
The compiler SHALL represent the preserved MATIEC language and the evolving 2025-oriented language as distinct, explicitly named profiles.

#### Scenario: Existing invocation omits a profile
- **WHEN** a user invokes a compiler tool without selecting a language profile
- **THEN** the compiler uses the legacy profile and preserves existing accepted input and generated output

#### Scenario: Experimental profile is selected
- **WHEN** a user explicitly selects `iec61131-3:2025-experimental`
- **THEN** the compiler identifies the selected profile as experimental in help or diagnostic output and does not describe it as conformant

### Requirement: Profiles and extensions remain distinguishable
The compiler MUST distinguish edition-oriented profile behavior from MATIEC-specific or independently controlled language extensions.

#### Scenario: A historical extension is enabled
- **WHEN** a user enables a MATIEC extension that is not verified as part of the selected profile
- **THEN** the compiler records or reports that behavior as an extension rather than attributing it to IEC 61131-3:2025

### Requirement: Profile changes are compatibility gated
A feature SHALL NOT become part of the legacy profile merely because it is implemented for the experimental profile.

#### Scenario: Experimental syntax is introduced
- **WHEN** a later change adds syntax to the experimental profile
- **THEN** the legacy profile retains its prior acceptance and rejection behavior unless a separate compatibility change explicitly modifies it

### Requirement: Language claims are bounded
Project documentation MUST state that the experimental profile follows publicly documented evolution and does not claim complete or certified IEC 61131-3:2025 conformance.

#### Scenario: A user reads profile documentation
- **WHEN** the experimental profile is presented in README, CLI, or user-manual documentation
- **THEN** a non-conformance disclaimer and the implemented feature boundary are available without requiring access to a paid standard
