## ADDED Requirements

### Requirement: Generators consume resolution records directly

Stage 4 SHALL provide its generators read-only access to the active Analysis Store
and SHALL resolve invocation metadata from typed records.

#### Scenario: C generation emits a resolved function call

- **WHEN** an invocation has a completed resolution record
- **THEN** C generation uses its declaration and extensible parameter count even if
  the corresponding AST compatibility fields are empty
