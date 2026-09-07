## ADDED Requirements

### Requirement: Datatype candidates follow compilation lifetime

Published datatype candidate records SHALL belong only to the context and AST arena
that produced them.

#### Scenario: A context is reused after candidate analysis

- **WHEN** a new compilation begins on that context
- **THEN** datatype records produced for the previous AST are no longer observable

#### Scenario: A candidate crosses arenas

- **WHEN** a datatype record references a candidate owned by another context
- **THEN** the store rejects the write without modifying its datatype table

#### Scenario: A compiler canonical datatype has no arena owner

- **WHEN** a datatype record references an immutable canonical datatype shared by compilations
- **THEN** the store accepts that reference while retaining ownership validation for context-owned nodes
