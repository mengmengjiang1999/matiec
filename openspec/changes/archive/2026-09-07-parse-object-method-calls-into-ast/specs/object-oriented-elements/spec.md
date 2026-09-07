## MODIFIED Requirements

### Requirement: Bounded object-oriented subset

The project SHALL document the exact enabled OO subset, SHALL represent supported
function-block methods as owner-contained primary AST nodes, SHALL represent
supported instance method invocations as primary AST nodes, and SHALL not imply
support for unimplemented or unverified elements.

#### Scenario: A supported method is parsed

- **WHEN** experimental source declares a supported public method in a function block
- **THEN** the owner AST contains one method node with its visibility, return type, parameters, local variables, and body

#### Scenario: A supported method invocation is parsed

- **WHEN** experimental source invokes a supported public method through a declared function-block instance
- **THEN** the primary AST contains the receiver, method name, arguments, and source range without pre-parser call replacement

#### Scenario: Only methods are implemented in an increment

- **WHEN** classes or interfaces remain incomplete
- **THEN** the catalogue records their actual status independently

#### Scenario: Unsupported dynamic OO syntax is encountered

- **WHEN** experimental source uses inheritance, interfaces, override, or non-public methods
- **THEN** compilation fails or the unsupported construct remains explicitly unimplemented

### Requirement: Deterministic object ABI

Enabled OO constructs SHALL have deterministic ownership, layout, dispatch, and C
generation behavior covered by runtime tests while native method syntax and calls
remain distinct from compatibility lowering.

#### Scenario: A method is statically dispatched

- **WHEN** an instance invokes a supported public method
- **THEN** an explicit AST compatibility pass binds the call and generated C calls the deterministic owner-and-method symbol exactly once

#### Scenario: A bounded method updates owner state

- **WHEN** a supported public method reads or writes a field of its owning function block
- **THEN** generated C passes the affected caller-owned fields by reference and preserves the update

#### Scenario: Normalized IEC is generated

- **WHEN** `iec2iec` processes a supported method and invocation with dependency ordering enabled
- **THEN** the function block structurally contains one `METHOD` boundary and the invocation retains its receiver-and-method form
