## MODIFIED Requirements

### Requirement: Memory source is replayable

The source manager and parser session SHALL support replaying an exact-byte
memory-backed primary source and memory-backed included sources.
Callback-provided bytes and display names SHALL be copied into context-owned
storage before another resolver invocation, including for nested includes.

#### Scenario: Borrowed include bytes expire

- **WHEN** a resolver returns a source view backed by temporary host storage
- **THEN** parsing remains valid after the callback returns because the compiler copied it
