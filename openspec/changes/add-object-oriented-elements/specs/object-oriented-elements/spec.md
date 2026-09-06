## ADDED Requirements

### Requirement: Bounded object-oriented subset
The project SHALL document the exact enabled OO subset and SHALL not imply support
for unimplemented or unverified elements.

#### Scenario: Only methods are implemented in an increment
- **WHEN** classes or interfaces remain incomplete
- **THEN** the catalogue records their actual status independently

### Requirement: Deterministic object ABI
Enabled OO constructs SHALL have deterministic ownership, layout, dispatch, and C
generation behavior covered by runtime tests.

#### Scenario: A method is overridden
- **WHEN** an instance is dispatched through a supported base or interface type
- **THEN** the documented target executes consistently in generated C
