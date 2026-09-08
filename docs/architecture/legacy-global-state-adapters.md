# Legacy global-state adapters

The refactor uses `CompilationContext` as the owner of new per-compilation
state. Legacy parser entry points and several existing visitors still discover
their current context through narrow compatibility scopes behind
`matiec::LegacyGlobalStateAdapter`.

## State behind the adapter

The adapter currently owns the transition into two legacy areas:

* the generated scanner/parser reads parser runtime options and transient
  transition controls through the currently scoped context-owned `ParserState`;
  derived parser permissions never mutate caller-configured options;
* `stage1_2()` retains generated scanner buffers, include-stack data, Bison
  lookahead/error variables, and the pre-parse/definitive-parse driver boundary
  in thread-local sessions; callers enter it only through
  `LegacyGlobalStateAdapter::parse()`;
* parser classification tables are owned by the active context's `ParserState`
  and are cleared before every parse;
* `absyntax_utils_init()` populates the context-owned function, function-block,
  program, and datatype declaration tables; callers enter it only through
  `LegacyGlobalStateAdapter::initialize_symbol_tables()` while legacy consumers
  use the current thread's nested access scope.

Semantic and generator analysis is not part of this adapter. Stage 3 and Stage 4
receive the context-owned `AnalysisStore` explicitly; no thread-local active
analysis binding remains.

Generated mutable variables are marked `thread_local` by a checked,
post-generation build step. Independent contexts can therefore execute the
frontend concurrently on separate threads without a process-wide lock. This is
not recursive reentrancy on one thread, and the project does not yet expose a
supported full-pipeline parallel API.

## Reentrancy inventory

| Boundary | Current owner | Isolation | Removal milestone |
| --- | --- | --- | --- |
| Parser compatibility access | `ParserState` | context-owned options, transition controls, and classification tables selected by a nested thread-local pointer | Pass an explicit parser session to scanner and grammar helpers |
| Generated scanner/parser | Flex/Bison compatibility interface | buffers, include stack, locations, semantic value, lookahead, error count, and start conditions are thread-local | Replace compatibility globals with reentrant scanner and pure-parser parameters if recursive same-thread parsing is required |
| Declaration compatibility access | `DeclarationSymbolTables` | context-owned entries selected by a nested thread-local pointer | Pass declaration tables explicitly to remaining visitors |
| AST allocation | `ActiveAstArenaScope` | context-owned arena selected by a thread-local active pointer | Pass the context arena explicitly through parser and synthetic-node constructors |

Stage 3 and Stage 4 analysis data is not part of this inventory: it is already
owned by `AnalysisStore` and passed explicitly. File-static debug flags and
immutable canonical datatype sentinels do not contain compilation results.

Declaration lookup storage has crossed this boundary. `CompilationContext` owns
its function, function-block, program, and datatype tables and clears them before
each compile. `ActiveDeclarationSymbolTablesScope` is a thread-local pointer-only
compatibility surface for existing visitors; nested scopes restore their caller,
and no declaration entries live in process-wide storage.

## AST allocation boundary

`ActiveAstArenaScope` is a separate, thread-local compatibility binding used
while legacy parser and visitor code still constructs `symbol_c` subclasses
with direct `new` expressions. The `Compiler` binds the current context's
arena for the complete parse, semantic, and generation sequence; the
`symbol_c` base constructor then registers each concrete node for destruction.
The scanner uses the same binding for retained token and filename strings.

The binding restores any previous arena when it leaves scope, so nested use
does not leak the inner context. Generated scanner/parser state is independently
thread-isolated; the arena binding exists to avoid changing hundreds of parser
actions before explicit allocation parameters are introduced. New code
must use `CompilationContext::ast_arena()` directly rather than read the active
binding.

## Migration rule

No new mutable process-wide compiler state may be added to this adapter. New state
belongs in `CompilationContext` or one of its services. As parser and symbol
APIs gain explicit context parameters, their corresponding adapter methods,
`current_parser_state`, and the `runtime_options` compatibility surface must be removed. The thread-local
AST allocation binding must likewise be removed when parser and pass APIs carry
the context explicitly.

## Final-state audit

The completion audit searches handwritten and regenerated C++ sources for
namespace/file-scope mutable definitions and process-termination calls. Mutable
frontend session definitions are either context-owned or explicitly
`thread_local`; the generation transformation fails if an expected Flex/Bison
declaration changes shape. Newly added file-scope constants are immutable.

There are no `exit()`, `_Exit()`, or `abort()` calls in handwritten parser,
semantic, or generation entry points. The remaining calls are emitted by the
checked-in Flex/Bison skeletons in `stage1_2/iec_flex.cc` and
`stage1_2/iec_bison.cc` for skeleton-level allocation and I/O failures. Those
generated files are entered only through `LegacyGlobalStateAdapter::parse()`.
Grammar
actions and handwritten lower layers report diagnostics, return failures, or
throw `CompilationAbort`; `Compiler::compile()` catches that internal unwind
at the embedding boundary.

Repeat the audit after changing the generated frontend, adding a compiler
service, or introducing a namespace/file-scope variable. Generated-skeleton
termination paths should disappear when fatal hooks are replaced; until then
they must not be called directly outside this adapter.
