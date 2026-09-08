# Legacy global-state adapters

The refactor uses `CompilationContext` as the owner of new per-compilation
state. Flex/Bison and several existing visitors are not reentrant yet, so their
process-wide state is temporarily isolated behind
`matiec::LegacyGlobalStateAdapter`.

## State behind the adapter

The adapter currently owns the transition into two legacy areas:

* the generated scanner/parser reads parser runtime options and transient
  transition controls through the currently scoped context-owned `ParserState`;
  derived parser permissions never mutate caller-configured options;
* `stage1_2()` retains generated scanner buffers, include-stack data, Bison
  lookahead/error variables, and the pre-parse/definitive-parse driver boundary;
  callers enter it only through `LegacyGlobalStateAdapter::parse()`;
* `absyntax_utils_init()` populates the context-owned function, function-block,
  program, and datatype declaration tables; callers enter it only through
  `LegacyGlobalStateAdapter::initialize_symbol_tables()` while legacy consumers
  use the current thread's nested access scope.

Semantic and generator analysis is not part of this adapter. Stage 3 and Stage 4
receive the context-owned `AnalysisStore` explicitly; no thread-local active
analysis binding remains.

The remaining parser-boundary variables are reset by `stage1_2()` for sequential
use, but remain process-wide and are part of the same non-reentrant compatibility
boundary. Parser options, feature flags, and transient lexer transition controls
are no longer members of that process-wide state.

This adapter is deliberately synchronous and does not claim thread safety.
Sequential compilation is covered by context-reuse regressions; generated scanner
state and parser classification tables still prevent concurrent compilation.

## Reentrancy inventory

| Boundary | Current owner | Shared state | Removal milestone |
| --- | --- | --- | --- |
| Parser classification | `stage1_2` compatibility boundary | library-element, variable-name, and direct-variable tables | Pass a context-owned parser symbol state to scanner callbacks |
| Generated scanner/parser | Flex/Bison globals | buffers, include stack, locations, semantic value, lookahead, error count, and scanner start conditions | Generate reentrant scanner and pure parser interfaces with an explicit parse session |
| AST allocation | `ActiveAstArenaScope` | thread-local active arena used by legacy direct `new` actions | Pass the context arena explicitly through parser and synthetic-node constructors |

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
does not leak the inner context. It does not make the generated scanner and
parser reentrant; it exists only to avoid changing hundreds of generated-parser
actions before explicit parser context parameters are introduced. New code
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

The completion audit compared this branch with its `default` merge base and
searched handwritten and generated C++ sources for new namespace/file-scope
mutable definitions and process-termination calls. It found no new mutable
process-wide compilation state. The apparent new `runtime_options` definition
is a relocation of the pre-existing CLI-owned compatibility object into this
adapter; new compiler services and generator state are context-, manager-, or
instance-owned. Newly added file-scope constants are immutable.

There are no `exit()`, `_Exit()`, or `abort()` calls in handwritten parser,
semantic, or generation entry points. The remaining calls are emitted by the
checked-in Flex/Bison skeletons in `stage1_2/iec_flex.cc` and
`stage1_2/iec_bison.cc` for skeleton-level allocation and I/O failures. Those
generated files are entered only through `LegacyGlobalStateAdapter::parse()`
and remain part of this documented non-reentrant frontend adapter. Grammar
actions and handwritten lower layers report diagnostics, return failures, or
throw `CompilationAbort`; `Compiler::compile()` catches that internal unwind
at the embedding boundary.

Repeat the audit after changing the generated frontend, adding a compiler
service, or introducing a namespace/file-scope variable. Generated-skeleton
termination paths should disappear when the frontend is made reentrant or its
fatal hooks are replaced; until then they must not be called directly outside
this adapter.
