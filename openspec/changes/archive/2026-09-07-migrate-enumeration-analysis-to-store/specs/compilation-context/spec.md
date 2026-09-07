## ADDED Requirements

### Requirement: Enumeration records follow compilation lifetime

Published enumeration records SHALL contain only declarations owned by the current
arena or immutable compiler declarations with no arena owner.

#### Scenario: An enum declaration crosses arenas

- **WHEN** an enumeration record references a declaration owned by another context
- **THEN** the store rejects the write without replacing an existing record

#### Scenario: Analysis state is reset

- **WHEN** the context begins another compilation
- **THEN** enumeration records from the prior AST are absent
