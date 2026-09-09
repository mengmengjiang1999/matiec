# compilation-context Specification

## Purpose
Define the per-compilation ownership boundary, compiler API responsibilities,
and controlled migration away from legacy process-wide state.
## Requirements
### Requirement: Per-compilation state

Each compilation SHALL use an explicit context that contains compiler options,
diagnostics, source identity and optional source bytes, structured experimental
syntax metadata, declaration symbol tables, AST storage, analysis records, and
output configuration for that compilation. Derived phase permissions SHALL NOT
mutate caller-provided options.

#### Scenario: A context is reused across profiles

- **WHEN** an experimental compilation derives permission for a built-in `VOID`
  function and the same context is then used for legacy source
- **THEN** the configured option remains unchanged and legacy parsing does not
  inherit the derived permission

#### Scenario: Two contexts recognize experimental declarations

- **WHEN** separate contexts compile sources containing different declarations
- **THEN** each context exposes only the declaration metadata and symbol-table
  entries from its own source

### Requirement: Thin executable boundary

The command-line executable SHALL translate arguments into compiler options,
invoke the compiler API, render diagnostics, and choose the final process status
without implementing compiler phases itself. Handwritten and generated lower
phases SHALL return or unwind failures to the compiler boundary and SHALL NOT
terminate the host process.

#### Scenario: Compilation fails in a lower phase

- **WHEN** parsing, semantic analysis, generation, or a generated scanner fatal
  path reports failure
- **THEN** the compiler returns a failed compilation result and the CLI or
  embedding host retains control

### Requirement: No new process-wide compilation state
New compiler functionality MUST NOT introduce mutable process-wide state outside an explicitly documented compatibility adapter.

#### Scenario: A component needs shared compilation data
- **WHEN** a compiler component requires options, symbols, diagnostics, or output configuration
- **THEN** it receives that data through the compilation context or a context-owned service

### Requirement: Context-owned analysis storage

Each compilation context SHALL own an independent typed analysis store and SHALL
clear that store before a new compilation begins.

#### Scenario: Context is reused

- **WHEN** the same context begins a second compilation
- **THEN** no analysis record from the previous compilation is observable

#### Scenario: Contexts coexist

- **WHEN** two contexts analyze separate AST arenas
- **THEN** records written through one context are absent from the other

### Requirement: Flow results follow compilation lifetime

Stored flow records SHALL belong only to the compilation that produced them.

#### Scenario: A context compiles a non-IL unit after an IL unit

- **WHEN** the second compilation begins on the reused context
- **THEN** flow records from the first compilation are cleared

### Requirement: Constant results follow compilation lifetime

Published constant records SHALL be cleared before a reused context starts its
next compilation.

#### Scenario: Context is reused after constant propagation

- **WHEN** a new compilation begins on that context
- **THEN** constant records produced for the prior AST are no longer observable

### Requirement: Parser state is context-owned

Each compilation context SHALL own its parser runtime options, transient lexer
transition controls, parser classification tables, and a non-owning reference to
its AST arena for legacy parser allocation. It SHALL reset transient controls and
classification entries before parsing.

#### Scenario: Parser contexts coexist

- **WHEN** two contexts configure different parser options and parse different
  declarations
- **THEN** each context retains only its own option values, transition controls,
  classification entries, and AST allocations

#### Scenario: Parser adapter scopes nest

- **WHEN** an inner context temporarily becomes active
- **THEN** leaving its scope restores the previously active context state and
  allocation target

### Requirement: Datatype candidates follow compilation lifetime

Published datatype candidate records SHALL belong only to the context and AST arena
that produced them.

#### Scenario: A context is reused after candidate analysis

- **WHEN** a new compilation begins on that context
- **THEN** datatype records produced for the previous AST are no longer observable

#### Scenario: A candidate crosses arenas

- **WHEN** a datatype record references a candidate owned by another context
- **THEN** the store rejects the write without modifying its datatype table

#### Scenario: A compiler canonical datatype has no arena owner

- **WHEN** a datatype record references an immutable canonical datatype shared by compilations
- **THEN** the store accepts that reference while retaining ownership validation for context-owned nodes

### Requirement: Datatype selections follow compilation lifetime

