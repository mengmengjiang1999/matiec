## ADDED Requirements

### Requirement: Flow results follow compilation lifetime

Stored flow records SHALL belong only to the compilation that produced them.

#### Scenario: A context compiles a non-IL unit after an IL unit

- **WHEN** the second compilation begins on the reused context
- **THEN** flow records from the first compilation are cleared
