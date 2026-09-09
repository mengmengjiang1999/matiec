# frontend-reentrancy Specification

## Purpose
TBD - created by archiving change make-generated-frontend-reentrant. Update Purpose after archive.
## Requirements
### Requirement: Concurrent generated frontend sessions

The generated scanner and parser SHALL isolate all mutable session state when
independent compilation contexts parse concurrently on separate threads, and
SHALL NOT serialize those sessions through a process-wide lock.

#### Scenario: Distinct sources parse concurrently

- **WHEN** two compilation contexts parse different valid sources at the same
  time on separate threads
- **THEN** both parses succeed and each result contains only its own source's AST
  and parser classification entries

### Requirement: Reproducible reentrant generation

The authoritative build SHALL generate Flex with its reentrant skeleton and
SHALL allocate all generated and handwritten mutable scanner state per parser
session. Generated frontend checks SHALL reject process termination without
rewriting generated storage duration.

#### Scenario: Clean build regenerates the frontend

- **WHEN** generated scanner and parser outputs do not exist before a build
- **THEN** the build creates a reentrant scanner whose mutable state belongs to
  an explicit scanner handle

#### Scenario: A compilation nests on the same thread

- **WHEN** an include resolver synchronously runs another compilation before
  returning the outer include source
- **THEN** both scanner sessions retain their own input, location, include, and
  start-condition state and both compilations complete correctly

#### Scenario: Scanner setup aborts

- **WHEN** cancellation, a resource error, or an include error unwinds parsing
- **THEN** that session's buffers, tracking records, files, and scanner handle
  are released without changing another active session

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

### Requirement: Handwritten frontend entry points name their parser session

Generated include actions SHALL resolve virtual sources through the ParserState
passed to that parser invocation. Resolver configuration and nested source bytes
SHALL NOT use a process-global selector or cross compilation contexts.

#### Scenario: Parallel contexts resolve the same include name

- **WHEN** two contexts concurrently resolve the same requested name to different bytes
- **THEN** each parser consumes only the source returned by its own resolver

### Requirement: Generated parser invocation state is pure

The generated parser SHALL keep lookahead, semantic/location values, and its
internal error counter local to each invocation and SHALL receive the owning
`ParserState` as an explicit parser and lexer parameter.

#### Scenario: Independent parser calls overlap

- **WHEN** separate contexts parse concurrently
- **THEN** their generated parser invocation state cannot alias

#### Scenario: Recovered errors occur

- **WHEN** the grammar recovers from syntax errors
- **THEN** the owning parser session reports the error total after parsing
