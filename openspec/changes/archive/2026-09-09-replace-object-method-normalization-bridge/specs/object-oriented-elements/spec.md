## MODIFIED Requirements

### Requirement: Deterministic object ABI

Enabled OO constructs SHALL have deterministic ownership, layout, static dispatch,
and C generation behavior covered by runtime tests. Semantic callable views SHALL
be attached to native method declarations and invocations without appending
synthetic declarations to the parsed library or preprocessing method source.

#### Scenario: A method is statically dispatched

- **WHEN** an instance invokes a supported public method
- **THEN** AST analysis and native binding resolve the deterministic owner-and-method callable representation exactly once

#### Scenario: A bounded method updates owner state

- **WHEN** a supported public method reads or writes a field of its owning function block
- **THEN** native binding passes the affected caller-owned fields by reference and preserves the update

#### Scenario: A nested method call is compiled

- **WHEN** one supported method invocation is nested inside another call expression
- **THEN** each native invocation is independently bound and generated C preserves evaluation and owner-state updates

#### Scenario: A method declaration is included virtually

- **WHEN** an FB method declaration is provided by the include resolver and invoked by the entry source
- **THEN** native binding finds the declaration without filesystem materialization

#### Scenario: Method source remains authoritative

- **WHEN** experimental source declares a supported method
- **THEN** parser input retains the user method text and no generated function text or top-level declaration is appended

#### Scenario: Normalized IEC is generated

- **WHEN** `iec2iec` processes a supported method and invocation with dependency ordering enabled
- **THEN** output contains one owner-contained `METHOD` boundary, retains the native invocation, and omits semantic callable views

#### Scenario: Method metadata is analyzed

- **WHEN** the primary AST contains a public FB method and a declared receiver
- **THEN** post-parse analysis records method, owner-field, local-shadowing, and receiver metadata with source ranges
