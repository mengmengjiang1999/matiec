## ADDED Requirements

### Requirement: Bounded object-oriented subset
The project SHALL document the exact enabled OO subset and SHALL not imply support
for unimplemented or unverified elements.

#### Scenario: Only methods are implemented in an increment
- **WHEN** classes or interfaces remain incomplete
- **THEN** the catalogue records their actual status independently

#### Scenario: Unsupported dynamic OO syntax is encountered
- **WHEN** experimental source uses inheritance, interfaces, override, or non-public methods
- **THEN** compilation fails or the unsupported construct remains explicitly unimplemented

### Requirement: Deterministic object ABI
Enabled OO constructs SHALL have deterministic ownership, layout, dispatch, and C
generation behavior covered by runtime tests.

#### Scenario: A method is overridden
- **WHEN** an instance is dispatched through a supported base or interface type
- **THEN** the documented target executes consistently in generated C

#### Scenario: A bounded method updates owner state
- **WHEN** a supported public method reads or writes a field of its owning function block
- **THEN** generated C passes the caller-owned instance by reference and preserves the update

### Requirement: Legacy profile isolation
Method syntax MUST remain unavailable in the legacy profile.

#### Scenario: A method declaration is compiled as legacy source
- **WHEN** source containing `METHOD` is compiled with `--std=legacy`
- **THEN** the legacy parser rejects it
