## MODIFIED Requirements

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
