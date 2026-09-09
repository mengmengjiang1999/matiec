## Why

Parser and embedding boundaries accept highly varied or simply malformed text,
but deterministic regressions cannot explore their combinatorial state space.
Bounded fuzz gates provide repeatable unexpected-termination detection without turning ordinary CI
into an unbounded job.

## What Changes

- Add one production-API harness covering file, memory, virtual-include,
  cancellation, and resource-limit modes.
- Support both libFuzzer and AFL-style file/stdin execution from that harness.
- Add maintained seed corpora and isolated build/run scripts.
- Run a short libFuzzer smoke gate on pushes and pull requests and a longer
  scheduled campaign with retained failure samples.
- Document local workflows and corpus hygiene.

## Capabilities

### New Capabilities

- None.

### Modified Capabilities

- `regression-validation`: Add bounded parser fuzzing, corpus maintenance, and
  hosted smoke/periodic execution requirements.

## Impact

This affects test-only harnesses and scripts, Automake distribution metadata,
GitHub Actions, and testing documentation. It does not change the public ABI or
introduce runtime dependencies for normal compiler builds.
