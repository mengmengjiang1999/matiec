# object-oriented-elements Specification

## Purpose
Define the bounded experimental function-block method subset, its deterministic
lowering ABI, and its isolation from the legacy language profile.
## Requirements
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
remain distinct from compatibility lowering. Compatibility declarations SHALL be
constructed from the primary AST without appending generated source to parser input.

#### Scenario: A method is statically dispatched

- **WHEN** an instance invokes a supported public method
- **THEN** explicit AST compatibility passes construct and bind the deterministic owner-and-method function representation exactly once

#### Scenario: A bounded method updates owner state

- **WHEN** a supported public method reads or writes a field of its owning function block
- **THEN** the compatibility AST passes the affected caller-owned fields by reference and preserves the update

#### Scenario: Method source remains authoritative

- **WHEN** experimental source declares a supported method
- **THEN** parser input retains the user method text and no generated compatibility function text is appended

#### Scenario: Normalized IEC is generated

- **WHEN** `iec2iec` processes a supported method and invocation with dependency ordering enabled
- **THEN** output contains one owner-contained `METHOD` boundary, retains the native invocation, and omits synthetic compatibility declarations

### Requirement: Legacy profile isolation

Method syntax MUST remain unavailable in the legacy profile and method keywords
MUST NOT become reserved there.

#### Scenario: A method declaration is compiled as legacy source

- **WHEN** source containing `METHOD` is compiled with `--std=legacy`
- **THEN** the legacy parser rejects it
