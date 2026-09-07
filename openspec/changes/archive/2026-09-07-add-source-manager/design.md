## Context

The compiler accepts a path through `CompilationContext`. Experimental profile
normalizers read that path separately, rewrite text, write a named file under
`/tmp`, and ask the legacy parser to reopen it while displaying the original path.
Flex already supports stream buffers, but the Bison orchestration exposes only a
filename API and parses the main input more than once when pre-parsing is enabled.

## Goals / Non-Goals

**Goals:**

- Represent source bytes and their diagnostic display name in one context-owned
  abstraction.
- Support compiling source supplied by an embedding caller without a named file.
- Preserve standard-library and include-file loading through the existing path
  mechanism.
- Preserve legacy CLI behavior and generated output.

**Non-Goals:**

- Make the generated parser reentrant in this increment.
- Replace include lookup with a virtual filesystem.
- Change language grammar, semantic analysis, or generated ABI.

## Decisions

### Use `SourceManager` as a small value-owning boundary

`SourceManager` stores the display path, optional source bytes, and whether the
main source is file- or memory-backed. File inputs remain lazily consumed by the
legacy path. Embedders can set bytes explicitly.

### Add a memory parser entry point without changing generated parser ownership

The legacy parser adapter receives bytes and a display name. The parser creates a
fresh anonymous C stream for each parse pass, so pre-parsing can replay the input.
Unlike the current `mkstemp` path, the stream has no visible filename, is closed
by the parser, and cannot leave a stale artifact behind. This is portable across
the supported Linux and macOS environments.

### Normalize one in-memory string pipeline

For the experimental profile the compiler loads the source once, validates and
normalizes the bytes, then passes the resulting string to the parser. Feature-
neutral legacy compilation continues using the file path to minimize behavior
change.

## Risks / Trade-offs

- `tmpfile()` may use OS-managed backing storage even though it has no named path;
  this increment removes user-visible temporary files but does not yet convert
  Flex/Bison to a reentrant byte scanner.
- Include pragmas remain filesystem-based and use existing include directories.
- Normalizer diagnostics must retain the original display path; tests cover this.

## Migration Plan

1. Introduce and unit-test `SourceManager`.
2. Add memory-source entry points at the lexer, parser, stage, and adapter layers.
3. Replace `TemporarySource` in the compiler driver.
4. Add end-to-end compilation regressions and run the full suite.
