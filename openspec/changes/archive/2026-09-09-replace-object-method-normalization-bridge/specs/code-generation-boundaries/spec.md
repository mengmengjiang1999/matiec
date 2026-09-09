## MODIFIED Requirements

### Requirement: Native method generator boundary

When an experimental construct has a primary AST representation, IEC generation SHALL preserve that native structure. C generation SHALL consume semantic bindings
attached to native method nodes and SHALL NOT require compatibility declarations in
the parsed top-level library.

#### Scenario: Native method structure is emitted as IEC

- **WHEN** a function block contains a supported method and source invokes it through an instance
- **THEN** IEC generation emits the owner-contained method and native call without an appended lowered function declaration

#### Scenario: C is generated from native method bindings

- **WHEN** the primary AST contains a supported method and bound invocation
- **THEN** C generation emits the deterministic lowered implementation and call path from bindings owned by the native nodes

### Requirement: Object compatibility metadata follows AST analysis

The compiler SHALL bind native method declarations and calls only from metadata
produced by post-parse analysis of the primary AST, without a separate compatibility
normalization phase.

#### Scenario: Native method binding starts

- **WHEN** the parsed AST contains a supported public method and invocation
- **THEN** AST analysis completes before semantic declaration and call bindings are attached
