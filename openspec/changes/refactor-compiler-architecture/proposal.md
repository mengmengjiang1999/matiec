## Why

Matiec's compiler stages are functionally separated but remain coupled through global mutable state, process-wide singletons, raw AST ownership, deep `exit()` calls, and a monolithic code generator. The project needs a behavior-preserving, test-gated refactor so it can support safer maintenance, repeatable builds, multiple compilations in one process, and future embedding without destabilizing IEC 61131-3 semantics.

## What Changes

- Establish automated regression, CLI, generated-code, sanitizer, and CI baselines before structural changes.
- Introduce a per-compilation context for options, diagnostics, source information, symbol tables, AST allocation, and outputs.
- Give AST allocations and generated output streams explicit RAII lifetimes while retaining non-owning node links where appropriate.
- Replace deep process termination and duplicated diagnostic macros with structured diagnostics and phase results.
- Make semantic pass ordering and dependencies explicit through a pass pipeline.
- Split the C code generator's included implementation files into independently compiled components behind stable interfaces.
- Modernize and document the build without immediately removing the existing Autotools entry point.
- Preserve accepted input, diagnostics intent, and generated-code behavior unless a task explicitly identifies and tests a correctness fix.

## Capabilities

### New Capabilities

- `regression-validation`: Reproducible automated validation for parsing, semantic analysis, CLI behavior, generated output, sanitizers, and supported build environments.
- `compilation-context`: A per-compilation state boundary that owns compiler options, diagnostics, source state, symbol state, and output configuration.
- `ast-lifecycle`: Deterministic AST and temporary-analysis lifetime management suitable for repeated compilation in one process.
- `semantic-pass-pipeline`: Explicit semantic pass contracts, ordering, results, and shared diagnostic behavior.
- `code-generation-boundaries`: Independently compiled code-generation components with injectable output sinks and reliable error propagation.
- `build-modernization`: Reproducible modern builds, version metadata, developer tooling, and compatibility with the existing Autotools workflow.

### Modified Capabilities

None. No existing OpenSpec capabilities are present, and this refactor is intended to preserve compiler-facing behavior.

## Impact

- Affects the CLI driver, lexer/parser integration, AST infrastructure, semantic analysis passes, C and IEC generators, test harnesses, and build configuration.
- Adds internal APIs and development tooling; it does not intentionally change the IEC input language or generated C ABI.
- Work will be delivered as small, ordered commits and review branches. Each increment must pass the established regression baseline before it is pushed to the fork's `origin`.
- The upstream repository will not be pushed to and no upstream pull request will be created as part of this change.
