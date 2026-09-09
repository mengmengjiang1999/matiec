## ADDED Requirements

### Requirement: One compatibility session selects context state

The legacy compatibility boundary SHALL use the selected context-owned parser
session for parser state, AST allocation, and declaration lookup without a
second independently nested declaration-table binding.

#### Scenario: A supported compilation enters legacy code

- **WHEN** `Compiler::compile()` enters the legacy frontend and downstream
  visitors
- **THEN** all three compatibility concerns resolve to the same compilation
  context
