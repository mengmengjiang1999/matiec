## ADDED Requirements

### Requirement: Datatype selections follow compilation lifetime

Published selected datatype and scope records SHALL belong to the context and AST
arena that produced them, except for immutable compiler canonical datatypes.

#### Scenario: A context begins another compilation

- **WHEN** context analysis state is reset
- **THEN** selected datatype and scope records from the prior AST are absent

#### Scenario: A scope crosses arenas

- **WHEN** a datatype record references a scope owned by another context
- **THEN** the store rejects the write without replacing the prior record
