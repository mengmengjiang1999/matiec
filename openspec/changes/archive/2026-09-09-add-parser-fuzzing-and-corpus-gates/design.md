# Design

`tests/fuzz/parser_fuzzer.cc` consumes the first byte as a mode and bounds the
remaining input before passing it through `libmatiec`'s C API. Each invocation
owns and destroys its context. File mode uses a temporary file; memory mode uses
the byte buffer directly; include mode returns the buffer from a resolver;
cancel mode requests cancellation before compilation; limit mode varies source,
diagnostic, and output limits while using an output callback.

When compiled with `MATIEC_LIBFUZZER`, the source exports
`LLVMFuzzerTestOneInput`. Otherwise it provides a small driver that reads stdin or
named corpus files, making the same entry point usable under AFL. Neither path
asserts that arbitrary input is valid; only memory safety violations, sanitizer
findings, hangs, and API contract failures are failures.

`tests/fuzz/runtests` copies the repository to a temporary directory, builds all
library objects with Clang fuzz/address instrumentation, links the harness, and
runs either a deterministic smoke count or a time-bounded periodic campaign. A
separate AFL builder performs an isolated instrumented build when AFL tooling is
installed. Hosted CI uploads findings only on failure.
