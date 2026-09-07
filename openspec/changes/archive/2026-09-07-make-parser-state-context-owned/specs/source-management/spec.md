## ADDED Requirements

### Requirement: Parse requests bind their owning state

File-backed and memory-backed parse requests SHALL expose the requesting
compilation context's parser state to the generated lexer and grammar for the
duration of the parse.

#### Scenario: A failed parse is followed by another parse

- **WHEN** the first parse leaves a transient lexer command set
- **THEN** the next adapter resets that command before entering the parser
