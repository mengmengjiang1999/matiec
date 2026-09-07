## ADDED Requirements

### Requirement: Declaration resolution is context-owned

After datatype narrowing completes, the compiler SHALL publish function and
function-block invocation resolution into the active context's typed Analysis Store.

#### Scenario: An overloaded function call is resolved

- **WHEN** narrowing selects a declaration from an ordered candidate list
- **THEN** the resolution record preserves the candidates, selected declaration,
  and extensible parameter count

#### Scenario: An implicit IL function-block operator is resolved

- **WHEN** semantic analysis identifies its function-block declaration
- **THEN** that declaration is published for the operator node

#### Scenario: A legacy generator reads resolution fields

- **WHEN** an unmigrated generator consumes an invocation
- **THEN** its compatibility fields have been materialized from the store record
