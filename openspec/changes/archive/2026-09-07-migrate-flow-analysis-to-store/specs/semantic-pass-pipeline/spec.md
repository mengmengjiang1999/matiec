## ADDED Requirements

### Requirement: Flow edges are context-owned

Flow control analysis SHALL record ordered IL predecessor and successor edges in
the active compilation context's analysis store and SHALL NOT produce canonical
edges directly on AST nodes.

#### Scenario: Fallthrough and jump edges are analyzed

- **WHEN** an IL body contains sequential instructions and a jump target
- **THEN** the store contains reciprocal ordered predecessor and successor records for those instructions

#### Scenario: Legacy consumers execute

- **WHEN** a downstream pass still reads legacy IL flow vectors
- **THEN** it observes a compatibility view derived from the completed store records
