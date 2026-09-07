## MODIFIED Requirements

### Requirement: Native method generator boundary

Code generators SHALL consume FB-owned method declaration and invocation AST nodes
without losing method structure in normalized IEC or duplicating
compatibility-lowered C behavior.

#### Scenario: IEC is generated from native method nodes

- **WHEN** a function block contains a supported method and source invokes it through an instance
- **THEN** IEC generation emits the declaration beneath its owner and emits the call in `instance.method(arguments)` form

#### Scenario: C is generated with AST compatibility lowering

- **WHEN** the primary AST contains a native method invocation bound to its compatibility function
- **THEN** C generation emits only the deterministic lowered implementation and call path without source-text call rewriting
