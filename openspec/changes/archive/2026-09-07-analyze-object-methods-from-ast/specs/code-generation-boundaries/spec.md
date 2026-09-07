## ADDED Requirements

### Requirement: Object compatibility metadata follows AST analysis

The compiler SHALL construct compatibility declarations and bind method calls only
from metadata produced by post-parse analysis of the primary AST.

#### Scenario: Compatibility lowering starts

- **WHEN** the parsed AST contains a supported public method and invocation
- **THEN** AST analysis completes before compatibility declarations or invocations are created
