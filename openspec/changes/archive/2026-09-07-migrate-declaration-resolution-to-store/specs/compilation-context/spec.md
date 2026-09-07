## ADDED Requirements

### Requirement: Resolution records follow compilation lifetime

Published resolution records SHALL contain only declarations owned by the current
arena or immutable compiler declarations with no arena owner.

#### Scenario: A declaration crosses arenas

- **WHEN** a resolution record references a declaration owned by another context
- **THEN** the store rejects the write without replacing an existing record

#### Scenario: Analysis state is reset

- **WHEN** the context begins another compilation
- **THEN** resolution records from the prior AST are absent
