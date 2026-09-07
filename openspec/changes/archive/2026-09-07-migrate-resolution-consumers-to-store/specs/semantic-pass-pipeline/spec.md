## ADDED Requirements

### Requirement: Downstream semantic passes consume resolution records

Semantic passes that execute after the completed type-safety boundary SHALL read
resolved invocation declarations from the active context's Analysis Store.

#### Scenario: Lvalue checking validates call output parameters

- **WHEN** lvalue checking visits a resolved function or function-block call
- **THEN** it obtains the declaration from the typed resolution record without
  requiring a materialized AST resolution field
