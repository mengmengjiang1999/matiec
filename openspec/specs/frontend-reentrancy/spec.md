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

### Requirement: Reproducible thread-local generation

The authoritative build SHALL apply and verify the frontend state-isolation
transformation whenever Flex or Bison sources are regenerated.

#### Scenario: Clean build regenerates the frontend

- **WHEN** generated scanner and parser outputs do not exist before a build
- **THEN** the build regenerates them and compiles only after all mutable session
  declarations have been made thread-local

### Requirement: One compatibility session selects context state

The legacy compatibility boundary SHALL use the selected context-owned parser
session for parser state, AST allocation, and declaration lookup without a
second independently nested binding or mutable fallback session. Required
compatibility access without an active session SHALL fail explicitly.

#### Scenario: A supported compilation enters legacy code

- **WHEN** `Compiler::compile()` enters the legacy frontend and downstream
  visitors
- **THEN** all compatibility concerns resolve to the same compilation context

#### Scenario: Required compatibility access is out of scope

- **WHEN** a legacy helper requests required parser state without an active
  parser session
- **THEN** the request fails instead of reading or mutating fallback state

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
