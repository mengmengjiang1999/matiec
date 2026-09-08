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
#include <algorithm>
#include <atomic>
#include <exception>
#include <thread>
#include <unordered_map>
#include <vector>

namespace matiec {

CompilationResult Compiler::compile(CompilationContext &context) const {
  context.analysis().clear();
  context.declaration_symbols().clear();
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
    ActiveDeclarationSymbolTablesScope declaration_symbols_scope(
        context.declaration_symbols());
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

std::vector<CompilationResult> Compiler::compile_parallel(
    const std::vector<std::reference_wrapper<CompilationContext>> &contexts,
    std::size_t max_concurrency) const {
  std::vector<CompilationResult> results(contexts.size());
  if (contexts.empty()) return results;

  std::unordered_map<CompilationContext *, std::size_t> occurrences;
  for (const std::reference_wrapper<CompilationContext> &context : contexts)
    ++occurrences[&context.get()];

  std::vector<bool> duplicate(contexts.size(), false);
  std::size_t accepted_count = 0;
  for (std::size_t index = 0; index < contexts.size(); ++index) {
    CompilationContext &context = contexts[index].get();
    if (occurrences[&context] > 1) {
      duplicate[index] = true;
      continue;
    }
    ++accepted_count;
  }

  for (const std::pair<CompilationContext *const, std::size_t> &entry :
       occurrences) {
    if (entry.second > 1)
      entry.first->diagnostics().error(
          "A compilation context may appear only once in a parallel batch");
  }
  for (std::size_t index = 0; index < contexts.size(); ++index) {
    if (duplicate[index])
      results[index] = contexts[index].get().diagnostics().result();
  }
  if (accepted_count == 0) return results;

  std::size_t worker_count = max_concurrency;
  if (worker_count == 0) worker_count = std::thread::hardware_concurrency();
  if (worker_count == 0) worker_count = 1;
  worker_count = std::min(worker_count, accepted_count);

  std::atomic<std::size_t> next_index(0);
  std::vector<std::exception_ptr> exceptions(contexts.size());
  const auto run_worker = [&] {
    while (true) {
      const std::size_t index = next_index.fetch_add(1);
      if (index >= contexts.size()) return;
      if (duplicate[index]) continue;
      try {
        results[index] = compile(contexts[index].get());
      } catch (...) {
        exceptions[index] = std::current_exception();
      }
    }
  };

  std::vector<std::thread> workers;
  workers.reserve(worker_count);
  try {
    for (std::size_t index = 0; index < worker_count; ++index)
      workers.emplace_back(run_worker);
  } catch (...) {
    for (std::thread &worker : workers) worker.join();
    throw;
  }
  for (std::thread &worker : workers) worker.join();
  for (const std::exception_ptr &exception : exceptions) {
    if (exception) std::rethrow_exception(exception);
  }
  return results;
}

}  // namespace matiec
