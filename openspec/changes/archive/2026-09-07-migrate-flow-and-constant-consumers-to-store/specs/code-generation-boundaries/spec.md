## ADDED Requirements

### Requirement: Generators read completed flow and constants from context
Stage 4 SHALL obtain completed IL flow edges and constant values through the
active compilation analysis boundary rather than depending on materialized AST
analysis fields.

#### Scenario: C generation emits an IL operation

- **WHEN** the generator needs the predecessor datatype for an arena-owned IL
  instruction
- **THEN** it follows the predecessor stored in the typed flow record

#### Scenario: C generation emits a symbolic constant

- **WHEN** the generator visits a symbolic constant with a published constant
  record
- **THEN** it emits the value from that typed record
