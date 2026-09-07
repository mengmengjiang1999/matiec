## ADDED Requirements

### Requirement: Context-owned analysis storage

Each compilation context SHALL own an independent typed analysis store and SHALL
clear that store before a new compilation begins.

#### Scenario: Context is reused

- **WHEN** the same context begins a second compilation
- **THEN** no analysis record from the previous compilation is observable

#### Scenario: Contexts coexist

- **WHEN** two contexts analyze separate AST arenas
- **THEN** records written through one context are absent from the other
