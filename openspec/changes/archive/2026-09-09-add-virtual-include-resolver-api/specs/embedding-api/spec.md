## MODIFIED Requirements

### Requirement: Versioned public embedding ABI

The public C API SHALL allow a context to install an include resolver callback.
For each include pragma, the callback SHALL return a borrowed, size-tagged source
view that remains valid until the next resolver call and which the compiler
copies before making that call. A configured resolver SHALL be authoritative
and SHALL NOT fall back to filesystem lookup on misses or errors; filesystem
lookup SHALL require an explicit callback result.

#### Scenario: A memory source includes another memory source

- **WHEN** an embedder compiles a memory source whose include is resolved by the callback
- **THEN** the full source graph compiles without materializing source files

#### Scenario: A resolver misses an include

- **WHEN** the installed resolver returns not found
- **THEN** compilation fails with an include diagnostic and does not search disk

#### Scenario: The resolver is cleared

- **WHEN** the host clears the resolver callback
- **THEN** the existing include-directory filesystem behavior is restored
