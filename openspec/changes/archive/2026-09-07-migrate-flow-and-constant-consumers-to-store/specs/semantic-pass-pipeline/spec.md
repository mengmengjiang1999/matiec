## MODIFIED Requirements

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

## ADDED Requirements

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
