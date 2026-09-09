## MODIFIED Requirements

### Requirement: One compatibility session selects context state

The legacy compatibility boundary SHALL use the selected context-owned parser
session for parser state, AST allocation, and declaration lookup without a
second independently nested binding or mutable fallback session. Required
compatibility access without an active session SHALL fail explicitly.

#### Scenario: A supported compilation enters legacy code

- **WHEN** `Compiler::compile()` enters the legacy frontend and downstream
  visitors
- **THEN** all compatibility concerns resolve to the same compilation context

#### Scenario: Required compatibility access is out of scope

- **WHEN** a legacy helper requests required parser state without an active
  parser session
- **THEN** the request fails instead of reading or mutating fallback state
