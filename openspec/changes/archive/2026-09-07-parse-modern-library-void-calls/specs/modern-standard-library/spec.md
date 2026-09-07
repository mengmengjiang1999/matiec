## MODIFIED Requirements

### Requirement: Experimental assertion contract

The experimental profile SHALL support `ASSERT(IN : BOOL) : VOID` using ordinary
Structured Text function-call grammar and documented runtime behavior.

#### Scenario: A standalone assertion spans lines

- **WHEN** whitespace, newlines, or block comments separate the call tokens
- **THEN** the ST parser accepts the call and evaluates its BOOL argument

#### Scenario: An assertion is embedded in an expression

- **WHEN** experimental source attempts to consume an assertion result
- **THEN** semantic analysis rejects the expression because `ASSERT` returns `VOID`
