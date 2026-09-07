## ADDED Requirements

### Requirement: Generator records follow compilation lifetime

Published generator records SHALL contain only symbols owned by the current arena
or immutable compiler symbols with no arena owner.

#### Scenario: A generator symbol crosses arenas

- **WHEN** a generator record references a symbol owned by another context
- **THEN** the store rejects the write without replacing an existing record

#### Scenario: Analysis state is reset

- **WHEN** the context begins another compilation
- **THEN** generator records from the prior AST are absent
