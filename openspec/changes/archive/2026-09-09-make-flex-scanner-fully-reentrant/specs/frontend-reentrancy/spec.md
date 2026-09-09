## MODIFIED Requirements

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
