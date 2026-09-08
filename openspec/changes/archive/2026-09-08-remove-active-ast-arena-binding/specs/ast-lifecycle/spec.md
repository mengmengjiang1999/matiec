## MODIFIED Requirements

### Requirement: Deterministic AST lifetime

All AST nodes and parser-owned source strings created for a compilation SHALL be
owned by that compilation's AST arena and released when its context is
destroyed. Ownership SHALL be established by an explicitly named `AstArena` or
by the context-owned parser session; a separate active-arena binding SHALL NOT
be required.

#### Scenario: Compilation context is destroyed

- **WHEN** a successful or failed compilation operation leaves scope
- **THEN** its AST arena releases all allocations owned by that operation

#### Scenario: A component constructs through an arena

- **WHEN** a component calls `AstArena::make()` without an active parser session
- **THEN** the returned AST node is owned by the explicitly named arena

#### Scenario: The legacy grammar constructs a node

- **WHEN** a parser action constructs an AST node through its legacy direct-new
  path
- **THEN** the active context-owned parser session registers the node with its
  own arena
