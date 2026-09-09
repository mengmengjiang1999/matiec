## ADDED Requirements

### Requirement: Handwritten frontend entry points name their parser session

Every handwritten file-backed and memory-backed stage 1/2 entry point SHALL
receive the `ParserState` it initializes and parses. Session classification and
option setup SHALL operate directly on that supplied object.

#### Scenario: A memory source is parsed

- **WHEN** the compiler sends owned source bytes to stage 1/2
- **THEN** it supplies the owning context's parser session explicitly

#### Scenario: Generated compatibility callbacks execute

- **WHEN** the generated frontend invokes an interim callback without a session
  parameter
- **THEN** a scope limited to that generated call selects the explicitly
  supplied session and restores any outer session afterward
