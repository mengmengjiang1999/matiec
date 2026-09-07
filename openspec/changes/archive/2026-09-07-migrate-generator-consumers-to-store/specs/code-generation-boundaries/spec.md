## ADDED Requirements

### Requirement: Stage 4 exchanges generator annotations through the context
Stage 4 SHALL write and read named generator symbols through the active
compilation context's Analysis Store and SHALL NOT require mutable AST annotation
maps as a production communication channel.

#### Scenario: Implicit datatype identifier is reused

- **WHEN** a datatype generator assigns an implicit type identifier to an
  arena-owned AST node
- **THEN** a later Stage 4 visitor resolves that identifier from the node's typed
  generator record

#### Scenario: Stage 4 completes successfully

- **WHEN** all generated output is flushed without errors
- **THEN** generator records already contain the live annotations and no AST
  publisher or materializer is run
