## ADDED Requirements

### Requirement: Declaration symbol tables are context-owned

Each `CompilationContext` SHALL own the function, function-block, program, and
datatype declaration tables used by its compilation. Legacy declaration lookup
SHALL resolve those tables through the context's parser session and SHALL NOT
maintain a separate thread-local declaration selector or mutable fallback table.

#### Scenario: A compilation initializes declarations

- **WHEN** parsing completes for a context
- **THEN** declaration collection populates only that context's tables

#### Scenario: Parser sessions are nested

- **WHEN** one context's parser session temporarily nests another context's
  parser session
- **THEN** legacy declaration lookup follows the innermost session and restores
  the outer context on exit
