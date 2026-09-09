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

* the generated scanner/parser receives the context-owned `ParserState`
  explicitly for runtime options and transient transition controls; derived
  parser permissions are session fields and never mutate caller options;
* each `stage1_2()` parse owns a reentrant Flex handle containing buffers,
  include-stack data, locations, and body lookahead state; Bison lookahead,
  value, location, and internal error state are automatic per invocation;
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

Flex is generated in reentrant mode; generated storage and handwritten scanner
state belong to an explicit handle. The pure Bison parser receives that handle
and `ParserState&`. Independent contexts can therefore execute concurrently or
nest synchronously on one thread without a process-wide lock. Linux CI runs the
same regression suite under ThreadSanitizer, including repeated opaque-context
reuse, mixed success/failure batches, and callback exception injection.

All handwritten file-backed and memory-backed stage 1/2 entry points receive a
`ParserState&`. They reset classifications and inspect options through that
argument. The generated parser and lexer entry signatures also receive that
session and its reentrant scanner handle. No scanner or grammar callback selects
an ambient parser. Stage 3/4 declaration lookup retains a narrow
declaration-table scope until those visitor APIs accept it explicitly.

## Reentrancy inventory

| Boundary | Current owner | Isolation | Removal milestone |
| --- | --- | --- | --- |
| Parser invocation | `ParserState` | explicitly supplied to handwritten stage 1/2 entry points and generated parser/lexer signatures; no selector or mutable fallback | Complete |
| Generated Bison parser | Pure Bison interface | semantic value, location, lookahead, and internal error state are invocation-local; recovered-error reporting is stored in `ParserState` | Complete |
| Generated Flex scanner | Reentrant Flex handle | generated buffers, include stack, tracking, and start conditions are per invocation | Complete |
| Declaration compatibility access | `DeclarationSymbolTables` | context-owned entries selected by a narrow, nestable declaration scope; no fallback table | Pass the tables explicitly to remaining consumers |

Stage 3 and Stage 4 analysis data is not part of this inventory: it is already
owned by `AnalysisStore` and passed explicitly. File-static debug flags and
immutable canonical datatype sentinels do not contain compilation results.

Declaration lookup storage has crossed this boundary. `CompilationContext` owns
its function, function-block, program, and datatype tables and clears them before
each compile. A narrow declaration-table scope serves legacy visitors without
exposing `ParserState`; there is no mutable fallback table.

## Parser-carried AST allocation

`CompilationContext` binds its arena to its owned `ParserState`. Explicit lexer
retention uses that arena directly, while a narrow, nestable arena scope covers
legacy parser actions and synthetic helpers that still use direct `new`.

New component code should call `CompilationContext::ast_arena().make<T>()`
directly. That operation explicitly attaches AST-derived objects to the named
arena, even when no parser session is active or another session is temporarily
active.

## Migration rule

No new mutable process-wide compiler state may be added to this adapter. New state
belongs in `CompilationContext` or one of its services. As parser and symbol
APIs gain explicit context parameters, their corresponding adapter methods and
the `runtime_options` compatibility surface must be removed. Parser and scanner
helpers now receive `ParserState` explicitly; there is no `current_parser_state`
selector. The remaining legacy bindings are narrow, independently nested scopes
for AST ownership, declaration lookup, and runtime options, and none exposes the
active parser invocation.

## Final-state audit

The completion audit searches handwritten and regenerated C++ sources for
namespace/file-scope mutable definitions and process-termination calls. Mutable
frontend session definitions are context-owned, automatic Bison parser state,
or fields of the reentrant Flex session. The generated-source verifier rejects
process termination, while architecture tests reject handwritten Flex or Bison
thread-local state. Newly added file-scope constants are immutable.

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

The frontend reentrancy track is complete and guarded by architecture,
concurrent-compilation, and nested include-callback tests.
