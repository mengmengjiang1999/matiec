## MODIFIED Requirements

### Requirement: Analysis records are typed and stateful
The compiler SHALL store flow, constant, datatype, declaration-resolution,
enumeration, and generator results in distinct typed `AnalysisStore` records
owned by `CompilationContext`, SHALL distinguish absent records from invalid
records, SHALL require producers and consumers to receive an explicit analysis
access object, and MUST NOT discover analysis state through AST fields,
thread-local active stores, or ambient process state.

#### Scenario: A pass records a negative result
- **WHEN** a semantic pass stores an invalid analysis entry for an arena node
- **THEN** consumers observe a present invalid entry rather than an absent entry

#### Scenario: A semantic producer writes a result
- **WHEN** a Stage 3 pass computes a semantic result
- **THEN** it writes through the explicit analysis dependency supplied to that pass

#### Scenario: A semantic consumer reads a result
- **WHEN** a later pass or generator consumes a semantic result
- **THEN** its interface identifies the analysis dependency used for that lookup

#### Scenario: No active analysis store exists
- **WHEN** compilation enters or leaves semantic analysis and generation
- **THEN** no thread-local analysis pointer or active analysis scope is installed
