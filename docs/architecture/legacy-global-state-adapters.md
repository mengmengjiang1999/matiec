# Legacy global-state adapters

The refactor uses `CompilationContext` as the owner of new per-compilation
state. Legacy parser entry points and several existing visitors still discover
their current context through narrow compatibility scopes behind
`matiec::LegacyGlobalStateAdapter`.

## State behind the adapter

The adapter currently owns the transition into two legacy areas. It has no
mutable fallback parser session: required compatibility access outside a scope
fails explicitly, while standalone AST construction merely omits arena
attachment.

* the generated scanner/parser reads parser runtime options and transient
  transition controls through the currently scoped context-owned `ParserState`;
  derived parser permissions never mutate caller-configured options;
* `stage1_2()` retains generated scanner buffers, include-stack data, and the
  pre-parse/definitive-parse driver boundary in thread-local sessions; Bison
  lookahead, value, location, and internal error state are automatic per parser
  invocation; callers enter the frontend only through
  `LegacyGlobalStateAdapter::parse()`;
* parser classification tables are owned by the active context's `ParserState`
  and are cleared before every parse;
* the same parser state carries a non-owning pointer to its context's `AstArena`,
  so legacy direct node construction and retained lexer strings join the correct
  compilation lifetime without a second active-state binding;
* `absyntax_utils_init()` populates the context-owned function, function-block,
  program, and datatype declaration tables; callers enter it only through
  `LegacyGlobalStateAdapter::initialize_symbol_tables()` while legacy consumers
  use the selected parser session.

Semantic and generator analysis is not part of this adapter. Stage 3 and Stage 4
receive the context-owned `AnalysisStore` explicitly; no thread-local active
analysis binding remains.

Remaining generated Flex mutable variables are marked `thread_local` by a
checked post-generation build step. The generated Bison parser is pure and
receives `ParserState&` explicitly. Independent contexts can therefore execute
the frontend concurrently on separate threads without a process-wide lock. The
scanner is not recursively reentrant on one thread. `Compiler::compile_parallel()` exposes
the supported bounded full-pipeline API for distinct contexts.

All handwritten file-backed and memory-backed stage 1/2 entry points receive a
`ParserState&`. They reset classifications and inspect options through that
argument. The generated parser and lexer entry signatures also receive that
session. A nested selector remains around the generated call for scanner and
grammar helpers that still use compatibility access; Stage 3/4 declaration lookup retains
a compiler-boundary session until that dependency is removed.

## Reentrancy inventory

| Boundary | Current owner | Isolation | Removal milestone |
| --- | --- | --- | --- |
| Parser compatibility access | `ParserState` | explicitly supplied to handwritten stage 1/2 entry points and generated parser/lexer signatures; selected only for remaining callback helpers; no mutable fallback | Pass the session explicitly to the remaining helpers |
| Generated Bison parser | Pure Bison interface | semantic value, location, lookahead, and internal error state are invocation-local; recovered-error reporting is stored in `ParserState` | Complete |
| Generated Flex scanner | Flex compatibility interface | buffers, include stack, and start conditions are thread-local | Use a reentrant scanner object if recursive same-thread parsing is required |
| Declaration compatibility access | `DeclarationSymbolTables` | context-owned entries selected by the active parser session; no separate TLS or fallback table | Pass the parser session explicitly to remaining entry points |

Stage 3 and Stage 4 analysis data is not part of this inventory: it is already
owned by `AnalysisStore` and passed explicitly. File-static debug flags and
immutable canonical datatype sentinels do not contain compilation results.

Declaration lookup storage has crossed this boundary. `CompilationContext` owns
its function, function-block, program, and datatype tables and clears them before
each compile. Its `ParserState` holds a non-owning link to those tables, so the
existing parser-session selector also serves legacy visitors. The independent
declaration-table TLS, mutable fallback, and nesting scope have been removed.

## Parser-carried AST allocation

The separate active-arena binding has been removed. `CompilationContext` binds
its arena to its owned `ParserState`, and the existing nested parser-state scope
selects that arena for legacy parser actions and synthetic helpers that still use
direct `new`. Retained token and filename strings follow the same parser session.
Nested parser scopes restore their caller and therefore restore the allocation
target without maintaining another thread-local pointer.

New component code should call `CompilationContext::ast_arena().make<T>()`
directly. That operation explicitly attaches AST-derived objects to the named
arena, even when no parser session is active or another session is temporarily
active.

## Migration rule

No new mutable process-wide compiler state may be added to this adapter. New state
belongs in `CompilationContext` or one of its services. As parser and symbol
APIs gain explicit context parameters, their corresponding adapter methods,
`current_parser_state`, and the `runtime_options` compatibility surface must be
removed. AST ownership and declaration lookup no longer have separate
compatibility bindings.

## Final-state audit

The completion audit searches handwritten and regenerated C++ sources for
namespace/file-scope mutable definitions and process-termination calls. Mutable
frontend session definitions are context-owned, automatic Bison parser state,
or explicitly `thread_local` Flex state; the generation transformation fails if
an expected Flex declaration changes shape. Newly added file-scope constants
are immutable.

There are no `exit()`, `_Exit()`, or `abort()` calls in handwritten or generated
parser, semantic, or generation entry points. Flex skeleton fatal paths throw
`CompilationAbort`; Bison memory exhaustion reports an error and returns a
nonzero parser status. Grammar actions and handwritten lower layers likewise
report diagnostics, return failures, or throw `CompilationAbort`.
`Compiler::compile()` catches that internal unwind at the embedding boundary.

Repeat the audit after changing the generated frontend, adding a compiler
service, or introducing a namespace/file-scope variable. The checked generated
rewrite rejects an unexpected fatal-hook shape or any regenerated frontend
process-termination call before compilation.

The supported cross-thread reentrancy track is complete and guarded by the
normal architecture and concurrent-compilation tests. A reentrant Flex scanner
would be a separate capability for recursive same-thread parsing, not a missing
part of the supported contract.
