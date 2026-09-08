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
