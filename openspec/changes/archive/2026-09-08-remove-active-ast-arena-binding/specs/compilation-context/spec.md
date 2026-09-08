## MODIFIED Requirements

### Requirement: Parser state is context-owned

Each compilation context SHALL own its parser runtime options, transient lexer
transition controls, parser classification tables, and a non-owning reference to
its AST arena for legacy parser allocation. It SHALL reset transient controls and
classification entries before parsing.

#### Scenario: Parser contexts coexist

- **WHEN** two contexts configure different parser options and parse different
  declarations
- **THEN** each context retains only its own option values, transition controls,
  classification entries, and AST allocations

#### Scenario: Parser adapter scopes nest

- **WHEN** an inner context temporarily becomes active
- **THEN** leaving its scope restores the previously active context state and
  allocation target
