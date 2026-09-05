# Legacy global-state adapters

The refactor uses `CompilationContext` as the owner of new per-compilation
state. Flex/Bison and several existing visitors are not reentrant yet, so their
process-wide state is temporarily isolated behind
`matiec::LegacyGlobalStateAdapter`.

## State behind the adapter

The adapter currently owns the transition into two legacy areas:

* `runtime_options`, consumed by the generated scanner/parser and existing
  semantic code, is populated exclusively from `CompilerOptions` when an
  adapter is created;
* `stage1_2()` contains the generated parser's pre-parse and definitive-parse
  state; callers enter it only through `LegacyGlobalStateAdapter::parse()`;
* `absyntax_utils_init()` populates the legacy global function, function-block,
  program, type, and enumerated-value symbol tables; callers enter it only
  through `LegacyGlobalStateAdapter::initialize_symbol_tables()`.

The parser boundary also contains the mutable scanner buffers, location and
include-stack tracking, Bison lookahead/error variables, grammar feature flags,
and the small pre-parse state machine in `stage1_2.cc`. These variables are
reset by `stage1_2()` for sequential use, but remain process-wide and are part
of the same non-reentrant compatibility boundary.

This adapter is deliberately synchronous and does not claim thread safety.
Sequential compilation becomes safe only after parser state, symbol tables,
and AST storage are reset or context-owned in the later lifetime tasks.

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
APIs gain explicit context parameters, their corresponding adapter methods and
the `runtime_options` compatibility structure must be removed. The thread-local
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
