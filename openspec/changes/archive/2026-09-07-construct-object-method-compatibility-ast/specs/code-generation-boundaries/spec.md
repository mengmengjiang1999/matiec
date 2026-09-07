## MODIFIED Requirements

### Requirement: Native method generator boundary

When an experimental construct has a primary AST representation, IEC generation SHALL preserve that native structure, while C generation MAY consume an explicit
compatibility AST. Compatibility declarations SHALL NOT be injected as parser source
or emitted as duplicate IEC structure.

#### Scenario: Native method structure is emitted as IEC

- **WHEN** a function block contains a supported method and source invokes it through an instance
- **THEN** IEC generation emits the owner-contained method and native call without an appended lowered function declaration

#### Scenario: C is generated from AST compatibility declarations

- **WHEN** the primary AST contains a supported method and bound invocation
- **THEN** C generation emits the deterministic lowered implementation and call path from explicit compatibility AST nodes
