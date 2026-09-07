## ADDED Requirements

### Requirement: Datatype candidates are context-owned

After candidate filling completes, the compiler SHALL publish each arena node's
ordered candidate datatype vector into the active context's typed analysis store
before narrowing executes.

#### Scenario: Candidate types are inferred

- **WHEN** candidate filling assigns one or more possible datatypes to an expression
- **THEN** the store contains the same ordered non-owning datatype references

#### Scenario: No candidate is available

- **WHEN** an analyzed node has an empty candidate vector
- **THEN** the store contains a valid datatype record with an empty vector rather
  than treating the result as absent

#### Scenario: Legacy narrowing executes

- **WHEN** narrowing still reads the AST compatibility vector
- **THEN** that vector is materialized from the completed store record before use
