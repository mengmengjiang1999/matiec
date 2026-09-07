## ADDED Requirements

### Requirement: Access metadata follows AST analysis

The compiler SHALL generate access metadata only from declarations accepted by the
post-parse AST analysis.

#### Scenario: CSV metadata is emitted

- **WHEN** AST analysis accepts access declarations and C output is requested
- **THEN** `ACCESS.csv` contains the same deterministic rows as the accepted AST nodes
