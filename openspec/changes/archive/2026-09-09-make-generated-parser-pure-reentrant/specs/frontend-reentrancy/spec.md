## ADDED Requirements

### Requirement: Generated parser invocation state is pure

The generated parser SHALL keep lookahead, semantic/location values, and its
internal error counter local to each invocation and SHALL receive the owning
`ParserState` as an explicit parser and lexer parameter.

#### Scenario: Independent parser calls overlap

- **WHEN** separate contexts parse concurrently
- **THEN** their generated parser invocation state cannot alias

#### Scenario: Recovered errors occur

- **WHEN** the grammar recovers from syntax errors
- **THEN** the owning parser session reports the error total after parsing
