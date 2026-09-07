## ADDED Requirements

### Requirement: Constant results follow compilation lifetime

Published constant records SHALL be cleared before a reused context starts its
next compilation.

#### Scenario: Context is reused after constant propagation

- **WHEN** a new compilation begins on that context
- **THEN** constant records produced for the prior AST are no longer observable
