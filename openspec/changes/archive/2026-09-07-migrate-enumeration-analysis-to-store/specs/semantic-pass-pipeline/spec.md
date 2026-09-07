## ADDED Requirements

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
