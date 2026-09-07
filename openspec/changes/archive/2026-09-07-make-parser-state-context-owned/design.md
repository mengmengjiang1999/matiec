## Context

The generated scanner and parser refer to `runtime_options`, while callback
functions in `stage1_2.cc` use static transition flags. Regenerating both tools
as fully pure/reentrant parsers is a separate invasive effort. This change moves
the caller-controlled state into the context while retaining a narrow scoped
compatibility accessor.

## Goals / Non-Goals

**Goals:**

- Isolate runtime options and lexer transition controls per context.
- Restore nested active-state bindings deterministically.
- Reset transient flags at the start of every parse.
- Preserve generated parser source compatibility.

**Non-Goals:**

- Regenerate Flex/Bison with pure-parser and reentrant-scanner modes.
- Move generated parser stacks, token globals, or scanner buffers.
- Enable simultaneous calls into the remaining generated singleton parser.

## Decisions

### Own state in `CompilationContext`

`ParserState` contains the legacy option structure plus preparse/body/SFC/task
and pop-state controls. Context accessors make ownership explicit and testable.

### Use a scoped compatibility binding

`ActiveParserStateScope` stores only a thread-local pointer to context-owned
state. The data itself is never thread-local or process-global. `main.hh` maps
the established `runtime_options` spelling to the active state's option value.

### Reset controls at adapter construction

The legacy adapter clears transient flags before binding options for a parse.
Nested adapters restore the previous state in their destructor.

## Risks / Trade-offs

Generated parser internals remain non-reentrant and require a later generator
conversion before truly concurrent parsing. This change removes caller-owned
global state without pretending those generated globals have disappeared.

## Migration Plan

Land the state owner and compatibility scope together, validate sequential and
nested use, then pursue pure Flex/Bison generation as a separate change.
