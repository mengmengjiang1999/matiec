## ADDED Requirements

### Requirement: Analysis identity is arena checked

The compiler SHALL reject an analysis-store write when its key or any non-null
symbol reference in its record is not owned by that store's AST arena.

#### Scenario: A record crosses contexts

- **WHEN** a record in one context refers to a node allocated by another context
- **THEN** the write fails without modifying the destination store
