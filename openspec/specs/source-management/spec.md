# source-management Specification

## Purpose
TBD - created by archiving change add-source-manager. Update Purpose after archive.
## Requirements
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

### Requirement: Parse requests bind their owning state

File-backed and memory-backed parse requests SHALL expose the requesting
compilation context's parser state to the generated lexer and grammar for the
duration of the parse.

#### Scenario: A failed parse is followed by another parse

- **WHEN** the first parse leaves a transient lexer command set
- **THEN** the next adapter resets that command before entering the parser
