## ADDED Requirements

### Requirement: Diagnostic metadata identifies its phase

The compilation context SHALL classify diagnostics by API, source, parser,
semantic, or generation phase and SHALL emit a stable generic or specific code.

#### Scenario: Semantic analysis reports a range

- **WHEN** semantic analysis diagnoses source symbols
- **THEN** the diagnostic reports the semantic phase and an explicit valid source range
