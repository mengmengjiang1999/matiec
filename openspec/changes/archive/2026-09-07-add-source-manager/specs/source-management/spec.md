## ADDED Requirements

### Requirement: Source identity is independent of storage

The compiler SHALL represent source bytes independently from the display name
reported in diagnostics.

#### Scenario: Memory source has a virtual name

- **WHEN** an embedding caller supplies source bytes named `memory://main.st`
- **THEN** parsing uses the supplied bytes and diagnostics identify that virtual name

### Requirement: Memory source is replayable

Memory-backed source SHALL support every parser pass without requiring a named
temporary file.

#### Scenario: Pre-parsing is enabled

- **WHEN** a memory source is compiled with pre-parsing enabled
- **THEN** both parser passes read the complete source and compilation succeeds

### Requirement: File behavior is preserved

Adding memory input MUST NOT change legacy file-based CLI behavior, include lookup,
or generated output.

#### Scenario: Existing CLI source is compiled

- **WHEN** a legacy-compatible file is compiled through the CLI
- **THEN** its acceptance and generated artifacts remain unchanged
