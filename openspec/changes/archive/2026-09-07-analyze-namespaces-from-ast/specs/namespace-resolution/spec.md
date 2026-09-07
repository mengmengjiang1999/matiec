## ADDED Requirements

### Requirement: Namespace structure follows post-parse AST analysis

The context-owned namespace model SHALL be populated only from accepted primary
AST namespace and USING nodes after parsing.

#### Scenario: Namespace metadata is recorded

- **WHEN** experimental source contains a valid namespace with visibility and imports
- **THEN** post-parse analysis records the namespace, visibility, imports, and source ranges from AST nodes

#### Scenario: A context is reused

- **WHEN** a context compiles namespaced source and then source without namespaces
- **THEN** no namespace metadata from the first compilation remains
