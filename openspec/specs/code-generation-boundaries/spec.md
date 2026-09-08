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

When an experimental construct has a primary AST representation, IEC generation SHALL preserve that native structure, while C generation MAY consume an explicit
compatibility AST. Compatibility declarations SHALL NOT be injected as parser source
or emitted as duplicate IEC structure.

#### Scenario: Native method structure is emitted as IEC

- **WHEN** a function block contains a supported method and source invokes it through an instance
- **THEN** IEC generation emits the owner-contained method and native call without an appended lowered function declaration

#### Scenario: C is generated from AST compatibility declarations

- **WHEN** the primary AST contains a supported method and bound invocation
- **THEN** C generation emits the deterministic lowered implementation and call path from explicit compatibility AST nodes

### Requirement: Access metadata follows AST analysis

The compiler SHALL generate access metadata only from declarations accepted by the
post-parse AST analysis.

#### Scenario: CSV metadata is emitted

- **WHEN** AST analysis accepts access declarations and C output is requested
- **THEN** `ACCESS.csv` contains the same deterministic rows as the accepted AST nodes

### Requirement: Object compatibility metadata follows AST analysis

The compiler SHALL construct compatibility declarations and bind method calls only
from metadata produced by post-parse analysis of the primary AST.

#### Scenario: Compatibility lowering starts

- **WHEN** the parsed AST contains a supported public method and invocation
- **THEN** AST analysis completes before compatibility declarations or invocations are created

### Requirement: Namespace side metadata mirrors parsed structure

Namespace side metadata SHALL mirror primary AST structure and SHALL NOT be copied
from a pre-parse recognizer.

#### Scenario: Parsing succeeds

- **WHEN** the experimental parser accepts namespace wrappers and imports
- **THEN** the namespace analysis result is derived from those parsed nodes before semantic generation

### Requirement: Profile library declarations are AST-owned

Profile-owned library declarations SHALL enter semantic analysis and generation
as explicit compiler-owned AST nodes. They SHALL NOT be prepended, appended, or
otherwise injected into user source text.

#### Scenario: Experimental ASSERT is compiled

- **WHEN** source invokes the profile-owned `ASSERT` function
- **THEN** generation consumes its AST declaration while diagnostics retain positions in the original source

#### Scenario: Source does not use a profile library function

- **WHEN** experimental source contains no call requiring a profile-owned declaration
- **THEN** no unused compiler-owned declaration is added to the compilation unit

### Requirement: Generators consume resolution records directly

Stage 4 SHALL provide its generators read-only access to the active Analysis Store
and SHALL resolve invocation metadata from typed records.

#### Scenario: C generation emits a resolved function call

- **WHEN** an invocation has a completed resolution record
- **THEN** C generation uses its declaration and extensible parameter count even if
  the corresponding AST compatibility fields are empty

### Requirement: Stage 4 exchanges generator annotations through the context
Stage 4 SHALL write and read named generator symbols through the active
compilation context's Analysis Store and SHALL NOT require mutable AST annotation
maps as a production communication channel.

#### Scenario: Implicit datatype identifier is reused

- **WHEN** a datatype generator assigns an implicit type identifier to an
  arena-owned AST node
- **THEN** a later Stage 4 visitor resolves that identifier from the node's typed
  generator record

#### Scenario: Stage 4 completes successfully

- **WHEN** all generated output is flushed without errors
- **THEN** generator records already contain the live annotations and no AST
  publisher or materializer is run

### Requirement: Generators read completed flow and constants from context
Stage 4 SHALL obtain completed IL flow edges and constant values through the
active compilation analysis boundary rather than depending on materialized AST
analysis fields.

#### Scenario: C generation emits an IL operation

- **WHEN** the generator needs the predecessor datatype for an arena-owned IL
  instruction
- **THEN** it follows the predecessor stored in the typed flow record

#### Scenario: C generation emits a symbolic constant

- **WHEN** the generator visits a symbolic constant with a published constant
  record
- **THEN** it emits the value from that typed record

### Requirement: Generators consume completed datatype records

Stage 4 SHALL obtain selected datatypes and declaration scopes through the active
compilation Analysis Store and SHALL NOT require production materialization of
completed datatype records onto AST fields.

#### Scenario: C generation emits a typed expression

- **WHEN** a generator requests the selected datatype of an arena-owned expression
- **THEN** it receives the datatype published in that expression's typed record

#### Scenario: Generation resolves declaration scope

- **WHEN** a generator utility requests the scope of an arena-owned variable node
- **THEN** it receives the scope published in that node's typed record
