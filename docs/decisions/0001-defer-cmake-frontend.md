# Decision 0001: Defer the CMake frontend

Status: Accepted

## Context

Autotools now bootstraps reproducibly and runs the shared regression suite on
GCC/Linux and Clang/macOS. The remaining compiler refactor will still change
library boundaries, source lists, generated parser handling, and test targets.
Adding CMake now would duplicate each of those changes while providing no new
compiler capability.

## Decision

CMake is out of scope for `refactor-compiler-architecture`. Autotools remains
the authoritative build frontend for this change.

A later OpenSpec change may introduce CMake as a parallel frontend. It must, at
minimum:

* build both `iec2c` and `iec2iec` from a clean checkout;
* regenerate or consume the Flex/Bison sources consistently;
* run the same `make check`-equivalent regression suites;
* preserve Git/archive revision metadata;
* pass GCC/Linux and Clang/macOS CI; and
* produce characterized IEC output and generated C equivalent to Autotools.

Autotools must not be removed until those parity checks are continuously green.

## Consequences

The current refactor has one authoritative source of build truth. CMake work is
smaller and safer after compilation and code-generation boundaries stabilize,
but developers who require CMake must wait for a dedicated follow-up change.
