## ADDED Requirements

### Requirement: Selected datatypes are context-owned

After narrowing and forced narrowing complete, the compiler SHALL publish each
arena node's selected datatype and scope into the active context's datatype record
before dependent semantic passes execute.

#### Scenario: Narrowing selects a datatype

- **WHEN** datatype narrowing assigns a final datatype to an expression
- **THEN** the expression's datatype record contains that same non-owning reference

#### Scenario: A variable has a declaration scope

- **WHEN** datatype analysis associates a variable expression with its POU scope
- **THEN** the same arena-owned scope is present in the expression's datatype record

#### Scenario: A legacy consumer executes

- **WHEN** an unmigrated pass reads selected datatype or scope from the AST
- **THEN** those fields have been materialized from the completed store record
