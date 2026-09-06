#include "compiler/compiler.hh"
#include "compiler/access_variable_normalizer.hh"
#include "compiler/compilation_abort.hh"
#include "compiler/legacy_global_state_adapter.hh"
#include "compiler/namespace_normalizer.hh"
#include "compiler/object_method_normalizer.hh"
#include "compiler/utf8_validation.hh"

#include "absyntax/absyntax.hh"
#include "stage3/stage3.hh"
#include "stage4/stage4.hh"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>

namespace {

class TemporarySource {
 public:
  ~TemporarySource() {
    if (!path_.empty()) std::remove(path_.c_str());
  }

  bool write(const std::string &source, std::string *error) {
    char path[] = "/tmp/matiec-namespace-XXXXXX";
    const int descriptor = mkstemp(path);
    if (descriptor < 0) {
      *error = std::strerror(errno);
      return false;
    }
    path_ = path;
    std::size_t written = 0;
    while (written < source.size()) {
      const ssize_t count = ::write(descriptor, source.data() + written,
                                    source.size() - written);
      if (count < 0) {
        *error = std::strerror(errno);
        ::close(descriptor);
        return false;
      }
      written += static_cast<std::size_t>(count);
    }
    if (::close(descriptor) != 0) {
      *error = std::strerror(errno);
      return false;
    }
    return true;
  }

  const std::string &path() const { return path_; }

 private:
  std::string path_;
};

}  // namespace

namespace matiec {

CompilationResult Compiler::compile(CompilationContext &context) const {
  if (context.source_path().empty()) {
    context.diagnostics().error("No source path was provided");
    return context.diagnostics().result();
  }

  try {
    if (!language_profile_is_experimental(context.options().language_profile) &&
        !reject_legacy_access_variables_in_file(context.source_path(),
                                                context.diagnostics()))
      return context.diagnostics().result();
    if (language_profile_is_experimental(context.options().language_profile)) {
      Utf8Error utf8_error;
      if (!validate_utf8_file(context.source_path(), &utf8_error)) {
        SourceLocation location{context.source_path(), utf8_error.line,
                                utf8_error.column, utf8_error.offset};
        context.diagnostics().error("Malformed UTF-8 source: " + utf8_error.reason,
                                    {location, location});
        return context.diagnostics().result();
      }
    }
    ActiveAstArenaScope ast_arena_scope(context.ast_arena());
    const CompilerOptions &options = context.options();
    LegacyGlobalStateAdapter legacy_state(context);

    NamespaceNormalizeResult namespace_result;
    ObjectMethodNormalizeResult method_result;
    AccessVariableNormalizeResult access_result;
    TemporarySource normalized_source;
    std::string parser_source_path = context.source_path();
    if (language_profile_is_experimental(options.language_profile)) {
      if (!normalize_experimental_namespace_file(
              context.source_path(), context.diagnostics(), &namespace_result))
        return context.diagnostics().result();
      if (!normalize_experimental_object_methods(
              namespace_result.source, context.source_path(), context.diagnostics(),
              &method_result))
        return context.diagnostics().result();
      if (!normalize_experimental_access_variables(
              method_result.source, context.source_path(), context.diagnostics(),
              &access_result))
        return context.diagnostics().result();
      if (namespace_result.used_namespaces || method_result.used_methods ||
          access_result.used_access_variables) {
        std::string error;
        if (!normalized_source.write(access_result.source, &error)) {
          context.diagnostics().fatal(
              "Cannot create normalized namespace source: " + error);
          return context.diagnostics().result();
        }
        parser_source_path = normalized_source.path();
      }
    }

    symbol_c *tree_root = NULL;
    if (legacy_state.parse(parser_source_path, context.source_path(), &tree_root) < 0)
      return CompilationResult::failure();

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
