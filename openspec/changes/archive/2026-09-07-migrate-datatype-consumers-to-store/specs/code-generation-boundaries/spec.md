## ADDED Requirements

### Requirement: Generators consume completed datatype records

Stage 4 SHALL obtain selected datatypes and declaration scopes through the active
compilation Analysis Store and SHALL NOT require production materialization of
completed datatype records onto AST fields.

#### Scenario: C generation emits a typed expression

- **WHEN** a generator requests the selected datatype of an arena-owned expression
- **THEN** it receives the datatype published in that expression's typed record

#### Scenario: Generation resolves declaration scope

- **WHEN** a generator utility requests the scope of an arena-owned variable node
- **THEN** it receives the scope published in that node's typed record
