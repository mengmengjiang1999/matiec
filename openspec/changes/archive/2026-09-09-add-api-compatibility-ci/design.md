## Context

The public header is intended for C and C++, and the static archive now provides
a supported installation artifact. Existing regression tests validate behavior
but do not make the exported symbol set explicit.

## Goals / Non-Goals

**Goals:** detect removed or unexpected public symbols, validate C11/C++17
signatures, exercise installation, and isolate failures in CI.

**Non-Goals:** platform-specific binary layout diffing, shared-library SONAME
policy, or compatibility with unreleased major versions.

## Decisions

A text allowlist is compared with globally defined `matiec_*` symbols normalized
across GNU and BSD `nm`. Header fixtures bind every function to an expected typed
pointer in C11 and include the same header in C++17.

`make check-api` builds `libmatiec.a`, runs header and symbol checks, then reuses
the installed-consumer smoke test. A dedicated Ubuntu CI job invokes this target
so failures are visible as an API-specific check.

Any intentional additive symbol change updates the allowlist and increments the
minor API version; removal or signature incompatibility requires a major version
change.

## Risks / Trade-offs

Static archives can contain unrelated global legacy symbols, so the allowlist is
deliberately scoped to the documented `matiec_*` namespace.

## Migration Plan

None. The change adds validation only.

## Open Questions

None.
