## Context

Matiec is a roughly 58,000-line C/C++ compiler frontend implemented as a Flex/Bison parser, a macro-generated AST, several semantic visitors, and C/IEC code generators. The current command-line tools work, but compilation state crosses module boundaries through globals and mutable AST annotations; AST allocations are intentionally process-lifetime; semantic pass dependencies are implicit; lower layers may terminate the process; and the C generator is assembled by including implementation files into one translation unit.

The refactor must preserve existing IEC 61131-3 acceptance and generated-code behavior. The current Autotools build remains supported throughout the migration. Each independently useful increment is committed on a workflow-named branch, validated locally and in the Linux build environment, and pushed only to the fork's `origin`.

## Goals / Non-Goals

**Goals:**

- Create a repeatable behavioral baseline before changing architecture.
- Make all mutable compilation state belong to one compilation operation.
- Make diagnostics, pass outcomes, allocations, and output lifetimes explicit.
- Permit two sequential compilations in one process without state leakage.
- Turn semantic analysis and code generation into components with visible interfaces and isolated tests.
- Modernize build and developer tooling without blocking existing users.

**Non-Goals:**

- Reimplement the IEC grammar or change the supported language dialects.
- Change the generated C ABI or standard runtime library behavior intentionally.
- Replace Flex/Bison in this change.
- Convert every visitor or raw pointer in a single commit.
- Remove Autotools before a compatible replacement has equivalent coverage.
- Push changes to the upstream repository or create upstream pull requests.

## Decisions

### 1. Use incremental, test-gated migration

Every architectural step SHALL preserve the regression baseline. Characterization tests are added before moving code whose exact behavior is not already specified. This is preferred over a rewrite because grammar behavior and generated C details are too broad to reproduce safely in one transition.

### 2. Introduce `CompilationContext` as the state boundary

`CompilationContext` will own or reference `CompilerOptions`, `DiagnosticEngine`, source paths, symbol state, AST storage, and output configuration. The CLI becomes a thin adapter. Existing globals will initially delegate to the active context where parser-generator constraints require it, then be removed as signatures migrate.

Alternative considered: immediately make the Flex and Bison outputs fully reentrant. Rejected as the first step because it combines generated-parser migration with every downstream state change and makes regressions difficult to localize.

### 3. Use an arena for AST ownership

AST nodes and parser-owned strings will be allocated from a per-compilation arena and released together. Existing raw pointers remain non-owning links during migration. This avoids introducing shared ownership cycles and minimizes changes to macro-generated node definitions.

Alternative considered: recursively owning `std::unique_ptr` fields. Deferred because shared and reordered node views currently make exclusive tree ownership ambiguous.

### 4. Centralize diagnostics and return results

Only the executable boundary may choose a process exit code. Parser, analysis, and generation layers report through `DiagnosticEngine` and return a typed success/failure result. Source ranges and severity are represented once, replacing duplicated stage macros over time.

### 5. Add a small explicit pass pipeline

Stage 3 will use a pass descriptor and runner that make ordering and prerequisites visible. Existing visitor implementations remain intact initially; the pipeline wraps them before analysis data is separated from AST nodes. This limits simultaneous semantic change.

### 6. Split code generation behind output abstractions

`OutputSink`/`OutputManager` will separate text emission and filesystem policy from visitor logic. Included `.cc` components will receive private headers and become normal compilation units in small batches, with generated-output comparisons guarding each batch.

### 7. Modernize the existing build first

Autotools will gain real test integration, refreshed helper scripts, explicit language standards, sanitizer targets, and Git-based version metadata. A CMake build may be added later as a parallel frontend, but Autotools stays authoritative until both produce and test equivalent binaries.

After stabilizing the Autotools baseline and CI matrix, CMake was explicitly deferred to a separate OpenSpec change. The parity criteria and rationale are recorded in `docs/decisions/0001-defer-cmake-frontend.md`; duplicating rapidly changing compiler source boundaries in this change would increase risk without improving compiler behavior.

## Risks / Trade-offs

- **Behavioral drift in semantic checks or generated C** → Add characterization and golden tests before each affected extraction and compare outputs byte-for-byte where stable.
- **Arena migration hides accidental long-lived references** → Scope the context per compilation and add sequential-compilation plus sanitizer tests.
- **Temporary compatibility adapters preserve some globals** → Track every adapter in OpenSpec tasks and forbid new direct global accesses.
- **Splitting the generator exposes circular dependencies** → Extract private interfaces one component at a time and keep a buildable commit after each extraction.
- **C++17 exposes extension-dependent code** → Start with GNU++17 compatibility, remove extensions separately, and test GCC plus Clang.
- **The complete route spans many commits** → Keep tasks ordered and independently revertible; stop a phase if its acceptance checks fail.

## Migration Plan

1. Land the test harness and CI/build baseline.
2. Introduce context and diagnostics types without changing parser behavior.
3. Route options, symbols, diagnostics, and stage results through the context.
4. Introduce per-compilation AST/output ownership and verify repeated compilation.
5. Wrap Stage 3 visitors in an explicit pass pipeline.
6. Extract code-generation output handling and split included implementation units.
7. Enable modern build modes and remove obsolete compatibility paths only after equivalent validation.

Each phase can be rolled back independently to the preceding passing commit. No persistent data migration is required.

## Open Questions

- Moving semantic annotations completely out of AST nodes may be split into a follow-up change if performance or churn exceeds the risk budget of this change.
