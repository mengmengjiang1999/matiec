## ADDED Requirements

### Requirement: Reentrancy architecture audit

The normal regression suite SHALL audit the source-of-truth reentrancy
invariants alongside behavioral concurrent-compilation tests.

#### Scenario: A completed invariant regresses

- **WHEN** pure parser parameters, generated failure isolation, or fallback-free
  compatibility access is removed
- **THEN** the normal test suite fails with the missing invariant
