#include "compilation_context.hh"

#include <utility>

namespace matiec {

CompilationContext::CompilationContext(CompilerOptions options)
    : options_(std::move(options)) {}

CompilerOptions &CompilationContext::options() {
  return options_;
}

const CompilerOptions &CompilationContext::options() const {
  return options_;
}

DiagnosticEngine &CompilationContext::diagnostics() {
  return diagnostics_;
}

const DiagnosticEngine &CompilationContext::diagnostics() const {
  return diagnostics_;
}

AstArena &CompilationContext::ast_arena() {
  return ast_arena_;
}

const AstArena &CompilationContext::ast_arena() const {
  return ast_arena_;
}

void CompilationContext::set_source_path(std::string source_path) {
  source_path_ = std::move(source_path);
}

const std::string &CompilationContext::source_path() const {
  return source_path_;
}

}  // namespace matiec
