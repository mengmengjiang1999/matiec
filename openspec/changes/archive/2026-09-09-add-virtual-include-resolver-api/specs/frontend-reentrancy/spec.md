## MODIFIED Requirements

### Requirement: Handwritten frontend entry points name their parser session

Generated include actions SHALL resolve virtual sources through the ParserState
passed to that parser invocation. Resolver configuration and nested source bytes
SHALL NOT use a process-global selector or cross compilation contexts.

#### Scenario: Parallel contexts resolve the same include name

- **WHEN** two contexts concurrently resolve the same requested name to different bytes
- **THEN** each parser consumes only the source returned by its own resolver
