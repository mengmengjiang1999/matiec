## ADDED Requirements

### Requirement: AST nodes are free of datatype analysis storage
The structural AST base class SHALL NOT contain candidate datatype vectors, selected datatype pointers, or declaration-scope analysis pointers.

#### Scenario: An AST node is constructed
- **WHEN** parsing or a compiler helper constructs a `symbol_c` subclass
- **THEN** the node carries source, token, parent, and ownership structure but no datatype-analysis storage

#### Scenario: A compilation context is reset
- **WHEN** one context starts a later compilation
- **THEN** both persistent and transient datatype working records from the previous compilation are cleared
