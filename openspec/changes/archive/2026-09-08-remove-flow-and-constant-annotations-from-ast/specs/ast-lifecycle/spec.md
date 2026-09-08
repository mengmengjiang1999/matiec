## ADDED Requirements

### Requirement: AST nodes are free of flow and constant analysis storage

The structural AST base and IL instruction classes SHALL NOT contain constant
lattice values or predecessor and successor analysis vectors.

#### Scenario: An AST node is constructed

- **WHEN** parsing or a compiler helper constructs a `symbol_c` or IL instruction subclass
- **THEN** the node carries structural and source information but no flow-edge or constant-analysis storage

#### Scenario: A compilation context is reset

- **WHEN** one context starts a later compilation
- **THEN** persistent and transient constant records and persistent flow records from the previous compilation are cleared
