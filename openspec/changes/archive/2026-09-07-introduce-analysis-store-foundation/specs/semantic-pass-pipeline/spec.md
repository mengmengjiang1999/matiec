## ADDED Requirements

### Requirement: Analysis records are typed and stateful

The analysis store SHALL provide distinct record types for flow, constants,
datatypes, declaration resolution, enumerations, and generator metadata, and
SHALL distinguish absent records from invalid records.

#### Scenario: A pass records a negative result

- **WHEN** a semantic pass stores an invalid analysis entry for an arena node
- **THEN** consumers observe a present invalid entry rather than an absent entry
