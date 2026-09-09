# source-management Specification

## Purpose
Define source identity, owned in-memory input, replayability, and compatibility
with existing file and include behavior.
## Requirements
### Requirement: Source identity is independent of storage

The compiler SHALL represent source bytes independently from the display name
reported in diagnostics.

#### Scenario: Memory source has a virtual name

- **WHEN** an embedding caller supplies source bytes named `memory://main.st`
- **THEN** parsing uses the supplied bytes and diagnostics identify that virtual name

### Requirement: Memory source is replayable

The source manager and parser session SHALL support replaying an exact-byte
memory-backed primary source and memory-backed included sources.
Callback-provided bytes and display names SHALL be copied into context-owned
storage before another resolver invocation, including for nested includes.

#### Scenario: Borrowed include bytes expire

- **WHEN** a resolver returns a source view backed by temporary host storage
- **THEN** parsing remains valid after the callback returns because the compiler copied it

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

### Requirement: Source bytes can be bounded

The compiler SHALL reject a primary source or virtual include whose byte count
exceeds the context's configured source-byte limit before parsing those bytes.

#### Scenario: Virtual include exceeds its budget

- **WHEN** a resolver returns more bytes than the configured source limit
- **THEN** compilation fails with a resource-limit diagnostic
