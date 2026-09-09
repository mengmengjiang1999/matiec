## ADDED Requirements

### Requirement: Semantic declaration lookup is query-local

Each variable-declaration lookup SHALL reset transient state, return complete
declaration specifications, and fail safely for unsupported selector shapes.

#### Scenario: One lookup object is reused

- **WHEN** successful, unsuccessful, and successful lookups run sequentially
- **THEN** each result, variable class, option, and base type reflects only its own query

#### Scenario: Aggregate and FB members are checked

- **WHEN** semantic analysis resolves structure fields, array elements, or FB members
- **THEN** lookup follows the declared aggregate type without retaining an earlier result

### Requirement: Parameter direction and writability are enforced

Semantic call checking SHALL reject directionally invalid implicit FB calls and
non-writable actual arguments for OUT or IN_OUT parameters.

#### Scenario: An implicit FB operator targets a non-input parameter

- **WHEN** an implicit operator name resolves to an OUT or IN_OUT declaration
- **THEN** semantic analysis reports that the required input parameter is absent

#### Scenario: An IN_OUT actual is not writable

- **WHEN** a literal or expression is passed to an IN_OUT parameter
- **THEN** semantic analysis reports an lvalue error at the argument
