## MODIFIED Requirements

### Requirement: Datatype candidates are context-owned

After candidate filling completes, the compiler SHALL publish each arena node's
ordered candidate datatype vector into the active context's typed analysis store
before narrowing executes. Narrowing and later consumers SHALL read that completed
vector from the typed record without production AST materialization.

#### Scenario: Candidate types are inferred

- **WHEN** candidate filling assigns one or more possible datatypes to an expression
- **THEN** the store contains the same ordered non-owning datatype references

#### Scenario: No candidate is available

- **WHEN** an analyzed node has an empty candidate vector
- **THEN** the store contains a valid datatype record with an empty vector rather
  than treating the result as absent

#### Scenario: Narrowing executes

- **WHEN** narrowing requests a completed candidate vector
- **THEN** it obtains the vector from the datatype record without requiring an AST copy

### Requirement: Selected datatypes are context-owned

After narrowing and forced narrowing complete, the compiler SHALL publish each
arena node's selected datatype and scope into the active context's datatype record
before dependent semantic passes execute. Downstream passes SHALL consume those
fields from the completed record without production AST materialization.

#### Scenario: Narrowing selects a datatype

- **WHEN** datatype narrowing assigns a final datatype to an expression
- **THEN** the expression's datatype record contains that same non-owning reference

#### Scenario: A variable has a declaration scope

- **WHEN** datatype analysis associates a variable expression with its POU scope
- **THEN** the same arena-owned scope is present in the expression's datatype record

#### Scenario: A downstream semantic consumer executes

- **WHEN** a pass after narrowing requests selected datatype or scope
- **THEN** it obtains the completed value from the datatype record without requiring AST copy-back

#### Scenario: A producer uses a transient node

- **WHEN** a datatype producer creates a stack-local or canonical node without a datatype record
- **THEN** typed access falls back to that node's producer-local AST field
