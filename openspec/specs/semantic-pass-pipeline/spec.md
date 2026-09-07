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

The analysis store SHALL provide distinct record types for flow, constants,
datatypes, declaration resolution, enumerations, and generator metadata, and
SHALL distinguish absent records from invalid records.

#### Scenario: A pass records a negative result

- **WHEN** a semantic pass stores an invalid analysis entry for an arena node
- **THEN** consumers observe a present invalid entry rather than an absent entry

### Requirement: Flow edges are context-owned

Flow control analysis SHALL record ordered IL predecessor and successor edges in
the active compilation context's analysis store and SHALL NOT produce canonical
edges directly on AST nodes. Downstream consumers SHALL read completed edges
from those records without production AST materialization.

#### Scenario: Fallthrough and jump edges are analyzed

- **WHEN** an IL body contains sequential instructions and a jump target
- **THEN** the store contains reciprocal ordered predecessor and successor records for those instructions

#### Scenario: Downstream consumers execute

- **WHEN** a downstream pass requests completed IL flow edges
- **THEN** it observes the typed store record without requiring legacy IL flow vectors

### Requirement: Constant results are context-owned

After constant propagation completes, the compiler SHALL publish each analyzed
arena node's complete constant lattice value into the active context's typed
analysis store before dependent passes execute.

#### Scenario: A literal is folded

- **WHEN** constant propagation evaluates an integer literal
- **THEN** the store contains a valid constant record with the evaluated integer value

#### Scenario: A value is not constant

- **WHEN** propagation determines that an expression is non-constant or overflowed
- **THEN** its stored scalar status preserves that result without converting it to an absent record

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

### Requirement: Enumeration analysis is context-owned

After enumeration declaration checking completes, the compiler SHALL publish each
enumeration scope's case-insensitive multimap into the active context's Analysis Store.

#### Scenario: Multiple enum types reuse a value spelling

- **WHEN** a scope table contains multiple declarations under the same
  case-insensitive key
- **THEN** the store record preserves every entry

#### Scenario: A legacy semantic consumer reads a scope table

- **WHEN** an unmigrated consumer accesses an AST enumeration table
- **THEN** the table has been materialized from the completed store record

### Requirement: Downstream semantic passes consume resolution records

Semantic passes that execute after the completed type-safety boundary SHALL read
resolved invocation declarations from the active context's Analysis Store.

#### Scenario: Lvalue checking validates call output parameters

- **WHEN** lvalue checking visits a resolved function or function-block call
- **THEN** it obtains the declaration from the typed resolution record without
  requiring a materialized AST resolution field

### Requirement: Enumeration results remain store-owned after their pass
The semantic pipeline SHALL retain completed scope-specific enumeration lookup
tables in the active compilation context's Analysis Store without materializing
those records back onto AST compatibility fields in production.

#### Scenario: Later passes follow enumeration publication

- **WHEN** enumeration checking completes and publishes its typed records
- **THEN** the pipeline continues without copying the records back to scope AST
  enum tables

#### Scenario: Typed lookup remains available without AST state

- **WHEN** an AST scope compatibility table is empty after enumeration results
  have been published
- **THEN** case-insensitive typed record lookup still returns every matching enum
  declaration

### Requirement: Completed flow and constants are consumed from typed records
Semantic passes after flow analysis and constant folding SHALL obtain completed
flow edges and constant values from the active compilation context's Analysis
Store without requiring production AST materialization.

#### Scenario: Type analysis follows completed flow analysis

- **WHEN** a datatype visitor requests predecessors or successors for an
  arena-owned IL node
- **THEN** it receives the edges published in that node's flow record

#### Scenario: Range and case checks follow constant folding

- **WHEN** range or case validation requests a completed constant value
- **THEN** it receives the value published in the node's constant record even if
  the corresponding AST compatibility value is empty

#### Scenario: A producer uses a transient node

- **WHEN** a semantic or generator producer creates a stack-local node that has
  no typed record
- **THEN** the scoped accessor uses that node's producer-local compatibility
  field
