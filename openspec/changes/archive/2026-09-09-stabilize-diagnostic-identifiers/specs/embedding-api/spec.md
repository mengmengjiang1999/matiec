## MODIFIED Requirements

### Requirement: Versioned public embedding ABI

The public C API SHALL retain its context-owned authoritative include resolver,
resource limits, and thread-safe sticky cancellation contract. It SHALL expose
stable diagnostic codes, compiler phases, severity, messages, and explicit
source-range validity. It SHALL append this metadata after the existing
size-tagged diagnostic prefix and SHALL accept a caller that provides the
complete legacy prefix.

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

#### Scenario: An old host retrieves a new diagnostic

- **WHEN** the host supplies the legacy diagnostic structure size
- **THEN** the compiler fills only the supported prefix without overwriting the extension

#### Scenario: An IDE classifies a diagnostic

- **WHEN** a compilation reports a diagnostic
- **THEN** its stable code and phase can be consumed without parsing the English message
