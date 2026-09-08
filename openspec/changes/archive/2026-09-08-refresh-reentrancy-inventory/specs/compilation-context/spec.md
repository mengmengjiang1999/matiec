## ADDED Requirements

### Requirement: Maintained compatibility-boundary inventory

The project SHALL document every remaining mutable process-wide or thread-local
compilation boundary, its current owner, and the milestone that removes it.

#### Scenario: A compatibility boundary changes

- **WHEN** a change removes, splits, or introduces a shared-state compatibility
  boundary
- **THEN** the architecture inventory and reader-facing roadmap are updated in
  the same change
