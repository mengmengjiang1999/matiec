## MODIFIED Requirements

### Requirement: Versioned public embedding ABI

The public C API SHALL allow a context to install an authoritative include
resolver callback whose borrowed, size-tagged source view is copied before the
next resolver call. It SHALL also provide size-tagged resource limits, a
thread-safe cancellation request, and an explicit cancellation reset. Limits
and cancellation state SHALL be owned by one context and SHALL NOT affect
another context or batch member.

#### Scenario: A memory source includes another memory source

- **WHEN** an embedder compiles a memory source whose include is resolved by the callback
- **THEN** the full source graph compiles without materializing source files

#### Scenario: A resolver misses an include

- **WHEN** the installed resolver returns not found
- **THEN** compilation fails with an include diagnostic and does not search disk

#### Scenario: The resolver is cleared

- **WHEN** the host clears the resolver callback
- **THEN** the existing include-directory filesystem behavior is restored

#### Scenario: Host cancels an in-flight compilation

- **WHEN** a host requests cancellation from another thread
- **THEN** compilation stops cooperatively, reports failure, and retains a cancellation diagnostic

#### Scenario: Host reuses a cancelled context

- **WHEN** a host resets cancellation before compiling again
- **THEN** the context can compile normally with its existing source and options
