# MATIEC

MATIEC is an open-source compiler for the textual programming languages defined
by IEC 61131-3, the standard commonly used to program programmable logic
controllers (PLCs). It accepts Structured Text (ST), Instruction List (IL), and
the textual form of Sequential Function Chart (SFC).

This repository preserves the original compiler and its language behavior while
providing a modernized compiler boundary: per-compilation state, structured
diagnostics, explicit semantic passes, owned AST lifetime, testable output
sinks, and automated regression and sanitizer coverage.

> MATIEC is not intended for safety-critical use without a complete,
> application-specific review and validation process.

## What MATIEC provides

The build produces two command-line tools:

- `iec2c` translates IEC 61131-3 source into ANSI C.
- `iec2iec` parses IEC 61131-3 source and emits normalized IEC source. It is
  primarily useful for frontend development, diagnostics, and regression
  testing.

IEC 61131-3 also defines graphical Function Block Diagram (FBD) and Ladder
Diagram (LD) languages. MATIEC does not directly parse their graphical
representations. ST, IL, and textual SFC may be used together in one input.

## Quick start

Building from a Git checkout requires Autoconf 2.69 or newer, Automake 1.16 or
newer, Bison 2.4 or newer, Flex 2.6 or newer, GNU Make (or a compatible
implementation), and C/C++ compilers. GCC on Linux and Clang on macOS are
covered by the project validation workflow.

```sh
autoreconf --install
./configure
make --jobs=2
make check
```

Generated `configure` and `Makefile.in` files are committed. A prepared source
archive can therefore start at `./configure`; a Git checkout should use the
complete bootstrap sequence above. See [README.build](README.build) for tool
versions, sanitizer targets, cross-compilation, and generated-file maintenance.

## Command-line use

Create the target directory before invoking `iec2c`:

```sh
mkdir -p build/generated
./iec2c -I lib -T build/generated program.st
```

The generated C sources and headers are written below `build/generated`.
Runtime support headers are in `lib/C`; the exact generated files depend on the
configuration and program declarations in the input.

To print normalized IEC source:

```sh
./iec2iec -I lib program.st > normalized.st
```

Run either tool with `-h` for all frontend and code-generation options. Both
tools accept one input file per invocation and return a non-zero status when
parsing, semantic analysis, or output generation fails.

MATIEC supports source inclusion with the historical extension:

```text
{#include "filename" }
```

Use `-I <directory>` to select the include and standard-library directory.

## Compiler architecture

The original compiler stages remain recognizable, but they now execute behind
an explicit compilation boundary:

```text
CLI or embedding code
        |
        v
CompilationContext
  options | diagnostics | AST arena | output manager
        |
        v
Flex scanner + Bison parser
        |
        v
symbol initialization -> SemanticPassManager -> C or IEC generator
                                                   |
                                                   v
                                                OutputSink
```

The pipeline consists of:

1. lexical and syntax analysis using the generated Flex/Bison frontend;
2. symbol-table initialization for declaration lookup;
3. ordered semantic passes for flow, declaration, type, and consistency
   analysis;
4. C or IEC generation through context-owned output management;
5. optional compilation of generated C by the user's C toolchain.

One `matiec::CompilationContext` owns the options, diagnostics, AST arena, and
generated outputs for one compilation. Separate contexts support sequential
compilations in one process. The legacy generated frontend is still a
synchronous compatibility boundary and is not thread-safe.

Detailed contracts and remaining compatibility boundaries are documented in:

- [Compiler architecture and embedding boundaries](docs/architecture/compiler-boundaries.md)
- [Legacy global-state adapters](docs/architecture/legacy-global-state-adapters.md)
- [AST ownership inventory](docs/architecture/ast-ownership-inventory.md)
- [Architecture decisions](docs/decisions/)

## Embedding boundary

The compiler driver can be invoked from C++ without routing through command-line
argument handling:

```cpp
#include <iostream>

#include "compiler/compilation_context.hh"
#include "compiler/compiler.hh"

matiec::CompilationContext context;
context.set_source_path("program.st");
context.options().include_directory = "lib";
context.options().output_directory = "build/generated";

const matiec::CompilationResult result = matiec::Compiler().compile(context);
context.diagnostics().render(std::cerr);
return result.succeeded() ? 0 : 1;
```

Generated output can also be directed to an in-memory or custom `OutputSink`.
Treat AST pointers as context-lifetime objects: they must not outlive their
`CompilationContext`. The interface is currently a source-level integration
boundary, not a versioned binary ABI.

## Tests and validation

The primary regression entry point is:

```sh
make check
```

It covers compiler value types and services, semantic pass behavior,
invalid-then-valid sequential compilation, in-memory generation, CLI failures,
syntax and initialization cases, byte-for-byte output characterization, and
representative generated-C ABI and runtime behavior.

Memory and undefined-behavior checks run in isolated source copies:

```sh
make check-asan
make check-ubsan
```

CI performs clean GCC/Linux and Clang/macOS builds using the same regression
entry point. Test artifacts are created outside tracked source locations so a
successful run leaves the worktree clean.

## Repository guide

| Path | Purpose |
| --- | --- |
| `compiler/` | Compilation context, diagnostics, driver, pass manager, AST arena, and output abstractions |
| `stage1_2/` | Flex scanner, Bison grammar, and parser compatibility boundary |
| `absyntax/` | Abstract syntax tree node definitions and visitors |
| `absyntax_utils/` | Symbol lookup and AST utility passes |
| `stage3/` | Semantic analysis passes |
| `stage4/` | C and normalized IEC generators |
| `lib/` | IEC standard library and generated-C runtime headers |
| `tests/` | Regression, characterization, CLI, generated-C, and sanitizer suites |
| `docs/` | Architecture notes and design decisions |
| `openspec/` | Executable requirements and archived change specifications |

## Contributing

Keep observable compiler behavior stable unless a behavior change is deliberate
and separately tested. Before submitting a change:

```sh
autoreconf --force --install  # when configure.ac or Makefile.am changes
./configure
make --jobs=2
make check
```

New mutable compilation state belongs in `CompilationContext` or a
context-owned service. New semantic work should be an explicit pass, generators
should write through output sinks, and lower layers should report structured
failures rather than terminate the process. Update the relevant specification
under `openspec/specs/` when a requirement changes.

## Historical notes and compatibility

The original [readme](readme) is retained as a detailed historical reference.
It describes the IEC language goals, individual compiler passes, grammar
extensions, implementation notes, and platform-specific background that shaped
MATIEC. Those notes remain useful to maintainers but do not replace the current
architecture contracts linked above.

MATIEC was originally based on the final draft of IEC 61131-3, 2nd Edition
(2001-12-10). Existing applications should validate generated code with their
target compiler, runtime, PLC integration, and applicable safety process.

## License and credits

MATIEC is distributed under the GNU General Public License, version 3. See
[COPYING](COPYING) for the complete license text.

The project was started by Mario de Sousa, with historical contributions and
copyright notices retained in the source files and original documentation.
