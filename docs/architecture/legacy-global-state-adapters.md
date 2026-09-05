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

## Migration rule

No new mutable global compiler state may be added to this adapter. New state
belongs in `CompilationContext` or one of its services. As parser and symbol
APIs gain explicit context parameters, their corresponding adapter methods and
the `runtime_options` compatibility structure must be removed.
