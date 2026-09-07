## ADDED Requirements

### Requirement: Constant results are context-owned

After constant propagation completes, the compiler SHALL publish each analyzed
arena node's complete constant lattice value into the active context's typed
analysis store before dependent passes execute.

#### Scenario: A literal is folded

- **WHEN** constant propagation evaluates an integer literal
- **THEN** the store contains a valid constant record with the evaluated integer value

#### Scenario: A value is not constant

- **WHEN** propagation determines that an expression is non-constant or overflowed
- **THEN** its stored scalar status preserves that result without converting it to an absent record
