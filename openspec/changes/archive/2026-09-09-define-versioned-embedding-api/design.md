## Context

`Compiler::compile()` is a useful C++ source integration API, but its public
objects expose STL and internal compiler services. Their layouts cannot be the
long-lived cross-toolchain ABI.

## Goals / Non-Goals

**Goals:** establish a C-callable version contract, define compatibility rules,
and keep future implementation details opaque.

**Non-Goals:** compilation entry points, diagnostics, outputs, installation, or
a stability promise for the existing C++ classes.

## Decisions

The public header is valid C11 and C++. API version 1 uses major/minor/patch
macros plus a packed numeric value and runtime query functions. Future ABI-
breaking changes increment the major version; additive compatible changes
increment minor; fixes increment patch. The project package version remains
independent.

Public implementation objects introduced later will use incomplete C structs.
The C++ API remains available for source integrations but outside the stable ABI.

## Risks / Trade-offs

Version 1 begins before compilation operations exist, but doing so makes every
subsequent symbol explicitly subject to the same contract.

## Migration Plan

No existing caller changes. New callers can compile-time and runtime check the
API version.

## Open Questions

None.
