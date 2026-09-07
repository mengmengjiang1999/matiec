## Why

Experimental syntax is currently lowered into a named temporary file before the
legacy parser can consume it. This makes in-memory compilation impossible and
adds filesystem failure and cleanup paths to an otherwise internal transform.

## What Changes

- Add a context-owned source manager that represents a display name and source
  bytes independently of a filesystem path.
- Add a parser entry point for source bytes while preserving the existing file
  entry point and include behavior.
- Feed experimental normalized source directly to the parser without creating a
  named temporary file.
- Add regression coverage for file and memory source compilation, diagnostics,
  pre-parsing, and sequential reuse.

## Capabilities

### New Capabilities

- `source-management`: Source loading, in-memory input, display-name preservation,
  and parser handoff without named temporary files.

### Modified Capabilities

- `compilation-context`: Compilation contexts own the source input used by the
  parser, including memory-backed input.

## Impact

This affects `CompilationContext`, the compiler driver, the legacy parser adapter,
the Flex/Bison integration boundary, Autotools source lists, and compiler API
tests. The CLI and generated C ABI remain unchanged.
