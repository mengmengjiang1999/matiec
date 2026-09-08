## Context

Both generator libraries listed `../stage4.o` as an archive input. During a clean
parallel build, Automake compiled that object from a child directory and asked GCC
to write `../.deps/stage4.Tpo`, but no parent `.deps` directory existed. Existing
local dependency directories masked the race. The sanitizer workflow separately
called `make check-asan` or `make check-ubsan` immediately after checkout, before
running `autoreconf` and `configure`.

## Decisions

- Give `stage4.cc` a single owner, `stage4/libstage4_common.a`, and link that
  archive before the selected generator archive whose factory it references.
- Repeat `libcompiler.a` after the AST and utility provider archives so GNU ld's
  left-to-right archive scan resolves the intentional compiler/AST dependency
  cycle; Apple ld had previously masked this ordering requirement.
- Give compiler unit tests the same explicit compiler/AST/compiler link chain so
  UBSan vptr instrumentation can resolve `symbol_c` RTTI without disabling checks.
- Keep `tests/sanitizers/runtests` as the canonical sanitizer entry point because
  it already creates an isolated copy, bootstraps, configures, builds, and tests.
- Pass the script's `address` and `undefined` modes directly from the workflow
  matrix.
- Delete copied `.deps` directories in isolated sanitizer workspaces so stale
  dependency state cannot make a local validation falsely pass.

## Risks / Trade-offs

The common stage 4 archive adds one build artifact, but eliminates duplicate and
racing ownership of the same object. Static-library order remains significant:
the generator follows its common consumer, while the circular compiler provider
is repeated at the end of the link list.
