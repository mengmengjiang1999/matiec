## ADDED Requirements

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
