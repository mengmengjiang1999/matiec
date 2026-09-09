## MODIFIED Requirements

### Requirement: One compatibility session selects context state

The generated frontend SHALL receive parser state explicitly at every parser,
scanner, and handwritten helper boundary. No active-parser selector or implicit
ParserState scope SHALL remain. Legacy bindings outside the frontend MAY expose
only their specifically owned AST, declaration-table, or runtime-option resource.

#### Scenario: A parser helper executes

- **WHEN** Bison or Flex calls a handwritten frontend helper
- **THEN** the invocation's `ParserState` is supplied explicitly

#### Scenario: Source architecture is audited

- **WHEN** the maintained architecture regression scans frontend and compiler code
- **THEN** active parser selectors and implicit ParserState scopes are rejected
