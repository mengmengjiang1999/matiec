## ADDED Requirements

### Requirement: AST nodes are free of analysis and generator result storage

AST classes SHALL contain structural/source information only and SHALL NOT contain
flow, constant, datatype, resolution, enumeration, or generator result storage.

#### Scenario: Any AST node is constructed

- **WHEN** parsing or a compiler helper constructs a `symbol_c` subclass
- **THEN** all semantic and generator results reside outside the node in compilation context records

#### Scenario: A compilation context is reset

- **WHEN** one context starts a later compilation
- **THEN** persistent and transient resolution/enumeration records from the previous compilation are cleared
