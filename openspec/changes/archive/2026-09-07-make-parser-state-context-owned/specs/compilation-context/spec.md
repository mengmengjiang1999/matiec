## ADDED Requirements

### Requirement: Parser state is context-owned

Each compilation context SHALL own its parser runtime options and transient lexer
transition controls, and SHALL reset transient controls before parsing.

#### Scenario: Parser contexts coexist

- **WHEN** two contexts configure different parser options
- **THEN** each context retains its own option values and transition controls

#### Scenario: Parser adapter scopes nest

- **WHEN** an inner context temporarily becomes active
- **THEN** leaving its scope restores the previously active context state
