# Compiler architecture and embedding boundaries

This document describes the refactored compilation boundary. It expands the
architecture overview in `README.md` and is the source of truth for new
compiler integrations.

## Compilation lifetime

Create one `matiec::CompilationContext` for one compilation and invoke it with
`matiec::Compiler::compile()`:

```cpp
matiec::CompilationContext context;
context.set_source_path("program.st");
context.options().include_directory = "lib";

const matiec::CompilationResult result = matiec::Compiler().compile(context);
if (!result.succeeded()) {
  context.diagnostics().render(std::cerr);
}
```

The context owns compiler options, diagnostics, generated-output management,
and the AST arena. Destroying it releases all AST nodes and retained parser
strings from that compilation. Do not retain AST pointers after the context is
destroyed.

Two compilations may run sequentially in one process with separate contexts.
The generated Flex/Bison frontend and legacy symbol-table initialization are
still synchronous compatibility boundaries and are not thread-safe. Their
remaining process-wide state is documented in
`docs/architecture/legacy-global-state-adapters.md`.

## Pipeline

The compiler executes these boundaries in order:

1. `LegacyGlobalStateAdapter::parse()` runs lexical and syntax analysis inside
   the context's active AST arena.
2. Legacy symbol-table initialization prepares declaration lookup.
3. `SemanticPassManager` runs the explicit Stage 3 pass order and stops after a
   failed pass. Pass IDs, prerequisites, and per-pass results are declared in
   `compiler/semantic_pass.hh`.
4. Stage 4 emits C or IEC text through the context-owned `OutputManager`.

Lower layers report through `DiagnosticEngine` and return failures. Only the
CLI adapter in `main.cc` converts the final result to a process exit status.

Semantic results are currently stored as AST annotations. Moving them into a
context-owned analysis store is intentionally deferred and scoped in
`docs/decisions/0002-defer-context-owned-analysis-store.md`.

## Generated output

`OutputSink` is the write/flush error boundary. Production generation uses
`FileOutputSink`; embedding code and tests may inject `MemoryOutputSink` or a
custom sink into `OutputManager`. All failures become diagnostics and cause
generation to return failure.

The C generator's declaration, configuration, ST, IL, inline-call, and SFC
implementations are normal translation units listed in
`stage4/generate_c/Makefile.am`. Shared declarations live in private headers;
implementation `.cc` files must never be included textually.

The generated C ABI is guarded by `tests/generated_c/runtests`. It compiles a
representative configuration, checks the configuration/resource symbols, links
the generated objects with a host driver, and executes one PLC cycle.

## Build and validation

Autotools is authoritative:

```sh
autoreconf --install
./configure
make --jobs=2
make check
```

Handwritten C++ is compiled as GNU++17. CI runs GCC on Linux and Clang on
macOS. `make check` covers focused compiler tests, sequential in-process
compilation, in-memory generation, generated-C ABI and runtime behavior, CLI
behavior, syntax cases, initialization cases, and byte-for-byte characterized
output.

Run the isolated sanitizer suites with:

```sh
make check-asan
make check-ubsan
```

AddressSanitizer includes leak detection. The scripts copy the source to a
temporary directory, regenerate parser sources, build, and execute the same
regression suite without modifying the developer's configured tree.

## Extension rules

- New mutable per-compilation state belongs in `CompilationContext` or one of
  its owned services.
- New semantic work is registered as a pass with an explicit ID, prerequisites,
  and typed result.
- New generators accept an output abstraction; they do not open files directly.
- New AST allocations and retained strings use the context's `AstArena`.
- New lower-layer failures become diagnostics/results, not `exit()` calls.
- Changes to generated output update characterization or ABI expectations only
  when the behavior change is deliberate and separately reviewed.
