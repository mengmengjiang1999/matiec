## MODIFIED Requirements

### Requirement: Bounded object-oriented subset

The project SHALL document the exact enabled OO subset, SHALL represent supported
function-block methods as owner-contained primary AST nodes, and SHALL not imply
support for unimplemented or unverified elements.

#### Scenario: A supported method is parsed

- **WHEN** experimental source declares a supported public method in a function block
- **THEN** the owner AST contains one method node with its visibility, return type, parameters, local variables, and body

#### Scenario: Only methods are implemented in an increment

- **WHEN** classes or interfaces remain incomplete
- **THEN** the catalogue records their actual status independently

#### Scenario: Unsupported dynamic OO syntax is encountered

- **WHEN** experimental source uses inheritance, interfaces, override, or non-public methods
- **THEN** compilation fails or the unsupported construct remains explicitly unimplemented

### Requirement: Deterministic object ABI

Enabled OO constructs SHALL have deterministic ownership, layout, dispatch, and C
generation behavior covered by runtime tests while native method syntax remains
distinct from compatibility lowering.

#### Scenario: A method is statically dispatched

- **WHEN** an instance invokes a supported public method
- **THEN** generated C calls the deterministic owner-and-method symbol exactly once

#### Scenario: A bounded method updates owner state

- **WHEN** a supported public method reads or writes a field of its owning function block
- **THEN** generated C passes the affected caller-owned fields by reference and preserves the update

#### Scenario: Normalized IEC is generated

- **WHEN** `iec2iec` processes a supported method with dependency ordering enabled
- **THEN** the function block structurally contains one `METHOD` boundary with its visibility, declarations, and body

### Requirement: Legacy profile isolation

Method syntax MUST remain unavailable in the legacy profile and method keywords
MUST NOT become reserved there.

#### Scenario: A method declaration is compiled as legacy source

- **WHEN** source containing `METHOD` is compiled with `--std=legacy`
- **THEN** the legacy parser rejects it
