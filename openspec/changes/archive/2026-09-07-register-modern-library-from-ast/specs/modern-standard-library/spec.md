## MODIFIED Requirements

### Requirement: Traceable library entries

Every new or changed experimental library function SHALL record its evidence,
signature, resolution rules, runtime implementation, tests, and owning language
profile. A profile-owned declaration SHALL be constructed as compiler-owned AST
rather than injected into user source.

#### Scenario: An experimental assertion is parsed

- **WHEN** the experimental profile parses a unit that invokes `ASSERT` without declaring it
- **THEN** the parser registers the profile-owned name and post-parse registration adds `ASSERT(IN : BOOL) : VOID` as compiler-owned AST

#### Scenario: A function appears in one vendor library

- **WHEN** no independent or official evidence supports it
- **THEN** it is not described as an IEC 61131-3:2025 requirement

### Requirement: Legacy library compatibility

Experimental library additions MUST NOT silently change legacy overload
resolution, reserve names in the legacy parser, or change generated runtime ABI.

#### Scenario: Legacy source is feature-neutral

- **WHEN** it is compiled after an experimental library addition
- **THEN** the selected overload and generated C remain unchanged

#### Scenario: Source declares the same function name

- **WHEN** a unit declares and calls its own expression-valued `ASSERT`
- **THEN** both profiles preserve normal user-function resolution and output without adding a compiler-owned declaration

### Requirement: Experimental assertion contract

The experimental profile SHALL support `ASSERT(IN : BOOL) : VOID` using ordinary
Structured Text function-call grammar, an AST-owned library declaration, and
documented runtime behavior.

#### Scenario: A standalone assertion spans lines

- **WHEN** whitespace, newlines, or block comments separate the call tokens
- **THEN** the ST parser accepts the call and evaluates its BOOL argument

#### Scenario: An assertion is embedded in an expression

- **WHEN** experimental source attempts to consume an assertion result
- **THEN** semantic analysis rejects the expression because `ASSERT` returns `VOID`
