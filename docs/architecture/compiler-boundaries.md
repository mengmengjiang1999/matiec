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
the source manager, and the AST arena. `set_source_path()` preserves CLI file
behavior, while `set_source(display_name, bytes)` supplies owned source text
without a named temporary file. Experimental lowering reaches the legacy parser
through a replayable anonymous stream; include pragmas remain path-based.
Destroying the context releases all AST nodes and retained parser
strings from that compilation. Do not retain AST pointers after the context is
destroyed.

Two compilations may run sequentially or concurrently in one process with
separate contexts. The generated Flex/Bison frontend parses independent contexts
concurrently on separate threads: its mutable session data is thread-local and
its classification tables are context-owned. Parser sessions carry their
context-owned AST arenas, so direct parser and synthetic-node construction also
remains isolated without a separate active-arena binding.

`Compiler::compile_parallel()` is the supported full-pipeline batch entry point.
It accepts caller-owned context references, bounds the short-lived worker pool,
and returns one result per context in input order. Each context must be unique in
the batch and use an independent output directory; duplicate context positions
are rejected before workers start. A failure in one job does not stop accepted
jobs. A zero concurrency limit selects a positive worker count automatically.
Unexpected worker exceptions are rethrown only after every worker is joined.

## Pipeline

The compiler executes these boundaries in order:

1. `LegacyGlobalStateAdapter::parse()` runs lexical and syntax analysis in the
   context's parser session, which carries the owning AST arena.
2. Experimental AST analysis and compatibility passes validate native constructs
   that still use legacy semantic implementations. Access-variable metadata is
   collected here, function-block method calls are resolved, and profile-owned
   library declarations are registered as AST without source-text injection.
3. Legacy symbol-table initialization prepares declaration lookup.
4. `SemanticPassManager` runs the explicit Stage 3 pass order and stops after a
   failed pass. Pass IDs, prerequisites, and per-pass results are declared in
   `compiler/semantic_pass.hh`.
5. Stage 4 emits C or IEC text through the context-owned `OutputManager`.

Lower layers report through `DiagnosticEngine` and return failures. Only the
CLI adapter in `main.cc` converts the final result to a process exit status.

`CompilationContext::analysis()` is the typed, arena-checked semantic result
boundary. Flow edges, constant values, and datatype working records are produced
directly there. Stage 3 visitors, datatype helpers, and Stage 4 generators receive
the store explicitly through constructors and helper signatures. Legacy-shaped
node accessors also require the store as an argument, so no active-store guard or
thread-local analysis pointer is needed. Non-arena helper nodes use context-owned
transient flow, constant, or datatype records; an absent flow record exposes
empty predecessor and successor sequences.
Candidate filling updates typed vectors used by narrowing; narrowing updates
selected datatypes and scopes used by later semantic checks and Stage 4. The AST
base class contains none of those datatype-analysis fields or constant values,
and IL instruction nodes contain no flow-edge vectors. Transient tables are
cleared with the store. Invocation declaration resolution and scope-specific
enumeration multimaps are updated live by their producers, and named generator
annotations are updated live during Stage 4. All typed record families
now have production boundaries and none uses a production compatibility
materializer. Flow, constants, datatypes, and invocation resolution have crossed
that boundary: downstream semantic checks and Stage 4 use typed store lookups, while the
producer's temporary AST fields remain confined to type-safety processing.
Enumeration lookup tables also remain store-owned after their completed pass,
with no production materialization because downstream passes do not read the AST
compatibility tables. Generator visitors exchange implicit-type identifiers
through typed records and do not publish or materialize AST annotation maps.
No whole-tree compatibility materializer remains in the compiler or its tests;
focused tests verify record authority directly.
The result-store and explicit-dependency migrations are complete. All
semantic/generator result fields have left the AST, and analysis state is reached
only through the store supplied by the current compilation context.

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

AddressSanitizer includes leak detection where the compiler runtime supports it.
Apple Clang does not provide LeakSanitizer, so the macOS local target disables
leak detection while retaining address checks; Linux keeps leak detection on.
The scripts copy the source to a temporary directory, regenerate parser sources,
build, and execute the same regression suite without modifying the developer's
configured tree. On failure they print both top-level and compiler-unit logs.
GitHub Actions runs ASan/LSan and UBSan as separate Linux jobs on pushes and pull
requests; both jobs can also be started manually.

## Experimental syntax model

`CompilationContext::experimental_syntax()` owns the structured declarations
recognized by the provisional namespace, function-block method, access-variable,
and modern-library frontends. The compiler clears this model before every compile
and populates it only after all experimental recognizers succeed, so a reused
context cannot expose metadata from an earlier source.

These records are a migration boundary, not a second semantic tree. Namespace and
function-block method declarations and invocations now enter the primary AST. An
explicit post-parse AST analysis derives method, field, and receiver metadata before
compatibility passes construct function declarations and bind native calls; method
source is not rescanned, appended, or rewritten before parsing.
Namespace structure and metadata now come from post-parse AST analysis; a bounded
pre-parse spelling bridge remains for legacy lexer symbol classification. Modern
library names use a narrow profile-aware lexer registration bridge; use and
shadowing are determined from parsed nodes, and compiler-owned declarations are
then added explicitly to the AST. Consumers must not treat the side model as
structural authority, rescan original source, or introduce process-wide caches.

Access-variable paths are structural primary-AST nodes: the context model retains
ordered field/subscript selector metadata, while the post-parse resolver walks
configuration, resource, program-output, structure, and array declarations before
publishing the canonical path or `ACCESS.csv` row.

## Extension rules

- New mutable per-compilation state belongs in `CompilationContext` or one of
  its owned services.
- New semantic work is registered as a pass with an explicit ID, prerequisites,
  and typed result.
- New generators accept an output abstraction; they do not open files directly.
- New AST allocations and retained strings use the context's `AstArena`.
- Parallel jobs use distinct contexts and independent output directories.
- New lower-layer failures become diagnostics/results, not `exit()` calls.
- Changes to generated output update characterization or ABI expectations only
  when the behavior change is deliberate and separately reviewed.
