## 1. Regression Baseline

- [x] 1.1 Integrate initialization and CLI regression suites with `make check`
- [x] 1.2 Isolate regression outputs in temporary directories and keep the source worktree clean
- [x] 1.3 Modernize and integrate the existing syntax/identifier suite
- [x] 1.4 Compile representative generated C artifacts as part of regression testing
- [x] 1.5 Add stable generated-output characterization fixtures for representative ST, IL, and SFC inputs
- [ ] 1.6 Add sanitizer build and test entry points for address and undefined-behavior checks
- [ ] 1.7 Add GCC/Linux and Clang/macOS CI jobs that bootstrap, build, and run the shared test entry point

## 2. Build Foundation

- [ ] 2.1 Refresh and document the Autotools bootstrap process and supported tool versions
- [ ] 2.2 Remove duplicate source entries and separate handwritten warning policy from generated parser warnings
- [ ] 2.3 Declare the initial GNU++17 compatibility mode and resolve high-confidence handwritten compiler warnings
- [ ] 2.4 Replace Mercurial revision discovery with Git metadata and an archive-safe fallback
- [ ] 2.5 Decide whether a parity-tested CMake frontend belongs in this change and record the outcome

## 3. Compilation Context and Diagnostics

- [ ] 3.1 Add `CompilerOptions`, `SourceLocation`, `Diagnostic`, and `CompilationResult` value types with unit tests
- [ ] 3.2 Add a `DiagnosticEngine` that collects and renders diagnostics without terminating the process
- [ ] 3.3 Add `CompilationContext` and route CLI options through it while preserving current behavior
- [ ] 3.4 Extract a `Compiler` driver API and reduce `main()` to argument adaptation, invocation, rendering, and exit status
- [ ] 3.5 Migrate pre-parser and global symbol-table access behind context-owned services or documented temporary adapters
- [ ] 3.6 Replace deep `exit()` paths in parser, semantic, and generation entry points with structured failure propagation

## 4. AST and Compilation Lifetime

- [ ] 4.1 Inventory AST, token-string, reordered-tree, and visitor allocation ownership in a checked-in migration note
- [ ] 4.2 Introduce a per-compilation `AstArena` with allocation and destruction tests
- [ ] 4.3 Route parser-created AST nodes and owned token strings through the arena without changing node relationships
- [ ] 4.4 Replace stateful singleton visitors with stack or context-owned instances
- [ ] 4.5 Add an in-process sequential-compilation regression covering invalid-then-valid and differing-option cases
- [ ] 4.6 Run sanitizer validation and resolve context-lifetime leaks or invalid accesses introduced by the migration

## 5. Semantic Pass Pipeline

- [ ] 5.1 Define semantic pass identifiers, prerequisites, and structured pass results
- [ ] 5.2 Wrap the existing Stage 3 order in a `SemanticPassManager` without changing individual visitors
- [ ] 5.3 Route Stage 3 diagnostics through `DiagnosticEngine` and remove duplicated diagnostic macros
- [ ] 5.4 Add focused tests for pass order, failure short-circuiting, and independently invocable passes
- [ ] 5.5 Evaluate moving semantic annotations to a context-owned analysis store and either implement it or record a scoped follow-up

## 6. Code-Generation Boundaries

- [ ] 6.1 Introduce filesystem and in-memory output sinks with uniform write and flush error handling
- [ ] 6.2 Route C and IEC generators through context-owned output management
- [ ] 6.3 Extract shared generator declarations into private headers
- [ ] 6.4 Convert declaration, configuration, ST, IL, and SFC generator implementations from textual `.cc` inclusion to normal compilation units in validated batches
- [ ] 6.5 Add in-memory generator tests and compare characterized output after every extraction batch
- [ ] 6.6 Verify generated C ABI and representative compiled behavior remain unchanged

## 7. Completion and Documentation

- [ ] 7.1 Update architecture, build, testing, and embedding documentation for the new boundaries
- [ ] 7.2 Run clean GCC and Clang builds, all regressions, generated-C compilation, sequential compilation, and sanitizer checks
- [ ] 7.3 Confirm no new mutable process-wide compilation state or deep process exits remain outside documented adapters
- [ ] 7.4 Sync the completed delta specifications to main OpenSpec specs and archive the change
- [ ] 7.5 Push each validated workflow-named branch only to the fork's `origin` and verify remote commit hashes
