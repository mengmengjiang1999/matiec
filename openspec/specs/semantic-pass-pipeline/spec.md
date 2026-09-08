# semantic-pass-pipeline Specification

## Purpose
TBD - created by archiving change refactor-compiler-architecture. Update Purpose after archive.
## Requirements
### Requirement: Explicit semantic pass order
Semantic checks SHALL be registered and executed through a pipeline whose order and prerequisites are visible in one location.

#### Scenario: The semantic pipeline executes
- **WHEN** a parsed AST enters semantic analysis
- **THEN** flow, constant, declaration, type, lvalue, range, case, and dependency passes execute in their declared order

### Requirement: Structured pass results
Each semantic pass SHALL return a structured result and report user-facing findings through the shared diagnostic service.

#### Scenario: A pass reports an error
- **WHEN** a semantic pass detects invalid IEC source
- **THEN** the pipeline records the diagnostic, prevents unsafe downstream generation, and returns failure without terminating the process

### Requirement: Pass isolation
Individual passes SHALL be invocable in tests with an explicit context and their declared prerequisite analyses.

#### Scenario: A pass is tested independently
- **WHEN** a test supplies a valid AST and required prior analysis state
- **THEN** the selected pass can execute without invoking the CLI or filesystem output layer

### Requirement: Analysis records are typed and stateful
The compiler SHALL store flow, constant, datatype, declaration-resolution,
enumeration, and generator results in distinct typed `AnalysisStore` records
owned by `CompilationContext`, SHALL distinguish absent records from invalid
records, SHALL require producers and consumers to receive an explicit analysis
access object, and MUST NOT discover analysis state through AST fields,
thread-local active stores, or ambient process state.

#### Scenario: A pass records a negative result
- **WHEN** a semantic pass stores an invalid analysis entry for an arena node
- **THEN** consumers observe a present invalid entry rather than an absent entry

#### Scenario: A semantic producer writes a result
- **WHEN** a Stage 3 pass computes a semantic result
- **THEN** it writes through the explicit analysis dependency supplied to that pass

#### Scenario: A semantic consumer reads a result
- **WHEN** a later pass or generator consumes a semantic result
- **THEN** its interface identifies the analysis dependency used for that lookup

#### Scenario: No active analysis store exists
- **WHEN** compilation enters or leaves semantic analysis and generation
- **THEN** no thread-local analysis pointer or active analysis scope is installed

### Requirement: Flow edges are context-owned

Flow control analysis SHALL record ordered IL predecessor and successor edges in
the active compilation context's analysis store and SHALL NOT store canonical or
fallback edges on AST nodes. Downstream consumers SHALL read completed edges from
typed records; a node without a flow record SHALL expose empty edge sequences.

#### Scenario: Fallthrough and jump edges are analyzed

- **WHEN** an IL body contains sequential instructions and a jump target
- **THEN** the store contains reciprocal ordered predecessor and successor records for those instructions

#### Scenario: Downstream consumers execute

- **WHEN** a downstream pass requests completed IL flow edges
- **THEN** it observes the typed store record without requiring IL flow vectors on the AST

#### Scenario: A node has no flow record

- **WHEN** a consumer requests edges for a transient or unanalyzed IL node
- **THEN** it receives an empty ordered edge sequence rather than AST fallback state

### Requirement: Constant results are context-owned

Constant propagation SHALL update each arena node's complete constant lattice
value directly in the active context's typed analysis store before dependent
passes execute. Producer-created transient nodes SHALL use context-owned transient
constant records, and no whole-tree publication traversal SHALL be required.

#### Scenario: A literal is folded

- **WHEN** constant propagation evaluates an integer literal
- **THEN** the store immediately contains a valid constant record with the evaluated integer value

#### Scenario: A value is not constant

- **WHEN** propagation determines that an expression is non-constant or overflowed
- **THEN** its stored scalar status preserves that result without converting it to an absent record

#### Scenario: A producer uses a transient node

- **WHEN** constant propagation or generation uses a stack-local node that cannot be a persistent record key
- **THEN** its working constant state is retained only in the active context's transient constant table

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

### Requirement: Downstream semantic passes consume resolution records

Semantic passes that execute after the completed type-safety boundary SHALL read
resolved invocation declarations from the active context's Analysis Store.

#### Scenario: Lvalue checking validates call output parameters

- **WHEN** lvalue checking visits a resolved function or function-block call
- **THEN** it obtains the declaration from the typed resolution record without
  requiring a materialized AST resolution field

### Requirement: Enumeration results remain store-owned after their pass

The semantic pipeline SHALL retain completed scope-specific enumeration lookup
tables in the active compilation context's Analysis Store without AST fields.

#### Scenario: Later passes follow enumeration checking

- **WHEN** enumeration checking completes and validates its typed records
- **THEN** the pipeline continues without copying records to scope AST nodes

#### Scenario: Typed lookup remains available

- **WHEN** a later consumer performs case-insensitive enum lookup
- **THEN** the typed record returns every matching declaration

### Requirement: Completed flow and constants are consumed from typed records

Semantic passes after flow analysis and constant folding SHALL obtain completed
flow edges and constant values from the active compilation context's Analysis
Store without AST compatibility storage or production materialization.

#### Scenario: Type analysis follows completed flow analysis

- **WHEN** a datatype visitor requests predecessors or successors for an arena-owned IL node
- **THEN** it receives the edges held in that node's flow record

#### Scenario: Range and case checks follow constant folding

- **WHEN** range or case validation requests a completed constant value
- **THEN** it receives the value held in the node's constant record

#### Scenario: A transient constant producer executes

- **WHEN** a semantic or generator producer creates a stack-local node
- **THEN** its scoped constant accessor uses the active context's transient record rather than an AST field

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
