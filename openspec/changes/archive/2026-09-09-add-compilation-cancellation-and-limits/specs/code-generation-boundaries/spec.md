## ADDED Requirements

### Requirement: Generated output can be bounded

The output manager SHALL enforce an aggregate per-compilation output byte limit
across generated files before delivering bytes to file or callback sinks.

#### Scenario: A write exceeds the remaining output budget

- **WHEN** generated output would exceed the configured byte limit
- **THEN** the write is rejected and compilation reports an output-limit diagnostic
