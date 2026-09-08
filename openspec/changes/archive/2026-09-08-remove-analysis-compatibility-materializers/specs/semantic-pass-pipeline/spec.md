## ADDED Requirements

### Requirement: Completed analysis has no AST materialization API
The compiler SHALL expose completed constant, datatype, resolution, enumeration, and generator results through typed Analysis Store records and SHALL NOT provide whole-tree APIs that copy those completed records back onto AST compatibility fields.

#### Scenario: A focused record test verifies completed analysis
- **WHEN** a test clears producer-local AST compatibility state after publication
- **THEN** the test reads the completed result from its typed record without invoking a materializer

#### Scenario: A compiler component needs completed analysis
- **WHEN** a production or test component consumes a completed record family
- **THEN** no public materialization declaration is available as an alternate result path

## MODIFIED Requirements

### Requirement: Declaration resolution is context-owned

After datatype narrowing completes, the compiler SHALL publish function and
function-block invocation resolution into the active context's typed Analysis Store.
Completed resolution SHALL remain in that store without an AST copy-back API.

#### Scenario: An overloaded function call is resolved

- **WHEN** narrowing selects a declaration from an ordered candidate list
- **THEN** the resolution record preserves the candidates, selected declaration,
  and extensible parameter count

#### Scenario: An implicit IL function-block operator is resolved

- **WHEN** semantic analysis identifies its function-block declaration
- **THEN** that declaration is published for the operator node

### Requirement: Enumeration analysis is context-owned

After enumeration declaration checking completes, the compiler SHALL publish each
enumeration scope's case-insensitive multimap into the active context's Analysis Store.
Completed enumeration records SHALL remain in that store without an AST copy-back
API.

#### Scenario: Multiple enum types reuse a value spelling

- **WHEN** a scope table contains multiple declarations under the same
  case-insensitive key
- **THEN** the store record preserves every entry
