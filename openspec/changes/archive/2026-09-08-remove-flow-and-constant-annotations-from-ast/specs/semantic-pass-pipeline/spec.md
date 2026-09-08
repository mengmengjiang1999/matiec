## MODIFIED Requirements

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
whole-tree APIs that copy those completed records onto AST compatibility fields.

#### Scenario: A focused record test verifies completed analysis

- **WHEN** a test completes producer updates for a node
- **THEN** it reads the result directly from its typed record without invoking a materializer

#### Scenario: A compiler component needs completed analysis

- **WHEN** a production or test component consumes a completed record family
- **THEN** no public materialization declaration is available as an alternate result path
