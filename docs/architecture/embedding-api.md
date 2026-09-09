# Versioned embedding API

The stable embedding boundary is the C header `include/matiec/api.h`. It is
valid in C11 and C++ translation units and does not expose STL types, AST nodes,
parser structures, or compiler service layouts.

API version 1 starts with compile-time and runtime version discovery. The packed
numeric form stores major, minor, and patch components so a caller can compare
the header used at build time with the loaded library.

Version 1.1 adds a single-compilation workflow. A caller creates an opaque
`matiec_context_t`, supplies either copied memory source or a source path, sets
the include directory and desired compilation options, then calls
`matiec_context_compile`. The context owns all compiler state and may be reused
for later compilations; destroy it with `matiec_context_destroy`.

`matiec_context_compile` returning `MATIEC_STATUS_OK` means the API call itself
completed. Language errors are reported by `matiec_result_t.succeeded` and its
error and warning counts. This keeps an invalid IEC program distinct from API
misuse, allocation failure, or an internal exception. No C++ exception crosses
the C boundary.

The memory-source setter copies exactly `source_size` bytes, so its input need
not be NUL-terminated and may be released after the call. Configuration strings
are also copied into the context. `matiec_context_last_error` is a borrowed,
context-owned string describing the most recent API failure; it remains valid
only until the next operation on that context or its destruction.

Version 1.2 exposes every context diagnostic as a size-tagged
`matiec_diagnostic_t`. Callers may enumerate diagnostics after compilation or
install a diagnostic callback; callback delivery happens synchronously, in
stored order, after compilation completes. Message and file pointers are
borrowed from the context and remain valid until its next compilation or
destruction. The callback's view itself is valid only for that invocation.

An output callback can replace generated filesystem artifacts. The callback
receives the intended path and one or more ordered byte chunks for that path;
the host is responsible for grouping and appending chunks. A zero-length chunk
represents an empty artifact. Returning zero rejects output, records a compiler
diagnostic, and makes the compilation result unsuccessful. Clearing the
callback restores ordinary file output.

Callbacks run on the thread calling `matiec_context_compile`. They must not
re-enter the same context, and all user-data storage must remain valid until the
callback is cleared or the context is destroyed.

Version 1.3 adds `matiec_compile_batch`. It accepts parallel arrays of opaque
contexts and initialized results, compiles independent entries with a bounded
worker count, and preserves input order in the result array. A concurrency value
of zero selects a positive implementation-defined bound. A zero-length batch is
a successful no-op.

Every context in a non-empty batch must be non-null and every result must carry
the expected `struct_size`; the entire batch is validated before workers start.
The same context cannot appear twice: duplicate positions receive failed
results and a context diagnostic while independent entries may still succeed.
Callbacks configured on batch contexts execute on worker threads, so callers
must synchronize user data shared by more than one context.

Version 1.4 adds a context-owned virtual include resolver. Include requests are
delivered synchronously with the requested pragma name. A resolved
`matiec_source_view_t` supplies an exact byte span and display name; the host
keeps that borrowed storage valid until the next resolver invocation, and the
compiler copies it before resolving another include. Nested virtual includes
use the same resolver and preserve their display names in diagnostics.

Once configured, the resolver is authoritative: `NOT_FOUND`, `ERROR`, malformed
views, and callback exceptions fail compilation without a filesystem fallback.
The callback may explicitly return `USE_FILESYSTEM` for compiler library data or
other approved paths. Clearing the callback restores the include-directory search. Resolver callbacks
run on the compiling thread (worker threads for batch compilation), must not
re-enter their context, and must synchronize shared user data.

Version 1.5 adds cooperative cancellation and resource budgets.
`matiec_context_set_limits` copies a size-tagged `matiec_limits_t`; zero fields
are unlimited, while nonzero fields cap each primary or virtual-include source,
the number of retained diagnostics, and aggregate generated-output bytes for a
compilation. Exceeding a budget is a compilation failure reported through the
normal result and diagnostic stream.

`matiec_context_cancel` atomically sets a sticky request and is the only context
operation permitted from another thread during compilation. The scanner polls
the request while consuming input and the compiler checks it between major
stages. Hosts must call `matiec_context_reset_cancel` before deliberately
reusing a cancelled context; reset and all other configuration operations must
not race with compilation.

## Installed package

`make install` provides the public header as `<matiec/api.h>`, a self-contained
static `libmatiec.a`, a platform-versioned shared library with ABI major 1, and
`matiec.pc` metadata. Compile
C source with a C compiler, then use a C++ linker for the final executable
because the library implementation uses the C++ runtime:

```sh
cc $(pkg-config --cflags matiec) -c host.c
c++ host.o $(pkg-config --libs matiec) -pthread -o host
```

The compiler's IEC definitions install under
`${datadir}/matiec/lib`; pass that path to
`matiec_context_set_include_directory`. Generated-C runtime headers install in
the `C` child directory. ELF hosts install an SONAME-compatible
`libmatiec.so.1`; Darwin hosts install `libmatiec.1.dylib` with a matching
install name. Unversioned links are intended for development, while runtime
consumers bind to the ABI-major name. Internal C++ symbols use hidden
visibility; only the public `matiec_*` C ABI is exported.

## Compatibility maintenance

`make check-api` is the focused public-boundary gate. It compares globally
defined `matiec_*` names in `libmatiec.a` with
`tests/api/public-symbols.txt`, compiles every published function signature as
both C11 and C++17, and runs the staged-install consumer smoke test. GitHub
Actions exposes this as the separately named `api-compatibility` job.

An intentional backward-compatible function addition must update the symbol
allowlist and increment `MATIEC_API_VERSION_MINOR`. Removing a function,
changing a published signature, ownership rule, enum value, or structure prefix
requires a major-version design review. Behavior-only compatible fixes increment
the patch version when callers need to distinguish them.

Compatibility follows these rules:

- a major increment may remove symbols, change signatures, or change ownership;
- a minor increment adds backward-compatible functions or enum values;
- a patch increment fixes behavior without intentionally changing the ABI.

Opaque objects are created and destroyed by matching API functions. Memory
remains owned by the side that allocated it unless a function explicitly
documents a borrowed view or transfer.

The existing `matiec::Compiler` and `matiec::CompilationContext` classes
remain supported source-integration interfaces, but their C++ layouts and STL
signatures are not part of the stable binary ABI. The package version reported
by the command-line tools is independent from the embedding API version.
