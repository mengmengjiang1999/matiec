# Exploration: parser fuzzing and corpus gates

The public C API already exposes every boundary that needs broad malformed-input
coverage: filesystem paths, in-memory sources, virtual includes, cancellation,
and source/diagnostic/output limits. A harness can therefore exercise production
compiler entry points without linking to generated lexer internals.

The existing sanitizer runner demonstrates how to copy and bootstrap the tree in
an isolated temporary directory. The fuzz runner will use the same pattern so
instrumentation never dirties a developer checkout. Clang's libFuzzer is suitable
for bounded hosted smoke runs; the same source also needs a conventional `main`
for AFL-compatible file/stdin execution.

Seed inputs should be small, valid or nearly valid IEC programs with a one-byte
mode selector. They are regression assets, not a collection of external or
network-derived samples. Fuzzing remains local and has no network behavior.
