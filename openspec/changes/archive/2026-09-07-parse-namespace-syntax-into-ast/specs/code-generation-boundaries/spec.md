## ADDED Requirements

### Requirement: Namespace-aware generator traversal

Code generators SHALL consume native namespace AST wrappers without losing their
contained declarations or changing the provisional lowered C ABI.

#### Scenario: C is generated from a namespace wrapper

- **WHEN** a namespace contains types or POUs and dependency ordering is enabled
- **THEN** every contained declaration reaches all required C generation passes with its deterministic lowered identifier

#### Scenario: IEC is generated from a namespace wrapper

- **WHEN** normalized IEC output is requested
- **THEN** namespace visibility, qualified name, directives, and declaration boundaries are emitted structurally
