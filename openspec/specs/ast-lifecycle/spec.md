# ast-lifecycle Specification

## Purpose
Define deterministic ownership and teardown for AST nodes, retained source text,
and analysis references across repeated compilations.
## Requirements
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

### Requirement: Explicit pointer ownership
AST and analysis interfaces SHALL distinguish owning storage from non-owning node references; shared ownership MUST NOT be introduced solely to avoid defining lifetimes.

#### Scenario: A visitor stores a node reference
- **WHEN** a visitor records a relationship to an existing AST node
- **THEN** the relationship is non-owning and cannot outlive the compilation context

### Requirement: Repeated compilation safety
The compiler SHALL support at least two sequential compilations in one process without stale symbols, diagnostics, parser state, output state, or analysis annotations leaking between operations.

#### Scenario: A valid source follows an invalid source
- **WHEN** one process compiles an invalid program and then a valid program with fresh contexts
- **THEN** the valid program succeeds as it would in a fresh process

### Requirement: Analysis identity is arena checked

The compiler SHALL reject an analysis-store write when its key or any non-null
symbol reference in its record is not owned by that store's AST arena.

#### Scenario: A record crosses contexts

- **WHEN** a record in one context refers to a node allocated by another context
- **THEN** the write fails without modifying the destination store

### Requirement: AST nodes are free of datatype analysis storage
The structural AST base class SHALL NOT contain candidate datatype vectors, selected datatype pointers, or declaration-scope analysis pointers.

#### Scenario: An AST node is constructed
- **WHEN** parsing or a compiler helper constructs a `symbol_c` subclass
- **THEN** the node carries source, token, parent, and ownership structure but no datatype-analysis storage

#### Scenario: A compilation context is reset
- **WHEN** one context starts a later compilation
- **THEN** both persistent and transient datatype working records from the previous compilation are cleared

### Requirement: AST nodes are free of flow and constant analysis storage

The structural AST base and IL instruction classes SHALL NOT contain constant
lattice values or predecessor and successor analysis vectors.

#### Scenario: An AST node is constructed

- **WHEN** parsing or a compiler helper constructs a `symbol_c` or IL instruction subclass
- **THEN** the node carries structural and source information but no flow-edge or constant-analysis storage

#### Scenario: A compilation context is reset

- **WHEN** one context starts a later compilation
- **THEN** persistent and transient constant records and persistent flow records from the previous compilation are cleared

### Requirement: AST nodes are free of analysis and generator result storage

AST classes SHALL contain structural/source information only and SHALL NOT contain
flow, constant, datatype, resolution, enumeration, or generator result storage.

#### Scenario: Any AST node is constructed

- **WHEN** parsing or a compiler helper constructs a `symbol_c` subclass
- **THEN** all semantic and generator results reside outside the node in compilation context records

#### Scenario: A compilation context is reset

- **WHEN** one context starts a later compilation
- **THEN** persistent and transient resolution/enumeration records from the previous compilation are cleared
