# ast-lifecycle Specification

## Purpose
TBD - created by archiving change refactor-compiler-architecture. Update Purpose after archive.
## Requirements
### Requirement: Deterministic AST lifetime
All AST nodes and parser-owned source strings created for a compilation SHALL be released when that compilation context is destroyed.

#### Scenario: Compilation context is destroyed
- **WHEN** a successful or failed compilation operation leaves scope
- **THEN** its AST arena releases all allocations owned by that operation

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
