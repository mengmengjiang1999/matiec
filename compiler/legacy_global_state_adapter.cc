#include "compiler/legacy_global_state_adapter.hh"

#include "absyntax_utils/absyntax_utils.hh"
#include "main.hh"
#include "stage1_2/stage1_2.hh"

/* Process-wide compatibility state consumed by the generated parser and
 * legacy visitors. It is populated only through LegacyGlobalStateAdapter. */
runtime_options_t runtime_options;

namespace matiec {

LegacyGlobalStateAdapter::LegacyGlobalStateAdapter(CompilationContext &context)
    : context_(context) {
  const CompilerOptions &options = context_.options();
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
  runtime_options.utf8_source_and_strings =
      options.language_profile == LanguageProfile::iec61131_3_2025_experimental;
  runtime_options.iec2025_experimental =
      options.language_profile == LanguageProfile::iec61131_3_2025_experimental;
  runtime_options.relaxed_datatype_model = options.relaxed_datatype_model;
  runtime_options.includedir = options.include_directory.empty()
                                  ? NULL
                                  : options.include_directory.c_str();
}

int LegacyGlobalStateAdapter::parse(symbol_c **tree_root) const {
  return stage1_2(context_.source_path().c_str(), tree_root);
}

void LegacyGlobalStateAdapter::initialize_symbol_tables(symbol_c *tree_root) const {
  absyntax_utils_init(tree_root);
}

}  // namespace matiec
