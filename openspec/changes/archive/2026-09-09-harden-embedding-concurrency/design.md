# Design: deterministic concurrency stress matrix

Eight long-lived opaque contexts run multiple rounds through
`matiec_compile_batch` with four workers. Each round rewrites sources and resets
sticky cancellation where appropriate. Expected success/failure is derived from
the context index and asserted at the same result index.

Diagnostic and resolver callbacks share only atomic counters. Dedicated fault
cases throw C++ exceptions through the C callback function pointer; the ABI
adapter must contain them and report a failed compilation. An output exception
is exercised outside syntax-only mode.

The sanitizer runner accepts `thread`, sets the matching compiler/linker flag,
and runs the same deterministic suite. Linux CI makes this a required matrix
member; local platforms may run `make check-tsan` when their compiler runtime
supports it.
