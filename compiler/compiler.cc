#include "compiler/compiler.hh"

#include "absyntax/absyntax.hh"
#include "absyntax_utils/absyntax_utils.hh"
#include "main.hh"
#include "stage1_2/stage1_2.hh"
#include "stage3/stage3.hh"
#include "stage4/stage4.hh"

/* Compatibility state for stages that have not yet migrated to
 * CompilationContext. New option state must be added to CompilerOptions first. */
runtime_options_t runtime_options;

namespace {

void apply_compiler_options(const matiec::CompilerOptions &options) {
  runtime_options.allow_void_datatype = options.allow_void_datatype;
  runtime_options.allow_missing_var_in = options.allow_missing_var_in;
  runtime_options.disable_implicit_en_eno = options.disable_implicit_en_eno;
  runtime_options.pre_parsing = options.pre_parsing;
  runtime_options.safe_extensions = options.safe_extensions;
  runtime_options.full_token_loc = options.full_token_location;
  runtime_options.conversion_functions = options.conversion_functions;
  runtime_options.nested_comments = options.nested_comments;
  runtime_options.ref_standard_extensions = options.reference_extensions;
  runtime_options.ref_nonstand_extensions = options.nonstandard_reference_extensions;
  runtime_options.nonliteral_in_array_size = options.nonliteral_array_size;
  runtime_options.relaxed_datatype_model = options.relaxed_datatype_model;
  runtime_options.includedir = options.include_directory.empty()
                                  ? NULL
                                  : options.include_directory.c_str();
}

}  // namespace

namespace matiec {

CompilationResult Compiler::compile(CompilationContext &context) const {
  if (context.source_path().empty()) {
    context.diagnostics().error("No source path was provided");
    return context.diagnostics().result();
  }

  const CompilerOptions &options = context.options();
  apply_compiler_options(options);

  symbol_c *tree_root = NULL;
  if (stage1_2(context.source_path().c_str(), &tree_root) < 0)
    return CompilationResult::failure();

  if (options.syntax_only)
    return CompilationResult::success();

  absyntax_utils_init(tree_root);

  symbol_c *ordered_tree_root = NULL;
  if (stage3(tree_root, &ordered_tree_root) < 0)
    return CompilationResult::failure();

  const char *output_directory = options.output_directory.empty()
                                     ? NULL
                                     : options.output_directory.c_str();
  if (stage4(ordered_tree_root, output_directory) < 0)
    return CompilationResult::failure();

  return CompilationResult::success();
}

}  // namespace matiec
