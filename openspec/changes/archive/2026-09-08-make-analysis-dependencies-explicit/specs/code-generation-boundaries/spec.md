## ADDED Requirements

### Requirement: Generators declare analysis dependencies
Stage 4 generator components SHALL receive semantic and generator analysis access explicitly through their construction or helper interfaces.

#### Scenario: A generator resolves a call
- **WHEN** a Stage 4 generator needs a resolved declaration, datatype, constant, flow edge, or generator record
- **THEN** it reads the result through its explicitly supplied analysis dependency
