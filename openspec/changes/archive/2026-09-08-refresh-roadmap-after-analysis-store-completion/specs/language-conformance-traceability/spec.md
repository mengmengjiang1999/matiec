## MODIFIED Requirements

### Requirement: Traceable language feature catalogue
The project SHALL maintain stable feature records containing an identifier, language family, implementation status, applicable profile or extension, evidence level, evidence references, tests, and unresolved questions. Reader-facing roadmap summaries SHALL distinguish completed foundations from remaining work and agree with maintained implementation evidence and archived change history.

#### Scenario: An implemented feature is catalogued
- **WHEN** a maintained lexer, parser, semantic, library, or generator feature is listed
- **THEN** its record identifies repository evidence and at least one relevant test location or explicitly records a test gap

#### Scenario: A planned feature is catalogued
- **WHEN** a feature is known but not implemented
- **THEN** its record says `not-implemented` or `planned` rather than implying partial conformance

#### Scenario: A roadmap foundation is completed
- **WHEN** maintained implementation evidence and archived changes show that a roadmap foundation is complete
- **THEN** reader-facing roadmap summaries no longer present that foundation as unfinished work