Published selected datatype and scope records SHALL belong to the context and AST
arena that produced them, except for immutable compiler canonical datatypes.

#### Scenario: A context begins another compilation

- **WHEN** context analysis state is reset
- **THEN** selected datatype and scope records from the prior AST are absent

#### Scenario: A scope crosses arenas

- **WHEN** a datatype record references a scope owned by another context
- **THEN** the store rejects the write without replacing the prior record

### Requirement: Resolution records follow compilation lifetime

Published resolution records SHALL contain only declarations owned by the current
arena or immutable compiler declarations with no arena owner.

#### Scenario: A declaration crosses arenas

- **WHEN** a resolution record references a declaration owned by another context
- **THEN** the store rejects the write without replacing an existing record

#### Scenario: Analysis state is reset

- **WHEN** the context begins another compilation
- **THEN** resolution records from the prior AST are absent

### Requirement: Enumeration records follow compilation lifetime

Published enumeration records SHALL contain only declarations owned by the current
arena or immutable compiler declarations with no arena owner.

#### Scenario: An enum declaration crosses arenas

- **WHEN** an enumeration record references a declaration owned by another context
- **THEN** the store rejects the write without replacing an existing record

#### Scenario: Analysis state is reset

- **WHEN** the context begins another compilation
- **THEN** enumeration records from the prior AST are absent

### Requirement: Generator records follow compilation lifetime

Published generator records SHALL contain only symbols owned by the current arena
or immutable compiler symbols with no arena owner.

#### Scenario: A generator symbol crosses arenas

- **WHEN** a generator record references a symbol owned by another context
- **THEN** the store rejects the write without replacing an existing record

#### Scenario: Analysis state is reset

- **WHEN** the context begins another compilation
- **THEN** generator records from the prior AST are absent

### Requirement: Compilation context exposes explicit analysis access
`CompilationContext` SHALL own the analysis store and SHALL provide the explicit dependency passed to semantic passes and generators without installing ambient analysis state.

#### Scenario: Two compilations execute sequentially
- **WHEN** separate compilation contexts execute on one thread
- **THEN** each pass and generator accesses only the store supplied by its own context

### Requirement: Maintained compatibility-boundary inventory

The project SHALL document every remaining mutable process-wide or thread-local
compilation boundary, its current owner, and the milestone that removes it.

#### Scenario: A compatibility boundary changes

- **WHEN** a change removes, splits, or introduces a shared-state compatibility
  boundary
- **THEN** the architecture inventory and reader-facing roadmap are updated in
  the same change

### Requirement: Declaration symbol tables are context-owned

Each `CompilationContext` SHALL own the function, function-block, program, and
datatype declaration tables used by its compilation. Legacy declaration lookup
SHALL resolve those tables through the context's parser session and SHALL NOT
maintain a separate thread-local declaration selector or mutable fallback table.

#### Scenario: A compilation initializes declarations

- **WHEN** parsing completes for a context
- **THEN** declaration collection populates only that context's tables

#### Scenario: Parser sessions are nested

- **WHEN** one context's parser session temporarily nests another context's
  parser session
- **THEN** legacy declaration lookup follows the innermost session and restores
  the outer context on exit

### Requirement: Compilation work is cooperatively cancellable

The compilation context SHALL retain an atomic cancellation request and the
compiler SHALL observe it during source scanning and between major stages.

#### Scenario: Cancellation is requested before parsing

- **WHEN** a context has a pending cancellation request
- **THEN** compilation fails before producing output

### Requirement: Resource limits are context-local

The compilation context SHALL own independent source-byte, diagnostic-count,
and output-byte limits, with zero representing unlimited.

#### Scenario: Parallel contexts use different budgets

- **WHEN** batch members have different resource limits
- **THEN** exhausting one member's budget does not change another member's result

### Requirement: Diagnostic metadata identifies its phase

The compilation context SHALL classify diagnostics by API, source, parser,
semantic, or generation phase and SHALL emit a stable generic or specific code.

#### Scenario: Semantic analysis reports a range

- **WHEN** semantic analysis diagnoses source symbols
- **THEN** the diagnostic reports the semantic phase and an explicit valid source range
