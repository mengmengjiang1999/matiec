# code-generation-boundaries Specification

## Purpose
TBD - created by archiving change refactor-compiler-architecture. Update Purpose after archive.
## Requirements
### Requirement: Injectable output destinations
Code generators SHALL emit through an output abstraction that supports filesystem and in-memory destinations with consistent error propagation.

#### Scenario: In-memory generation
- **WHEN** a test invokes a generator with an in-memory output sink
- **THEN** generated text is available for assertions without creating files

#### Scenario: Output write fails
- **WHEN** an output destination rejects a write or flush
- **THEN** generation returns failure and records an actionable diagnostic

### Requirement: Independently compiled generator components
C, IEC, ST, IL, SFC, declaration, and configuration generator components SHALL have explicit interfaces and MUST NOT depend on including implementation `.cc` files.

#### Scenario: A generator component changes
- **WHEN** one independently compiled component is modified
- **THEN** the build resolves dependencies through declarations rather than textual inclusion of its implementation

### Requirement: Generated behavior preservation
Component extraction SHALL preserve generated output for existing characterized inputs unless a separately tested correctness change is specified.

#### Scenario: A generator component is extracted
- **WHEN** regression inputs are compiled before and after extraction
- **THEN** stable generated artifacts match the approved baseline and compile successfully

### Requirement: Namespace-aware generator traversal

Code generators SHALL consume native namespace AST wrappers without losing their
contained declarations or changing the provisional lowered C ABI.

#### Scenario: C is generated from a namespace wrapper

- **WHEN** a namespace contains types or POUs and dependency ordering is enabled
- **THEN** every contained declaration reaches all required C generation passes with its deterministic lowered identifier

#### Scenario: IEC is generated from a namespace wrapper

- **WHEN** normalized IEC output is requested
- **THEN** namespace visibility, qualified name, directives, and declaration boundaries are emitted structurally

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
