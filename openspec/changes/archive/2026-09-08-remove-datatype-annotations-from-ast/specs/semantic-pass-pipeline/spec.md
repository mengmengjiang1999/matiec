## MODIFIED Requirements

### Requirement: Datatype candidates are context-owned

Candidate filling SHALL update each arena node's ordered candidate datatype
vector directly in the active context's typed analysis store before narrowing
executes. Narrowing and later consumers SHALL read that live completed vector
without an AST field or publication traversal.

#### Scenario: Candidate types are inferred

- **WHEN** candidate filling assigns one or more possible datatypes to an expression
- **THEN** the store contains the same ordered non-owning datatype references

#### Scenario: No candidate is available

- **WHEN** an analyzed node has an empty candidate vector
- **THEN** the store contains a valid datatype record with an empty vector rather
  than treating the result as absent

#### Scenario: Narrowing executes

- **WHEN** narrowing requests a completed candidate vector
- **THEN** it obtains the vector from the datatype record without requiring an AST field or copy

### Requirement: Selected datatypes are context-owned

Narrowing and forced narrowing SHALL update each arena node's selected datatype
and scope directly in the active context's datatype record. The compiler SHALL
validate the completed persistent records before dependent semantic passes run,
and downstream passes SHALL consume those fields without AST storage.

#### Scenario: Narrowing selects a datatype

- **WHEN** datatype narrowing assigns a final datatype to an expression
- **THEN** the expression's datatype record contains that same non-owning reference

#### Scenario: A variable has a declaration scope

- **WHEN** datatype analysis associates a variable expression with its POU scope
- **THEN** the same arena-owned scope is present in the expression's datatype record

#### Scenario: A downstream semantic consumer executes

- **WHEN** a pass after narrowing requests selected datatype or scope
- **THEN** it obtains the completed value from the datatype record without an AST field or copy-back

#### Scenario: A producer uses a transient node

- **WHEN** a datatype producer uses a stack-local or immutable shared node that cannot be a persistent record key
- **THEN** its working datatype state is retained only in the active context's transient datatype table
