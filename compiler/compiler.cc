#include "compiler/compiler.hh"
#include "compiler/compilation_abort.hh"
#include "compiler/legacy_global_state_adapter.hh"

#include "absyntax/absyntax.hh"
#include "stage3/stage3.hh"
#include "stage4/stage4.hh"

namespace matiec {

CompilationResult Compiler::compile(CompilationContext &context) const {
  if (context.source_path().empty()) {
    context.diagnostics().error("No source path was provided");
    return context.diagnostics().result();
  }

  try {
    const CompilerOptions &options = context.options();
    LegacyGlobalStateAdapter legacy_state(context);

    symbol_c *tree_root = NULL;
    if (legacy_state.parse(&tree_root) < 0)
      return CompilationResult::failure();

    if (options.syntax_only)
      return CompilationResult::success();

    legacy_state.initialize_symbol_tables(tree_root);

    symbol_c *ordered_tree_root = NULL;
    if (stage3(tree_root, &ordered_tree_root) < 0)
      return CompilationResult::failure();

    const char *output_directory = options.output_directory.empty()
                                       ? NULL
                                       : options.output_directory.c_str();
    if (stage4(ordered_tree_root, output_directory) < 0)
      return CompilationResult::failure();

    return CompilationResult::success();
  } catch (const CompilationAbort &abort) {
    if (!abort.diagnostic_reported())
      context.diagnostics().fatal(abort.what());
    return context.diagnostics().has_errors()
               ? context.diagnostics().result()
               : CompilationResult::failure();
  }
}

}  // namespace matiec
