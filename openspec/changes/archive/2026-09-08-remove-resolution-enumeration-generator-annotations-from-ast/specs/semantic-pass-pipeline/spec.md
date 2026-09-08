## MODIFIED Requirements

### Requirement: Declaration resolution is context-owned

Datatype filling and narrowing SHALL update invocation resolution directly in
the active context's typed Analysis Store. Persistent and transient working
records SHALL replace AST fields and no publication traversal SHALL be required.

#### Scenario: An overloaded function call is resolved

- **WHEN** narrowing selects a declaration from an ordered candidate list
- **THEN** the live resolution record preserves candidates, declaration, and extensible parameter count

#### Scenario: An implicit IL function-block operator is resolved

- **WHEN** semantic analysis identifies its function-block declaration
- **THEN** that declaration is retained in the operator's live resolution record

### Requirement: Enumeration analysis is context-owned

Enumeration declaration checking SHALL update each scope's case-insensitive
multimap directly in the active context's Analysis Store. Persistent and
transient working records SHALL replace AST tables and no publication traversal
SHALL be required.

#### Scenario: Multiple enum types reuse a value spelling

- **WHEN** a scope table contains multiple declarations under the same case-insensitive key
- **THEN** the live store record preserves every entry

### Requirement: Enumeration results remain store-owned after their pass

The semantic pipeline SHALL retain completed scope-specific enumeration lookup
tables in the active compilation context's Analysis Store without AST fields.

#### Scenario: Later passes follow enumeration checking

- **WHEN** enumeration checking completes and validates its typed records
- **THEN** the pipeline continues without copying records to scope AST nodes

#### Scenario: Typed lookup remains available

- **WHEN** a later consumer performs case-insensitive enum lookup
- **THEN** the typed record returns every matching declaration

### Requirement: Completed analysis has no AST materialization API

The compiler SHALL expose completed constant, datatype, resolution, enumeration,
and generator results through typed Analysis Store records and SHALL NOT provide
AST compatibility result fields or whole-tree materialization APIs.

#### Scenario: A focused record test verifies completed analysis

- **WHEN** a test completes producer updates for a record family
- **THEN** it reads the completed result directly from the typed record

#### Scenario: Stage 4 exchanges generator metadata

- **WHEN** a generator writes or reads named metadata
- **THEN** it uses the generator record without an AST annotation map
