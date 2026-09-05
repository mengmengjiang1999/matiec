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
