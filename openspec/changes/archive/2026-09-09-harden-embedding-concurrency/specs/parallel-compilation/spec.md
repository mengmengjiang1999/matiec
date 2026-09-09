## ADDED Requirements

### Requirement: Reused contexts remain isolated under mixed batches

The regression suite SHALL repeatedly compile many reused contexts in bounded
batches containing successful, failed, cancelled, and callback-driven jobs.

#### Scenario: Mixed batch runs repeatedly

- **WHEN** contexts with different outcomes are reused across multiple rounds
- **THEN** every result and callback remains associated with its input context
