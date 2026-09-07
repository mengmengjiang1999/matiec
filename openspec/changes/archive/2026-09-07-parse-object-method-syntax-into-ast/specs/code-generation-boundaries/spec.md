## ADDED Requirements

### Requirement: Native method generator boundary

Code generators SHALL consume FB-owned method AST nodes without losing method
structure in normalized IEC or duplicating compatibility-lowered C behavior.

#### Scenario: IEC is generated from a native method node

- **WHEN** a function block contains a supported method node
- **THEN** IEC generation emits its visibility, name, return type, declarations, body, and boundaries beneath the owner

#### Scenario: C is generated with compatibility lowering

- **WHEN** the primary AST contains a native method and its compatibility-lowered function
- **THEN** C generation emits only the deterministic lowered implementation and call path
