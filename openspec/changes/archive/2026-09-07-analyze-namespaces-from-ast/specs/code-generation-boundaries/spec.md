## ADDED Requirements

### Requirement: Namespace side metadata mirrors parsed structure

Namespace side metadata SHALL mirror primary AST structure and SHALL NOT be copied
from a pre-parse recognizer.

#### Scenario: Parsing succeeds

- **WHEN** the experimental parser accepts namespace wrappers and imports
- **THEN** the namespace analysis result is derived from those parsed nodes before semantic generation
