## ADDED Requirements

### Requirement: Generator annotations are context-owned

After Stage 4 generation and output flushing succeed, the compiler SHALL publish
each AST node's named generator symbol annotations into the active context's typed
Analysis Store.

#### Scenario: A generator creates an implicit type identifier

- **WHEN** Stage 4 associates a named symbol with an AST node
- **THEN** the node's generator record contains the same name and non-owning symbol reference

#### Scenario: Generation output fails

- **WHEN** the output manager reports an error
- **THEN** Stage 4 returns failure without publishing a completed generator record set

#### Scenario: A compatibility consumer reads an annotation

- **WHEN** an unmigrated consumer accesses the AST annotation map after publication
- **THEN** the map has been materialized from the completed store record
