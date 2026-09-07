#include "compiler/compiler.hh"
#include "compiler/access_variable_normalizer.hh"
#include "compiler/access_variable_ast.hh"
#include "compiler/compilation_abort.hh"
#include "compiler/legacy_global_state_adapter.hh"
#include "compiler/modern_library_registry.hh"
#include "compiler/namespace_ast_analysis.hh"
#include "compiler/namespace_normalizer.hh"
#include "compiler/object_method_ast_analysis.hh"
#include "compiler/object_method_call_lowering.hh"
#include "compiler/object_method_compatibility_ast.hh"
#include "compiler/utf8_validation.hh"

#include "absyntax/absyntax.hh"
#include "stage3/stage3.hh"
#include "stage4/stage4.hh"

#include <string>

namespace matiec {

CompilationResult Compiler::compile(CompilationContext &context) const {
  context.analysis().clear();
  context.experimental_syntax().clear();
  if (context.source_path().empty()) {
    context.diagnostics().error("No source path was provided");
    return context.diagnostics().result();
  }

  try {
    std::string source;
    const bool needs_source_bytes = context.sources().is_memory_backed() ||
        language_profile_is_experimental(context.options().language_profile);
    if (needs_source_bytes) {
      std::string error;
      if (!context.sources().load(&source, &error)) {
        context.diagnostics().error("Cannot load source " +
                                    context.source_path() + ": " + error);
        return context.diagnostics().result();
      }
    }
    if (!language_profile_is_experimental(context.options().language_profile)) {
      const bool accepted = context.sources().is_memory_backed()
          ? reject_legacy_access_variables(source, context.source_path(),
                                           context.diagnostics())
          : reject_legacy_access_variables_in_file(context.source_path(),
                                                   context.diagnostics());
      if (!accepted) return context.diagnostics().result();
    }
    if (language_profile_is_experimental(context.options().language_profile)) {
      Utf8Error utf8_error;
      if (!validate_utf8_bytes(
              reinterpret_cast<const unsigned char *>(source.data()),
              source.size(), &utf8_error)) {
        SourceLocation location{context.source_path(), utf8_error.line,
                                utf8_error.column, utf8_error.offset};
        context.diagnostics().error("Malformed UTF-8 source: " + utf8_error.reason,
                                    {location, location});
        return context.diagnostics().result();
      }
    }
    ActiveAstArenaScope ast_arena_scope(context.ast_arena());
    CompilerOptions &options = context.options();

    NamespaceNormalizeResult namespace_result;
    NamespaceAnalysisResult namespace_analysis;
    ObjectMethodAnalysisResult method_result;
    AccessVariableNormalizeResult access_result;
    ModernLibraryRegistrationResult modern_library_result;
    if (language_profile_is_experimental(options.language_profile)) {
      if (!normalize_experimental_namespaces(
              source, context.source_path(), context.diagnostics(),
              &namespace_result))
        return context.diagnostics().result();
      source = std::move(namespace_result.source);
    }

    LegacyGlobalStateAdapter legacy_state(context, options);

    symbol_c *tree_root = NULL;
    const int parse_status = needs_source_bytes
        ? legacy_state.parse_source(source, context.source_path(), &tree_root)
        : legacy_state.parse(&tree_root);
    if (parse_status < 0)
      return CompilationResult::failure();

    if (language_profile_is_experimental(options.language_profile)) {
      if (!register_experimental_modern_library_from_ast(
              tree_root, options.disable_implicit_en_eno,
              context.diagnostics(), &modern_library_result) ||
          !analyze_access_variables_from_ast(
              tree_root, context.diagnostics(), &access_result) ||
          !analyze_namespaces_from_ast(
              tree_root, context.diagnostics(), &namespace_analysis) ||
          !analyze_object_methods_from_ast(
              tree_root, context.diagnostics(), &method_result))
        return context.diagnostics().result();
      context.experimental_syntax().access_variables = access_result.declarations;
      context.experimental_syntax().namespaces = namespace_analysis.declarations;
      context.experimental_syntax().methods = method_result.methods;
      context.experimental_syntax().library_functions =
          modern_library_result.functions;
    }

    if (language_profile_is_experimental(options.language_profile) &&
        (!construct_object_method_compatibility_ast(
             tree_root, method_result, context.diagnostics()) ||
         !lower_object_method_calls(tree_root, method_result,
                                    context.diagnostics())))
        return context.diagnostics().result();

    if (options.syntax_only)
      return CompilationResult::success();

    legacy_state.initialize_symbol_tables(tree_root);

    symbol_c *ordered_tree_root = NULL;
    if (stage3(tree_root, &ordered_tree_root, context) < 0)
      return context.diagnostics().result();

    if (stage4(ordered_tree_root, context) < 0)
      return context.diagnostics().result();

    if (!write_access_variable_metadata(access_result, options.output_directory,
                                        context.outputs()))
      return context.diagnostics().result();

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
