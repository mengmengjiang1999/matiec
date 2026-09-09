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
