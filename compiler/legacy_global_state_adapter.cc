#include "compiler/legacy_global_state_adapter.hh"

#include "absyntax_utils/absyntax_utils.hh"
#include "main.hh"
#include "stage1_2/stage1_2.hh"

namespace matiec {

LegacyGlobalStateAdapter::LegacyGlobalStateAdapter(
    CompilationContext &context, const CompilerOptions &options)
    : context_(context) {
  context_.parser_state().reset_for_parse();
  runtime_options_t &runtime = context_.parser_state().options;
  runtime.allow_void_datatype = options.allow_void_datatype;
  runtime.allow_missing_var_in = options.allow_missing_var_in;
  runtime.disable_implicit_en_eno = options.disable_implicit_en_eno;
  runtime.pre_parsing = options.pre_parsing;
  runtime.safe_extensions = options.safe_extensions;
  runtime.full_token_loc = options.full_token_location;
  runtime.conversion_functions = options.conversion_functions;
  runtime.nested_comments = options.nested_comments;
  runtime.ref_standard_extensions = options.reference_extensions;
  runtime.ref_nonstand_extensions = options.nonstandard_reference_extensions;
  runtime.nonliteral_in_array_size = options.nonliteral_array_size;
  runtime.utf8_source_and_strings =
      options.language_profile == LanguageProfile::iec61131_3_2025_experimental;
  runtime.iec2025_experimental =
      options.language_profile == LanguageProfile::iec61131_3_2025_experimental;
  runtime.register_experimental_assert =
      options.language_profile == LanguageProfile::iec61131_3_2025_experimental;
  runtime.relaxed_datatype_model = options.relaxed_datatype_model;
  runtime.includedir = options.include_directory.empty()
                                  ? NULL
                                  : options.include_directory.c_str();
}

LegacyGlobalStateAdapter::~LegacyGlobalStateAdapter() = default;

int LegacyGlobalStateAdapter::parse(symbol_c **tree_root) const {
  return stage1_2(context_.parser_state(), context_.source_path().c_str(),
                  tree_root);
}

int LegacyGlobalStateAdapter::parse(const std::string &source_path,
                                    const std::string &display_path,
                                    symbol_c **tree_root) const {
  return stage1_2(context_.parser_state(), source_path.c_str(),
                  display_path.c_str(), tree_root);
}

int LegacyGlobalStateAdapter::parse_source(const std::string &source,
                                           const std::string &display_path,
                                           symbol_c **tree_root) const {
  return stage1_2_from_source(context_.parser_state(), source.data(),
                              source.size(), display_path.c_str(), tree_root);
}

void LegacyGlobalStateAdapter::initialize_symbol_tables(symbol_c *tree_root) const {
  absyntax_utils_init(tree_root, context_.declaration_symbols());
}

}  // namespace matiec
